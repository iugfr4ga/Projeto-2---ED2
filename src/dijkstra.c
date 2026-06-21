#include "dijkstra.h"
#include "mapa_viario.h"
#include <stdlib.h>
#include <float.h>

struct Caminho {
    const MapaViario* m;
    double* dist;
    int* anterior;
    const Aresta** aresta_anterior;
    int nv;
    Criterio criterio;
};

struct PassoCaminho {
    const Vertice* vertice;
    const Aresta* aresta;
    struct PassoCaminho* prox;
};

Caminho* dijkstra(const MapaViario* m, const Vertice* origem, Criterio criterio) {
    if(m == NULL || origem == NULL)
        return NULL;

    int nv = mapa_get_n_inseridos(m);
    if(nv <= 0)
        return NULL;

    Caminho* c = malloc(sizeof(Caminho));
    if(c == NULL)
        return NULL;

    c->m = m;
    c->nv = nv;
    c->criterio = criterio;
    c->dist = malloc(nv * sizeof(double));
    c->anterior = malloc(nv * sizeof(int));
    c->aresta_anterior = malloc(nv * sizeof(Aresta*));

    if(c->dist == NULL || c->anterior == NULL || c->aresta_anterior == NULL) {
        caminho_fechar(c);
        return NULL;
    }

    int* visitado = calloc(nv, sizeof(int));
    if(visitado == NULL) {
        caminho_fechar(c);
        return NULL;
    }

    for(int i = 0; i < nv; i++) {
        c->dist[i] = DBL_MAX;
        c->anterior[i] = -1;
        c->aresta_anterior[i] = NULL;
    }

    int orig = vertice_get_indice(origem);
    c->dist[orig] = 0.0;

    for(int j = 0; j < nv; j++) {
        // busca vertice não visitado com menor custo
        // depois fazer fila de prioridade se necessario
        int u = -1;
        for(int i = 0; i < nv; i++) {
            if(!visitado[i] && (u == -1 || c->dist[i] < c->dist[u]))
                u = i;
        }

        if(u == -1 || c->dist[u] == DBL_MAX)
            break;

        visitado[u] = 1;

        const Vertice* vu = mapa_get_vertice_por_indice(m, u);
        for(const Aresta* a = vertice_get_arestas(vu); a != NULL; a = aresta_get_prox(a)) {
            const Vertice* dest = aresta_get_destino(a);
            int v = vertice_get_indice(dest);

            double peso = (criterio == DISTANCIA) ? aresta_get_cmp(a) : aresta_get_cmp(a) / aresta_get_vm(a);

            // relaxamento
            if(c->dist[v] > c->dist[u] + peso) {
                c->dist[v] = c->dist[u] + peso;
                c->anterior[v] = u;
                c->aresta_anterior[v] = a;
            }
        }
    }
    free(visitado);
    return c;
}

PassoCaminho* caminho_reconstruir(const Caminho* c, const Vertice* destino) {
    if(c == NULL || destino == NULL)
        return NULL;

    int d = vertice_get_indice(destino);
    if(c->dist[d] == DBL_MAX)
        return NULL;

    PassoCaminho* lista = NULL;
    int v = d;

    while(v != -1) {
        PassoCaminho* p = malloc(sizeof(struct PassoCaminho));
        if(p == NULL) {
            caminho_lista_fechar(lista);
            return NULL;
        }

        p->vertice = mapa_get_vertice_por_indice(c->m, v);
        p->aresta = c->aresta_anterior[v];
        p->prox = lista;
        lista = p;

        v = c->anterior[v];
    }
    return lista;
}

double caminho_custo(const Caminho* c, const Vertice* destino) {
    if(c == NULL || destino == NULL)
        return -1.0;
    int d = vertice_get_indice(destino);
    if(c->dist[d] == DBL_MAX)
        return -1.0;
    return c->dist[d];
}

void caminho_fechar(Caminho* c) {
    if(c == NULL)
        return;
    free(c->dist);
    free(c->anterior);
    free(c->aresta_anterior);
    free(c);
}

const Vertice* passo_get_vertice(const PassoCaminho* p) {
    return p ? p->vertice : NULL;
}

const Aresta* passo_get_aresta(const PassoCaminho* p) {
    return p ? p->aresta : NULL;
}

const PassoCaminho* passo_get_prox(const PassoCaminho* p) {
    return p ? p->prox : NULL;
}

void caminho_lista_fechar(PassoCaminho* p) {
    while(p != NULL) {
        PassoCaminho* prox = p->prox;
        free(p);
        p = prox;
    }
}