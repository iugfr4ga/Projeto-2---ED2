#include "geo.h"
#include "cidade.h"
#include <stdio.h>
#include <string.h>

#define GEO_LINHA_TAM 256
#define COR_TAM 16
#define CEP_TAM 32

int geo_processar(const char* caminho) {
    FILE* f = fopen(caminho, "r");
    if(f == NULL)
        return -1;

    char cfill[COR_TAM] = "\0";
    char cstrk[COR_TAM] = "\0";
    char sw[COR_TAM] = "\0";

    char linha[GEO_LINHA_TAM];
    while(fgets(linha, sizeof(linha), f)) {
        if(linha[0] == '\n')
            continue;

        char cmd[4];
        if(sscanf(linha, "%3s", cmd) != 1)
            continue;

        if(strcmp(cmd, "q") == 0) {
            char cep[CEP_TAM];
            double x, y, w, h;
            if(sscanf(linha, "%*s %31s %lf %lf %lf %lf", cep, &x, &y, &w, &h) == 5) {
                cidade_inserir_quadra(cep, x, y, w, h, cfill, cstrk, sw);
            }
        } 
        else if(strcmp(cmd, "cq") == 0) {
            char novo_fill[COR_TAM], novo_strk[COR_TAM], novo_sw[COR_TAM];
            if(sscanf(linha, "%*s %31s %15s %15s", novo_sw, novo_fill, novo_strk) == 3) {
                strncpy(sw, novo_sw, COR_TAM - 1);
                sw[COR_TAM - 1] = '\0';
                strncpy(cfill, novo_fill, COR_TAM - 1);
                cfill[COR_TAM - 1] = '\0';
                strncpy(cstrk, novo_strk, COR_TAM - 1);
                cstrk[COR_TAM - 1] = '\0';
            }
        }
    }
    fclose(f);
    return 0;
}