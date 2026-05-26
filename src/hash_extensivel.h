#ifndef HASH_EXTENSIVEL_H
#define HASH_EXTENSIVEL_H

#include <stddef.h>
#include <stdbool.h>

/*  Hashfile Extensível — dicionário em arquivo binário (.hf).
    
    É baseado em hashing extensível: um diretório global mapeia prefixos de bits
    da chave para buckets em disco. Quando um bucket estoura sua capacidade,
    ele é dividido (split) e o diretório dobra de tamanho se necessário,
    sem precisar reescrever todos os registros.

    Cada registro é um bloco de bytes de tamanho fixo (definido em hash_criar), 
    identificado por uma chave string única (de 64 caracteres). É responsabilidade do 
    chamador que o tamanho e o tipo sejam os mesmos em todas as operações para que não haja erros.

    Um mesmo hashfile não deve ser aberto mais de uma vez simultaneamente.
*/

typedef struct HashExtensivel HashExtensivel;

/*  Cria um novo hashfile em disco no path passado em caminho que armazena registros do tamanho size_registro em
    buckets de capacidade bucket_cape retorna o handle ou NULL em caso de erro.
*/
HashExtensivel* hash_criar(const char* caminho, int bucket_cap, size_t size_registro);

/* Abre um hashfile existente no path indicado por caminho e retorna o handle. */
HashExtensivel* hash_abrir(const char* caminho);

/* Fecha e libera o handle do hashfile */
void hash_fechar(HashExtensivel* h);

/*  Insere um registro em um hashfile a partir de uma chave única (ex: ID do registro)
    e o ponteiro para o registro (deve ser do mesmo tamanho em bytes passado ao criar o hashfile). 
    Retorna 0 caso ocorra sucesso ou -1 em erro. 
*/
int hash_inserir(HashExtensivel* h, const char* chave, const void* registro);

/* Busca o valor da chave e copia o registro em saida. Retorna 0 se encontrou ou -1 caso contrário. */
int hash_buscar(HashExtensivel* h, const char* chave, void* saida);

/*  Atualiza o conteúdo de um registro armazenado na chave indicada por um novo registro ou registro atualizado. 
    Retorna 0 em sucesso ou -1 se não encontrou/erro. 
*/
int hash_atualizar(HashExtensivel* h, const char* chave, const void* registro);

/* Remove o registro armazenado na chave indicada. Retorna 0 em sucesso ou -1 se não encontrou/erro. */
int hash_remover(HashExtensivel* h, const char* chave);

/* Verifica se uma chave existe no hashfile. Retorna true se a chave existe ou false caso contrário */
bool hash_contem(HashExtensivel* h, const char* chave);

/*  Itera sobre todos os registros existentes do hashfile. A função de callback é executada em cada registro
    e pode usar ctx para armazenar resultados ou ctx pode ser passado como NULL. O callback deve retornar 0 
    para continuar a iteração ou qualquer valor diferente de 0 para interrompê-la imediatamente. 
    Retorna 0 se iterou sobre todos os registros, o valor de parada do callback se a iteração foi interrompida, 
    ou -1 em erro.
*/
int hash_iterar(HashExtensivel* h, int (*callback)(const char* chave, const void* registro, void* ctx), void* ctx);


/*  Gera um arquivo-texto de formato .hfd no path indicado em *caminho_hfd, com representação esquemática legível 
    do hashfile e também com informação sobre os splits dos buckets.
    Retorna 0 caso ocorra sucesso ou -1 em erro.  
*/
int hash_dump(HashExtensivel* h, const char* caminho_hfd);

#endif