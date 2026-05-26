#ifndef GEO_H
#define GEO_H

/*  Um arquivo .geo executa comandos sobre as quadras de uma cidade. 
    A cidade deve ser inicializada antes de processar o arquivo .geo.
    Os comandos do arquivo .geo são:

    q <cep> <x> <y> <w> <h>
    - Insere uma quadra (retangular) com os dados fornecidos. As cores de preenchimento e borda e a largura 
    de borda são as atuais (definidas pelo comando cq ou vazias no início).

    cq <sw> <cfill> <cstrk>
    - Define as cores de preenchimento e borda e a largura de borda atuais para os próximos comandos q.
*/

/*  Le e processa os comandos do arquivo .geo.
    O arquivo deve estar no caminho indicado por *caminho.
    Retorna 0 em sucesso ou -1 se o arquivo não puder ser aberto.
*/
int geo_processar(const char* caminho);

#endif