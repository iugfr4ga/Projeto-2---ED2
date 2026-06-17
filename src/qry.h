#ifndef QRY_H
#define QRY_H

#include "mapa_viario.h"
#include <stdio.h>

/*  Um arquivo .qry executa comandos sobre o mapa viário de uma cidade gera saídas em TXT e SVG.
    Os comandos do arquivo .qry são:

    @o? <reg> <cep> <face> <num>
    - Armazena a posição geográfica do endereço <cep> <face> <num> no registrador <reg>.
    SVG: Desenha uma linha vertical pontilhada vermelha mostrando a posição do endereço e coloca o número do 
    registrador na outra extremidade da linha (topo da página).
    TXT: Reportar a coordenada relativa ao endereço.

    mvm <v> <x> <y> <w> <h>         
    - Atualiza a velocidade média das arestas dentro da região <x> <y> <w> <h> para <v>.

    regs <vl>               
    - Considera como insuficiente os trechos com velocidade média inferior a <vl> e calcula a quantidade de
    componentes conexos do grafo do mapa viário.
    TXT: Reporta o número de componentes conexos.
    SVG: Desenha os respectivos retângulos com cores diferentes e transparência de 50%.

    exp <vl>                
    - Calcula a árvore geradora mínima, selecionando apenas as arestas com velocidade média inferior a
    <vl> e aumenta em 50% a velocidade média das arestas selecionadas.
    SVG: Pintar as arestas selecionadas com linha grossa e vermelha.

    p? <reg1> <reg2> <cc> <cr> 
    - Determina o melhor trajeto entre a origem <reg1> e o destino <reg2>, desenhando os percursos 
    (mais curto e mais rápido) com as cores <cc> e <cr>, respectivamente.
    TXT: Descrição textual do percurso. Caso destino inacessível, reporta "Destino inacessível".
    SVG: Mostra os percursos pintados e animados. Marca os extremos do percurso com “placas” com as letras I e F.
*/

/*  Le e processa o arquivo .qry.
    O arquivo deve estar no caminho indicado por *caminho.
    Retorna 0 em sucesso ou -1 se o arquivo em erro (arquivo não pode ser aberto, parâmetros inválidos).
*/
int qry_processar(const char* caminho, MapaViario* m);

#endif