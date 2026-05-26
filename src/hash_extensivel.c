#include "hash_extensivel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HF_CHAVE_TAM 64 // tamanho em bytes das chaves

typedef struct {
    int prof_global;    // profundidade global atual do arquivo
    int bucket_cap;     // capacidade de slots por bucket
    int size_registro;  // tamanho dos registros
    int num_registros;  // numero total de registros vivos
    int num_buckets;    // numero total de buckets
    int num_expansoes;  // numero total de splits realizados
    long dir_offset;    // offset do diretorio no arquivo
    long prox_livre;    // proximo byte livre
} Cabecalho;

typedef struct {
    int prof_local;     // profundidade do bucket
    int num_registros;  // numero de registros vivos no bucket
    int num_splits;     // numero de splits que o bucket ja sofreu
} Bucket;

typedef struct {
    int ativo;                  // 1 se o slot estiver ocupado, 0 caso contrário
    char chave[HF_CHAVE_TAM];   // chave do registro
} Slot;

struct HashExtensivel {
    FILE* arquivo;      // ponteiro para o arquivo
    Cabecalho cab;      // cabeçalho do hashfile
    long* diretorio;    // vetor de offsets de cada bucket
};

static void cab_escrever(HashExtensivel* h) {
    rewind(h->arquivo);
    fwrite(&h->cab, sizeof(Cabecalho), 1, h->arquivo);
    fflush(h->arquivo);
}

static void cab_ler(HashExtensivel* h) {
    rewind(h->arquivo);
    fread(&h->cab, sizeof(Cabecalho), 1, h->arquivo);
}

static void dir_escrever(HashExtensivel* h) {
    int tam = 1 << h->cab.prof_global;  // quantidade atual de diretorios = 2^prof_global
    fseek(h->arquivo, h->cab.dir_offset, SEEK_SET);
    fwrite(h->diretorio, sizeof(long), tam, h->arquivo);
    fflush(h->arquivo);
}

static void dir_ler(HashExtensivel* h) {
    int tam = 1 << h->cab.prof_global; // 2^prof_global
    fseek(h->arquivo, h->cab.dir_offset, SEEK_SET);
    fread(h->diretorio, sizeof(long), tam, h->arquivo);
}


// Função de hash FNV-1a
static int hash_indice(const char* chave, int prof_global) {
    unsigned int hash = 2166136261u;
    for(const unsigned char *p = (const unsigned char *)chave; *p; p++) {
        hash ^= *p;
        hash *= 16777619u;
    }
    return (int)(hash & ((1u << prof_global) - 1));
}

static long bucket_alocar(HashExtensivel* h, int prof_local) {
    long offset = h->cab.prox_livre;

    Bucket b;
    b.prof_local = prof_local;
    b.num_registros = 0;
    b.num_splits = 0;
 
    fseek(h->arquivo, offset, SEEK_SET);
    fwrite(&b, sizeof(Bucket), 1, h->arquivo);
 
    // zera todos os slots do bucket
    int slot_tam = sizeof(Slot) + h->cab.size_registro;
    char* zero = calloc(1, slot_tam);
    for(int i = 0; i < h->cab.bucket_cap; i++)
        fwrite(zero, slot_tam, 1, h->arquivo);

    free(zero);
 
    h->cab.prox_livre += sizeof(Bucket) + h->cab.bucket_cap * slot_tam;
    h->cab.num_buckets++;
    return offset;
}

static long slot_offset(HashExtensivel* h, long bucket_off, int i) {
    int slot_tam = sizeof(Slot) + h->cab.size_registro;
    return bucket_off + sizeof(Bucket) + i * slot_tam;
}

