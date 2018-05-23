#include "preprocessamento.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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


//Preprocessamento ----------------------------------------------------------

DiretivaEQU * criaDiretivaEQU ()
{
	return NULL;
}
void liberaEQU (DiretivaEQU *lista)
{
	DiretivaEQU *temp = lista->prox;
	while (temp != NULL)
	{
		free (lista);
		lista = temp;
		temp = temp->prox;
	}
}


Tokens* criaTokens ()
{
	return NULL;
}

Token* criaToken ()
{
	return NULL;
}

void liberaTokens (Tokens * lista)
//*
{
	Tokens *prox = lista->proximaLinha;
	Token *atual, *seguinte;
	while (prox != NULL){
        prox = lista->proximaLinha;
        atual = lista->token;
        while (atual != NULL)
        {
            if (atual->prox != NULL)
                seguinte = atual->prox;
            else {
                seguinte = NULL;
            }
            if (atual->string != NULL)
            {
                char *tempDEL = atual->string;
                free (tempDEL);
            }
            if (atual != NULL)
                free (atual);
            atual = seguinte;
        }
        if (lista!=NULL)
            free (lista);
        lista = prox;
	}
//*/
}


void printTokens (Tokens *lista)
{
	printf ("Impressao Tokens\n");
	Tokens *temp;
	Token *aux;
	temp = lista;
	while (temp != NULL)
	{
		aux = temp->token;
		printf ("Linha: %d", temp->linhaOriginal);
		while (aux != NULL)
		{
			printf ("\t%d: ", aux->tipo);
			if (aux->string != NULL)
			{
				printf ("'%s'", aux->string);
			}
			aux = aux->prox;
		}
		printf ("\n");
		temp = temp->proximaLinha;
	}
}


Tokens* scannerPreprocessamento (Tokens* listaDeTokens, char* linhaLida, int quantia)
{
	int i, tamanho = strlen (linhaLida), contador = 0;
	char palavra[21]; //(20 � o maior tamanho de rotulo possivel + ':')
	//Separador de tokens
	for (i = 0; i <= tamanho; i++)
	{
		if (linhaLida[i] == ' ' || linhaLida[i] == '\0') //Fim do caracter
		{
			palavra[contador] = '\0'; //Finaliza token
			if (palavra[0] != '\0') {
				listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
			}
			contador = 0; //Prepara para novo token
		}
		else if (linhaLida[i] == ',') //Encontrou virgula
		{
			short int aux = 0;
			if (i > 0) //N�o � o primeiro elemento
			{
				if (linhaLida[i - 1] == ' ') //Antes da virgula tem espa�o
				{
					if (contador > 0) //Ele tava processando um token
					{
						palavra[contador + 1] = '\0'; //Finaliza token
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
						contador = 0; //Prepara para novo token
					}
					palavra[aux] = ' '; //Adiciona espa�o no token da virgula
					aux++;
				}
			}

			if (i + 1 <= tamanho) //Ainda tem pelo menos mais um char
			{
				if (linhaLida[i + 1] == ' ') //Depois da virgula tem espa�o
				{
					if (contador > 0) //Ele tava processando um token
					{
						palavra[contador] = '\0'; //Finaliza token
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
						contador = 0; //Prepara para novo token
					}
					palavra[aux + 1] = ' ';
					palavra[aux + 2] = '\0'; //Prepara para finalizar token da virgula
				}
				else //N�o tem espa�o depois da virgula
				{
					if (contador > 0)
					{
						palavra [contador] = '\0'; //Prepara para finalizar token da virgula
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
					}
					palavra [aux + 1] = '\0';
					contador = 0; //Prepara para novo token
				}
			}
			else //Virgula � o ultimo caracter
			{
				palavra [aux + 1] = '\0'; //Prepara para finalizar token da virgula
			}
			palavra[aux] = ',';
			listaDeTokens = insereToken (listaDeTokens, palavra);
		}
		else //Copia caracter
		{
			palavra[contador] = linhaLida [i];
			contador++;
		}
	}
	//printTokens (listaDeTokens);
	return listaDeTokens;
}

