#include "cidade.h"
#include "mapa_viario.h"
#include "geo.h"
#include "via.h"
#include "qry.h"
#include "svg.h"
//#include "txt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_TAM 512
#define NOME_TAM 128

// remove barra final de um path se houver
static void tratar_path(char *path) {
    int len = strlen(path);
    if(len > 0 && path[len - 1] == '/')
        path[len - 1] = '\0';
}

// extrai nome base do arquivo sem extensão e sem diretório
static void extrair_base(char *base, int base_tam, const char *arq) {
    const char *barra = strrchr(arq, '/');
    const char *inicio = (barra != NULL) ? barra + 1 : arq;

    strncpy(base, inicio, base_tam - 1);
    base[base_tam - 1] = '\0';

    char *ponto = strrchr(base, '.');
    if(ponto != NULL)
        *ponto = '\0';
}

// monta caminho completo: path + "/" + arq (ou só arq se base vazio)
static void montar_caminho(char *path, int path_tam, const char *dir, const char *arq) {
    if(dir != NULL && dir[0] != '\0') {
        char temp[strlen(arq) + 1];
        strncpy(temp, arq, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        while(temp[0] == '.' || temp[0] == '/') {
            memmove(temp, temp + 1, strlen(temp));
        }
        snprintf(path, path_tam, "%s/%s", dir, temp);
    }
    else {
        snprintf(path, path_tam, "%s", arq);
    }
}

int main(int argc, char *argv[]) {

    char bed[PATH_TAM] = "";   // diretório base de entrada (opcional)
    char bsd[PATH_TAM] = "";   // diretório base de saída (obrigatório)
    char arq_geo[PATH_TAM] = "";
    char arq_via[PATH_TAM] = "";
    char arq_qry[PATH_TAM] = "";

    int i = 1;
    while(i < argc) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            strncpy(bed, argv[i + 1], PATH_TAM - 1);
            tratar_path(bed);
        } 
        else if(strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strncpy(bsd, argv[i + 1], PATH_TAM - 1);
            tratar_path(bsd);
        } 
        else if(strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            strncpy(arq_geo, argv[i + 1], PATH_TAM - 1);
        } 
        else if(strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            strncpy(arq_via, argv[i + 1], PATH_TAM - 1);
        } 
        else if(strcmp(argv[i], "-q") == 0 && i + 1 < argc) {
            strncpy(arq_qry, argv[i + 1], PATH_TAM - 1);
        }
        i += 2;
    }

    // -f e -o são obrigatórios
    if(arq_geo[0] == '\0' || bsd[0] == '\0') {
        printf("Uso (parametros entre [] sao opcionais): ted [-e entrada/] -f arq.geo -o saida/ [-q arq.qry] [-v arqvias.via]\n");
        return 1;
    }

    // monta caminhos completos de entrada
    char caminho_geo[PATH_TAM], caminho_via[PATH_TAM], caminho_qry[PATH_TAM];

    montar_caminho(caminho_geo, PATH_TAM, bed, arq_geo);

    if(arq_via[0] != '\0')
        montar_caminho(caminho_via, PATH_TAM, bed, arq_via);

    if(arq_qry[0] != '\0')
        montar_caminho(caminho_qry, PATH_TAM, bed, arq_qry);

    // extrai nomes base para nomear arquivos de saída
    char base_geo[NOME_TAM], base_qry[NOME_TAM];

    extrair_base(base_geo, NOME_TAM, arq_geo);
    extrair_base(base_qry, NOME_TAM, arq_qry);

    // monta caminhos dos arquivos de saída
    char caminho_svg_base[PATH_TAM + NOME_TAM + 4];
    char caminho_svg_qry[PATH_TAM + 2 * NOME_TAM + 4];
    char caminho_txt_qry[PATH_TAM + 2 * NOME_TAM + 4];
    char caminho_hf_quadras[PATH_TAM + NOME_TAM + 11];
    char caminho_hfd_quadras[PATH_TAM + NOME_TAM + 12];

    snprintf(caminho_svg_base, PATH_TAM + NOME_TAM + 4, "%s/%s.svg", bsd, base_geo);
    snprintf(caminho_hf_quadras, PATH_TAM + NOME_TAM + 11, "%s/%s_quadras.hf", bsd, base_geo);
    snprintf(caminho_hfd_quadras, PATH_TAM + NOME_TAM + 12, "%s/%s_quadras.hfd", bsd, base_geo);

    if(arq_qry[0] != '\0') {
        snprintf(caminho_svg_qry, PATH_TAM + 2 * NOME_TAM + 4, "%s/%s-%s.svg", bsd, base_geo, base_qry);
        snprintf(caminho_txt_qry, PATH_TAM + 2 * NOME_TAM + 4, "%s/%s-%s.txt", bsd, base_geo, base_qry);
    }

    // inicializa módulos de dados
    if(cidade_inicializar(caminho_hf_quadras) != 0) {
        printf("Erro ao inicializar hashfile de quadras.\n");
        return 1;
    }

    // processa .geo e .via
    if(svg_inicializar(caminho_svg_base, 50000, 50000) == 0) {
        geo_processar(caminho_geo);
        cidade_desenhar_quadras();
        svg_finalizar();
    } 
    else {
        printf("Erro ao inicializar SVG base.\n");
        return 1;
    }

    MapaViario* m = NULL;
    if(arq_via[0] != '\0')
        m = via_processar(caminho_via);

    // processa .qry e gera SVG + TXT combinados (se houver)
    if(arq_qry[0] != '\0') {
        if(svg_inicializar(caminho_svg_qry, 50000, 50000) != 0) {
            printf("Erro ao inicializar SVG de consultas.\n");
        } 
        //else if(txt_inicializar(caminho_txt_qry) != 0) {
        //    printf("Erro ao inicializar TXT de consultas.\n");
         //   svg_finalizar();
        //} 
        else {          
            cidade_desenhar_quadras();
            qry_processar(caminho_qry, m);
            //txt_finalizar();
            svg_finalizar();
        }
    }

    // finaliza módulos
    cidade_finalizar(caminho_hfd_quadras);
    mapa_fechar(m);

    return 0;
}