#include "svg.h"
#include "mapa_viario.h"
#include "comp_conexos.h"
#include "dijkstra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SVG_MAX_CORES 12

static FILE* svg_arquivo = NULL;
static double svg_altura = 0.0;

static const char* cores_componentes[SVG_MAX_CORES] = {"red", "blue", "green", "orange", "purple", "cyan", "magenta", 
    "yellow", "brown", "pink", "lime", "teal"};

// contador global para IDs únicos de path
static int svg_path_id = 0;

int svg_inicializar(const char* caminho, double largura, double altura) {
    if(caminho == NULL || largura <= 0 || altura <= 0) 
        return -1;

    svg_arquivo = fopen(caminho, "w");
    if(svg_arquivo == NULL) 
        return -1;

    svg_altura = altura;

    fprintf(svg_arquivo,"<svg xmlns=\"http://www.w3.org/2000/svg\" "
            " xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
            " width=\"%.2f\" height=\"%.2f\">\n", 
            largura, altura);

    return 0;
}

void svg_finalizar(void) {
    if(svg_arquivo == NULL) 
        return;
    fprintf(svg_arquivo, "</svg>\n");
    fclose(svg_arquivo);
    svg_arquivo = NULL;
    svg_path_id = 0;
}

void svg_desenhar_quadra(const char* cep, double x, double y, double w, double h, const char* cfill, const char* cstrk, const char* sw) {
    if(svg_arquivo == NULL) 
        return;

    fprintf(svg_arquivo,
            "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\""
            " fill=\"%s\" stroke=\"%s\" stroke-width=\"%s\"/>\n",
            x, y, w, h, cfill, cstrk, sw);

    fprintf(svg_arquivo,
            "<text x=\"%.2f\" y=\"%.2f\""
            " text-anchor=\"start\" dominant-baseline=\"hanging\""
            " font-size=\"10\" font-weight=\"bold\" fill=\"black\">%s</text>\n",
            x, y, cep);
}

void svg_desenhar_registrador(double x, int reg) {
    if(svg_arquivo == NULL)
        return;

    // linha vertical pontilhada vermelha
    fprintf(svg_arquivo,
            "<line x1=\"%.2f\" y1=\"0\" x2=\"%.2f\" y2=\"%.2f\" "
            " stroke=\"red\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n",
            x, x, svg_altura);

    // numero do registrador no topo
    fprintf(svg_arquivo, "<text x=\"%.2f\" y=\"12\" fill=\"red\" font-size=\"10\">R%d</text>\n", x, reg);
}

void svg_desenhar_componentes(const Componentes* c) {
    if(svg_arquivo == NULL || c == NULL) 
        return;

    int n = componentes_get_quantidade(c);
    for(int k = 0; k < n; k++) {
        const char* cor = cores_componentes[k % SVG_MAX_CORES];
        double xmin = componentes_get_xmin(c, k);
        double ymin = componentes_get_ymin(c, k);
        double xmax = componentes_get_xmax(c, k);
        double ymax = componentes_get_ymax(c, k);
        double w = xmax - xmin;
        double h = ymax - ymin;

        fprintf(svg_arquivo,
                "<rect x=\"%.2f\" y=\"%.2f\" width=\"%.2f\" height=\"%.2f\" "
                " fill=\"%s\" fill-opacity=\"0.5\" stroke=\"%s\" stroke-width=\"2\"/>\n",
                xmin, ymin, w, h, cor, cor);
    }
}

void svg_desenhar_aresta(const Vertice* origem, const Aresta* a) {
    if(svg_arquivo == NULL || origem == NULL || a == NULL) 
        return;

    const Vertice* dest = aresta_get_destino(a);
    double x1 = vertice_get_x(origem);
    double y1 = vertice_get_y(origem);
    double x2 = vertice_get_x(dest);
    double y2 = vertice_get_y(dest); 

    fprintf(svg_arquivo,
            "<line x1=\"%.2f\" y1=\"%.2f\" x2=\"%.2f\" y2=\"%.2f\" "
            " stroke=\"red\" stroke-width=\"4\"/>\n",
            x1, y1, x2, y2);
}

void svg_desenhar_percurso(const PassoCaminho* percurso, const char* cor, double x_inicio, double y_inicio, double x_fim, double y_fim, double dur) {
    if(svg_arquivo == NULL || percurso == NULL || cor == NULL) 
        return;

    // monta o <path> com o percurso completo
    char path_id[32];
    snprintf(path_id, sizeof(path_id), "path_%d", svg_path_id++);

    // comeca com a coordenada real de início
    fprintf(svg_arquivo, "<path id=\"%s\" d=\"M %.2f,%.2f", path_id, x_inicio, y_inicio);

    // percorre a lista de passos
    for(const PassoCaminho* p = percurso; p != NULL; p = passo_get_prox(p)) {
        const Vertice* v = passo_get_vertice(p);
        fprintf(svg_arquivo, " L %.2f,%.2f", vertice_get_x(v), vertice_get_y(v));
    }

    // termina na coordenada real de fim
    fprintf(svg_arquivo, " L %.2f,%.2f\" fill=\"none\" stroke=\"%s\" stroke-width=\"3\"/>\n", x_fim, y_fim, cor);

    // placa I no início
    fprintf(svg_arquivo,
        "<text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" font-size=\"12\" "
        "font-weight=\"bold\">I</text>\n",
        x_inicio, y_inicio, cor);

    // placa F no fim
    fprintf(svg_arquivo,
        "<text x=\"%.2f\" y=\"%.2f\" fill=\"%s\" font-size=\"12\" "
        "font-weight=\"bold\">F</text>\n",
        x_fim, y_fim, cor);

    // animação do circulo percorrendo o caminho
    fprintf(svg_arquivo,
        "<circle r=\"5\" fill=\"%s\">\n"
        "  <animateMotion dur=\"%.2fs\" repeatCount=\"indefinite\">\n"
        "    <mpath xlink:href=\"#%s\"/>\n"
        "  </animateMotion>\n"
        "</circle>\n",
        cor, dur, path_id);
}