Tokens* preprocessamento (FILE *fp, char *arquivoSaida, Tokens *listaDeTokensPre)
{
	int qtdLinhasLidas = 0;
	short int flagDiretivas = 0, flagFalsoIF = 0, aux = 0;
	char *nomeArquivo, *linhaLida;
	DiretivaEQU *listaDiretivasEQU = criaDiretivaEQU();


	//Cria arquivo de saida
	nomeArquivo = (char *)calloc ((strlen (arquivoSaida) + 5), sizeof (char));
	strcpy (nomeArquivo, arquivoSaida);
	strcat (nomeArquivo, ".pre");
	FILE *saida = fopen (nomeArquivo, "w");
	if (saida == NULL)
	{
		printf ("ERRO_5\nErro ao gerar arquivo de sa�da do preprocessamento");
		return NULL;
	}
	free (nomeArquivo);


	//Leitura de linhas
	linhaLida = (char *)calloc (100, sizeof (char)); //Cria uma string com tamanho de 100 para ler linha
	while (qtdLinhasLidas != -1)
	{

		//Leitura de linha, ignorando comentario e espa�os extras!
		qtdLinhasLidas = leituraDeLinhaPre (fp, linhaLida);

		//Execu��o das diretivas EQU e IF
		if (strlen (linhaLida) > 0)
		{
			if (aux != 0)
			{
				listaDeTokensPre = novaLinhaTokens(listaDeTokensPre, aux);
			}
			else
			{
				listaDeTokensPre = novaLinhaTokens(listaDeTokensPre, qtdLinhasLidas);
			}
			aux = 0;
			listaDeTokensPre = scannerPreprocessamento (listaDeTokensPre, linhaLida, qtdLinhasLidas);

			listaDiretivasEQU = diretivasPreprocessamento (listaDiretivasEQU, listaDeTokensPre, &flagDiretivas);

			switch (flagDiretivas)
			{
			case 0:
				//Adiciona linha ao arquivo de sa�da
				if (flagFalsoIF == 0) //Comando anterior N�O era um if com condi��o falsa
				{
					fputs (linhaLida, saida);
					fputc ('\n', saida);
				}
				else //Comando anterior ERA um IF com condi��o falsa
				{
					flagFalsoIF = 0;
				}
				break;
			case 1: //IF com condi��o falsa
				flagFalsoIF = 1;
				break;
			case 2: // Lido um EQU ou IF verdadeiro, com erro sintatico ou n�o s� ignora
				break;
			default:
				break;
			}
		}
		else
		{
			aux += qtdLinhasLidas;
		}

	}


	free (linhaLida);
	fclose (saida);
	rewind (fp);
	return listaDeTokensPre;
}

void printDiretivaEQU (DiretivaEQU* lista)
{
	DiretivaEQU* aux = lista;
	while (aux != NULL)
	{
		printf ("Rotulo: %s\t", aux->nomeRotulo);
		printf ("Operando: %s\n", aux->operando);
		aux = aux->prox;
	}
}

DiretivaEQU* diretivasPreprocessamento (DiretivaEQU *lista, Tokens *listaDeTokens, short int *flagDiretivas)
{

	short int contador = 0;
	Token *temp, *ant;
	Tokens *aux = listaDeTokens, *anteriorTokens;
	while (aux->proximaLinha != NULL)
	{
        anteriorTokens = aux;
		aux = aux->proximaLinha;
	}
	temp = aux->token;
	ant = temp;
	while (temp != NULL)
	{
		if (temp->tipo == EQU) //EQU
		{
			if (contador != 1 || aux->qtdToken != 3) //EQU n�o � o segundo token da linha
				printf ("Erro 8 - Uso invalido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", aux->linhaOriginal);
			else if (ant->tipo != ROTULO && ant->tipo > 2)//Token anterior n�o � rotulo, nem virgula
				printf ("Erro 9 - Uso inv�lido de instru��o - Linha: %d\n", aux->linhaOriginal);
			else if (ant->tipo <= 2)
				printf ("Erro 10 - Virgula inesperada - Linha: %d\n", aux->linhaOriginal);
			else if(temp->prox->tipo != NUMERO)
				printf ("Erro 8 - Uso inv�lido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", aux->linhaOriginal);
			else  //n�o tem erro...ainda:
			{
				short int flag = 0;
				if (ant->string[strlen (ant->string)-1] != ':')
                    printf ("Erro lexico, falta : no rotulo - linha: %d\n", aux->linhaOriginal);
                else
                    ant->string[strlen (ant->string)-1] = '\0';


				lista = adicionaDiretivaPre (lista, ant->string, temp->prox->string, &flag);
				anteriorTokens->proximaLinha = NULL;
				liberaTokens(aux);
				aux = anteriorTokens;
				if (flag)
					printf ("Erro 12 - Rotulo Repetido - Linha: %d\n", aux->linhaOriginal);
			}
			*flagDiretivas = 2;
			return lista;
		}
		else if (temp->tipo == IF) //IF
		{
			if (contador != 0 || aux->qtdToken != 2) //If n�o � o primeiro da linha ou tem mais de 3 elementos
			{
				printf ("Erro 11 - Uso inv�lido da diretiva IF, use IF OPERANDO - Linha: %d\n", aux->linhaOriginal);
				*flagDiretivas = 1;
			}
			else if (temp->prox->tipo != ROTULO && temp->prox->tipo != NUMERO)
			{
				printf ("Erro 11 - Uso inv�lido da diretiva IF, use IF OPERANDO - Linha: %d\n", aux->linhaOriginal);
				printf ("Erro 9 - Uso inv�lido de instru��o - Linha: %d\n", aux->linhaOriginal);
				*flagDiretivas = 1;
			}
			else
			{
				short int resultado = procuraEQU (lista, temp->prox->string);
				if (resultado == -1)
				{
					printf ("Erro 7 - Rotulo N�o Declarado - Linha: %d\n", aux->linhaOriginal);
					*flagDiretivas = 1;
				}
				else if (resultado == 0) //Operando diferente de 1
					*flagDiretivas = 1;
				else //Operando igual a 1
					*flagDiretivas = 2;
			}
            anteriorTokens->proximaLinha = NULL;
			liberaTokens(aux);
			aux = anteriorTokens;

			return lista;
		}
		else
		{
			contador++;
			ant = temp;
			temp = temp->prox;
		}

	}
	*flagDiretivas = 0;
	return lista;
}



