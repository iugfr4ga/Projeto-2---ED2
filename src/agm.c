#include "agm.h"
#include "mapa_viario.h"
#include <stdlib.h>

struct AGM {
    const Aresta** arestas;
    int n_arestas;
};

static int encontrar_raiz(int* pai, int x) {
    while(pai[x] != x)
        x = pai[x] = pai[pai[x]]; // compressao de caminho
    return x;
}

static int unir_componentes(int* pai, int* rank, int x, int y) {
    int rx = encontrar_raiz(pai, x);
    int ry = encontrar_raiz(pai, y);

    if(rx == ry) 
        return 0;   // mesmo componente = ciclo
    if(rank[rx] < rank[ry])      
        pai[rx] = ry;
    else if(rank[rx] > rank[ry]) 
        pai[ry] = rx;
    else { 
        pai[ry] = rx; 
        rank[rx]++; 
    }
    return 1;
}

typedef struct {
    const Aresta* aresta;
    int origem;
    double cmp;
} ArestaOrdenada;

static int cmp_aresta(const void* a, const void* b) {
    double da = ((ArestaOrdenada*)a)->cmp;
    double db = ((ArestaOrdenada*)b)->cmp;
    return (da > db) - (da < db);
}

AGM* agm_calcular(const MapaViario* m) {
    if(m == NULL) 
        return NULL;

    int nv = mapa_get_n_inseridos(m);
    if(nv <= 0) 
        return NULL;

    // coleta todas as arestas
    int cap = nv * 4; // estimativa inicial do numero de arestas
    ArestaOrdenada* todas = malloc(cap * sizeof(ArestaOrdenada));
    if(todas == NULL) 
        return NULL;

    int n_todas = 0;
    for(int i = 0; i < nv; i++) {
        const Vertice* v = mapa_get_vertice_por_indice(m, i);
        for(const Aresta* a = vertice_get_arestas(v); a != NULL; a = aresta_get_prox(a)) {
            if(n_todas >= cap) {
                cap *= 2;
                ArestaOrdenada* tmp = realloc(todas, cap * sizeof(ArestaOrdenada));
                if(tmp == NULL) { 
                    free(todas); 
                    return NULL; 
                }
                todas = tmp;
            }
            todas[n_todas].aresta = a;
            todas[n_todas].origem = i;
            todas[n_todas].cmp = aresta_get_cmp(a);
            n_todas++;
        }
    }

    // ordena por cmp
    qsort(todas, n_todas, sizeof(ArestaOrdenada), cmp_aresta);

    // inicializa union-find
    int* pai = malloc(nv * sizeof(int));
    int* rank = calloc(nv, sizeof(int));
    if(pai == NULL || rank == NULL) {
        free(pai); 
        free(rank); 
        free(todas);
        return NULL;
    }
    for(int i = 0; i < nv; i++) 
        pai[i] = i;

    // kruskal
    AGM* agm = malloc(sizeof(AGM));
    if(agm == NULL) { 
        free(pai); 
        free(rank); 
        free(todas); 
        return NULL; 
    }

    agm->arestas = malloc((nv - 1) * sizeof(Aresta*));
    agm->n_arestas = 0;
    if(agm->arestas == NULL) {
        free(pai); 
        free(rank); 
        free(todas);
        agm_fechar(agm);
        return NULL;
    }

    for(int i = 0; i < n_todas && agm->n_arestas < nv - 1; i++) {
        int u = todas[i].origem;
        int v = vertice_get_indice(aresta_get_destino(todas[i].aresta));
        if(unir_componentes(pai, rank, u, v))
            agm->arestas[agm->n_arestas++] = todas[i].aresta;
    }

    free(pai);
    free(rank);
    free(todas);
    return agm;
}

int agm_get_n_arestas(const AGM* a) {
    return a ? a->n_arestas : 0;
}

int agm_contem_aresta(const AGM* a, const Aresta* aresta) {
    if(a == NULL || aresta == NULL) 
        return 0;
    for(int i = 0; i < a->n_arestas; i++)
        if(a->arestas[i] == aresta) 
            return 1;
    return 0;
}

void agm_fechar(AGM* a) {
    if(a == NULL) 
        return;
    free(a->arestas);
    free(a);
}