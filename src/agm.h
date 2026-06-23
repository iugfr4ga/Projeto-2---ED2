#ifndef AGM_H
#define AGM_H

#include "mapa_viario.h"

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