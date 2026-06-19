#include "unity.h"
#include "dijkstra.h"
#include "mapa_viario.h"
#include <stdlib.h>

static MapaViario* m;

void setUp(void) {
    m = mapa_criar(6);
    mapa_inserir_vertice(m, "v1", 0.0, 0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 200.0, 0.0);
    mapa_inserir_vertice(m, "v4", 0.0, 100.0);
    mapa_inserir_vertice(m, "v5", 100.0, 100.0);
    mapa_inserir_vertice(m, "v6", 200.0, 100.0);

    // grafo:
    // v1 -> v2 (100m, 10m/s)
    // v2 -> v3 (100m, 2m/s)
    // v1 -> v4 (100m, 5m/s)
    // v4 -> v5 (100m, 5m/s)
    // v5 -> v6 (100m, 5m/s)
    // v2 -> v5 (100m, 5m/s)
    // v3 -> v6 (100m, 5m/s)
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 100.0, 10.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v3", "-", "-", 100.0,  2.0, "Rua_A");
    mapa_inserir_aresta(m, "v1", "v4", "-", "-", 100.0,  5.0, "Rua_B");
    mapa_inserir_aresta(m, "v4", "v5", "-", "-", 100.0,  5.0, "Rua_C");
    mapa_inserir_aresta(m, "v5", "v6", "-", "-", 100.0,  5.0, "Rua_C");
    mapa_inserir_aresta(m, "v2", "v5", "-", "-", 100.0,  5.0, "Rua_D");
    mapa_inserir_aresta(m, "v3", "v6", "-", "-", 100.0,  5.0, "Rua_E");
}

void tearDown(void) {
    mapa_fechar(m);
    m = NULL;
}

void test_dijkstra_retorna_nao_nulo(void) {
    const Vertice* origem = mapa_buscar_vertice(m, "v1");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    TEST_ASSERT_NOT_NULL(c);
    caminho_fechar(c);
}

void test_dijkstra_origem_nula_retorna_nulo(void) {
    Caminho* c = dijkstra(m, NULL, DISTANCIA);
    TEST_ASSERT_NULL(c);
}

void test_dijkstra_mapa_nulo_retorna_nulo(void) {
    Caminho* c = dijkstra(NULL, mapa_buscar_vertice(m, "v1"), DISTANCIA);
    TEST_ASSERT_NULL(c);
}

void test_custo_origem_e_zero(void) {
    const Vertice* origem = mapa_buscar_vertice(m, "v1");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    double custo = caminho_custo(c, origem);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, custo);
    caminho_fechar(c);
}

void test_custo_distancia_v1_v3(void) {
    // v1->v2->v3 = 200m (mais curto)
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    double custo = caminho_custo(c, destino);
    TEST_ASSERT_EQUAL_DOUBLE(200.0, custo);
    caminho_fechar(c);
}

void test_custo_tempo_v1_v3(void) {
    // v1->v2->v3 = 60s (caminho mais rapido)
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, TEMPO);
    double custo = caminho_custo(c, destino);
    TEST_ASSERT_EQUAL_DOUBLE(60.0, custo);
    caminho_fechar(c);
}

void test_custo_tempo_v1_v6(void) {
    // v1->v2->v5->v6 = 50s (caminho mais rapido)
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v6");
    Caminho* c = dijkstra(m, origem, TEMPO);
    double custo = caminho_custo(c, destino);
    TEST_ASSERT_EQUAL_DOUBLE(50.0, custo);
    caminho_fechar(c);
}

void test_custo_inacessivel_retorna_menos_um(void) {
    // v6 nao tem arestas saindo (inacessivel a partir dele)
    const Vertice* origem  = mapa_buscar_vertice(m, "v6");
    const Vertice* destino = mapa_buscar_vertice(m, "v1");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    double custo = caminho_custo(c, destino);
    TEST_ASSERT_EQUAL_DOUBLE(-1.0, custo);
    caminho_fechar(c);
}

void test_reconstruir_destino_inacessivel_retorna_nulo(void) {
    const Vertice* origem  = mapa_buscar_vertice(m, "v6");
    const Vertice* destino = mapa_buscar_vertice(m, "v1");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    TEST_ASSERT_NULL(p);
    caminho_fechar(c);
}

void test_reconstruir_origem_tem_aresta_nula(void) {
    // primeiro passo deve ter aresta NULL
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_NULL(passo_get_aresta(p));
    caminho_lista_fechar(p);
    caminho_fechar(c);
}

void test_reconstruir_primeiro_vertice_e_origem(void) {
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_STRING("v1", vertice_get_id(passo_get_vertice(p)));
    caminho_lista_fechar(p);
    caminho_fechar(c);
}

void test_reconstruir_ultimo_vertice_e_destino(void) {
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    TEST_ASSERT_NOT_NULL(p);

    // percorre até o fim
    const PassoCaminho* atual = p;
    while(passo_get_prox(atual) != NULL)
        atual = passo_get_prox(atual);

    TEST_ASSERT_EQUAL_STRING("v3", vertice_get_id(passo_get_vertice(atual)));
    caminho_lista_fechar(p);
    caminho_fechar(c);
}

void test_reconstruir_caminho_distancia_v1_v6(void) {
    // v1->v2->v5->v6 ou v1->v4->v5->v6 ou v1->v2->v3->v6 = 300m
    const Vertice* origem  = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v6");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    double custo = caminho_custo(c, destino);
    TEST_ASSERT_EQUAL_DOUBLE(300.0, custo);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    TEST_ASSERT_NOT_NULL(p);
    caminho_lista_fechar(p);
    caminho_fechar(c);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_dijkstra_retorna_nao_nulo);
    RUN_TEST(test_dijkstra_origem_nula_retorna_nulo);
    RUN_TEST(test_dijkstra_mapa_nulo_retorna_nulo);
    RUN_TEST(test_custo_origem_e_zero);
    RUN_TEST(test_custo_distancia_v1_v3);
    RUN_TEST(test_custo_tempo_v1_v3);
    RUN_TEST(test_custo_tempo_v1_v6);
    RUN_TEST(test_custo_inacessivel_retorna_menos_um);
    RUN_TEST(test_reconstruir_destino_inacessivel_retorna_nulo);
    RUN_TEST(test_reconstruir_origem_tem_aresta_nula);
    RUN_TEST(test_reconstruir_primeiro_vertice_e_origem);
    RUN_TEST(test_reconstruir_ultimo_vertice_e_destino);
    RUN_TEST(test_reconstruir_caminho_distancia_v1_v6);

    return UNITY_END();
}