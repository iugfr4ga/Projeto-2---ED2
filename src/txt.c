#include "txt.h"
#include "dijkstra.h"
#include "mapa_viario.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static FILE* txt_arquivo = NULL;

int txt_inicializar(const char* caminho) {
    if(caminho == NULL) 
        return -1;
    txt_arquivo = fopen(caminho, "w");
    return txt_arquivo != NULL ? 0 : -1;
}

void txt_finalizar(void) {
    if(txt_arquivo == NULL) 
        return;
    fclose(txt_arquivo);
    txt_arquivo = NULL;
}

void txt_escrever(const char* linha) {
    if(txt_arquivo == NULL || linha == NULL) 
        return;
    fprintf(txt_arquivo, "%s", linha);
}

static const char* calcular_direcao(const Vertice* de, const Vertice* para) {
    double dx = vertice_get_x(para) - vertice_get_x(de);
    double dy = vertice_get_y(para) - vertice_get_y(de);

    if(fabs(dx) >= fabs(dy))
        return dx > 0 ? "oeste" : "leste";
    else
        return dy > 0 ? "norte" : "sul";
}

void txt_descrever_percurso(const PassoCaminho* percurso) {
    if(txt_arquivo == NULL || percurso == NULL) 
        return;

    const PassoCaminho* atual = percurso;

    // pula o primeiro passo
    if(passo_get_prox(atual) == NULL) 
        return;
        
    atual = passo_get_prox(atual);

    while(atual != NULL) {
        const Aresta* aresta = passo_get_aresta(atual);
        if(aresta == NULL) { 
            atual = passo_get_prox(atual); 
            continue; 
        }

        const char* rua_atual = aresta_get_nome(aresta);
        double dist_total = aresta_get_cmp(aresta);

        // obtem vértice anterior para calcular direção
        const PassoCaminho* anterior = percurso;
        const PassoCaminho* tmp = passo_get_prox(percurso);
        while(tmp != atual) { 
            anterior = tmp; 
            tmp = passo_get_prox(tmp); 
        }

        const Vertice* v_de = passo_get_vertice(anterior);
        const Vertice* v_para = passo_get_vertice(atual);
        const char* direcao = calcular_direcao(v_de, v_para);

        // soma a distancia enquanto a rua for a mesma
        const PassoCaminho* prox = passo_get_prox(atual);
        while(prox != NULL) {
            const Aresta* prox_aresta = passo_get_aresta(prox);
            if(prox_aresta == NULL) 
                break;

            if(strcmp(aresta_get_nome(prox_aresta), rua_atual) != 0) 
                break;
                
            dist_total += aresta_get_cmp(prox_aresta);
            atual = prox;
            prox = passo_get_prox(atual);
        }

        // proxima rua para descrever o cruzamento
        const char* prox_rua = NULL;
        if(prox != NULL && passo_get_aresta(prox) != NULL)
            prox_rua = aresta_get_nome(passo_get_aresta(prox));

        if(prox_rua != NULL)
            fprintf(txt_arquivo, "Siga na %s na direção %s por %.2fm até o cruzamento com %s.\n", rua_atual, direcao, dist_total, prox_rua);
        else
            fprintf(txt_arquivo, "Siga na %s na direção %s por %.2fm até o destino.\n", rua_atual, direcao, dist_total);

        atual = passo_get_prox(atual);
    }
}