DiretivaEQU* adicionaDiretivaPre (DiretivaEQU *lista, char *nome, char *operando, short int *flag)
{
	DiretivaEQU *novo = (DiretivaEQU *)malloc(sizeof (DiretivaEQU));
	strcpy (novo->nomeRotulo, nome);
	strcpy (novo->operando, operando);

	if (lista == NULL)
	{
		novo->prox = lista;
		lista = novo;
		*flag = 0;
		return lista;
	}
	DiretivaEQU *temp, *ant;
	temp = lista;
	ant = lista;
	while (temp != NULL)
	{
		short int res = strcmp (temp->nomeRotulo, novo->nomeRotulo);
		if (res == 0)
		{
			*flag = 1;
			free (novo);
			return lista;
		}
		else if (res < 0)
		{
			if (temp == lista)
			{
				novo->prox = lista;
				lista = novo;
				*flag = 0;
				return lista;
			}
			else
			{
				novo->prox = temp;
				ant->prox = novo;
				*flag = 0;
				return lista;
			}
		}
		else
		{
			ant = temp;
			temp = temp->prox;
		}
		ant->prox = novo;
	}

	*flag = 0;
	return lista;
}

short int procuraEQU (DiretivaEQU *lista, char *string)
{
	DiretivaEQU *temp = lista;
	while (temp != NULL)
	{
		int res = strcmp(string, temp->nomeRotulo);
		if (res == 0)
			if (strcmp (temp->operando, "1") == 0)
				return 1;
			else
				return 0;
		else if (res > 0)
			return -1;
		else
			temp = temp->prox;
	}
	return -1;
}


//*/

int tipoToken (char * string)
{
	char tabelaTokens [27][13] =
	{
		" ,", /*0*/
		" , ", /*1*/
		",", /*2*/
		", ", /*3*/
		"ADD", /*4*/
		"CONST", /*5*/
		"COPY", /*6*/
		"DATA", /*7*/
		"DIV", /*8*/
		"ENDMACRO", /*9*/
		"EQU", /*10*/
		"IF", /*11*/
		"INPUT", /*12*/
		"JMP", /*13*/
		"JMPN", /*14*/
		"JMPP", /*15*/
		"JMPZ", /*16*/
		"LOAD", /*17*/
		"MACRO", /*18*/
		"MULT", /*19*/
		"OUTPUT", /*20*/
		"SECTION", /*21*/
		"SPACE", /*22*/
		"STOP", /*23*/
		"STORE", /*24*/
		"SUB", /*25*/
		"TEXT", /*26*/
	};
	int i;
	for (i = 0; i <= 26; i++)
	{
		if (strcmp (string, tabelaTokens[i]) == 0)
		{
			return i;
		}
		else if (strcmp (string, tabelaTokens[i]) < 0)
		{
			if (isNumber (string))
				return NUMERO;
			return ROTULO;
		}
	}
	if (isNumber (string))
		return NUMERO;
	return ROTULO;
}


