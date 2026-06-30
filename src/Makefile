PROJ_NAME=ted

ALUNO=Guilherme Matheus Fraga
LIBS=
OBJETOS=hash_extensivel.o cidade.o mapa_viario.o dijkstra.o comp_conexos.o agm.o geo.o via.o svg.o qry.o txt.o ted.o fila_prioridade.o

CC=gcc
CFLAGS=-ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS=-O0

UNITY=tst/unity/unity.c

$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Dependencias de cada modulo
hash_extensivel.o: hash_extensivel.c hash_extensivel.h
cidade.o: cidade.c cidade.h hash_extensivel.h svg.h
mapa_viario.o: mapa_viario.c mapa_viario.h
dijkstra.o: dijkstra.c dijkstra.h mapa_viario.h fila_prioridade.h
comp_conexos.o: comp_conexos.c comp_conexos.h mapa_viario.h
agm.o: agm.c agm.h mapa_viario.h
geo.o: geo.c geo.h cidade.h
via.o: via.c via.h mapa_viario.h
svg.o: svg.c svg.h mapa_viario.h comp_conexos.h dijkstra.h
txt.o: txt.c txt.h dijkstra.h
qry.o: qry.c qry.h cidade.h mapa_viario.h dijkstra.h comp_conexos.h agm.h svg.h txt.h
ted.o: ted.c cidade.h mapa_viario.h geo.h via.h qry.h svg.h txt.h
fila_prioridade.o: fila_prioridade.c fila_prioridade.h

# Testes unitarios
tst_hash_extensivel: tst/teste_hash_extensivel.c hash_extensivel.o
	$(CC) $(CFLAGS) -o tst/tst_hash_extensivel tst/teste_hash_extensivel.c hash_extensivel.o $(UNITY)

tst_cidade: tst/teste_cidade.c cidade.o hash_extensivel.o svg.o comp_conexos.o mapa_viario.o dijkstra.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_cidade tst/teste_cidade.c cidade.o hash_extensivel.o svg.o comp_conexos.o mapa_viario.o dijkstra.o fila_prioridade.o $(UNITY)

tst_mapa_viario: tst/teste_mapa_viario.c mapa_viario.o 
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_mapa_viario tst/teste_mapa_viario.c mapa_viario.o $(UNITY)

tst_geo: tst/teste_geo.c geo.o cidade.o hash_extensivel.o svg.o comp_conexos.o mapa_viario.o dijkstra.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_geo tst/teste_geo.c geo.o cidade.o hash_extensivel.o svg.o comp_conexos.o mapa_viario.o dijkstra.o fila_prioridade.o $(UNITY)

tst_via: tst/teste_via.c via.o mapa_viario.o 
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_via tst/teste_via.c via.o mapa_viario.o $(UNITY)

tst_dijkstra: tst/teste_dijkstra.c dijkstra.o mapa_viario.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_dijkstra tst/teste_dijkstra.c dijkstra.o mapa_viario.o fila_prioridade.o $(UNITY)

tst_comp_conexos: tst/teste_comp_conexos.c comp_conexos.o mapa_viario.o 
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_comp_conexos tst/teste_comp_conexos.c comp_conexos.o mapa_viario.o $(UNITY)

tst_agm: tst/teste_agm.c agm.o mapa_viario.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_agm tst/teste_agm.c agm.o mapa_viario.o $(UNITY)

tst_svg: tst/teste_svg.c svg.o mapa_viario.o comp_conexos.o dijkstra.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_svg tst/teste_svg.c svg.o mapa_viario.o comp_conexos.o dijkstra.o fila_prioridade.o $(UNITY)

tst_txt: tst/teste_txt.c txt.o mapa_viario.o dijkstra.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_txt tst/teste_txt.c txt.o mapa_viario.o dijkstra.o fila_prioridade.o $(UNITY)

tst_qry: tst/teste_qry.c qry.o mapa_viario.o svg.o txt.o cidade.o dijkstra.o comp_conexos.o agm.o via.o hash_extensivel.o fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_qry tst/teste_qry.c qry.o cidade.o mapa_viario.o svg.o txt.o dijkstra.o comp_conexos.o agm.o via.o hash_extensivel.o fila_prioridade.o $(UNITY)

tst_fila_prioridade: tst/teste_fila_prioridade.c fila_prioridade.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_fila_prioridade tst/teste_fila_prioridade.c fila_prioridade.o $(UNITY)

# Compila e executa todos os testes unitários
tstall: tst_hash_extensivel tst_cidade tst_mapa_viario tst_dijkstra tst_comp_conexos tst_agm tst_geo tst_via tst_svg tst_txt tst_qry tst_fila_prioridade
	./tst/tst_hash_extensivel
	./tst/tst_cidade
	./tst/tst_mapa_viario
	./tst/tst_dijkstra
	./tst/tst_comp_conexos
	./tst/tst_agm
	./tst/tst_geo
	./tst/tst_via
	./tst/tst_qry
	./tst/tst_svg
	./tst/tst_txt
	./tst/tst_fila_prioridade

# Limpa arquivos .o e executáveis
clean:
	rm -f *.o $(PROJ_NAME) tst/tst_cidade tst/tst_mapa_viario tst/tst_dijkstra tst/tst_comp_conexos tst/tst_agm tst/tst_geo tst/tst_via tst/tst_svg tst/tst_qry tst/tst_hash_extensivel tst/tst_txt tst/tst_fila_prioridade