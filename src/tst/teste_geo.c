#include "unity/unity.h"
#include "../geo.h"
#include "../cidade.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAMINHO_HF "teste_geo_quadras.hf"
#define CAMINHO_HFD "teste_geo_quadras.hfd"
#define CAMINHO_GEO "teste.geo"

// para criar arquivos .geo temporarios
static void criar_geo(const char* conteudo) {
    FILE* f = fopen(CAMINHO_GEO, "w");
    fprintf(f, "%s", conteudo);
    fclose(f);
}

void setUp(void) {
    remove(CAMINHO_HF);
    remove(CAMINHO_HFD);
    remove(CAMINHO_GEO);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cidade_inicializar(CAMINHO_HF), "cidade_inicializar falhou.");
}

void tearDown(void) {
    cidade_finalizar(CAMINHO_HFD);
    remove(CAMINHO_HF);
    remove(CAMINHO_HFD);
    remove(CAMINHO_GEO);
}

void test_geo_processar_arquivo_valido(void) {
    criar_geo("cq 1.0 orange black\nq cep01 0 0 100 60\n");
    int ret = geo_processar(CAMINHO_GEO);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_geo_processar_arquivo_inexistente(void) {
    int ret = geo_processar("nao_existe.geo");
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_geo_insere_quadra(void) {
    criar_geo("cq 1.0 orange black\nq cep01 10 20 100 60\n");
    geo_processar(CAMINHO_GEO);
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NOT_NULL(q);
}

void test_geo_coordenadas_corretas(void) {
    criar_geo("cq 1.0 orange black\nq cep01 10 20 100 60\n");
    geo_processar(CAMINHO_GEO);
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_DOUBLE(10, quadra_get_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20, quadra_get_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(100, quadra_get_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(60, quadra_get_h(q));
}

void test_geo_estilo_aplicado(void) {
    criar_geo("cq 2.5 blue red\nq cep01 0 0 100 60\n");
    geo_processar(CAMINHO_GEO);
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_cfill(q));
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_cstrk(q));
    TEST_ASSERT_EQUAL_FLOAT(2.5, atof(quadra_get_sw(q)));
}

void test_geo_multiplas_quadras(void) {
    criar_geo(
        "cq 1.0 orange black\n"
        "q cep01 0 0 100 60\n"
        "q cep02 120 0 100 60\n"
        "q cep03 240 0 100 60\n"
    );
    geo_processar(CAMINHO_GEO);

    TEST_ASSERT_NOT_NULL(cidade_buscar_quadra("cep01"));
    TEST_ASSERT_NOT_NULL(cidade_buscar_quadra("cep02"));
    TEST_ASSERT_NOT_NULL(cidade_buscar_quadra("cep03"));
}

void test_geo_arquivo_vazio(void) {
    criar_geo("");
    int ret = geo_processar(CAMINHO_GEO);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_geo_processar_arquivo_valido);
    RUN_TEST(test_geo_processar_arquivo_inexistente);
    RUN_TEST(test_geo_insere_quadra);
    RUN_TEST(test_geo_coordenadas_corretas);
    RUN_TEST(test_geo_estilo_aplicado);
    RUN_TEST(test_geo_multiplas_quadras);
    RUN_TEST(test_geo_arquivo_vazio);

    return UNITY_END();
}