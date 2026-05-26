#include "unity/unity.h"
#include "../hash_extensivel.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char chave[64];
    int valor;
} Registro;

#define CAMINHO_TESTE "teste.hf"

HashExtensivel* h;

void setUp(void) {
    remove(CAMINHO_TESTE); 
    h = hash_criar(CAMINHO_TESTE, 4, sizeof(Registro));
    TEST_ASSERT_NOT_NULL_MESSAGE(h, "hash_criar falhou.");
}

void tearDown(void) {
    if(h) {
        hash_fechar(h);
        h = NULL;
    }
    remove(CAMINHO_TESTE);
}

void test_criar_retorna_handle_valido(void) {
    TEST_ASSERT_NOT_NULL(h);
}

void test_criar_falha_se_arquivo_ja_existe(void) {
    // arquivo já foi criado no setUp 
    HashExtensivel* h2 = hash_criar(CAMINHO_TESTE, 4, sizeof(Registro));
    TEST_ASSERT_NULL(h2);
}

void test_abrir_hashfile_existente(void) {
    hash_fechar(h);
    h = NULL;
    h = hash_abrir(CAMINHO_TESTE);
    TEST_ASSERT_NOT_NULL(h);
}

void test_abrir_arquivo_inexistente_retorna_null(void) {
    HashExtensivel* h2 = hash_abrir("nao_existe.hf");
    TEST_ASSERT_NULL(h2);
}

void test_dados_persistem_apos_fechar_e_reabrir(void) {
    Registro r = { .chave = "chave01", .valor = 42 };
    TEST_ASSERT_EQUAL_INT(0, hash_inserir(h, r.chave, &r));

    hash_fechar(h);
    h = hash_abrir(CAMINHO_TESTE);
    TEST_ASSERT_NOT_NULL(h);

    Registro saida = {0};
    TEST_ASSERT_EQUAL_INT(0, hash_buscar(h, "chave01", &saida));
    TEST_ASSERT_EQUAL_INT(42, saida.valor);
}

void test_inserir_registro_simples(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    int ret = hash_inserir(h, r.chave, &r);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_inserir_multiplos_registros(void) {
    for(int i = 0; i < 10; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "chave%02d", i);
        r.valor = i;
        TEST_ASSERT_EQUAL_INT(0, hash_inserir(h, r.chave, &r));
    }
}

void test_inserir_chave_duplicada_retorna_erro(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    TEST_ASSERT_EQUAL_INT( 0, hash_inserir(h, r.chave, &r));
    TEST_ASSERT_EQUAL_INT(-1, hash_inserir(h, r.chave, &r));
}

void test_inserir_alem_da_capacidade_do_bucket_causa_split(void) {
    // insere 16 registros gerando 4 splits
    for (int i = 0; i < 16; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "k%d", i);
        r.valor = i;
        int ret = hash_inserir(h, r.chave, &r);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, r.chave);
    }
    // todos os registros devem ser encontradoss apos os splits
    for(int i = 0; i < 16; i++) {
        char chave[64];
        snprintf(chave, sizeof(chave), "k%d", i);
        Registro saida = {0};
        TEST_ASSERT_EQUAL_INT(0, hash_buscar(h, chave, &saida));
        TEST_ASSERT_EQUAL_INT(i, saida.valor);
    }
}

void test_inserir_apos_split_persiste_apos_reabrir(void) {
    for(int i = 0; i < 16; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "k%d", i);
        r.valor = i;
        hash_inserir(h, r.chave, &r);
    }
    hash_fechar(h);
    h = hash_abrir(CAMINHO_TESTE);
    TEST_ASSERT_NOT_NULL(h);

    for(int i = 0; i < 16; i++) {
        char chave[64];
        snprintf(chave, sizeof(chave), "k%d", i);
        Registro saida = {0};
        TEST_ASSERT_EQUAL_INT(0, hash_buscar(h, chave, &saida));
        TEST_ASSERT_EQUAL_INT(i, saida.valor);
    }
}

