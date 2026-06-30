#ifndef AGM_H
#define AGM_H

#include "mapa_viario.h"

/*  Módulo responsável pelo cálculo da Árvore Geradora Mínima (AGM) do mapa viário.
    Para esse cálculo, o grafo é tratado como não direcionado: embora o mapa viário tenha ruas com direção, 
    a AGM ignora esse sentido - uma aresta de A para B é tratada apenas como uma ligação entre A e B. 
    A AGM é calculada usando o comprimento das arestas como peso.
    A estrutura AGM armazena as arestas selecionadas para a árvore.
*/

typedef struct AGM AGM;

/*  Calcula a AGM do mapa viário.
    Retorna uma AGM alocada ou NULL em caso de erro.
*/
AGM* agm_calcular(const MapaViario* m);

/*  Retorna o número de arestas da AGM. */
int agm_get_n_arestas(const AGM* a);

/*  Retorna 1 se a aresta pertence à AGM, 0 caso contrário. */
int agm_contem_aresta(const AGM* a, const Aresta* aresta);

/*  Libera a memória da AGM. */
void agm_fechar(AGM* a);

#endif