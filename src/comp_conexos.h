#ifndef COMP_CONEXOS_H
#define COMP_CONEXOS_H

#include "mapa_viario.h"

typedef struct Componentes Componentes;

/*  Calcula os componentes conexos do mapa viário desconsiderando
    arestas com vm inferior a vl.
    Retorna uma lista de Componentes alocado ou NULL em erro.
*/
Componentes* componentes_calcular(const MapaViario* m, double vl);

/*  Retorna o número de componentes conexos encontrados. */
int componentes_get_quantidade(const Componentes* c);

/*  Retorna o índice do componente ao qual o vértice pertence.
    Retorna -1 se vértice inválido.
*/
int componentes_get_componente(const Componentes* c, const Vertice* v);

/*  Retorna a coordenada x mínima do bounding box do componente n. */
double componentes_get_xmin(const Componentes* c, int n);

/*  Retorna a coordenada y mínima do bounding box do componente n. */
double componentes_get_ymin(const Componentes* c, int n);

/*  Retorna a coordenada x máxima do bounding box do componente n. */
double componentes_get_xmax(const Componentes* c, int n);

/*  Retorna a coordenada y máxima do bounding box do componente n. */
double componentes_get_ymax(const Componentes* c, int n);

/*  Libera a memória dos Componentes c. */
void componentes_fechar(Componentes* c);

#endif