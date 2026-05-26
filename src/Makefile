PROJ_NAME=ted

ALUNO=Guilherme Matheus Fraga
LIBS=
OBJETOS=hash_extensivel.o cidade.o pessoas.o geo.o pm.o svg.o txt.o qry.o ted.o

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
pessoas.o: pessoas.c pessoas.h hash_extensivel.h
geo.o: geo.c geo.h cidade.h
pm.o: pm.c pm.h pessoas.h
svg.o: svg.c svg.h
txt.o: txt.c txt.h pessoas.h
qry.o: qry.c qry.h cidade.h pessoas.h svg.h txt.h
ted.o: ted.c cidade.h pessoas.h geo.h pm.h qry.h svg.h txt.h

# Testes unitarios
tst_hash_extensivel: tst/teste_hash_extensivel.c hash_extensivel.o
	$(CC) $(CFLAGS) -o tst/tst_hash_extensivel tst/teste_hash_extensivel.c hash_extensivel.o $(UNITY)

tst_cidade: tst/teste_cidade.c cidade.o hash_extensivel.o svg.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_cidade tst/teste_cidade.c cidade.o hash_extensivel.o svg.o $(UNITY)

tst_pessoas: tst/teste_pessoas.c pessoas.o hash_extensivel.o 
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_pessoas tst/teste_pessoas.c pessoas.o hash_extensivel.o $(UNITY)

tst_geo: tst/teste_geo.c geo.o cidade.o hash_extensivel.o svg.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_geo tst/teste_geo.c geo.o cidade.o hash_extensivel.o svg.o $(UNITY)

tst_pm: tst/teste_pm.c pm.o pessoas.o hash_extensivel.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_pm tst/teste_pm.c pm.o pessoas.o hash_extensivel.o $(UNITY)

tst_svg: tst/teste_svg.c svg.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_svg tst/teste_svg.c svg.o $(UNITY)

tst_txt: tst/teste_txt.c txt.o pessoas.o hash_extensivel.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_txt tst/teste_txt.c txt.o pessoas.o hash_extensivel.o $(UNITY)

tst_qry: tst/teste_qry.c qry.o cidade.o pessoas.o svg.o txt.o hash_extensivel.o
	$(CC) $(CFLAGS) -DUNITY_INCLUDE_DOUBLE -o tst/tst_qry tst/teste_qry.c qry.o cidade.o pessoas.o svg.o txt.o hash_extensivel.o $(UNITY)

# Compila e executa todos os testes unitários
tstall: tst_hash_extensivel tst_cidade tst_pessoas tst_geo tst_pm tst_svg tst_txt tst_qry
	./tst/tst_hash_extensivel
	./tst/tst_cidade
	./tst/tst_pessoas
	./tst/tst_geo
	./tst/tst_pm
	./tst/tst_svg
	./tst/tst_txt
	./tst/tst_qry

# Limpa arquivos .o e executáveis
clean:
	rm -f *.o $(PROJ_NAME) tst/tst_cidade tst/tst_pessoas tst/tst_geo tst/tst_pm tst/tst_svg tst/tst_txt tst/tst_qry tst/tst_hash_extensivel