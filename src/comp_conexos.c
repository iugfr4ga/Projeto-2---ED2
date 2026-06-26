#include "comp_conexos.h"
#include "mapa_viario.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>

struct Componentes {
    int* comp;      // comp[i] = índice do componente do vértice i
    int nv;
    int quantidade;
    double* xmin;
    double* ymin;
    double* xmax;
    double* ymax;
};

typedef struct {
    int* dados;
    int topo;
    int capacidade;
} Pilha;

static Pilha* pilha_criar(int capacidade) {
    Pilha* p = malloc(sizeof(Pilha));
    if(p == NULL) 
        return NULL;

    p->dados = malloc(capacidade * sizeof(int));
    if(p->dados == NULL) { 
        free(p); 
        return NULL; 
    }
    p->topo = 0;
    p->capacidade = capacidade;
    return p;
}

static void pilha_fechar(Pilha* p) {
    if(p == NULL) 
        return;
    free(p->dados);
    free(p);
}

static void pilha_push(Pilha* p, int v) {
    p->dados[p->topo++] = v;
}

static int pilha_pop(Pilha* p) {
    return p->dados[--p->topo];
}

static int pilha_vazia(Pilha* p) {
    return p->topo == 0;
}

static void dfs1(const MapaViario* m, int inicio, int* visitado, Pilha* pilha_fim, double vl, int nv) {
    // cada entrada guarda o vértice e a aresta atual sendo processada
    int* vertice_pilha = malloc(nv * sizeof(int));
    const Aresta** aresta_pilha = malloc(nv * sizeof(Aresta*));
    int topo = 0;

    visitado[inicio] = 1;
    vertice_pilha[topo] = inicio;
    aresta_pilha[topo] = vertice_get_arestas(mapa_get_vertice_por_indice(m, inicio));
    topo++;

    while(topo > 0) {
        int u = vertice_pilha[topo - 1];
        const Aresta* a = aresta_pilha[topo - 1];

        // avança até achar aresta valida não visitada
        while(a != NULL && (aresta_get_vm(a) < vl || visitado[vertice_get_indice(aresta_get_destino(a))]))
            a = aresta_get_prox(a);

        if(a != NULL) {
            // empilha vizinho
            int j = vertice_get_indice(aresta_get_destino(a));
            aresta_pilha[topo - 1] = aresta_get_prox(a); // avança aresta atual
            visitado[j] = 1;
            vertice_pilha[topo] = j;
            aresta_pilha[topo] = vertice_get_arestas(mapa_get_vertice_por_indice(m, j));
            topo++;
        } else {
            // terminou vizinhos (empilha na pilha de fim)
            pilha_push(pilha_fim, u);
            topo--;
        }
    }
    free(vertice_pilha);
    free(aresta_pilha);
}

static MapaViario* transpor_grafo(const MapaViario* m, double vl) {
    int nv = mapa_get_n_inseridos(m);
    MapaViario* t = mapa_criar(nv);
    if(t == NULL) 
        return NULL;

    for(int i = 0; i < nv; i++) {
        const Vertice* v = mapa_get_vertice_por_indice(m, i);
        mapa_inserir_vertice(t, vertice_get_id(v), vertice_get_x(v), vertice_get_y(v));
    }

    // insere arestas invertidas
    for(int i = 0; i < nv; i++) {
        const Vertice* v = mapa_get_vertice_por_indice(m, i);
        for(const Aresta* a = vertice_get_arestas(v); a != NULL; a = aresta_get_prox(a)) {
            if(aresta_get_vm(a) < vl)
                continue;
            const Vertice* dest = aresta_get_destino(a);
            mapa_inserir_aresta(t, vertice_get_id(dest), vertice_get_id(v), aresta_get_ldir(a), aresta_get_lesq(a), 
            aresta_get_cmp(a), aresta_get_vm(a), aresta_get_nome(a));
        }
    }
    return t;
}

static void dfs2(const MapaViario* t, int inicio, int* visitado, int* comp, int k, int nv) {
    int* pilha = malloc(nv * sizeof(int));
    if(pilha == NULL) 
        return;

    int topo = 0;
    pilha[topo++] = inicio;
    visitado[inicio] = 1;
    comp[inicio] = k;

    while(topo > 0) {
        int u = pilha[--topo];
        const Vertice* v = mapa_get_vertice_por_indice(t, u);
        for(const Aresta* a = vertice_get_arestas(v); a != NULL; a = aresta_get_prox(a)) {
            int j = vertice_get_indice(aresta_get_destino(a));
            if(!visitado[j]) {
                visitado[j] = 1;
                comp[j] = k;
                pilha[topo++] = j;
            }
        }
    }
    free(pilha);
}

