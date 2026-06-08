#include "unity.h"
#include "mapa_viario.h"
#include <stdlib.h>

static MapaViario* m;

void setUp(void) {
    m = mapa_criar();
}

void tearDown(void) {
    mapa_fechar(m);
    m = NULL;
}

void test_mapa_criar_retorna_nao_nulo(void) {
    TEST_ASSERT_NOT_NULL(m);
}

void test_inserir_vertice_sucesso(void) {
    int r = mapa_inserir_vertice(m, "v1", 10.0, 20.0);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_inserir_vertice_id_duplicado_retorna_erro(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 20.0);
    int r = mapa_inserir_vertice(m, "v1", 30.0, 40.0);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_inserir_aresta_sucesso(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    mapa_inserir_vertice(m, "v2", 110.0, 10.0);
    int r = mapa_inserir_aresta(m, "v1", "v2", "cep1", "-", 100.0, 4.0, "Rua_A");
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_inserir_aresta_origem_inexistente_retorna_erro(void) {
    mapa_inserir_vertice(m, "v2", 110.0, 10.0);
    int r = mapa_inserir_aresta(m, "v_nao_existe", "v2", "-", "-", 100.0, 4.0, "Rua_A");
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_inserir_aresta_destino_inexistente_retorna_erro(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    int r = mapa_inserir_aresta(m, "v1", "v_nao_existe", "-", "-", 100.0, 4.0, "Rua_A");
    TEST_ASSERT_EQUAL_INT(-1, r);
}

void test_inserir_aresta_sem_quadras_adjacentes(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    mapa_inserir_vertice(m, "v2", 110.0, 10.0);
    int r = mapa_inserir_aresta(m, "v1", "v2", "-", "-", 50.0, 3.5, "Rua_B");
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_buscar_vertice_existente(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 20.0);
    const Vertice* v = mapa_buscar_vertice(m, "v1");
    TEST_ASSERT_NOT_NULL(v);
}

void test_buscar_vertice_inexistente_retorna_nulo(void) {
    const Vertice* v = mapa_buscar_vertice(m, "nao_existe");
    TEST_ASSERT_NULL(v);
}

void test_vertice_getters(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 20.0);
    const Vertice* v = mapa_buscar_vertice(m, "v1");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(v));
    TEST_ASSERT_EQUAL_DOUBLE(10.0, vertice_get_x(v));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, vertice_get_y(v));
}

void test_aresta_getters(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    mapa_inserir_vertice(m, "v2", 110.0, 10.0);
    mapa_inserir_aresta(m, "v1", "v2", "cep1", "cep2", 100.0, 4.0, "Av_Principal");
    const Vertice* v = mapa_buscar_vertice(m, "v1");
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(v));
}

void test_vertice_mais_proximo_unico(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    const Vertice* v = mapa_vertice_mais_proximo(m, 15.0, 15.0);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(v));
}

void test_vertice_mais_proximo_entre_varios(void) {
    mapa_inserir_vertice(m, "v1", 0.0,   0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 0.0,   100.0);
    const Vertice* v = mapa_vertice_mais_proximo(m, 5.0, 5.0);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(v));
}

void test_vertice_mais_proximo_ponto_exato(void) {
    mapa_inserir_vertice(m, "v1", 50.0, 50.0);
    mapa_inserir_vertice(m, "v2", 200.0, 200.0);
    const Vertice* v = mapa_vertice_mais_proximo(m, 50.0, 50.0);
    TEST_ASSERT_NOT_NULL(v);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(v));
}

void test_atualizar_vm_regiao_atualiza_arestas_dentro(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    mapa_inserir_vertice(m, "v2", 50.0, 10.0);
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 40.0, 3.0, "Rua_A");
    int atualizadas = mapa_atualizar_vm_regiao(m, 0.0, 0.0, 100.0, 100.0, 6.0);
    TEST_ASSERT_EQUAL_INT(1, atualizadas);
}

void test_atualizar_vm_regiao_nao_atualiza_fora(void) {
    mapa_inserir_vertice(m, "v1", 200.0, 200.0);
    mapa_inserir_vertice(m, "v2", 300.0, 200.0);
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 100.0, 3.0, "Rua_B");
    int atualizadas = mapa_atualizar_vm_regiao(m, 0.0, 0.0, 100.0, 100.0, 6.0);
    TEST_ASSERT_EQUAL_INT(0, atualizadas);
}

void test_atualizar_vm_regiao_multiplas_arestas(void) {
    mapa_inserir_vertice(m, "v1", 10.0, 10.0);
    mapa_inserir_vertice(m, "v2", 20.0, 10.0);
    mapa_inserir_vertice(m, "v3", 500.0, 500.0);
    mapa_inserir_vertice(m, "v4", 600.0, 500.0);
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 10.0, 2.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v1", "-", "-", 10.0, 2.0, "Rua_A");
    mapa_inserir_aresta(m, "v3", "v4", "-", "-", 100.0, 2.0, "Rua_B");
    int atualizadas = mapa_atualizar_vm_regiao(m, 0.0, 0.0, 100.0, 100.0, 5.0);
    TEST_ASSERT_EQUAL_INT(2, atualizadas);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_mapa_criar_retorna_nao_nulo);
    RUN_TEST(test_inserir_vertice_sucesso);
    RUN_TEST(test_inserir_vertice_id_duplicado_retorna_erro);
    RUN_TEST(test_inserir_aresta_sucesso);
    RUN_TEST(test_inserir_aresta_origem_inexistente_retorna_erro);
    RUN_TEST(test_inserir_aresta_destino_inexistente_retorna_erro);
    RUN_TEST(test_inserir_aresta_sem_quadras_adjacentes);
    RUN_TEST(test_buscar_vertice_existente);
    RUN_TEST(test_buscar_vertice_inexistente_retorna_nulo);
    RUN_TEST(test_vertice_getters);
    RUN_TEST(test_aresta_getters);
    RUN_TEST(test_vertice_mais_proximo_unico);
    RUN_TEST(test_vertice_mais_proximo_entre_varios);
    RUN_TEST(test_vertice_mais_proximo_ponto_exato);
    RUN_TEST(test_atualizar_vm_regiao_atualiza_arestas_dentro);
    RUN_TEST(test_atualizar_vm_regiao_nao_atualiza_fora);
    RUN_TEST(test_atualizar_vm_regiao_multiplas_arestas);

    return UNITY_END();
}