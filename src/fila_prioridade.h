#ifndef FILA_PRIORIDADE_H
#define FILA_PRIORIDADE_H

typedef struct FilaPrioridade FilaPrioridade;

/*  Cria uma fila de prioridade vazia. Retorna NULL em erro.
*/
FilaPrioridade* fila_prioridade_criar(void);

/*  Insere o par (prioridade, valor) na fila.
    Retorna 0 em sucesso ou -1 em erro.
*/
int fila_prioridade_inserir(FilaPrioridade* f, double prioridade, int valor);

/*  Remove e retorna o valor do elemento com menor prioridade.
    Retorna -1 se vazio ou NULL.
*/
int fila_prioridade_remover(FilaPrioridade* f);

/*  Retorna 1 se a fila está vazia, 0 caso contrário. */
int fila_prioridade_vazia(const FilaPrioridade* f);

/*  Retorna o número de elementos na fila. */
int fila_prioridade_tamanho(const FilaPrioridade* f);

/*  Libera a memória da fila. */
void fila_prioridade_fechar(FilaPrioridade* f);

#endif