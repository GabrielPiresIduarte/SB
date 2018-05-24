all: program clean executar

program: montador.o preprocessamento.o objeto.o macros.o semantico.o
	gcc -Wall montador.o preprocessamento.o objeto.o macros.o semantico.o -o montador

preprocessamento.o: preprocessamento.h
	gcc -c preprocessamento.c

objeto.o: preprocessamento.h objeto.h
	gcc -c objeto.c

montador.o: preprocessamento.h objeto.h macros.h semantico.h
	gcc -c montador.c

semantico.o: preprocessamento.o semantico.h
	gcc	-c semantico.c

macros.o: preprocessamento.h macros.h
	gcc -c macros.c 


executar:
	valgrind ./montador teste -o saida --leak-check=yes

clean:
	rm *.o
