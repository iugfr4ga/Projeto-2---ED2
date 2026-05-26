#include "unity/unity.h"
#include "../cidade.h"
#include <string.h>
#include <stdio.h>

#define CAMINHO_HF_QUADRAS "teste_quadras.hf"
#define CAMINHO_HFD_QUADRAS "teste_quadras.hfd"

void setUp(void) {
    remove(CAMINHO_HF_QUADRAS);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cidade_inicializar(CAMINHO_HF_QUADRAS), "cidade_inicializar falhou.");
}

void tearDown(void) {
    cidade_finalizar(CAMINHO_HFD_QUADRAS);
    remove(CAMINHO_HF_QUADRAS);
    remove(CAMINHO_HFD_QUADRAS);
}

void test_cidade_inicializar(void) {
    // ja inicializado no setUp, só verifica que não crashou
    TEST_PASS();
}

void test_cidade_inserir_quadra(void) {
    int ret = cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_cidade_inserir_quadra_duplicada_retorna_erro(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    int ret = cidade_inserir_quadra("cep01", 10, 10, 50, 30, "blue", "red", "2.0");
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_cidade_buscar_quadra_existente(void) {
    cidade_inserir_quadra("cep01", 10, 20, 100, 60, "orange", "black", "1.0");
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_DOUBLE(10, quadra_get_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20, quadra_get_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(100, quadra_get_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(60, quadra_get_h(q));
    TEST_ASSERT_EQUAL_STRING("orange", quadra_get_cfill(q));
    TEST_ASSERT_EQUAL_STRING("black", quadra_get_cstrk(q));
    TEST_ASSERT_EQUAL_STRING("1.0", quadra_get_sw(q));
}

void test_cidade_buscar_quadra_inexistente_retorna_null(void) {
    const Quadra* q = cidade_buscar_quadra("nao_existe");
    TEST_ASSERT_NULL(q);
}

void test_cidade_remover_quadra_existente(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    int ret = cidade_remover_quadra("cep01");
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_cidade_remover_quadra_inexistente_retorna_erro(void) {
    int ret = cidade_remover_quadra("nao_existe");
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_cidade_buscar_apos_remover_retorna_null(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    cidade_remover_quadra("cep01");
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NULL(q);
}

void test_cidade_coordenadas_face_s(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    double px, py;
    int ret = cidade_coordenadas("cep01", 'S', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_DOUBLE(10, px);   
    TEST_ASSERT_EQUAL_DOUBLE(1, py); 
}

void test_cidade_coordenadas_face_n(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    double px, py;
    cidade_coordenadas("cep01", 'N', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_DOUBLE(10, px);   
    TEST_ASSERT_EQUAL_DOUBLE(59, py);   
}

void test_cidade_coordenadas_face_o(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    double px, py;
    cidade_coordenadas("cep01", 'O', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_DOUBLE(99, px);   
    TEST_ASSERT_EQUAL_DOUBLE(10, py); 
}

void test_cidade_coordenadas_face_l(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    double px, py;
    cidade_coordenadas("cep01", 'L', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_DOUBLE(1, px); 
    TEST_ASSERT_EQUAL_DOUBLE(10, py);  
}

void test_cidade_coordenadas_quadra_inexistente_retorna_erro(void) {
    double px, py;
    int ret = cidade_coordenadas("nao_existe", 'S', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_cidade_coordenadas_face_invalida_retorna_erro(void) {
    cidade_inserir_quadra("cep01", 0, 0, 100, 60, "orange", "black", "1.0");
    double px, py;
    int ret = cidade_coordenadas("cep01", 'X', 10, "1.0", &px, &py);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

void test_cidade_persistencia_apos_reabrir(void) {
    cidade_inserir_quadra("cep01", 10, 20, 100, 60, "orange", "black", "1.0");
    cidade_finalizar(CAMINHO_HFD_QUADRAS);
    TEST_ASSERT_EQUAL_INT(0, cidade_inicializar(CAMINHO_HF_QUADRAS));
    const Quadra* q = cidade_buscar_quadra("cep01");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_DOUBLE(10, quadra_get_x(q));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_cidade_inicializar);
    RUN_TEST(test_cidade_inserir_quadra);
    RUN_TEST(test_cidade_inserir_quadra_duplicada_retorna_erro);
    RUN_TEST(test_cidade_buscar_quadra_existente);
    RUN_TEST(test_cidade_buscar_quadra_inexistente_retorna_null);
    RUN_TEST(test_cidade_remover_quadra_existente);
    RUN_TEST(test_cidade_remover_quadra_inexistente_retorna_erro);
    RUN_TEST(test_cidade_buscar_apos_remover_retorna_null);
    RUN_TEST(test_cidade_coordenadas_face_s);
    RUN_TEST(test_cidade_coordenadas_face_n);
    RUN_TEST(test_cidade_coordenadas_face_o);
    RUN_TEST(test_cidade_coordenadas_face_l);
    RUN_TEST(test_cidade_coordenadas_quadra_inexistente_retorna_erro);
    RUN_TEST(test_cidade_coordenadas_face_invalida_retorna_erro);
    RUN_TEST(test_cidade_persistencia_apos_reabrir);

    return UNITY_END();
}