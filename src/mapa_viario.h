#ifndef MAPA_VIARIO_H
#define MAPA_VIARIO_H

typedef struct MapaViario MapaViario;
typedef struct Vertice Vertice;
typedef struct Aresta Aresta;

/*  Cria um mapa viário vazio com nv vértices. Retorna NULL em erro. */
MapaViario* mapa_criar(int nv);

/*  Fecha o mapa e libera toda a memória. */
void mapa_fechar(MapaViario* m);

/*  Insere um vértice com identificador id e coordenadas (x, y).
    Retorna 0 em sucesso ou -1 se id já existe ou erro.
*/
int mapa_inserir_vertice(MapaViario* m, const char* id, double x, double y);

/*  Insere uma aresta direcionada de i para j com os atributos do segmento de rua.
    ldir e lesq são CEPs das quadras adjacentes (ou "-" se não houver).
    Retorna 0 em sucesso ou -1 se i ou j não existem ou erro.
*/
int mapa_inserir_aresta(MapaViario* m, const char* i, const char* j, const char* ldir, const char* lesq, double cmp, double vm, const char* nome);

/*  Retorna o vértice com o id indicado ou NULL se não encontrado. */
const Vertice* mapa_buscar_vertice(const MapaViario* m, const char* id);

/*  Retorna o vértice mais próximo do ponto (x, y).
*/
const Vertice* mapa_vertice_mais_proximo(const MapaViario* m, double x, double y);

/*  Atualiza a velocidade média de todas as arestas cujos vértices de origem
    estejam dentro do retângulo (x, y, w, h) para o valor vm.
    Retorna o número de arestas atualizadas.
*/
int mapa_atualizar_vm_regiao(MapaViario* m, double x, double y, double w, double h, double vm);

/*  Getters */
const char* vertice_get_id(const Vertice* v);
double vertice_get_x(const Vertice* v);
double vertice_get_y(const Vertice* v);
const Vertice* aresta_get_destino(const Aresta* a);
const char* aresta_get_nome(const Aresta* a);
const char* aresta_get_ldir(const Aresta* a);
const char* aresta_get_lesq(const Aresta* a);
double aresta_get_cmp(const Aresta* a);
double aresta_get_vm(const Aresta* a);

#endif