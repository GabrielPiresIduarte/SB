#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
Tabela de erros:
Erro 1 - Quantidade invalidade de argumentos
Erro 2 - Arquivo de entrada invalido
Erro 3 - Comando de argumetno invalido

*/

int main(int argc, char** argv)
{
	
	if (argc != 3)
	{
		printf ("ERRO_1");
		printf ("\nQuantidade de argumentos inválida");
		return 1;
	}
	//Primeiro argumento - Abertura do arquivo de entrada
	FILE *arqIN = fopen (strcat (argv [1], ".asm"), "r");
	if (arqIN == NULL)
	{
		printf ("ERRO_2");
		printf ("\nArquivo de entrada inválido ou inexistente");
		return 2;
	}
	
	//Segundo argumento - Operação
	switch(toupper (argv [2][1]))
	{
	case 'P':
		printf ("Preprocessamento\n.");
		break;
	case 'M':
		printf ("Preprocessamento\n.");
		printf ("Macros\n.");
		break;
	case 'O':
		printf ("Preprocessamento\n.");
		printf ("Macros\n.");
		printf ("CodigoObjeto\n.");
		break;
	default:
		printf("Erro_3");
		printf ("\nComando de argumetno invalido");
		break;
	}

	

	fclose (arqIN);
	return 0;
}

