#include "unity/unity.h"
#include "../fila_prioridade.h"
#include <stdlib.h>

static FilaPrioridade* f;

void setUp(void) {
    f = fila_prioridade_criar();
}

void tearDown(void) {
    fila_prioridade_fechar(f);
    f = NULL;
}

void test_criar_retorna_nao_nulo(void) {
    TEST_ASSERT_NOT_NULL(f);
}

void test_criar_fila_vazia(void) {
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_vazia(f));
}

void test_criar_tamanho_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_tamanho(f));
}

void test_inserir_retorna_sucesso(void) {
    int r = fila_prioridade_inserir(f, 1.0, 42);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_inserir_nulo_retorna_erro(void) {
    int r = fila_prioridade_inserir(NULL, 1.0, 42);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_inserir_aumenta_tamanho(void) {
    fila_prioridade_inserir(f, 1.0, 1);
    fila_prioridade_inserir(f, 2.0, 2);
    TEST_ASSERT_EQUAL_INT(2, fila_prioridade_tamanho(f));
}

void test_inserir_fila_nao_vazia(void) {
    fila_prioridade_inserir(f, 1.0, 1);
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_vazia(f));
}

void test_remover_min_unico_elemento(void) {
    fila_prioridade_inserir(f, 5.0, 99);
    int v = fila_prioridade_remover(f);
    TEST_ASSERT_EQUAL_INT(99, v);
}

void test_remover_min_fila_vazia_retorna_menos_um(void) {
    int v = fila_prioridade_remover(f);
    TEST_ASSERT_EQUAL_INT(-1, v);
}

void test_remover_min_nulo_retorna_menos_um(void) {
    int v = fila_prioridade_remover(NULL);
    TEST_ASSERT_EQUAL_INT(-1, v);
}

void test_remover_min_diminui_tamanho(void) {
    fila_prioridade_inserir(f, 1.0, 1);
    fila_prioridade_inserir(f, 2.0, 2);
    fila_prioridade_remover(f);
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_tamanho(f));
}

void test_remover_min_esvazia_fila(void) {
    fila_prioridade_inserir(f, 1.0, 1);
    fila_prioridade_remover(f);
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_vazia(f));
}

void test_ordem_crescente(void) {
    // insere fora de ordem, deve sair em ordem crescente de prioridade
    fila_prioridade_inserir(f, 3.0, 30);
    fila_prioridade_inserir(f, 1.0, 10);
    fila_prioridade_inserir(f, 2.0, 20);

    TEST_ASSERT_EQUAL_INT(10, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(20, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(30, fila_prioridade_remover(f));
}

void test_ordem_insercao_ja_ordenada(void) {
    fila_prioridade_inserir(f, 1.0, 10);
    fila_prioridade_inserir(f, 2.0, 20);
    fila_prioridade_inserir(f, 3.0, 30);

    TEST_ASSERT_EQUAL_INT(10, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(20, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(30, fila_prioridade_remover(f));
}

void test_ordem_insercao_reversa(void) {
    fila_prioridade_inserir(f, 3.0, 30);
    fila_prioridade_inserir(f, 2.0, 20);
    fila_prioridade_inserir(f, 1.0, 10);

    TEST_ASSERT_EQUAL_INT(10, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(20, fila_prioridade_remover(f));
    TEST_ASSERT_EQUAL_INT(30, fila_prioridade_remover(f));
}

void test_vazio_nulo_retorna_um(void) {
    TEST_ASSERT_EQUAL_INT(1, fila_prioridade_vazia(NULL));
}

void test_tamanho_nulo_retorna_zero(void) {
    TEST_ASSERT_EQUAL_INT(0, fila_prioridade_tamanho(NULL));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_criar_retorna_nao_nulo);
    RUN_TEST(test_criar_fila_vazia);
    RUN_TEST(test_criar_tamanho_zero);
    RUN_TEST(test_inserir_retorna_sucesso);
    RUN_TEST(test_inserir_nulo_retorna_erro);
    RUN_TEST(test_inserir_aumenta_tamanho);
    RUN_TEST(test_inserir_fila_nao_vazia);
    RUN_TEST(test_remover_min_unico_elemento);
    RUN_TEST(test_remover_min_fila_vazia_retorna_menos_um);
    RUN_TEST(test_remover_min_nulo_retorna_menos_um);
    RUN_TEST(test_remover_min_diminui_tamanho);
    RUN_TEST(test_remover_min_esvazia_fila);
    RUN_TEST(test_ordem_crescente);
    RUN_TEST(test_ordem_insercao_ja_ordenada);
    RUN_TEST(test_ordem_insercao_reversa);
    RUN_TEST(test_vazio_nulo_retorna_um);
    RUN_TEST(test_tamanho_nulo_retorna_zero);

    return UNITY_END();
}