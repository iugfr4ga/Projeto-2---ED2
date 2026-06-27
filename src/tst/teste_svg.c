#include "unity/unity.h"
#include "../svg.h"
#include "../mapa_viario.h"
#include "../comp_conexos.h"
#include "../dijkstra.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CAMINHO_SVG "teste_svg.svg"

static MapaViario* m;

static void ler_svg(char* buf, int tam) {
    memset(buf, 0, tam);
    FILE* f = fopen(CAMINHO_SVG, "r");
    if(f == NULL) 
        return;
    fread(buf, 1, tam - 1, f);
    fclose(f);
}

void setUp(void) {
    remove(CAMINHO_SVG);
    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);

    m = mapa_criar(4);
    mapa_inserir_vertice(m, "v1", 0.0, 0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 100.0, 100.0);
    mapa_inserir_vertice(m, "v4", 0.0, 100.0);
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 100.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v3", "-", "-", 100.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v3", "v4", "-", "-", 100.0, 5.0, "Rua_C");
    mapa_inserir_aresta(m, "v4", "v1", "-", "-", 100.0, 2.0, "Rua_D");
}

void tearDown(void) {
    svg_finalizar();
    mapa_fechar(m);
    m = NULL;
    remove(CAMINHO_SVG);
}

void test_svg_inicializar_valido(void) {
    TEST_PASS();
}

void test_svg_contem_tag_svg(void) {
    svg_finalizar();
    char buf[512];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<svg"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "</svg>"));
    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_quadra_rect(void) {
    svg_desenhar_quadra("cep01", 10.0, 20.0, 100.0, 60.0, "orange", "black", "1.0");
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<rect"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "orange"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "black"));

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_quadra_cep(void) {
    svg_desenhar_quadra("cep01", 10.0, 20.0, 100.0, 60.0, "orange", "black", "1.0");
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "cep01"));

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_registrador_line(void) {
    svg_desenhar_registrador(50.0, 0.0,3);
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<line"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "stroke=\"red\""));

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_registrador_numero(void) {
    svg_desenhar_registrador(50.0, 0.0, 7);
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "R7"));

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_componentes_rect(void) {
    Componentes* c = componentes_calcular(m, 3.0);
    svg_desenhar_componentes(c);
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<rect"));
    componentes_fechar(c);

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_aresta_line(void) {
    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    const Aresta* a = vertice_get_arestas(v1);
    svg_desenhar_aresta(v1, a);
    svg_finalizar();

    char buf[1024];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<line"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "stroke=\"red\""));

    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_percurso_path(void) {
    const Vertice* origem = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    svg_desenhar_percurso(p, "blue", 0.0, 0.0, 100.0, 100.0, 5.0);
    svg_finalizar();

    char buf[2048];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "<path"));

    caminho_lista_fechar(p);
    caminho_fechar(c);
    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

void test_svg_desenhar_percurso_animate(void) {
    const Vertice* origem = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    svg_desenhar_percurso(p, "red", 0.0, 0.0, 100.0, 100.0, 5.0);
    svg_finalizar();

    char buf[2048];
    ler_svg(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "animateMotion"));

    caminho_lista_fechar(p);
    caminho_fechar(c);
    svg_inicializar(CAMINHO_SVG, 500.0, 500.0);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_svg_inicializar_valido);
    RUN_TEST(test_svg_contem_tag_svg);
    RUN_TEST(test_svg_desenhar_quadra_rect);
    RUN_TEST(test_svg_desenhar_quadra_cep);
    RUN_TEST(test_svg_desenhar_registrador_line);
    RUN_TEST(test_svg_desenhar_registrador_numero);
    RUN_TEST(test_svg_desenhar_componentes_rect);
    RUN_TEST(test_svg_desenhar_aresta_line);
    RUN_TEST(test_svg_desenhar_percurso_path);
    RUN_TEST(test_svg_desenhar_percurso_animate);

    return UNITY_END();
}