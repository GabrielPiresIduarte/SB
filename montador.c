#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

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
*/




typedef struct tokenNo //Cada elemento corresponde a um token
{
	int tipo;
	char *string;
	struct tokenNo *prox;
} Token;

typedef struct listaToken //Cada elemento corresponde a uma linha
{
	short int qtdToken;
	int linhaOriginal; //De qual linha do arquivo original foram retirados os tokens;
	Token *token; //acessar tokens individualmente
	struct listaToken *proximaLinha;
} Tokens;

typedef struct diretivaPre
{
	char nomeRotulo [20];
	int operando;
	struct diretivaPre *prox;
} DiretivaEQU;

DiretivaEQU* diretivasPreprocessamento (DiretivaEQU *lista, Tokens *listaDeTokens, short int *flagDiretivas);
DiretivaEQU* adicionaDiretivaPre (DiretivaEQU *lista, char *nome, short int *flag);
Tokens* scannerPreprocessamento (Tokens* lista, char* linhaLida, int quantia);
Tokens* novaLinhaTokens (Tokens *lista, int linhaOriginal);
short int procuraEQU (DiretivaEQU *lista, char *string);
int preprocessamento (FILE *fp, char *arquivoSaida);
Tokens* insereToken (Tokens *lista, char *string);
int leituraDeLinhaPre (FILE *fp, char *linha);
void liberaEQU (DiretivaEQU *lista);
void liberaTokens (Tokens *lista);
void printTokens (Tokens *lista);
DiretivaEQU* criaDiretivaEQU ();
Tokens* criaTokens ();
Token* criaToken ();

