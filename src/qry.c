#include "qry.h"
#include "mapa_viario.h"
#include "cidade.h"
#include "svg.h"
#include "txt.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define QRY_LINHA_TAM 512
#define ID_TAM 64
#define CEP_TAM 32
#define COR_TAM 16
#define N_REGISTRADORES 11  // R0..R10

typedef struct {
    double x, y;
    int valido;             // 1 se registrador foi preenchido, 0 caso contrário
} Registrador;

static Registrador registradores[N_REGISTRADORES];

static void cmd_o(MapaViario* m, int reg, const char* cep, char face, double num) {

}

static void cmd_mvm(MapaViario* m, double v, double x, double y, double w, double h) {

}

static void cmd_regs(MapaViario* m, double vl) {

}

static void cmd_exp(MapaViario* m, double vl) {

}

static void cmd_p(MapaViario* m, int reg1, int reg2, const char* cc, const char* cr) {

}

static int get_registrador(const char* s) {
    if(s == NULL || (s[0] != 'R' && s[0] != 'r'))
        return -1;
    int n = atoi(s[1]);
    if(n < 0 || n > 10)
        return -1;
    return n;
}

int qry_processar(const char* caminho, MapaViario* m) {
    if(caminho == NULL || m == NULL)
        return -1;

    FILE* f = fopen(caminho, "r");
    if(f == NULL)
        return -1;

    // inicializa registradores
    for(int i = 0; i < N_REGISTRADORES; i++)
        registradores[i].valido = 0;

    char linha[QRY_LINHA_TAM];
    while(fgets(linha, sizeof(linha), f)) {
        if(linha[0] == '\n')
            continue;

        char cmd[6];
        if(sscanf(linha, "%5s", cmd) != 1)
            continue;

        if(strcmp(cmd, "@o?") == 0) {
            char sreg[4], cep[CEP_TAM],face;
            double num;
            if(sscanf(linha, "%*s %3s %31s %c %lf", sreg, cep, &face, &num) == 4) {
                int reg = get_registrador(sreg);
                if(reg != -1)
                    cmd_o(m, reg, cep, face, num);
            }
        }
        else if(strcmp(cmd, "mvm") == 0) {
            double v, x, y, w, h;
            if(sscanf(linha, "%*s %lf %lf %lf %lf %lf", &v, &x, &y, &w, &h) == 5)
                cmd_mvm(m, v, x, y, w, h);
        }
        else if(strcmp(cmd, "regs") == 0) {
            double vl;
            if(sscanf(linha, "%*s %lf", &vl) == 1)
                cmd_regs(m, vl);
        }
        else if(strcmp(cmd, "exp") == 0) {
            double vl;
            if(sscanf(linha, "%*s %lf", &vl) == 1)
                cmd_exp(m, vl);
        }
        else if(strcmp(cmd, "p?") == 0) {
            char sreg1[ID_TAM], sreg2[ID_TAM], cc[COR_TAM], cr[COR_TAM];
            if(sscanf(linha, "%*s %3s %3s %15s %15s", sreg1, sreg2, cc, cr) == 4) {
                int reg1 = get_registrador(sreg1);
                int reg2 = get_registrador(sreg2);
                if(reg1 != -1 && reg2 != -1)
                    cmd_p(m, reg1, reg2, cc, cr);
            }
        }
    }
    fclose(f);
    return 0;
}