void test_buscar_registro_existente(void) {
    Registro r = { .chave = "chave01", .valor = 99 };
    hash_inserir(h, r.chave, &r);

    Registro saida = {0};
    int ret = hash_buscar(h, "chave01", &saida);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(99, saida.valor);
}

void test_buscar_chave_inexistente_retorna_erro(void) {
    Registro saida = {0};
    int ret = hash_buscar(h, "nao_existe", &saida);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_buscar_com_saida_null_verifica_existencia(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);

    TEST_ASSERT_EQUAL_INT( 0, hash_buscar(h, "chave01", NULL));
    TEST_ASSERT_EQUAL_INT(-1, hash_buscar(h, "nao_existe", NULL));
}

void test_atualizar_registro_existente(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);

    r.valor = 100;
    TEST_ASSERT_EQUAL_INT(0, hash_atualizar(h, "chave01", &r));

    Registro saida = {0};
    hash_buscar(h, "chave01", &saida);
    TEST_ASSERT_EQUAL_INT(100, saida.valor);
}

void test_atualizar_chave_inexistente_retorna_erro(void) {
    Registro r = { .chave = "nao_existe", .valor = 1 };
    TEST_ASSERT_EQUAL_INT(-1, hash_atualizar(h, "nao_existe", &r));
}

void test_remover_registro_existente(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);

    TEST_ASSERT_EQUAL_INT(0, hash_remover(h, "chave01"));
}

void test_remover_chave_inexistente_retorna_erro(void) {
    TEST_ASSERT_EQUAL_INT(-1, hash_remover(h, "nao_existe"));
}

void test_buscar_apos_remover_retorna_erro(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);
    hash_remover(h, "chave01");

    Registro saida = {0};
    TEST_ASSERT_EQUAL_INT(-1, hash_buscar(h, "chave01", &saida));
}

void test_reinserir_apos_remover(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);
    hash_remover(h, "chave01");

    r.valor = 2;
    TEST_ASSERT_EQUAL_INT(0, hash_inserir(h, r.chave, &r));

    Registro saida = {0};
    hash_buscar(h, "chave01", &saida);
    TEST_ASSERT_EQUAL_INT(2, saida.valor);
}

void test_contem_chave_existente(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);
    TEST_ASSERT_TRUE(hash_contem(h, "chave01"));
}

void test_contem_chave_inexistente(void) {
    TEST_ASSERT_FALSE(hash_contem(h, "nao_existe"));
}

void test_contem_retorna_false_apos_remover(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);
    hash_remover(h, "chave01");
    TEST_ASSERT_FALSE(hash_contem(h, "chave01"));
}

typedef struct {
    int contagem;
    int soma_valores;
} CtxIterar;

int cb_somar(const char* chave, const void* registro, void* ctx) {
    (void) chave;
    CtxIterar* c = (CtxIterar*) ctx;
    const Registro* r = (const Registro*) registro;
    c->contagem++;
    c->soma_valores += r->valor;
    return 0;
}

int cb_parar_no_primeiro(const char* chave, const void* registro, void* ctx) {
    (void) chave; 
    (void) registro;
    int* contagem = (int*) ctx;
    (*contagem)++;
    return 1;  // para imediatamente no primeiro
}

void test_iterar_conta_registros_corretos(void) {
    for(int i = 0; i < 5; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "k%d", i);
        r.valor = i;
        hash_inserir(h, r.chave, &r);
    }

    CtxIterar ctx = {0};
    TEST_ASSERT_EQUAL_INT(0, hash_iterar(h, cb_somar, &ctx));
    TEST_ASSERT_EQUAL_INT(5, ctx.contagem);
    TEST_ASSERT_EQUAL_INT(10, ctx.soma_valores);
}

void test_iterar_nao_visita_removidos(void) {
    for(int i = 0; i < 5; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "k%d", i);
        r.valor = i;
        hash_inserir(h, r.chave, &r);
    }
    hash_remover(h, "k2");

    CtxIterar ctx = {0};
    hash_iterar(h, cb_somar, &ctx);
    TEST_ASSERT_EQUAL_INT(4, ctx.contagem);
    TEST_ASSERT_EQUAL_INT(8, ctx.soma_valores); 
}