static int inserir_em_bucket(HashExtensivel* h, long bucket_off, const char* chave, const void* registro) {
    Bucket b;
    fseek(h->arquivo, bucket_off, SEEK_SET);
    fread(&b, sizeof(Bucket), 1, h->arquivo);
 
    if(b.num_registros >= h->cab.bucket_cap)
        return -1;  // bucket cheio
 
    int slot_tam = sizeof(Slot) + h->cab.size_registro;
 
    // procura primeiro slot inativo 
    for(int i = 0; i < h->cab.bucket_cap; i++) {
        long off = slot_offset(h, bucket_off, i);
        Slot s;
        fseek(h->arquivo, off, SEEK_SET);
        fread(&s, sizeof(Slot), 1, h->arquivo);
 
        if(s.ativo == 0) {
            // escreve no slot 
            s.ativo = 1;
            strncpy(s.chave, chave, HF_CHAVE_TAM - 1);
            s.chave[HF_CHAVE_TAM - 1] = '\0';
 
            fseek(h->arquivo, off, SEEK_SET);
            fwrite(&s, sizeof(Slot), 1, h->arquivo);
            fwrite(registro, h->cab.size_registro, 1, h->arquivo);
 
            // atualiza cabeçalho do bucket 
            b.num_registros++;
            fseek(h->arquivo, bucket_off, SEEK_SET);
            fwrite(&b, sizeof(Bucket), 1, h->arquivo);
            return 0;
        }
    }
    return -1;  // não encontrou slot livre (bucket cheio) 
}

static void split(HashExtensivel* h, int indice) {
    long old_off = h->diretorio[indice];
    Bucket b_old;
    fseek(h->arquivo, old_off, SEEK_SET);
    fread(&b_old, sizeof(Bucket), 1, h->arquivo);
 
    int prof_local = b_old.prof_local;
 
    // se necessário, dobra o diretório
    if(prof_local == h->cab.prof_global) {
        int tam_old = 1 << h->cab.prof_global; // 2^prof_global
        int tam_new = tam_old * 2;
 
        long* novo_dir = realloc(h->diretorio, tam_new * sizeof(long));
        h->diretorio = novo_dir;
 
        // duplica entradas
        for(int i = 0; i < tam_old; i++)
            h->diretorio[tam_old + i] = h->diretorio[i];
 
        h->cab.prof_global++;
 
        // reloca diretório para o fim do arquivo para não colidir com buckets
        h->cab.dir_offset = h->cab.prox_livre;
        h->cab.prox_livre = h->cab.dir_offset + tam_new * sizeof(long);
 
        cab_escrever(h);
        dir_escrever(h);
    }
    h->cab.num_expansoes++;

    // aloca novo bucket com prof_local + 1 
    int nova_prof = prof_local + 1;
    long new_off = bucket_alocar(h, nova_prof);
 
    // atualiza prof_local do bucket antigo
    b_old.prof_local = nova_prof;
    b_old.num_registros = 0;
    b_old.num_splits++;
    fseek(h->arquivo, old_off, SEEK_SET);
    fwrite(&b_old, sizeof(Bucket), 1, h->arquivo);
 
    // redistribui entradas do diretório para os buckets antigo e novo
    int tam = 1 << h->cab.prof_global;
    int prefixo_base = indice & ((1 << prof_local) - 1); // prefixo compartilhado pelos buckets antigo e novo
    int mascara_nova = (1 << nova_prof) - 1; // mascara para extrair os bits relevantes para o novo bucket
 
    for(int i = 0; i < tam; i++) {
        if((i & ((1 << prof_local) - 1)) != prefixo_base) 
            continue;
            
        if((i & mascara_nova) == prefixo_base)
            h->diretorio[i] = old_off;   // bit nova_prof-1 == 0
        else
            h->diretorio[i] = new_off;   // bit nova_prof-1 == 1 
    }
 
    // coleta registros vivos do bucket antigo 
    int slot_tam = sizeof(Slot) + h->cab.size_registro;
    char* buf = malloc(slot_tam);
 
    char** chaves = malloc(h->cab.bucket_cap * sizeof(char*));
    void** dados = malloc(h->cab.bucket_cap * sizeof(void*));
    int count = 0;
 
    for(int i = 0; i < h->cab.bucket_cap; i++) {
        long off = slot_offset(h, old_off, i);
        Slot s;
        fseek(h->arquivo, off, SEEK_SET);
        fread(&s, sizeof(Slot), 1, h->arquivo);
 
        if(s.ativo == 0) 
            continue;
 
        chaves[count] = malloc(HF_CHAVE_TAM);
        dados[count] = malloc(h->cab.size_registro);
        strncpy(chaves[count], s.chave, HF_CHAVE_TAM - 1);
        chaves[count][HF_CHAVE_TAM - 1] = '\0';
        fread(dados[count], h->cab.size_registro, 1, h->arquivo);
 
        // zera slot no bucket antigo 
        fseek(h->arquivo, off, SEEK_SET);
        memset(buf, 0, slot_tam);
        fwrite(buf, slot_tam, 1, h->arquivo);
        count++;
    }
    free(buf);
 
    // zera contagem do bucket antigo
    b_old.num_registros = 0;
    fseek(h->arquivo, old_off, SEEK_SET);
    fwrite(&b_old, sizeof(Bucket), 1, h->arquivo);
 
    // redistribui os registros coletados 
    for(int i = 0; i < count; i++) {
        int idx = hash_indice(chaves[i], h->cab.prof_global);
        inserir_em_bucket(h, h->diretorio[idx], chaves[i], dados[i]);
        free(chaves[i]);
        free(dados[i]);
    }
    free(chaves);
    free(dados);
}