Componentes* componentes_calcular(const MapaViario* m, double vl) {
    if(m == NULL) 
        return NULL;

    int nv = mapa_get_n_inseridos(m);
    if(nv <= 0) 
        return NULL;

    Componentes* c = malloc(sizeof(Componentes));
    if(c == NULL) 
        return NULL;

    c->nv = nv;
    c->quantidade = 0;
    c->comp = malloc(nv * sizeof(int));
    c->xmin = c->ymin = c->xmax = c->ymax = NULL;

    if(c->comp == NULL) { 
        componentes_fechar(c); 
        return NULL; 
    }

    int* visitado = calloc(nv, sizeof(int));
    Pilha* pilha = pilha_criar(nv);

    if(visitado == NULL || pilha == NULL) {
        free(visitado);
        pilha_fechar(pilha);
        componentes_fechar(c);
        return NULL;
    }

    // passo 1: DFS no grafo original
    for(int i = 0; i < nv; i++)
        if(!visitado[i])
            dfs1(m, i, visitado, pilha, vl, nv);

    // passo 2: transpor o grafo
    MapaViario* t = transpor_grafo(m, vl);
    if(t == NULL) {
        free(visitado);
        pilha_fechar(pilha);
        componentes_fechar(c);
        return NULL;
    }

    // passo 3: DFS no grafo transposto na ordem inversa da pilha
    memset(visitado, 0, nv * sizeof(int));
    for(int i = 0; i < nv; i++) 
        c->comp[i] = -1;

    int k = 0;
    while(!pilha_vazia(pilha)) {
        int u = pilha_pop(pilha);
        if(!visitado[u]) {
            dfs2(t, u, visitado, c->comp, k, nv);
            k++;
        }
    }
    c->quantidade = k;

    free(visitado);
    pilha_fechar(pilha);
    mapa_fechar(t);

    c->xmin = malloc(k * sizeof(double));
    c->ymin = malloc(k * sizeof(double));
    c->xmax = malloc(k * sizeof(double));
    c->ymax = malloc(k * sizeof(double));

    if(c->xmin == NULL || c->ymin == NULL || c->xmax == NULL || c->ymax == NULL) {
        componentes_fechar(c);
        return NULL;
    }

    for(int i = 0; i < k; i++) {
        c->xmin[i] =  DBL_MAX;
        c->ymin[i] =  DBL_MAX;
        c->xmax[i] = -DBL_MAX;
        c->ymax[i] = -DBL_MAX;
    }

    for(int i = 0; i < nv; i++) {
        const Vertice* v = mapa_get_vertice_por_indice(m, i);
        int ki = c->comp[i];
        double x = vertice_get_x(v);
        double y = vertice_get_y(v);
        if(x < c->xmin[ki]) 
            c->xmin[ki] = x;
        if(y < c->ymin[ki]) 
            c->ymin[ki] = y;
        if(x > c->xmax[ki]) 
            c->xmax[ki] = x;
        if(y > c->ymax[ki]) 
            c->ymax[ki] = y;
    }
    return c;
}

int componentes_get_quantidade(const Componentes* c) {
    return c ? c->quantidade : 0;
}

int componentes_get_componente(const Componentes* c, const Vertice* v) {
    if(c == NULL || v == NULL) 
        return -1;
    int i = vertice_get_indice(v);
    if(i < 0 || i >= c->nv) 
        return -1;
    return c->comp[i];
}

double componentes_get_xmin(const Componentes* c, int k) {
    if(c == NULL || k < 0 || k >= c->quantidade) 
        return 0.0;
    return c->xmin[k];
}

double componentes_get_ymin(const Componentes* c, int k) {
    if(c == NULL || k < 0 || k >= c->quantidade) 
        return 0.0;
    return c->ymin[k];
}

double componentes_get_xmax(const Componentes* c, int k) {
    if(c == NULL || k < 0 || k >= c->quantidade) 
        return 0.0;
    return c->xmax[k];
}

double componentes_get_ymax(const Componentes* c, int k) {
    if(c == NULL || k < 0 || k >= c->quantidade) 
        return 0.0;
    return c->ymax[k];
}

void componentes_fechar(Componentes* c) {
    if(c == NULL) 
        return;
    free(c->comp);
    free(c->xmin);
    free(c->ymin);
    free(c->xmax);
    free(c->ymax);
    free(c);
}