int isNumber (char *string)
{
	int tamanho = strlen (string), i;
	for (i = 0; i < tamanho; i++)
	{
		if (string [i] < '0' || string [i] > '9')
			return 0;
	}
	return 1;
}

/*
Fun��o de leitura de linha:
	Recebe arquivo e ponteiro de string, elimina coment�rio e espa�os extras.
	Retorna -1 caso o arquivo TENHA chegado ao fim, caso contrario quantidade lida de
	linhas at� encontrar uma que n�o seja somente comentario.
*/
int leituraDeLinhaPre (FILE * fp, char * linha)
{
	char novoC;
	unsigned short int contador = 0, quantidadeLinhas = 1;
	if (feof (fp))
	{
		return -1;
	}
	do
	{
		novoC = toupper (fgetc(fp));
	}
	while ((novoC == ' ' || novoC == '\t') && novoC != EOF);

	while (novoC != EOF)  //Enquanto n�o chegar no fim do arquivo e linha
	{

		if (novoC == ';') //Verifica se � inicio de comentario
		{
			while (novoC != '\n') //L� at� o final da linha
				novoC = (fgetc(fp));
			if (contador != 0) // Se leu uma string termina
				break;
			while ((novoC == ' ' || novoC == '\t') && novoC != EOF) //Se n�o tinha lido string tira espa�o inicial
			{
				novoC = (fgetc(fp));
			}
			novoC = toupper (novoC);

		}
		//Elimina espa�o desnecessarios
		if (contador > 0)
		{
			if (novoC == ' ' && linha [contador - 1] == novoC)
			{
				novoC = toupper (fgetc(fp));
				continue;
			}
			else//Adiciona caracter lido � string
			{
				linha [contador] = novoC;
				contador++;
			}
		}
		else if (contador == 0 && novoC == '\n')
		{
			quantidadeLinhas++;
		}
		else//Adiciona caracter lido � string
		{
			linha [contador] = novoC;
			contador++;
		}
		novoC = toupper (fgetc(fp)); //Le mais um caracter
		if (novoC == '\t') //Para eliminar tabula��es extras
		{
			novoC = ' ';
		}
		else if (novoC == '\n'  && contador != 0)
		{
			break;
		}
		while (novoC == '\n' && contador == 0)
		{
			novoC = toupper (fgetc(fp));
			quantidadeLinhas++;
		}
	}
	linha [contador] = '\0'; //Fim da string

	if (novoC == EOF)  //Fim do arquivo
	{
		if (contador != 0)
		{
			fseek(fp, 0, SEEK_END);
			return quantidadeLinhas;
		}
		return -1;
	}
	return quantidadeLinhas; //Retorna quantidade de linhas lidas at� achar uma "v�lida"
}

/*
Insere uma nova linha de codigo na lista de tokens
*/
Tokens* novaLinhaTokens (Tokens * lista, int linhaOriginal)
{
	Tokens *novo = (Tokens *)malloc(sizeof (Tokens));
	novo->linhaOriginal = linhaOriginal;
	novo->token = NULL;
	novo->qtdToken = 0;
	if (lista == NULL)
	{
		novo->proximaLinha = lista;
		lista = novo;
	}
	else
	{
		Tokens *temp;
		temp = lista;
		while (temp->proximaLinha != NULL)
		{
			temp = temp->proximaLinha;
		}
		novo->linhaOriginal += temp->linhaOriginal;
		novo->proximaLinha = temp->proximaLinha;
		temp->proximaLinha = novo;
	}
	return lista;
}

/*
Insere um novo token encontrado em uma linha
*/


Tokens* insereToken (Tokens * lista, char * string)
{
	Token *novo = (Token *)malloc(sizeof (Token));
	novo->tipo = tipoToken (string);
	novo->prox = NULL;
	if (string != NULL) //Somente se for um rotulo
	{
		novo->string = (char *)malloc((strlen (string)+1) * sizeof (char));
		strcpy (novo->string, string);

	}
	else
	{

		novo->string = NULL;
	}

	Token *temp;
	Tokens *aux = lista;
	while (aux->proximaLinha != NULL)
	{

		aux = aux->proximaLinha;
	}

	aux->qtdToken++;
	temp = aux->token;
	if (temp != NULL)
	{

		while (temp->prox != NULL)
		{
			temp = temp->prox;
		}

		novo->prox = temp->prox;
		temp->prox = novo;
	}
	else
	{

		novo->prox = aux->token;
		aux->token = novo;
	}

	return lista;
}
