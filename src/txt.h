#ifndef TXT_H
#define TXT_H

#include "dijkstra.h"

/*  Inicializa o módulo TXT criando o arquivo de saída no caminho indicado.
    Retorna 0 em sucesso ou -1 em erro.
*/
int txt_inicializar(const char* caminho);

/*  Finaliza o módulo TXT fechando o arquivo de saída. */
void txt_finalizar(void);

/*  Escreve uma linha no arquivo TXT. */
void txt_escrever(const char* linha);

/*  Descreve textualmente o percurso dado como lista de PassoCaminho.
*/
void txt_descrever_percurso(const PassoCaminho* percurso);

#endif