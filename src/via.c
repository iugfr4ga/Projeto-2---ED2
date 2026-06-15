#include "via.h"
#include "mapa_viario.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VIA_LINHA_TAM 256
#define ID_TAM 64
#define CEP_TAM 32
#define NOME_TAM 64

MapaViario* via_processar(const char* caminho) {
    if(caminho == NULL)
        return NULL;

    FILE* f = fopen(caminho, "r");
    if(f == NULL)
        return NULL;

    int nv;
    if(fscanf(f, "%d\n", &nv) != 1 || nv <= 0) {
        fclose(f);
        return NULL;
    }

    MapaViario* m = mapa_criar(nv);
    if(m == NULL) {
        fclose(f);
        return NULL;
    }

    char linha[VIA_LINHA_TAM];
    while(fgets(linha, sizeof(linha), f)) {
        if(linha[0] == '\n')
            continue;

        char cmd[4];
        if(sscanf(linha, "%3s", cmd) != 1)
            continue;

        if(strcmp(cmd, "v") == 0) {
            char id[ID_TAM];
            double x, y;
            if(sscanf(linha, "%*s %63s %lf %lf", id, &x, &y) == 3) {
                mapa_inserir_vertice(m, id, x, y);
            }
        } 
        else if(strcmp(cmd, "e") == 0) {
            char i[ID_TAM], j[ID_TAM];
            char ldir[CEP_TAM], lesq[CEP_TAM];
            char nome[NOME_TAM];
            double cmp, vm;

            if(sscanf(linha, "%*s %63s %63s %31s %31s %lf %lf %63s", i, j, ldir, lesq, &cmp, &vm, nome) == 7) {
                mapa_inserir_aresta(m, i, j, ldir, lesq, cmp, vm, nome);
            }
        }
    }

    fclose(f);
    return m;
}