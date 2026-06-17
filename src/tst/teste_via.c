#include "unity.h"
#include "via.h"
#include "mapa_viario.h"
#include <stdio.h>
#include <stdlib.h>

#define VIA_VALIDO "/tmp/test_valido.via"
#define VIA_VAZIO "/tmp/test_vazio.via"
#define VIA_SEM_ARESTAS "/tmp/test_sem_arestas.via"
#define VIA_INEXISTENTE "/tmp/nao_existe.via"

static void criar_arquivo(const char* caminho, const char* conteudo) {
    FILE* f = fopen(caminho, "w");
    fprintf(f, "%s", conteudo);
    fclose(f);
}

static void remover_arquivo(const char* caminho) {
    remove(caminho);
}

void setUp(void) {

}

void tearDown(void) {

}

void test_via_processar_arquivo_inexistente_retorna_nulo(void) {
    MapaViario* m = via_processar(VIA_INEXISTENTE);
    TEST_ASSERT_NULL(m);
}

void test_via_processar_caminho_nulo_retorna_nulo(void) {
    MapaViario* m = via_processar(NULL);
    TEST_ASSERT_NULL(m);
}

void test_via_processar_arquivo_vazio_retorna_nulo(void) {
    criar_arquivo(VIA_VAZIO, "");
    MapaViario* m = via_processar(VIA_VAZIO);
    TEST_ASSERT_NULL(m);
    remover_arquivo(VIA_VAZIO);
}

void test_via_processar_so_vertices(void) {
    criar_arquivo(VIA_SEM_ARESTAS,
        "3\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "v v3 210.0 10.0\n"
    );

    MapaViario* m = via_processar(VIA_SEM_ARESTAS);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_EQUAL_INT(3, mapa_num_vertices(m));

    mapa_destruir(m);
    remover_arquivo(VIA_SEM_ARESTAS);
}

void test_via_processar_retorna_mapa_nao_nulo(void) {
    criar_arquivo(VIA_VALIDO,
        "3\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "v v3 210.0 10.0\n"
        "e v1 v2 cep1 - 100.0 4.0 Rua_A\n"
        "e v2 v3 cep2 cep1 100.0 5.0 Rua_B\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_numero_correto_de_vertices(void) {
    criar_arquivo(VIA_VALIDO,
        "3\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "v v3 210.0 10.0\n"
        "e v1 v2 cep1 - 100.0 4.0 Rua_A\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_EQUAL_INT(3, mapa_num_vertices(m));

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_vertices_com_coordenadas_corretas(void) {
    criar_arquivo(VIA_VALIDO,
        "2\n"
        "v v1 10.0 20.0\n"
        "v v2 110.0 70.0\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);

    const Vertice* v1 = mapa_buscar_vertice(m, "v1");
    TEST_ASSERT_NOT_NULL(v1);
    TEST_ASSERT_EQUAL_DOUBLE(10.0,  vertice_get_x(v1));
    TEST_ASSERT_EQUAL_DOUBLE(20.0,  vertice_get_y(v1));

    const Vertice* v2 = mapa_buscar_vertice(m, "v2");
    TEST_ASSERT_NOT_NULL(v2);
    TEST_ASSERT_EQUAL_DOUBLE(110.0, vertice_get_x(v2));
    TEST_ASSERT_EQUAL_DOUBLE(70.0,  vertice_get_y(v2));

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_aresta_sem_quadras_adjacentes(void) {
    criar_arquivo(VIA_VALIDO,
        "2\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "e v1 v2 - - 100.0 3.5 Rua_Belo_Horizonte\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_aresta_com_quadras_adjacentes(void) {
    criar_arquivo(VIA_VALIDO,
        "2\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "e v1 v2 cep6 cep2 100.0 4.0 Av_Higienopolis\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_arestas_opostas(void) {
    criar_arquivo(VIA_VALIDO,
        "2\n"
        "v v1 10.0 10.0\n"
        "v v2 110.0 10.0\n"
        "e v1 v2 cep6 cep2 100.0 4.0 Av_Higienopolis\n"
        "e v2 v1 cep2 cep6 100.0 5.0 Av_10_de_Dezembro\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

void test_via_processar_aresta_vertice_inexistente_nao_quebra(void) {
    criar_arquivo(VIA_VALIDO,
        "1\n"
        "v v1 10.0 10.0\n"
        "e v1 v_nao_existe - - 100.0 4.0 Rua_A\n"
    );

    MapaViario* m = via_processar(VIA_VALIDO);
    TEST_ASSERT_NOT_NULL(m);
    TEST_ASSERT_EQUAL_INT(1, mapa_num_vertices(m));

    mapa_destruir(m);
    remover_arquivo(VIA_VALIDO);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_via_processar_arquivo_inexistente_retorna_nulo);
    RUN_TEST(test_via_processar_caminho_nulo_retorna_nulo);
    RUN_TEST(test_via_processar_arquivo_vazio_retorna_nulo);
    RUN_TEST(test_via_processar_so_vertices);
    RUN_TEST(test_via_processar_retorna_mapa_nao_nulo);
    RUN_TEST(test_via_processar_numero_correto_de_vertices);
    RUN_TEST(test_via_processar_vertices_com_coordenadas_corretas);
    RUN_TEST(test_via_processar_aresta_sem_quadras_adjacentes);
    RUN_TEST(test_via_processar_aresta_com_quadras_adjacentes);
    RUN_TEST(test_via_processar_arestas_opostas);
    RUN_TEST(test_via_processar_aresta_vertice_inexistente_nao_quebra);

    return UNITY_END();
}