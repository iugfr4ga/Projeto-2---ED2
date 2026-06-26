#include "unity/unity.h"
#include "../comp_conexos.h"
#include "../mapa_viario.h"
#include <stdlib.h>

static MapaViario* m;

void setUp(void) {
    m = mapa_criar(6);
    //  v1 - v2    v3
    //  |           |
    //  v4    v5 - v6
    mapa_inserir_vertice(m, "v1", 0.0,   0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 200.0, 0.0);
    mapa_inserir_vertice(m, "v4", 0.0,   100.0);
    mapa_inserir_vertice(m, "v5", 100.0, 100.0);
    mapa_inserir_vertice(m, "v6", 200.0, 100.0);

    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 100.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v1", "-", "-", 100.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v1", "v4", "-", "-", 100.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v4", "v1", "-", "-", 100.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v3", "v6", "-", "-", 100.0, 5.0, "Rua_C");
    mapa_inserir_aresta(m, "v6", "v3", "-", "-", 100.0, 5.0, "Rua_C");
    mapa_inserir_aresta(m, "v2", "v3", "-", "-", 100.0, 1.0, "Rua_D");
    mapa_inserir_aresta(m, "v5", "v6", "-", "-", 100.0, 1.0, "Rua_D");
}

void tearDown(void) {
    mapa_fechar(m);
    m = NULL;
}

void test_componentes_calcular_retorna_nao_nulo(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    TEST_ASSERT_NOT_NULL(c);
    componentes_fechar(c);
}

void test_componentes_calcular_mapa_nulo_retorna_nulo(void) {
    Componentes* c = componentes_calcular(NULL, 4.0);
    TEST_ASSERT_NULL(c);
}

void test_quantidade_com_vl_alta(void) {
    // vl=4.0 ignora arestas lentas -> 3 componentes
    Componentes* c = componentes_calcular(m, 4.0);
    TEST_ASSERT_EQUAL_INT(3, componentes_get_quantidade(c));
    componentes_fechar(c);
}

void test_quantidade_com_vl_baixa(void) {
    // vl=0.5 aceita todas as arestas -> 1 componente
    Componentes* c = componentes_calcular(m, 0.5);
    TEST_ASSERT_EQUAL_INT(1, componentes_get_quantidade(c));
    componentes_fechar(c);
}

void test_quantidade_sem_arestas_validas(void) {
    // vl=10.0 ignora todas as arestas -> 6 componentes
    Componentes* c = componentes_calcular(m, 10.0);
    TEST_ASSERT_EQUAL_INT(6, componentes_get_quantidade(c));
    componentes_fechar(c);
}

void test_vertices_do_mesmo_componente(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    const Vertice* v2 = mapa_buscar_vertice(m, "v2");
    const Vertice* v4 = mapa_buscar_vertice(m, "v4");
    int c1 = componentes_get_componente(c, v1);
    int c2 = componentes_get_componente(c, v2);
    int c4 = componentes_get_componente(c, v4);
    TEST_ASSERT_EQUAL_INT(c1, c2);
    TEST_ASSERT_EQUAL_INT(c1, c4);
    componentes_fechar(c);
}

void test_vertices_de_componentes_diferentes(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    const Vertice* v3 = mapa_buscar_vertice(m, "v3");
    const Vertice* v5 = mapa_buscar_vertice(m, "v5");
    int c1 = componentes_get_componente(c, v1);
    int c3 = componentes_get_componente(c, v3);
    int c5 = componentes_get_componente(c, v5);
    TEST_ASSERT_NOT_EQUAL(c1, c3);
    TEST_ASSERT_NOT_EQUAL(c1, c5);
    TEST_ASSERT_NOT_EQUAL(c3, c5);
    componentes_fechar(c);
}

void test_componente_vertice_nulo_retorna_menos_um(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    int r = componentes_get_componente(c, NULL);
    TEST_ASSERT_EQUAL_INT(-1, r);
    componentes_fechar(c);
}

void test_bounding_box_componente_v1_v2_v4(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    int k = componentes_get_componente(c, v1);
    // v1(0,0) v2(100,0) v4(0,100) -> xmin=0 ymin=0 xmax=100 ymax=100
    TEST_ASSERT_EQUAL_DOUBLE(0.0, componentes_get_xmin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, componentes_get_ymin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_xmax(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_ymax(c, k));
    componentes_fechar(c);
}

void test_bounding_box_componente_v3_v6(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    const Vertice* v3 = mapa_buscar_vertice(m, "v3");
    int k = componentes_get_componente(c, v3);
    // v3(200,0) v6(200,100) -> xmin=200 ymin=0 xmax=200 ymax=100
    TEST_ASSERT_EQUAL_DOUBLE(200.0, componentes_get_xmin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, componentes_get_ymin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(200.0, componentes_get_xmax(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_ymax(c, k));
    componentes_fechar(c);
}

void test_bounding_box_vertice_isolado(void) {
    Componentes* c = componentes_calcular(m, 4.0);
    const Vertice* v5 = mapa_buscar_vertice(m, "v5");
    int k = componentes_get_componente(c, v5);
    // v5(100,100) -> xmin=xmax=100 ymin=ymax=100
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_xmin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_ymin(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_xmax(c, k));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, componentes_get_ymax(c, k));
    componentes_fechar(c);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_componentes_calcular_retorna_nao_nulo);
    RUN_TEST(test_componentes_calcular_mapa_nulo_retorna_nulo);
    RUN_TEST(test_quantidade_com_vl_alta);
    RUN_TEST(test_quantidade_com_vl_baixa);
    RUN_TEST(test_quantidade_sem_arestas_validas);
    RUN_TEST(test_vertices_do_mesmo_componente);
    RUN_TEST(test_vertices_de_componentes_diferentes);
    RUN_TEST(test_componente_vertice_nulo_retorna_menos_um);
    RUN_TEST(test_bounding_box_componente_v1_v2_v4);
    RUN_TEST(test_bounding_box_componente_v3_v6);
    RUN_TEST(test_bounding_box_vertice_isolado);

    return UNITY_END();
}