HashExtensivel* hash_criar(const char* caminho, int bucket_cap, size_t size_registro) {
    // falha se algum dos parametros for invalido
    if(bucket_cap <= 0 || size_registro <= 0)
        return NULL;

    // falha se o arquivo já existir
    FILE* f = fopen(caminho, "rb");
    if(f != NULL) { 
        fclose(f); 
        return NULL; 
    }
 
    f = fopen(caminho, "w+b");
    if(f == NULL) 
        return NULL;
 
    HashExtensivel* h = calloc(1, sizeof(HashExtensivel));
    if(h == NULL) { 
        fclose(f); 
        return NULL; 
    }
    h->arquivo = f;
    

    // inicializa cabeçalho
    h->cab.prof_global = 1;
    h->cab.bucket_cap = bucket_cap;
    h->cab.size_registro = (int) size_registro;
    h->cab.num_registros = 0;
    h->cab.num_buckets = 0;
    h->cab.num_expansoes = 0;
    h->cab.dir_offset = sizeof(Cabecalho);
    h->cab.prox_livre = h->cab.dir_offset + 2 * sizeof(long);
 
    // aloca o diretório na memória 
    h->diretorio = calloc(2, sizeof(long));
    if(h->diretorio == NULL) { 
        fclose(f); 
        free(h); 
        return NULL; 
    }
 
    cab_escrever(h);
 
    // aloca 2 buckets iniciais e preenche o diretório
    for(int i = 0; i < 2; i++)
        h->diretorio[i] = bucket_alocar(h, 1);
 
    // reescreve cabeçalho e diretório com os offsets dos buckets
    cab_escrever(h);
    dir_escrever(h);
    return h;
}

HashExtensivel* hash_abrir(const char* caminho) {
    FILE* f = fopen(caminho, "r+b");
    if(f == NULL) 
        return NULL;
 
    HashExtensivel* h = calloc(1, sizeof(HashExtensivel));
    if(h == NULL) { 
        fclose(f); 
        return NULL; 
    }
    h->arquivo = f;
 
    cab_ler(h);
 
    // aloca e carrega o diretório
    int tam = 1 << h->cab.prof_global;
    h->diretorio = malloc(tam * sizeof(long));
    if(h->diretorio == NULL) { 
        fclose(f); 
        free(h); 
        return NULL; 
    }
    dir_ler(h);
 
    return h;
}
 
void hash_fechar(HashExtensivel* h) {
    if(h == NULL) 
        return;

    cab_escrever(h);
    dir_escrever(h);
    fclose(h->arquivo);
    free(h->diretorio);
    free(h);
}

