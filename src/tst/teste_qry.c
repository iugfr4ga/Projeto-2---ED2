#include "unity.h"
#include "../qry.h"
#include "mapa_viario.h"
#include "via.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VIA_TEST "/tmp/test_qry.via"
#define QRY_TEST "/tmp/test_qry.qry"
#define TXT_TEST "/tmp/test_qry.txt"

static MapaViario* m;

static void criar_arquivo(const char* caminho, const char* conteudo) {
    FILE* f = fopen(caminho, "w");
    fprintf(f, "%s", conteudo);
    fclose(f);
}

static void remover_arquivos(void) {
    remove(VIA_TEST);
    remove(QRY_TEST);
    remove(TXT_TEST);
}

static char* ler_txt(void) {
    FILE* f = fopen(TXT_TEST, "r");
    if(f == NULL) return NULL;

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    rewind(f);

    char* buf = malloc(tam + 1);
    fread(buf, 1, tam, f);
    buf[tam] = '\0';
    fclose(f);
    return buf;
}

void setUp(void) {
    criar_arquivo(VIA_TEST,
        "4\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "v v3 10.0 110.0\n"
        "v v4 110.0 110.0\n"
        "e v1 v2 cep1 - 100.0 4.0 Rua_A\n"
        "e v2 v1 - cep1 100.0 4.0 Rua_A\n"
        "e v1 v3 cep2 - 100.0 4.0 Rua_B\n"
        "e v3 v4 cep3 - 100.0 4.0 Rua_C\n"
        "e v4 v2 cep4 - 100.0 4.0 Rua_D\n"
    );
    m = via_processar(VIA_TEST);
    txt_inicializar(TXT_TEST);
}

void tearDown(void) {
    txt_finalizar();
    mapa_destruir(m);
    m = NULL;
    remover_arquivos();
}

void test_qry_processar_caminho_nulo_retorna_erro(void) {
    int r = qry_processar(NULL, m);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_qry_processar_mapa_nulo_retorna_erro(void) {
    criar_arquivo(QRY_TEST, "mvm 2.0 0.0 0.0 200.0 200.0\n");
    int r = qry_processar(QRY_TEST, NULL);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_qry_processar_arquivo_inexistente_retorna_erro(void) {
    int r = qry_processar("/tmp/nao_existe.qry", m);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_qry_processar_arquivo_vazio_retorna_sucesso(void) {
    criar_arquivo(QRY_TEST, "");
    int r = qry_processar(QRY_TEST, m);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_qry_mvm_retorna_sucesso(void) {
    criar_arquivo(QRY_TEST, "mvm 2.0 0.0 0.0 200.0 200.0\n");
    int r = qry_processar(QRY_TEST, m);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_qry_mvm_reporta_arestas_atualizadas(void) {
    criar_arquivo(QRY_TEST, "mvm 2.0 0.0 0.0 200.0 200.0\n");
    qry_processar(QRY_TEST, m);
    txt_finalizar();

    char* saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    TEST_ASSERT_NOT_NULL(strstr(saida, "mvm:"));
    free(saida);

    txt_inicializar(TXT_TEST);
}

void test_qry_mvm_regiao_sem_arestas_reporta_zero(void) {
    criar_arquivo(QRY_TEST, "mvm 2.0 9000.0 9000.0 100.0 100.0\n");
    qry_processar(QRY_TEST, m);
    txt_finalizar();

    char* saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    TEST_ASSERT_NOT_NULL(strstr(saida, "0 arestas"));
    free(saida);

    txt_inicializar(TXT_TEST);
}

void test_qry_geo_registrador_invalido_ignorado(void) {
    criar_arquivo(QRY_TEST, "@o? R99 cep1 S 10\n");
    int r = qry_processar(QRY_TEST, m);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_qry_geo_cep_inexistente_reporta_erro(void) {
    criar_arquivo(QRY_TEST, "@o? R0 cep_nao_existe S 10\n");
    qry_processar(QRY_TEST, m);
    txt_finalizar();

    char* saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    TEST_ASSERT_NOT_NULL(strstr(saida, "não encontrado"));
    free(saida);

    txt_inicializar(TXT_TEST);
}

void test_qry_percurso_registrador_nao_preenchido_reporta_erro(void) {
    criar_arquivo(QRY_TEST, "p? R0 R1 blue red\n");
    qry_processar(QRY_TEST, m);
    txt_finalizar();

    char* saida = ler_txt();
    TEST_ASSERT_NOT_NULL(saida);
    TEST_ASSERT_NOT_NULL(strstr(saida, "não preenchido"));
    free(saida);

    txt_inicializar(TXT_TEST);
}

void test_qry_multiplos_comandos_retorna_sucesso(void) {
    criar_arquivo(QRY_TEST,
        "mvm 2.0 0.0 0.0 200.0 200.0\n"
        "mvm 5.0 0.0 0.0 50.0 50.0\n"
    );
    int r = qry_processar(QRY_TEST, m);
    TEST_ASSERT_EQUAL_INT(0, r);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_qry_processar_caminho_nulo_retorna_erro);
    RUN_TEST(test_qry_processar_mapa_nulo_retorna_erro);
    RUN_TEST(test_qry_processar_arquivo_inexistente_retorna_erro);
    RUN_TEST(test_qry_processar_arquivo_vazio_retorna_sucesso);
    RUN_TEST(test_qry_mvm_retorna_sucesso);
    RUN_TEST(test_qry_mvm_reporta_arestas_atualizadas);
    RUN_TEST(test_qry_mvm_regiao_sem_arestas_reporta_zero);
    RUN_TEST(test_qry_geo_registrador_invalido_ignorado);
    RUN_TEST(test_qry_geo_cep_inexistente_reporta_erro);
    RUN_TEST(test_qry_percurso_registrador_nao_preenchido_reporta_erro);
    RUN_TEST(test_qry_multiplos_comandos_retorna_sucesso);

    return UNITY_END();
}