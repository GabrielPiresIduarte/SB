#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "preprocessamento.h"

#define ADD 4
#define CONST 5
#define COPY 6
#define DATA 7
#define DIV 8
#define ENDMACRO 9
#define EQU 10
#define IF 11
#define INPUT 12
#define JMP 13
#define JMPN 14
#define JMPP 15
#define JMPZ 16
#define LOAD 17
#define MACRO 18
#define MULT 19
#define OUTPUT 20
#define SECTION 21
#define SPACE 22
#define STOP 23
#define STORE 24
#define SUB 25
#define TEXT 26
#define ROTULO 27
#define NUMERO 28

/*
Tabela de erros:
Erro 1 - Quantidade invalidade de argumentos
Erro 2 - Arquivo de entrada invalido
Erro 3 - Argumento de comando invalido
Erro 4 - Mem�ria insuficiente
Erro 5 - Erro ao gerar arquivo de sa�da do preprocessamento
Erro 6 - Rotulo Inv�lido
Erro 7 - Rotulo N�o Declarado
Erro 8 - Diretiva EQU errada
Erro 9 - Uso inv�lido de instru��o
Erro 10 - Virgula inesperada
Erro 11 - Uso inv�lido da diretiva IF, use IF VALOR
Erro 12 - Rotulo Repetido
Erro 13 - Uso inv�lido de MACRO
Erro 14 - Parametro invalido
*/


int main(int argc, char** argv)
{
	//Checa quantidade de argumentos
	if (argc != 4)
	{
		printf ("ERRO_1\nQuantidade de argumentos inv�lida");
		return 1;
	}

	//Primeiro argumento - Abertura do arquivo de entrada
	char *nomeArquivo = (char *)malloc ((strlen (argv[1]) + 7) * sizeof (char));
	if(nomeArquivo == NULL)
	{
		printf("ERRO_4\nMem�ria insuficiente");
		return 4;
	}
	strcpy (nomeArquivo, argv[1]);
	strcat (nomeArquivo, ".asm");
	FILE *arqIN = fopen (nomeArquivo, "r");

	strcpy (nomeArquivo, argv[3]);
	strcat (nomeArquivo, ".macro");
	FILE *arqOUTmacro = fopen (nomeArquivo, "w");

	if (arqOUTmacro == NULL)
	{
		printf ("ERRO_16\nErro ao gerar arquivo de saida");
		return 2;
	}

	free (nomeArquivo);
	if (arqIN == NULL)
	{
		printf ("ERRO_2\nArquivo de entrada inv�lido ou inexistente");
		return 2;
	}

	//Segundo argumento - Opera��o
	if (argv [2][2] != '\0' || argv [2][0] != '-') //Checa se argumento come�a com '-' e se tem somente uma letra
	{
		printf("Erro_3\nComando de argumento invalido");
		return 3;
	}


	Tokens *listaDeTokens = criaTokens();
	switch(toupper (argv [2][1])) //Deixa segundo caracter do segundo argumento maisculo
	{
	case 'P':
		printf ("Preprocessamento\n");
		listaDeTokens = preprocessamento (arqIN, argv[3], listaDeTokens);
		break;
	case 'M':
		printf ("Macros\n");
		listaDeTokens = preprocessamento (arqIN, argv[3], listaDeTokens);
		listaDeTokens = macros (listaDeTokens, arqOUTmacro);
		break;
	case 'O':
		printf ("Preprocessamento\n");
		printf ("Macros\n");
		printf ("CodigoObjeto\n");
		break;
	default:
		printf("Erro_3\nComando de argumento invalido");
		return 3;
	}
	liberaTokens(listaDeTokens);
	if (arqIN != NULL)
        fclose (arqIN);
	if (arqOUTmacro != NULL)
        fclose (arqOUTmacro);
    return 0;
}

