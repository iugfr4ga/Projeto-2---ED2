#include "txt.h"
#include "dijkstra.h"
#include "mapa_viario.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static FILE* txt_arquivo = NULL;

int txt_inicializar(const char* caminho) {
    if(caminho == NULL) 
        return -1;
    txt_arquivo = fopen(caminho, "w");
    return txt_arquivo != NULL ? 0 : -1;
}

void txt_finalizar(void) {
    if(txt_arquivo == NULL) 
        return;
    fclose(txt_arquivo);
    txt_arquivo = NULL;
}

void txt_escrever(const char* linha) {
    if(txt_arquivo == NULL || linha == NULL) 
        return;
    fprintf(txt_arquivo, "%s\n", linha);
}
