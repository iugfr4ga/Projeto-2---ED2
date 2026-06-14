#ifndef VIA_H
#define VIA_H

#include "mapa_viario.h"

/*  Um arquivo .via cria e processa um mapa viário. 
    Os comandos do arquivo .via são:

    nv
    - Número de vértices do mapa viário (deve ser a primeira linha do arquivo).

    v <id> <x> <y>
    - Cria o vértice id posicionado nas coordenadas [x, y].

    e <i> <j> <ldir> <lesq> <cmp> <vm> <nome>
    - Cria a aresta com origem i e destino j e associa as informações ldir (CEP da quadra que está do lado direito do 
    segmento de rua à aresta), lesq (CEP da quadra que está do lado esquerdo do segmento de rua), 
    cmp (comprimento em metros do segmento de rua), vm (velocidade média do tráfego em m/s do segmento de rua) 
    e nome (nome do segmento de rua). Caso a aresta não possua quadras em algum de seus lados, esta ausência 
    é indicada por um hífen (-).
*/

/*  Lê e processa os comandos do arquivo .via criando o mapa viário.
    Retorna o mapa criado em sucesso ou NULL em erro.
*/
MapaViario* via_processar(const char* caminho);

#endif