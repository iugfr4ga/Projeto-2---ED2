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

static int buscar_indice(const MapaViario* m, const char* id) {
    for(int i = 0; i < m->n_inseridos; i++) {
        if(strcmp(m->vertices[i].id, id) == 0)
            return i;
    }
    return -1;
}

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

int mapa_inserir_vertice(MapaViario* m, const char* id, double x, double y) {
    if(m == NULL || id == NULL)
        return -1;
 
    if(m->n_inseridos >= m->nv)
        return -1;
 
    if(buscar_indice(m, id) != -1)
        return -1;
 
    Vertice* v = &m->vertices[m->n_inseridos];
    strncpy(v->id, id, ID_TAM - 1);
    v->id[ID_TAM - 1] = '\0';
    v->x = x;
    v->y = y;
    v->indice = m->n_inseridos;
    v->arestas = NULL;
    m->n_inseridos++;

    return 0;
}

int mapa_inserir_aresta(MapaViario* m, const char* i, const char* j, const char* ldir, const char* lesq, double cmp, double vm, const char* nome) {
    if(m == NULL || i == NULL || j == NULL)
        return -1;
 
    int idc_i = buscar_indice(m, i);
    int idc_j = buscar_indice(m, j);
 
    if(idc_i == -1 || idc_j == -1)
        return -1;
 
    Aresta* a = malloc(sizeof(Aresta));
    if(a == NULL)
        return -1;
 
    a->destino = &m->vertices[idc_j];   
    strncpy(a->ldir, ldir, CEP_TAM - 1);  
    a->ldir[CEP_TAM - 1] = '\0';
    strncpy(a->lesq, lesq, CEP_TAM - 1);  
    a->lesq[CEP_TAM - 1] = '\0';
    strncpy(a->nome, nome, NOME_TAM - 1); 
    a->nome[NOME_TAM - 1] = '\0';
    a->cmp = cmp;
    a->vm = vm;
 
    a->prox = m->vertices[idc_i].arestas;
    m->vertices[idc_i].arestas = a;
 
    return 0;
}

