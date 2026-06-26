#include "qry.h"
#include "mapa_viario.h"
#include "cidade.h"
#include "comp_conexos.h"
#include "agm.h"
#include "dijkstra.h"
#include "svg.h"
//#include "txt.h"
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

static void cmd_o(int reg, const char* cep, char face, int num) {
    double x, y;
    if(cidade_coordenadas(cep, face, num, "1", &x, &y) != 0) 
        return;

    registradores[reg].x = x;
    registradores[reg].y = y;
    registradores[reg].valido = 1;

    svg_desenhar_registrador(x, reg);
    // TODO: txt reportar coordenada
}

static void cmd_mvm(MapaViario* m, double v, double x, double y, double w, double h) {
    mapa_atualizar_vm_regiao(m, x, y, w, h, v);
}

static void cmd_regs(MapaViario* m, double vl) {
    Componentes* c = componentes_calcular(m, vl);
    if(c == NULL) 
        return;

    // TODO: txt reportar num componentes conexos
    svg_desenhar_componentes(c);
    componentes_fechar(c);
}

static void cmd_exp(MapaViario* m, double vl) {
    AGM* agm = agm_calcular(m);
    if(agm == NULL) 
        return;

    int nv = mapa_get_n_inseridos(m);
    for(int i = 0; i < nv; i++) {
        const Vertice* v = mapa_get_vertice_por_indice(m, i);
        for(const Aresta* a = vertice_get_arestas(v); a != NULL; a = aresta_get_prox(a)) {
            if(agm_contem_aresta(agm, a) && aresta_get_vm(a) < vl) {
                // TODO: atualizar velocidade da aresta
                svg_desenhar_aresta(v, a);
            }
        }
    }
    agm_fechar(agm);
}

static void cmd_p(MapaViario* m, int reg1, int reg2, const char* cc, const char* cr) {
    if(!registradores[reg1].valido || !registradores[reg2].valido) 
        return;
    
    double x0 = registradores[reg1].x, y0 = registradores[reg1].y;
    double xf = registradores[reg2].x, yf = registradores[reg2].y;

    const Vertice* origem  = mapa_vertice_mais_proximo(m, x0, y0);
    const Vertice* destino = mapa_vertice_mais_proximo(m, xf, yf);

    // caminho mais curto
    Caminho* cd = dijkstra(m, origem, DISTANCIA);
    PassoCaminho* pd = caminho_reconstruir(cd, destino);
    if(pd == NULL) {
        // TODO: txt destino inacessivel
    } else {
        svg_desenhar_percurso(pd, cc, x0, y0, xf, yf, caminho_custo(cd, destino) * 0.01);
        // TODO: txt descrever percurso
        caminho_lista_fechar(pd);
    }
    caminho_fechar(cd);

    // caminho mais rapido
    Caminho* ct = dijkstra(m, origem, TEMPO);
    PassoCaminho* pt = caminho_reconstruir(ct, destino);
    if(pt == NULL) {
        // TODO: txt destino inacessivel
    } else {
        svg_desenhar_percurso(pt, cr, x0, y0, xf, yf, caminho_custo(ct, destino) * 0.01);
        // TODO: txt descrever percurso
        caminho_lista_fechar(pt);
    }
    caminho_fechar(ct);
}

static int get_registrador(const char* s) {
    if(s == NULL || (s[0] != 'R' && s[0] != 'r'))
        return -1;
    int n = atoi(&s[1]);
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
            char sreg[4], cep[CEP_TAM], face;
            int num;
            if(sscanf(linha, "%*s %3s %31s %c %d", sreg, cep, &face, &num) == 4) {
                int reg = get_registrador(sreg);
                if(reg != -1)
                    cmd_o(reg, cep, face, num);
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
            char sreg1[4], sreg2[4], cc[COR_TAM], cr[COR_TAM];
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
