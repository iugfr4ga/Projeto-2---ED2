#ifndef CIDADE_H
#define CIDADE_H

/*  Uma cidade é composta por um conjunto de quadras armazenadas em um hashfile e desenhadas em um arquivo SVG. 
    As quadras são identificadas por seu CEP. Cada quadra é um retângulo definido por suas coordenadas ancora (x, y) 
    (canto superior esquerdo), largura (w) e altura (h), cor de preenchimento (cfill), cor de borda (cstrk) 
    e largura de borda (sw).
*/

typedef struct Quadra Quadra;

/* Inicializa a cidade abrindo ou criando o hashfile de quadras no caminho indicado */
int cidade_inicializar(const char* caminho_hf);

/* Finaliza a cidade fechando o hashfile e gerando dump no caminho indicado*/
void cidade_finalizar(const char* caminho_hfd);

/*  Insere uma quadra com os dados fornecidos.
    Retorna 0 em sucesso ou -1 em erro.
*/
int cidade_inserir_quadra(const char* cep, double x, double y, double w, double h, const char* cfill, const char* cstrk, const char* sw);

/*  Remove a quadra identificada pelo cep.
    Retorna 0 em sucesso ou -1 se não encontrada.
*/
int cidade_remover_quadra(const char* cep);

/*  Busca a quadra identificada pelo cep.
    Retorna ponteiro para Quadra em sucesso ou NULL se não encontrada.
    O ponteiro só é válido até a próxima chamada de qualquer função deste módulo.
*/
const Quadra* cidade_buscar_quadra(const char* cep);

/*  Dado um endereço CEP/face(S, N, O, L)/num/largura da borda, calcula as coordenadas do ponto
    e armazena em *px e *py.
    Retorna 0 em sucesso ou -1 se a quadra não for encontrada.
*/
int cidade_coordenadas(const char* cep, char face, int num, const char* sw, double* px, double* py);

/*  Desenha todas as quadras da cidade no arquivo SVG.
    O arquivo SVG deve ser inicializado antes de chamar esta função.
*/
void cidade_desenhar_quadras(void);

// getters
const char* quadra_get_cep(const Quadra* q);
double quadra_get_x(const Quadra* q);
double quadra_get_y(const Quadra* q);
double quadra_get_w(const Quadra* q);
double quadra_get_h(const Quadra* q);
const char* quadra_get_cfill(const Quadra* q);
const char* quadra_get_cstrk(const Quadra* q);
const char* quadra_get_sw(const Quadra* q);

#endif