int hash_inserir(HashExtensivel* h, const char* chave, const void* registro) {
    if(h == NULL || chave == NULL || registro == NULL) 
        return -1;
 
    // rejeita duplicata
    if(hash_buscar(h, chave, NULL) == 0) 
        return -1;
 
    int indice = hash_indice(chave, h->cab.prof_global);
    long buck_off = h->diretorio[indice];
 
    // tenta inserir, se bucket estiver cheio faz split e tenta novamente
    if(inserir_em_bucket(h, buck_off, chave, registro) != 0) {
        split(h, indice);
        indice = hash_indice(chave, h->cab.prof_global);
        buck_off = h->diretorio[indice];
        if(inserir_em_bucket(h, buck_off, chave, registro) != 0)
            return -1;
    }
 
    h->cab.num_registros++;
    cab_escrever(h);
    dir_escrever(h);
    return 0;
}

int hash_buscar(HashExtensivel* h, const char* chave, void* saida) {
    if(h == NULL || chave == NULL) 
        return -1;
 
    int indice = hash_indice(chave, h->cab.prof_global);
    long buck_off = h->diretorio[indice];
 
    for(int i = 0; i < h->cab.bucket_cap; i++) {
        long off = slot_offset(h, buck_off, i);
        Slot s;
        fseek(h->arquivo, off, SEEK_SET);
        fread(&s, sizeof(Slot), 1, h->arquivo);
        
        // verifica se slot do bucket esta ativo e compara as chaves
        if(s.ativo == 1 && strncmp(s.chave, chave, HF_CHAVE_TAM) == 0) {
            if(saida != NULL)
                fread(saida, h->cab.size_registro, 1, h->arquivo);
            return 0;
        }
    }
    return -1;
}

int hash_atualizar(HashExtensivel* h, const char* chave, const void* registro) {
    if(h == NULL || chave == NULL || registro == NULL)
        return -1;

    int indice = hash_indice(chave, h->cab.prof_global);
    long buck_off = h->diretorio[indice];

    for(int i = 0; i < h->cab.bucket_cap; i++) {
        long off = slot_offset(h, buck_off, i);
        Slot s;
        fseek(h->arquivo, off, SEEK_SET);
        fread(&s, sizeof(Slot), 1, h->arquivo);

        if(s.ativo == 1 && strncmp(s.chave, chave, HF_CHAVE_TAM) == 0) {
            // posiciona logo após o slot e sobrescreve o registro
            fseek(h->arquivo, off + sizeof(Slot), SEEK_SET);
            fwrite(registro, h->cab.size_registro, 1, h->arquivo);
            fflush(h->arquivo);
            return 0;
        }
    }
    return -1;
}

int hash_remover(HashExtensivel* h, const char* chave) {
    if(h == NULL || chave == NULL) 
        return -1;
 
    int indice = hash_indice(chave, h->cab.prof_global);
    long buck_off = h->diretorio[indice];
    Bucket b;
    fseek(h->arquivo, buck_off, SEEK_SET);
    fread(&b, sizeof(Bucket), 1, h->arquivo);
 
    for(int i = 0; i < h->cab.bucket_cap; i++) {
        long off = slot_offset(h, buck_off, i);
        Slot s;
        fseek(h->arquivo, off, SEEK_SET);
        fread(&s, sizeof(Slot), 1, h->arquivo);
 
        if(s.ativo == 1 && strncmp(s.chave, chave, HF_CHAVE_TAM) == 0) {
            // marca slot como inativo, podendo ser reutilizado
            s.ativo = 0;
            fseek(h->arquivo, off, SEEK_SET);
            fwrite(&s, sizeof(Slot), 1, h->arquivo);
 
            // atualiza cabeçalhos
            b.num_registros--;
            fseek(h->arquivo, buck_off, SEEK_SET);
            fwrite(&b, sizeof(Bucket), 1, h->arquivo);
            h->cab.num_registros--;
            cab_escrever(h);
            return 0;
        }
    }
    return -1;
}

bool hash_contem(HashExtensivel* h, const char* chave) {
    return hash_buscar(h, chave, NULL) == 0;
}

