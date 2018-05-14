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
/*
Tabela de erros:
Erro 1 - Quantidade invalidade de argumentos
Erro 2 - Arquivo de entrada invalido
Erro 3 - Argumento de comando invalido
Erro 4 - Memória insuficiente
Erro 5 - Erro ao gerar arquivo de saída do preprocessamento
Erro 6 - Rotulo Inválido
Erro 7 - Rotulo Não Declarado
Erro 8 - Diretiva EQU errada
Erro 9 - Uso inválido de instrução
Erro 10 - Virgula inesperada
Erro 11 - Uso inválido da diretiva IF, use IF VALOR
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
	char operando [10];
	struct diretivaPre *prox;
} DiretivaEQU;


DiretivaEQU* diretivasPreprocessamento (DiretivaEQU *lista, Tokens *listaDeTokens, short int *flagDiretivas);
DiretivaEQU* adicionaDiretivaPre (DiretivaEQU *lista, char *nome, char *operando, short int *flag);
Tokens* scannerPreprocessamento (Tokens* lista, char* linhaLida, int quantia);
Tokens* novaLinhaTokens (Tokens *lista, int linhaOriginal);
short int procuraEQU (DiretivaEQU *lista, char *string);
int preprocessamento (FILE *fp, char *arquivoSaida);
Tokens* insereToken (Tokens *lista, char *string);
int leituraDeLinhaPre (FILE *fp, char *linha);
void printDiretivaEQU (DiretivaEQU* lista);
void liberaEQU (DiretivaEQU *lista);
void liberaTokens (Tokens *lista);
void printTokens (Tokens *lista);
DiretivaEQU* criaDiretivaEQU ();
int isNumber (char *string);
Tokens* criaTokens ();
Token* criaToken ();

int main(int argc, char** argv)
{
	//Checa quantidade de argumentos
	if (argc != 4)
	{
		printf ("ERRO_1\nQuantidade de argumentos inválida");
		return 1;
	}

	//Primeiro argumento - Abertura do arquivo de entrada
	char *nomeArquivo = (char *)malloc ((strlen (argv[1]) + 4) * sizeof (char));
	if(nomeArquivo == NULL)
	{
		printf("ERRO_4\nMemória insuficiente");
		return 4;
	}
	strcpy (nomeArquivo, argv[1]);
	strcat (nomeArquivo, ".asm");
	printf ("_%s_\n", nomeArquivo);
	FILE *arqIN = fopen ("teste.asm", "r");
	free (nomeArquivo);
	if (arqIN == NULL)
	{
		printf ("ERRO_2\nArquivo de entrada inválido ou inexistente");
		return 2;
	}

	//Segundo argumento - Operação
	if (argv [2][2] != '\0' || argv [2][0] != '-') //Checa se argumento começa com '-' e se tem somente uma letra
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
	char palavra[21]; //(20 é o maior tamanho de rotulo possivel + ':')
	//Separador de tokens
	for (i = 0; i <= tamanho; i++)
	{
		if (linhaLida[i] == ' ' || linhaLida[i] == '\0') //Fim do caracter
		{
			palavra[contador] = '\0'; //Finaliza token
			if (palavra[0] != '\0')
				listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
			contador = 0; //Prepara para novo token
		}
		else if (linhaLida[i] == ',') //Encontrou virgula
		{
			short int aux = 0;
			if (i > 0) //Não é o primeiro elemento
			{
				if (linhaLida[i - 1] == ' ') //Antes da virgula tem espaço
				{
					if (contador > 0) //Ele tava processando um token
					{
						palavra[contador + 1] = '\0'; //Finaliza token
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
						contador = 0; //Prepara para novo token
					}
					palavra[aux] = ' '; //Adiciona espaço no token da virgula
					aux++;
				}
			}
			if (i + 1 <= tamanho) //Ainda tem pelo menos mais um char
			{
				if (linhaLida[i + 1] == ' ') //Depois da virgula tem espaço
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
				else //Não tem espaço depois da virgula
				{
					if (contador > 0)
					{
						palavra [contador] = '\0'; //Prepara para finalizar token da virgula
						listaDeTokens = insereToken (listaDeTokens, palavra); //Adiciona token
					}
					palavra [aux+1] = '\0';
					contador = 0; //Prepara para novo token
				}
			}
			else //Virgula é o ultimo caracter
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

void printTokens (Tokens *lista)
{
	printf ("Impressao\n");
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

int preprocessamento (FILE *fp, char *arquivoSaida)
{
	int qtdLinhasLidas = 0;
	short int flagDiretivas = 0, flagFalsoIF = 0, aux = 0;
	char *nomeArquivo, *linhaLida;
	DiretivaEQU *listaDiretivasEQU = criaDiretivaEQU();
	Tokens *listaDeTokensPre = criaTokens();

	//Cria arquivo de saida
	nomeArquivo = (char *)calloc ((strlen (arquivoSaida) + 4), sizeof (char));
	strcpy (nomeArquivo, arquivoSaida);
	strcat (nomeArquivo, ".pre");
	FILE *saida = fopen (nomeArquivo, "w");
	if (saida == NULL)
	{
		printf ("ERRO_5\nErro ao gerar arquivo de saída do preprocessamento");
		return 5;
	}
	free (nomeArquivo);


	//Leitura de linhas
	linhaLida = (char *)calloc (100, sizeof (char)); //Cria uma string com tamanho de 100 para ler linha
	while (qtdLinhasLidas != -1)
	{
		//Leitura de linha, ignorando comentario e espaços extras!
		qtdLinhasLidas = leituraDeLinhaPre (fp, linhaLida);

		//Execução das diretivas EQU e IF
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
				//Adiciona linha ao arquivo de saída
				if (flagFalsoIF == 0) //Comando anterior NÂO era um if com condição falsa
				{
					fputs (linhaLida, saida);
					fputc ('\n', saida);
				}
				else //Comando anterior ERA um IF com condição falsa
				{
					flagFalsoIF = 0;
				}
				break;
			case 1: //IF com condição falsa
				flagFalsoIF = 1;
				break;
			case 2: // Lido um EQU ou IF verdadeiro, com erro sintatico ou não só ignora
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
	return 0;
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
	Tokens *aux = listaDeTokens;
	while (aux->proximaLinha != NULL)
	{
		aux = aux->proximaLinha;
	}
	temp = aux->token;
	ant = temp;
	while (temp != NULL)
	{
		if (temp->tipo == EQU) //EQU
		{
			if (contador != 1 || aux->qtdToken != 3) //EQU não é o segundo token da linha
				printf ("Erro 8 - Uso invalido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", aux->linhaOriginal);
			else if (ant->tipo != ROTULO && ant->tipo > 2)//Token anterior não é rotulo, nem virgula
				printf ("Erro 9 - Uso inválido de instrução - Linha: %d\n", aux->linhaOriginal);
			else if (ant->tipo <= 2)
				printf ("Erro 10 - Virgula inesperada - Linha: %d\n", aux->linhaOriginal);
			else if(temp->prox->tipo != NUMERO)
				printf ("Erro 8 - Uso inválido da diretiva EQU, use ROTULO: EQU VALOR - Linha: %d\n", aux->linhaOriginal);
			else  //não tem erro...ainda:
			{
				short int flag = 0;
				lista = adicionaDiretivaPre (lista, ant->string, temp->prox->string, &flag);
				if (flag)
					printf ("Erro 12 - Rotulo Repetido - Linha: %d\n", aux->linhaOriginal);
			}
			*flagDiretivas = 2;
			return lista;
		}
		else if (temp->tipo == IF) //IF
		{
			if (contador != 0 || aux->qtdToken != 2) //If não é o primeiro da linha ou tem mais de 3 elementos
			{
				printf ("Erro 11 - Uso inválido da diretiva IF, use IF OPERANDO - Linha: %d\n", aux->linhaOriginal);
				*flagDiretivas = 1;
			}
			else if (temp->prox->tipo != ROTULO && temp->prox->tipo != NUMERO)
			{
				printf ("Erro 11 - Uso inválido da diretiva IF, use IF OPERANDO - Linha: %d\n", aux->linhaOriginal);
				printf ("Erro 9 - Uso inválido de instrução - Linha: %d\n", aux->linhaOriginal);
				*flagDiretivas = 1;
			}
			else
			{
				short int resultado = procuraEQU (lista, temp->prox->string);
				if (resultado == -1)
				{
					printf ("Erro 7 - Rotulo Não Declarado - Linha: %d\n", aux->linhaOriginal);
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
	novo->nomeRotulo[strlen (nome) - 1] = '\0';
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
Função de leitura de linha:
	Recebe arquivo e ponteiro de string, elimina comentário e espaços extras.
	Retorna -1 caso o arquivo TENHA chegado ao fim, caso contrario quantidade lida de
	linhas até encontrar uma que não seja somente comentario.
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

	while (novoC != EOF)  //Enquanto não chegar no fim do arquivo e linha
	{

		if (novoC == ';') //Verifica se é inicio de comentario
		{
			while (novoC != '\n') //Lê até o final da linha
				novoC = (fgetc(fp));
			if (contador != 0) // Se leu uma string termina
				break;
			while ((novoC == ' ' || novoC == '\t') && novoC != EOF) //Se não tinha lido string tira espaço inicial
			{
				novoC = (fgetc(fp));
			}
			novoC = toupper (novoC);

		}
		//Elimina espaço desnecessarios
		if (contador > 0)
		{
			if (novoC == ' ' && linha [contador - 1] == novoC)
			{
				novoC = toupper (fgetc(fp));
				continue;
			}
			else//Adiciona caracter lido à string
			{
				linha [contador] = novoC;
				contador++;
			}
		}
		else if (contador == 0 && novoC == '\n')
		{
			quantidadeLinhas++;
		}
		else//Adiciona caracter lido à string
		{
			linha [contador] = novoC;
			contador++;
		}
		novoC = toupper (fgetc(fp)); //Le mais um caracter
		if (novoC == '\t') //Para eliminar tabulações extras
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
	return quantidadeLinhas; //Retorna quantidade de linhas lidas até achar uma "válida"
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

