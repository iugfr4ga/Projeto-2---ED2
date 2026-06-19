#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "mapa_viario.h"

typedef struct Caminho Caminho;
typedef struct PassoCaminho PassoCaminho; // Armazena o vértice e a aresta usada pra chegar nele.

typedef enum {
    DISTANCIA,
    TEMPO
} Criterio;

/*  Executa o algoritmo Dijkstra no mapa viário a partir da origem com o critério dado (DISTANCIA ou TEMPO).
    Retorna um Caminho ou NULL em erro.
*/
Caminho* dijkstra(const MapaViario* m, const Vertice* origem, Criterio c);

/*  Reconstrói o percurso da origem até o destino como lista de PassoCaminho.
    Retorna NULL se destino inacessível.
    A lista deve ser liberada com caminho_lista_fechar.
*/
PassoCaminho* caminho_reconstruir(const Caminho* c, const Vertice* destino);

/*  Retorna o custo total do caminho c até o destino.
    Retorna -1.0 se inacessível.
*/
double caminho_custo(const Caminho* c, const Vertice* destino);

/* Libera a memória do Caminho. */
void caminho_fechar(Caminho* c);

/* Libera a lista de PassoCaminho. */
void caminho_lista_fechar(PassoCaminho* p);

/* Getters do PassoCaminho */
const Vertice* passo_get_vertice(const PassoCaminho* p);
const Aresta* passo_get_aresta(const PassoCaminho* p);
const PassoCaminho* passo_get_prox(const PassoCaminho* p);

#endif