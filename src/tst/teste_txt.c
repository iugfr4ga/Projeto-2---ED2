#include "unity/unity.h"
#include "../txt.h"
#include "../mapa_viario.h"
#include "../dijkstra.h"
#include <stdio.h>
#include <string.h>

#define CAMINHO_TXT "teste_txt.txt"

static MapaViario* m;

// le o conteúdo do arquivo txt gerado para verificar a saída 
static void ler_txt(char* buf, int tam) {
    memset(buf, 0, tam);
    FILE* f = fopen(CAMINHO_TXT, "r");
    if(f == NULL)
        return;
    fread(buf, 1, tam - 1, f);
    fclose(f);
}

void setUp(void) {
    remove(CAMINHO_TXT);
    txt_inicializar(CAMINHO_TXT);

    m = mapa_criar(4);
    mapa_inserir_vertice(m, "v1", 0.0, 0.0);
    mapa_inserir_vertice(m, "v2", 100.0, 0.0);
    mapa_inserir_vertice(m, "v3", 100.0, 100.0);
    mapa_inserir_vertice(m, "v4", 0.0, 100.0);
    mapa_inserir_aresta(m, "v1", "v2", "-", "-", 100.0, 5.0, "Rua_A");
    mapa_inserir_aresta(m, "v2", "v3", "-", "-", 100.0, 5.0, "Rua_B");
    mapa_inserir_aresta(m, "v3", "v4", "-", "-", 100.0, 5.0, "Rua_A");
}

void tearDown(void) {
    txt_finalizar();
    mapa_fechar(m);
    m = NULL;
    remove(CAMINHO_TXT);
}

void test_txt_inicializar_cria_arquivo(void) {
    FILE* f = fopen(CAMINHO_TXT, "r");
    TEST_ASSERT_NOT_NULL(f); 
    fclose(f);
}

void test_txt_inicializar_caminho_nulo_retorna_erro(void) {
    txt_finalizar();
    int r = txt_inicializar(NULL);
    TEST_ASSERT_EQUAL_INT(-1, r);
    txt_inicializar(CAMINHO_TXT);
}

void test_txt_escrever_conteudo(void) {
    txt_escrever("linha de teste");
    txt_finalizar();
    char buf[256];
    ler_txt(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "linha de teste"));
    txt_inicializar(CAMINHO_TXT);
}

void test_txt_escrever_multiplas_linhas(void) {
    txt_escrever("primeira linha\n");
    txt_escrever("segunda linha\n");
    txt_finalizar();
    char buf[256];
    ler_txt(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "primeira linha"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "segunda linha"));
    txt_inicializar(CAMINHO_TXT);
}

void test_txt_descrever_percurso_contem_rua(void) {
    const Vertice* origem = mapa_buscar_vertice(m, "v1");
    const Vertice* destino = mapa_buscar_vertice(m, "v3");
    Caminho* c = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* p = caminho_reconstruir(c, destino);
    txt_descrever_percurso(p);
    txt_finalizar();
    char buf[512];
    ler_txt(buf, sizeof(buf));
    TEST_ASSERT_NOT_NULL(strstr(buf, "Rua_A"));
    caminho_lista_fechar(p);
    caminho_fechar(c);
    txt_inicializar(CAMINHO_TXT);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_txt_inicializar_cria_arquivo);
    RUN_TEST(test_txt_inicializar_caminho_nulo_retorna_erro);
    RUN_TEST(test_txt_escrever_conteudo);
    RUN_TEST(test_txt_escrever_multiplas_linhas);
    RUN_TEST(test_txt_descrever_percurso_contem_rua);
    
    return UNITY_END();
}