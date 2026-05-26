#include "cidade.h"
#include "hash_extensivel.h"
#include "svg.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CEP_TAM 32
#define COR_TAM 16

struct Quadra {
    char cep[CEP_TAM];
    double x, y;
    double w, h;
    char cfill[COR_TAM];
    char cstrk[COR_TAM];
    char sw[COR_TAM];
};

static HashExtensivel* hf_quadras = NULL;
static Quadra buf_quadra;  // buffer para retorno de cidade_buscar_quadra

int cidade_inicializar(const char* caminho_hf) {
    hf_quadras = hash_abrir(caminho_hf);
    if(hf_quadras == NULL)
        hf_quadras = hash_criar(caminho_hf, 50, sizeof(Quadra));
    return hf_quadras != NULL ? 0 : -1;
}

void cidade_finalizar(const char* caminho_hfd) {
    if(hf_quadras != NULL && caminho_hfd != NULL) {
        hash_dump(hf_quadras, caminho_hfd);
        hash_fechar(hf_quadras);
        hf_quadras = NULL;
    }
}

int cidade_inserir_quadra(const char* cep, double x, double y, double w, double h, const char* cfill, const char* cstrk, const char* sw) {
    if(cep == NULL) 
        return -1;

    Quadra q;
    strncpy(q.cep, cep, CEP_TAM - 1);  
    q.cep[CEP_TAM - 1] = '\0';
    strncpy(q.cfill, cfill, COR_TAM - 1);  
    q.cfill[COR_TAM - 1] = '\0';
    strncpy(q.cstrk, cstrk, COR_TAM - 1);  
    q.cstrk[COR_TAM - 1] = '\0';
    strncpy(q.sw, sw, COR_TAM - 1);  
    q.sw[COR_TAM - 1] = '\0';

    q.x = x;
    q.y = y;
    q.w = w;
    q.h = h;

    return hash_inserir(hf_quadras, cep, &q);
}

int cidade_remover_quadra(const char* cep) {
    if(cep == NULL) 
        return -1;
    return hash_remover(hf_quadras, cep);
}

const Quadra* cidade_buscar_quadra(const char* cep) {
    if(cep == NULL) 
        return NULL;
    if(hash_buscar(hf_quadras, cep, &buf_quadra) != 0)
        return NULL;
    return &buf_quadra;
}

int cidade_coordenadas(const char* cep, char face, int num, const char* sw, double* px, double* py) {
    if(px == NULL || py == NULL) 
        return -1;

    const Quadra* q = cidade_buscar_quadra(cep);
    if(q == NULL) 
        return -1;
    
    float b = atof(sw);

    switch(face) {
        case 'S': *px = q->x + num; *py = q->y + b; break;
        case 'N': *px = q->x + num; *py = q->y + q->h - b; break;
        case 'O': *px = q->x + q->w - b; *py = q->y + num; break;
        case 'L': *px = q->x + b; *py = q->y + num; break;
        default: return -1;
    }
    return 0;
}

static int cb_desenhar_quadra(const char* chave, const void* registro, void* ctx) {
    (void) chave;
    (void) ctx;
    const Quadra* q = (const Quadra*) registro;
    svg_desenhar_quadra(quadra_get_cep(q), quadra_get_x(q), quadra_get_y(q), quadra_get_w(q), quadra_get_h(q), 
    quadra_get_cfill(q), quadra_get_cstrk(q), quadra_get_sw(q));
    return 0;
}

void cidade_desenhar_quadras(void) {
    hash_iterar(hf_quadras, cb_desenhar_quadra, NULL);
}

const char* quadra_get_cep(const Quadra* q) { 
    return q->cep;   
}

double quadra_get_x(const Quadra* q) { 
    return q->x;     
}

double quadra_get_y(const Quadra* q) { 
    return q->y;     
}

double quadra_get_w(const Quadra* q) { 
    return q->w;     
}

double quadra_get_h(const Quadra* q) { 
    return q->h;     
}

const char* quadra_get_cfill(const Quadra* q) { 
    return q->cfill; 
}

const char* quadra_get_cstrk(const Quadra* q) { 
    return q->cstrk; 
}

const char* quadra_get_sw(const Quadra* q) { 
    return q->sw;    
}