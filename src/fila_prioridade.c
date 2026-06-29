#include "fila_prioridade.h"
#include <stdlib.h>

typedef struct No {
    double prioridade;
    int valor;
    struct No* prox;
} No;

struct FilaPrioridade {
    No* inicio;
    int tamanho;
};

FilaPrioridade* fila_prioridade_criar(void) {
    FilaPrioridade* f = malloc(sizeof(FilaPrioridade));
    if(f == NULL) 
        return NULL;
    f->inicio = NULL;
    f->tamanho = 0;
    return f;
}

int fila_prioridade_inserir(FilaPrioridade* f, double prioridade, int valor) {
    if(f == NULL) 
        return -1;

    No* novo = malloc(sizeof(No));
    if(novo == NULL) return -1;

    novo->prioridade = prioridade;
    novo->valor = valor;
    novo->prox = NULL;

    // insere no início se lista vazia ou prioridade menor que o primeiro
    if(f->inicio == NULL || prioridade < f->inicio->prioridade) {
        novo->prox = f->inicio;
        f->inicio = novo;
        f->tamanho++;
        return 0;
    }

    // caso contrario, acha posição correta
    No* atual = f->inicio;
    while(atual->prox != NULL && atual->prox->prioridade <= prioridade)
        atual = atual->prox;

    novo->prox = atual->prox;
    atual->prox = novo;
    f->tamanho++;
    return 0;
}

int fila_prioridade_remover(FilaPrioridade* f) {
    if(f == NULL || f->inicio == NULL) 
        return -1;

    No* removido = f->inicio;
    int valor = removido->valor;
    f->inicio = removido->prox;
    f->tamanho--;
    free(removido);
    return valor;
}

int fila_prioridade_vazia(const FilaPrioridade* f) {
    return f == NULL || f->inicio == NULL;
}

int fila_prioridade_tamanho(const FilaPrioridade* f) {
    return f ? f->tamanho : 0;
}

void fila_prioridade_fechar(FilaPrioridade* f) {
    if(f == NULL) 
        return;
    No* atual = f->inicio;
    while(atual != NULL) {
        No* prox = atual->prox;
        free(atual);
        atual = prox;
    }
    free(f);
}