int hash_iterar(HashExtensivel* h, int (*callback)(const char* chave, const void* registro, void* ctx), void* ctx) {
    if(h == NULL || callback == NULL)
        return -1;

    int tam_dir = 1 << h->cab.prof_global;
    int slot_tam = sizeof(Slot) + h->cab.size_registro;

    // marca quais offsets de bucket ja foram visitados
    long* visitados = calloc(h->cab.num_buckets, sizeof(long));
    if(visitados == NULL)
        return -1;
    int n_visitados = 0;

    void* buf_registro = malloc(h->cab.size_registro);
    if(buf_registro == NULL) {
        free(visitados);
        return -1;
    }

    int ret = 0;

    for(int d = 0; d < tam_dir && ret == 0; d++) {
        long buck_off = h->diretorio[d];

        // pula bucket já visitado (entradas duplicadas no diretório)
        int ja_visto = 0;
        for(int v = 0; v < n_visitados; v++) {
            if(visitados[v] == buck_off) { 
                ja_visto = 1; 
                break; 
            }
        }
        if(ja_visto) 
            continue;

        visitados[n_visitados++] = buck_off;

        Bucket b;
        fseek(h->arquivo, buck_off, SEEK_SET);
        fread(&b, sizeof(Bucket), 1, h->arquivo);

        for(int i = 0; i < h->cab.bucket_cap; i++) {
            long off = slot_offset(h, buck_off, i);
            Slot s;
            fseek(h->arquivo, off, SEEK_SET);
            fread(&s, sizeof(Slot), 1, h->arquivo);

            if(s.ativo == 0) 
                continue;

            fread(buf_registro, h->cab.size_registro, 1, h->arquivo);

            ret = callback(s.chave, buf_registro, ctx);
            if(ret != 0) 
                break;
        }
    }
    free(visitados);
    free(buf_registro);
    return ret;
}

int hash_dump(HashExtensivel* h, const char* caminho_hfd) {
    if(h == NULL || caminho_hfd == NULL)
        return -1;

    FILE* out = fopen(caminho_hfd, "w");
    if(out == NULL)
        return -1;

    int tam_dir = 1 << h->cab.prof_global;

    fprintf(out, "DUMP\n");
    fprintf(out, "*Dump cabecalho\n");
    fprintf(out, "prof_global=%d\n", h->cab.prof_global);
    fprintf(out, "bucket_cap=%d\n", h->cab.bucket_cap);
    fprintf(out, "size_registro=%d\n", h->cab.size_registro);
    fprintf(out, "num_registros=%d\n", h->cab.num_registros);
    fprintf(out, "num_buckets=%d\n", h->cab.num_buckets);
    fprintf(out, "num_expansoes=%d\n", h->cab.num_expansoes);

    fprintf(out, "*Dump diretorios\n");
    for(int i = 0; i < tam_dir; i++)
        fprintf(out,"[%d] %ld\n", i, h->diretorio[i]);

    fprintf(out, "*Dump buckets\n");

    // visita cada bucket unico 
    long* visitados = calloc(h->cab.num_buckets + 1, sizeof(long));
    if(visitados == NULL) { 
        fclose(out); 
        return -1; 
    }
    int n_visitados = 0;

    int i = 1;
    for(int d = 0; d < tam_dir; d++) {
        long buck_off = h->diretorio[d];

        int ja_visto = 0;
        for(int v = 0; v < n_visitados; v++) {
            if(visitados[v] == buck_off) { 
                ja_visto = 1; 
                break; 
            }
        }
        if(ja_visto) 
            continue;
        visitados[n_visitados++] = buck_off;

        Bucket b;
        fseek(h->arquivo, buck_off, SEEK_SET);
        fread(&b, sizeof(Bucket), 1, h->arquivo);

        fprintf(out, "Bucket %d | offset=%ld | prof_local=%d | registros=%d/%d | splits=%d\n",
                i, buck_off, b.prof_local, b.num_registros, h->cab.bucket_cap, b.num_splits);

        for(int i = 0; i < h->cab.bucket_cap; i++) {
            long off = slot_offset(h, buck_off, i);
            Slot s;
            fseek(h->arquivo, off, SEEK_SET);
            fread(&s, sizeof(Slot), 1, h->arquivo);

            fprintf(out, "%d | %s\n", s.ativo, s.chave);
        }
        i++;
    }
    fprintf(out, "FIM DUMP");

    free(visitados);
    fclose(out);
    return 0;
}