int main(int argc, char** argv)
{
	//Checa quantidade de argumentos
	if (argc != 4)
	{
		printf ("ERRO_1\nQuantidade de argumentos inv�lida");
		return 1;
	}

	//Primeiro argumento - Abertura do arquivo de entrada
	char *nomeArquivo = (char *)malloc ((strlen (argv[1]) + 4) * sizeof (char));
	if(nomeArquivo == NULL)
	{
		printf("ERRO_4\nMem�ria insuficiente");
		return 4;
	}

	strcpy (nomeArquivo, argv[1]);
	strcat (nomeArquivo, ".asm");
	printf ("_%s_\n", nomeArquivo);
	FILE *arqIN = fopen ("teste.asm", "r");
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
	switch(toupper (argv [2][1])) //Deixa segundo caracter do segundo argumento maisculo
	{
	case 'P':
		printf ("Preprocessamento\n");
		preprocessamento (arqIN, argv[3]);
		break;
	case 'M':
		printf ("Preprocessamento\n");
		printf ("Macros\n");
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

	fclose (arqIN);
	return 0;
}

DiretivaEQU* criaDiretivaEQU ()
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
Tokens* scannerPreprocessamento (Tokens* listaDeTokens, char* linhaLida, int quantia)
{
	int i, tamanho = strlen (linhaLida), contador = 0;
	char palavra[21]; //(20 � o maior tamanho de rotulo possivel + ':')

	//Separador de tokens
	for (i = 0; i < tamanho; i++)
	{
		if (linhaLida[i] == ' ')  //Fim do caracter
		{
			palavra[contador] = '\0'; //Finaliza token
			listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
			contador = 0; //Prepara para novo token
		}
		else if (linhaLida[i] == ',') //Encontrou virgula
		{
		printf ("aqui\n Encontrou virgula");
			short int aux = 0;
			if (i > 0) //N�o � o primeiro elemento
			{
				
				if (linhaLida[i - 1] == ' ') //Antes da virgula tem espa�o
				{
					printf ("virgula antes\n");
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
					printf ("virgula depois\n");
					if (contador > 0) //Ele tava processando um token
					{
						palavra[contador + 1] = '\0'; //Finaliza token
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
						contador = 0; //Prepara para novo token
					}
					palavra[aux + 1] = ' ';
					palavra[aux + 2] = '\0'; //Prepara para finalizar token da virgula
				}
				else //N�o tem espa�o depois da virgula
					palavra [aux + 1] = '\0'; //Prepara para finalizar token da virgula
			}
			else //Virgula � o ultimo caracter
				palavra [aux + 1] = '\0'; //Prepara para finalizar token da virgula
			palavra[aux] = ',';
			listaDeTokens = insereToken (listaDeTokens, palavra);
		}
		else //Copia caracter
		{
			palavra[contador] = linhaLida [i];
			contador++;
		}
	}
	printTokens (listaDeTokens);
	return listaDeTokens;
}

void printTokens (Tokens *lista){
	Tokens *temp;
	Token *aux;
	temp = lista;
	while (temp != NULL){
		aux = temp->token;
		printf ("Linha: %d", temp->linhaOriginal);
		while (aux != NULL){
			printf ("\t%d: ", aux->tipo);
			if (aux->string != NULL){
				printf ("'%s'", aux->string);
			}
			aux = aux->prox;
		}
		printf ("\n");
		temp = temp->proximaLinha;
	}
}

int preprocessamento (FILE *fp, char *arquivoSaida)
{
	int qtdLinhasLidas = 0;
	short int flagDiretivas = 0, flagFalsoIF = 0;
	char *nomeArquivo, *linhaLida;
	DiretivaEQU *listaDiretivasEQU = criaDiretivaEQU();
	Tokens *listaDeTokensPre = criaTokens();
	listaDeTokensPre = novaLinhaTokens(listaDeTokensPre, -1);

	//Cria arquivo de saida
	nomeArquivo = (char *)calloc ((strlen (arquivoSaida) + 4), sizeof (char));
	strcpy (nomeArquivo, arquivoSaida);
	strcat (nomeArquivo, ".pre");
	FILE *saida = fopen (nomeArquivo, "w");
	if (saida == NULL)
	{
		printf ("ERRO_5\nErro ao gerar arquivo de sa�da do preprocessamento");
		return 5;
	}
	free (nomeArquivo);


	//Leitura de linhas
	linhaLida = (char *)calloc (100, sizeof (char)); //Cria uma string com tamanho de 100 para ler linha
	while (qtdLinhasLidas != -1)
	{
		
			
		//Leitura de linha, ignorando comentario e espa�os extras!
		qtdLinhasLidas = leituraDeLinhaPre (fp, linhaLida);
		
		printf ("Quantidade de linhas lidas: %d\n", qtdLinhasLidas);
		//linhaLida = (char *)realloc (linhaLida, strlen (linhaLida) * sizeof (char)); //Diminui tamanho da string
		if (listaDeTokensPre->linhaOriginal == -1){ //corre��o da primeira linha. melhorar aqui
			listaDeTokensPre->linhaOriginal = qtdLinhasLidas;
		}
		printf ("Quantidade de linhas no token: %d\n", listaDeTokensPre->linhaOriginal);

		//Execu��o das diretivas EQU e IF
		if (qtdLinhasLidas != -1)
		{
			listaDeTokensPre = scannerPreprocessamento (listaDeTokensPre, linhaLida, qtdLinhasLidas);
			listaDiretivasEQU = diretivasPreprocessamento (listaDiretivasEQU, listaDeTokensPre, &flagDiretivas);
			printf ("Linhas lida: %s\n", linhaLida);

			switch (flagDiretivas)
			{
			case 0:
				//Adiciona linha ao arquivo de sa�da
				if (flagFalsoIF == 0) //Comando anterior N�O era um if com condi��o falsa
				{
					
					listaDeTokensPre = novaLinhaTokens(listaDeTokensPre, qtdLinhasLidas+listaDeTokensPre->linhaOriginal);
					printf ("Nova lina original: %d\n", qtdLinhasLidas+listaDeTokensPre->linhaOriginal);
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
	}
	free (linhaLida);
	fclose (saida);
	rewind (fp);
	return 0;
}

DiretivaEQU* diretivasPreprocessamento (DiretivaEQU *lista, Tokens *listaDeTokens, short int *flagDiretivas)
{
	short int contador = 0;
	Token *temp;
	temp = listaDeTokens->token;
	while (temp != NULL)
	{
		if (temp->tipo == 10) //EQU
		{
			if (contador != 1 || listaDeTokens->qtdToken != 3) //EQU n�o � o segundo token da linha
				printf ("Erro 8 - Uso inv�lido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", listaDeTokens->linhaOriginal);
			else if (temp->tipo != ROTULO && temp->tipo > 2)//Token anterior n�o � rotulo, nem virgula
				printf ("Erro 9 - Uso inv�lido de instru��o - Linha: %d\n", listaDeTokens->linhaOriginal);
			else if (temp->tipo <= 2)
				printf ("Erro 10 - Virgula inesperada - Linha: %d\n", listaDeTokens->linhaOriginal);
			else if(temp->prox->tipo != NUMERO)
				printf ("Erro 8 - Uso inv�lido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", listaDeTokens->linhaOriginal);
			else  //n�o tem erro...ainda:
			{
				short int flag = 0;
				lista = adicionaDiretivaPre (lista, temp->string, &flag);
				if (flag)
					printf ("Erro 12 - Rotulo Repetido - Linha: %d\n", listaDeTokens->linhaOriginal);
			}
			*flagDiretivas = 2;
			return lista;
		}
		else if (temp->tipo == 11) //IF
		{
			if (contador != 0 || listaDeTokens->qtdToken != 2) //If n�o � o primeiro da linha ou tem mais de 3 elementos
				printf ("Erro 11 - Uso inv�lido da diretiva IF, use IF OPERANDO - Linha: %d\n", listaDeTokens->linhaOriginal);
			else if (temp->prox->tipo != ROTULO || temp->prox->tipo != NUMERO)
				printf ("Erro 11 - Uso inv�lido da diretiva IF, use IF OPERANDO - Linha: %d\n", listaDeTokens->linhaOriginal);
			else
			{
				short int resultado = procuraEQU (lista, temp->prox->string);
				if (resultado == -1)
				{
					printf ("Erro 7 - Rotulo N�o Declarado - Linha: %d\n", listaDeTokens->linhaOriginal);
					*flagDiretivas = 1;
				}
				else if (resultado == 0) //Operando diferente de 1
					*flagDiretivas = 1;
				else //Operando igual a 1
					*flagDiretivas = 2;
			}
			return lista;
		}
		else
		{
			contador++;
			temp = temp->prox;
		}

	}
	*flagDiretivas = 0;
	return lista;
}

DiretivaEQU* adicionaDiretivaPre (DiretivaEQU *lista, char *nome, short int *flag)
{
	DiretivaEQU *novo = (DiretivaEQU *)malloc(sizeof (DiretivaEQU));
	strcpy (novo->nomeRotulo, nome);
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
		short int res = strcmp (temp->nomeRotulo, nome);
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
	}
	ant->prox = novo;
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
			if (temp->operando == 1)
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
		", ", /*2*/
		",", /*3*/
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
		else if (strcmp (string, tabelaTokens[i]) > 0)
		{
			return 27;
		}
	}
	return 27;
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
	unsigned short int comentario = 0, contador = 0, quantidadeLinhas = 1;
	do
	{
		novoC = toupper (fgetc(fp));
	}
	while (novoC == ' ' || novoC == '\t');
	while (novoC != EOF && novoC != '\n')  //Enquanto n�o chegar no fim do arquivo e linha
	{
		if (!comentario) //Se n�o � comentario
		{
			if (novoC == ';') //Verifica se � inicio de comentario
			{
				comentario = 1; //Set flag de comentario
				novoC = toupper (fgetc(fp));
				continue;
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


			else//Adiciona caracter lido � string
			{
				linha [contador] = novoC;
				contador++;
			}
		}
		novoC = toupper (fgetc(fp)); //Le mais um caracter
	}

	//Se n�o foi lida nenhuma linha, chamada recursiva at� ler uma ou
	if (contador == 0)
	{
		unsigned short int resposta;
		resposta = leituraDeLinhaPre (fp, linha);
		if (resposta == -1)  //Chegou ao fim do arquivo
		{
			return -1;
		}
		else //Leu uma linha
		{
			quantidadeLinhas += resposta;
		}
	}
	linha [contador] = '\0'; //Fim da string

	if (novoC == EOF)  //Fim do arquivo
	{
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
	novo->token = criaToken ();
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
	lista->qtdToken++;
	if (string != NULL) //Somente se for um rotulo
	{
		novo->string = (char *)malloc(strlen (string) * sizeof (char));
		strcpy (novo->string, string);
	}
	else
	{
		novo->string = NULL;
	}
	Token *temp;
	temp = lista->token;
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
		novo->prox = lista->token;
		lista->token = novo;
	}
	return lista;
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
{
	Tokens *temp;
	Token *temp2, *temp3;

	while (lista != NULL)
	{
		temp = lista;
		while (temp3 != NULL)
		{
			temp2 = temp->token;
			temp3 = temp2->prox;
			if (temp2->string != NULL)
				free (temp2->string);
			free (temp2);
		}
		free (temp3);
		lista = lista->proximaLinha;
		free (temp);
	}
}

