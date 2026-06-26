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

const Vertice* mapa_buscar_vertice(const MapaViario* m, const char* id) {
    if(m == NULL || id == NULL)
        return NULL;
 
    int idc = buscar_indice(m, id);
    if(idc == -1)
        return NULL;
 
    return &m->vertices[idc];
}

const Vertice* mapa_vertice_mais_proximo(const MapaViario* m, double x, double y) {
    if(m == NULL || m->n_inseridos == 0)
        return NULL;
 
    const Vertice* mais_proximo = &m->vertices[0];
    double dx = m->vertices[0].x - x;
    double dy = m->vertices[0].y - y;
    double menor_dist = dx * dx + dy * dy;
 
    for(int i = 1; i < m->n_inseridos; i++) {
        dx = m->vertices[i].x - x;
        dy = m->vertices[i].y - y;
        double dist = dx * dx + dy * dy;
        if(dist < menor_dist) {
            menor_dist = dist;
            mais_proximo = &m->vertices[i];
        }
    }
 
    return mais_proximo;
}

int mapa_atualizar_vm_regiao(MapaViario* m, double x, double y, double w, double h, double vm) {
    if(m == NULL)
        return 0;
 
    int atualizadas = 0;
 
    for(int i = 0; i < m->n_inseridos; i++) {
        Vertice* v = &m->vertices[i];
 
        if(v->x >= x && v->x <= x + w && v->y >= y && v->y <= y + h) {
            for(Aresta* a = v->arestas; a != NULL; a = a->prox) {
                a->vm = vm;
                atualizadas++;
            }
        }
    }
    return atualizadas;
}

int aresta_atualizar_vm(Aresta* a) {
    if(a == NULL)
        return -1; 
    
    a->vm += a->vm * 0.5;
    return a->vm;
}

const char* vertice_get_id(const Vertice* v) { 
    return v->id; 
}

double vertice_get_x(const Vertice* v) { 
    return v->x;  
}

double vertice_get_y(const Vertice* v) { 
    return v->y;  
}

const Vertice* aresta_get_destino(const Aresta* a) { 
    return a->destino; 
}

const char* aresta_get_nome(const Aresta* a) { 
    return a->nome;    
}

const char* aresta_get_ldir(const Aresta* a) { 
    return a->ldir;    
}

const char* aresta_get_lesq(const Aresta* a) { 
    return a->lesq;    
}

double aresta_get_cmp(const Aresta* a) { 
    return a->cmp;     
}

double aresta_get_vm(const Aresta* a) { 
    return a->vm;      
}

const Aresta* vertice_get_arestas(const Vertice* v) { 
    return v ? v->arestas : NULL; 
}

const Aresta* aresta_get_prox(const Aresta* a) { 
    return a ? a->prox : NULL; 
}

int vertice_get_indice(const Vertice* v) { 
    return v ? v->indice  : -1;  
}

int mapa_get_n_inseridos(const MapaViario* m) { 
    return m ? m->n_inseridos : 0; 
}

const Vertice* mapa_get_vertice_por_indice(const MapaViario* m, int i) {
    if(m == NULL || i < 0 || i >= m->n_inseridos) 
        return NULL;

    return &m->vertices[i];
}