void test_iterar_para_quando_callback_retorna_nonzero(void) {
    for(int i = 0; i < 5; i++) {
        Registro r;
        snprintf(r.chave, sizeof(r.chave), "k%d", i);
        r.valor = i;
        hash_inserir(h, r.chave, &r);
    }

    int contagem = 0;
    hash_iterar(h, cb_parar_no_primeiro, &contagem);
    TEST_ASSERT_EQUAL_INT(1, contagem);
}

void test_iterar_em_hashfile_vazio(void) {
    CtxIterar ctx = {0};
    TEST_ASSERT_EQUAL_INT(0, hash_iterar(h, cb_somar, &ctx));
    TEST_ASSERT_EQUAL_INT(0, ctx.contagem);
}

void test_dump_cria_arquivo_hfd(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);

    int ret = hash_dump(h, "teste_temp.hfd");
    TEST_ASSERT_EQUAL_INT(0, ret);

    FILE* f = fopen("teste_temp.hfd", "r");
    TEST_ASSERT_NOT_NULL_MESSAGE(f, "arquivo .hfd não foi criado");
    fclose(f);
    remove("teste_temp.hfd");
}

void test_dump_arquivo_contem_chave_inserida(void) {
    Registro r = { .chave = "chave01", .valor = 1 };
    hash_inserir(h, r.chave, &r);
    hash_dump(h, "teste_temp.hfd");

    FILE* f = fopen("teste_temp.hfd", "r");
    TEST_ASSERT_NOT_NULL(f);

    char linha[256];
    int encontrou = 0;
    while (fgets(linha, sizeof(linha), f)) {
        if(strstr(linha, "chave01")) {
            encontrou = 1;
            break;
        }
    }
    fclose(f);
    remove("teste_temp.hfd");
    TEST_ASSERT_TRUE_MESSAGE(encontrou, "chave01 não aparece no .hfd");
}

int main(void) {
    UNITY_BEGIN();

    // criar, abrir e fechar 
    RUN_TEST(test_criar_retorna_handle_valido);
    RUN_TEST(test_criar_falha_se_arquivo_ja_existe);
    RUN_TEST(test_abrir_hashfile_existente);
    RUN_TEST(test_abrir_arquivo_inexistente_retorna_null);
    RUN_TEST(test_dados_persistem_apos_fechar_e_reabrir);

    // inserir 
    RUN_TEST(test_inserir_registro_simples);
    RUN_TEST(test_inserir_multiplos_registros);
    RUN_TEST(test_inserir_chave_duplicada_retorna_erro);
    RUN_TEST(test_inserir_alem_da_capacidade_do_bucket_causa_split);
    RUN_TEST(test_inserir_apos_split_persiste_apos_reabrir);

    // buscar 
    RUN_TEST(test_buscar_registro_existente);
    RUN_TEST(test_buscar_chave_inexistente_retorna_erro);
    RUN_TEST(test_buscar_com_saida_null_verifica_existencia);

    // atualizar 
    RUN_TEST(test_atualizar_registro_existente);
    RUN_TEST(test_atualizar_chave_inexistente_retorna_erro);

    // remover 
    RUN_TEST(test_remover_registro_existente);
    RUN_TEST(test_remover_chave_inexistente_retorna_erro);
    RUN_TEST(test_buscar_apos_remover_retorna_erro);
    RUN_TEST(test_reinserir_apos_remover);

    // contém
    RUN_TEST(test_contem_chave_existente);
    RUN_TEST(test_contem_chave_inexistente);
    RUN_TEST(test_contem_retorna_false_apos_remover);

    // iterar 
    RUN_TEST(test_iterar_conta_registros_corretos);
    RUN_TEST(test_iterar_nao_visita_removidos);
    RUN_TEST(test_iterar_para_quando_callback_retorna_nonzero);
    RUN_TEST(test_iterar_em_hashfile_vazio);

    // dump
    RUN_TEST(test_dump_cria_arquivo_hfd);
    RUN_TEST(test_dump_arquivo_contem_chave_inserida);
    return UNITY_END();
}