#include "unity.h"
#include "agm.h"
#include "mapa_viario.h"
#include <stdlib.h>

static MapaViario* m;

void setUp(void) {
    m = mapa_criar(4);
    //  v1 --10-- v2
    //  |         |
    //  5         8
    //  |         |
    //  v3 --3--- v4
    //
    // AGM: v3-v4 (3), v1-v3 (5), v2-v4 (8) — total 16
    // aresta v1-v2 (10) fica de fora
    mapa_inserir_vertice(m, "v1", 0.0,   0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 0.0,   100.0);
    mapa_inserir_vertice(m, "v4", 100.0, 100.0);

    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 10.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v1", "-", "-", 10.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v1", "v3", "-", "-",  5.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v3", "v1", "-", "-",  5.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v2", "v4", "-", "-",  8.0, 5.0, "Rua_C");
    mapa_inserir_aresta(m, "v4", "v2", "-", "-",  8.0, 5.0, "Rua_C");
    mapa_inserir_aresta(m, "v3", "v4", "-", "-",  3.0, 2.0, "Rua_D");
    mapa_inserir_aresta(m, "v4", "v3", "-", "-",  3.0, 2.0, "Rua_D");
}

void tearDown(void) {
    mapa_fechar(m);
    m = NULL;
}

void test_agm_calcular_retorna_nao_nulo(void) {
    AGM* a = agm_calcular(m);
    TEST_ASSERT_NOT_NULL(a);
    agm_fechar(a);
}

void test_agm_calcular_mapa_nulo_retorna_nulo(void) {
    AGM* a = agm_calcular(NULL);
    TEST_ASSERT_NULL(a);
}

void test_agm_n_arestas_correto(void) {
    // AGM tem nv - 1 arestas
    AGM* a = agm_calcular(m);
    TEST_ASSERT_EQUAL_INT(3, agm_get_n_arestas(a));
    agm_fechar(a);
}

void test_agm_contem_aresta_leve(void) {
    // v3->v4 tem cmp=3 e deve estar na AGM
    AGM* a = agm_calcular(m);
    const Vertice* v3 = mapa_buscar_vertice(m, "v3");
    const Aresta* aresta = NULL;
    for(const Aresta* ar = vertice_get_arestas(v3); ar != NULL; ar = aresta_get_prox(ar)) {
        if(aresta_get_cmp(ar) == 3.0) {
            aresta = ar;
            break;
        }
    }
    TEST_ASSERT_NOT_NULL(aresta);
    TEST_ASSERT_EQUAL_INT(1, agm_contem_aresta(a, aresta));
    agm_fechar(a);
}

void test_agm_nao_contem_aresta_pesada(void) {
    // v1->v2 tem cmp=10 e não deve estar na AGM
    AGM* a = agm_calcular(m);
    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    const Aresta* aresta = NULL;
    for(const Aresta* ar = vertice_get_arestas(v1); ar != NULL; ar = aresta_get_prox(ar)) {
        if(aresta_get_cmp(ar) == 10.0) {
            aresta = ar;
            break;
        }
    }
    TEST_ASSERT_NOT_NULL(aresta);
    TEST_ASSERT_EQUAL_INT(0, agm_contem_aresta(a, aresta));
    agm_fechar(a);
}

void test_agm_contem_aresta_nula_retorna_zero(void) {
    AGM* a = agm_calcular(m);
    TEST_ASSERT_EQUAL_INT(0, agm_contem_aresta(a, NULL));
    agm_fechar(a);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_agm_calcular_retorna_nao_nulo);
    RUN_TEST(test_agm_calcular_mapa_nulo_retorna_nulo);
    RUN_TEST(test_agm_n_arestas_correto);
    RUN_TEST(test_agm_contem_aresta_leve);
    RUN_TEST(test_agm_nao_contem_aresta_pesada);
    RUN_TEST(test_agm_contem_aresta_nula_retorna_zero);

    return UNITY_END();
}