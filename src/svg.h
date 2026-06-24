#ifndef SVG_H
#define SVG_H

#include "mapa_viario.h"
#include "comp_conexos.h"
#include "dijkstra.h"

/*  Inicializa o arquivo SVG no caminho dado com as dimensões fornecidas.
    Retorna 0 em sucesso ou -1 em erro.
*/
int svg_inicializar(const char* caminho, double largura, double altura);

/*  Finaliza e fecha o arquivo SVG. */
void svg_finalizar(void);

/*  Desenha uma quadra como retângulo com preenchimento, borda e espessura dados.
    O ponto (x, y) é o canto superior esquerdo no sistema SVG (y cresce para baixo).
*/
void svg_desenhar_quadra(const char* cep, double x, double y, double w, double h, const char* cfill, const char* cstrk, const char* sw);

/*  Desenha uma linha vertical pontilhada vermelha na posição x com o número do registrador no topo da página. */
void svg_desenhar_registrador(double x, int reg);

/*  Desenha os bounding boxes dos componentes conexos com cores diferentes e transparência de 50%. */
void svg_desenhar_componentes(const Componentes* c);

/*  Desenha uma aresta da AGM com linha grossa vermelha, sendo a origem o vértice de onde a aresta parte. */
void svg_desenhar_aresta(const Vertice* origem, const Aresta* a);

/*  Desenha e anima o percurso dado com a cor especificada, as coordenadas de inicio e fim do trajeto e a
    duração dur em segundos da animação.
*/
void svg_desenhar_percurso(const PassoCaminho* percurso, const char* cor, double x_inicio, double y_inicio, double x_fim, double y_fim, double dur);

#endif