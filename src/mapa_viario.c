#include "mapa_viario.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define ID_TAM 64
#define CEP_TAM 32
#define NOME_TAM 64

struct Aresta { 
    Vertice* destino;
    char ldir[CEP_TAM];
    char lesq[CEP_TAM];
    char nome[NOME_TAM];
    double cmp;
    double vm;
    struct Aresta* prox;
};

struct Vertice {
    char id[ID_TAM];
    double x, y;
    int indice;           // indice no vetor de vértices
    Aresta* arestas;      
};

struct MapaViario {
    Vertice* vertices;    
    int nv;               // numero de vertices
    int n_inseridos;      // quantos vértices foram inseridos até agora
};

MapaViario* mapa_criar(int nv) {
    if(nv <= 0)
        return NULL;

    MapaViario* m = malloc(sizeof(MapaViario));
    if(m == NULL)
        return NULL;

    m->vertices = calloc(nv, sizeof(Vertice));  // ja inicializa as arestas dos vértices como nulo
    if(m->vertices == NULL) {
        free(m);
        return NULL;
    }

    m->nv = nv;
    m->n_inseridos = 0;
    return m;
}

void mapa_fechar(MapaViario* m) {
    if(m == NULL)
        return;

    for(int i = 0; i < m->n_inseridos; i++) {
        Aresta* a = m->vertices[i].arestas;
        while(a != NULL) {
            Aresta* prox = a->prox;
            free(a);
            a = prox;
        }
    }

    free(m->vertices);
    free(m);
}



