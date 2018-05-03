#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*
Tabela de erros:
Erro 1 - Quantidade invalidade de argumentos
Erro 2 - Arquivo de entrada invalido
Erro 3 - Argumento de comando invalido
Erro 4 - Memória insuficiente
Erro 5 - Erro ao gerar arquivo de saída do preprocessamento
Erro 6 - Rotulo Inválido
Erro 7 - Rotulo Não Declarado
*/




typedef struct tokenNo //Cada elemento corresponde a um token
{
	int tipo;
	char *string;
	struct tokenNo *prox;
} Token;

typedef struct listaToken //Cada elemento corresponde a uma linha
{
	int linhaOriginal; //De qual linha do arquivo original foram retirados os tokens;
	struct Token *token; //acessar tokens individualmente
	struct listaToken *proximaLinha;
} Tokens;



Tokens* scannerPreprocessamento (Tokens* lista, char* linhaLida, int quantia);
Tokens* novaLinhaTokens (Tokens *lista, int linhaOriginal);
int preprocessamento (FILE *fp, char *arquivoSaida);
Token* insereToken (Tokens *lista, char *string);
int leituraDeLinhaPre (FILE *fp, char *linha);
void liberaTokens (Tokens *lista);
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

Tokens* scannerPreprocessamento (Tokens* lista, char* linhaLida, int quantia)
{
	int i, tamanho = strlen (linhaLida), contador = 0, ntoken = 0;
	char palavra[21]; //(20 é o maior tamanho de rotulo possivel + ':')

	//Separador de tokens
	for (i = 0; i < tamanho; i++)
	{
		if (linhaLida[i] == ' ')  //Fim do caracter
		{
			palavra[contador + 1] = '\0'; //Finaliza token
			lista = insereToken (lista, palavra); //Adiciona token
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
						lista = insereToken (lista, palavra); //Adiciona token
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
						palavra[contador + 1] = '\0'; //Finaliza token
						lista = insereToken (lista, palavra); //Adiciona token
						contador = 0; //Prepara para novo token
					}
					palavra[aux + 1] = ' ';
					palavra[aux + 2] = '\0'; //Prepara para finalizar token da virgula
				}
				else //Não tem espaço depois da virgula
					palavra [aux + 1] = '\0'; //Prepara para finalizar token da virgula
			}
			else //Virgula é o ultimo caracter
				palavra [aux + 1] = '\0'; //Prepara para finalizar token da virgula
			palavra[aux] = ',';
			lista = insereToken (lista, palavra);
		}
		else //Copia caracter
		{
			palavra[contador] = linhaLida [i];
			contador++;
		}
	}
}

int preprocessamento (FILE *fp, char *arquivoSaida)
{
	int qtdLinhasLidas = 0;
	unsigned short int flagDiretivas = 0, flagFalsoIF = 0;
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
	while (qtdLinhasLidas != -1)
	{
		//Leitura de linha, ignorando comentario e espaços extras!
		linhaLida = (char *)calloc (100, sizeof (char)); //Cria uma string com tamanho de 200 para ler linha
		qtdLinhasLidas = leituraDeLinhaPre (fp, linhaLida);
		linhaLida = (char *)realloc (linhaLida, strlen (linhaLida) * sizeof (char)); //Diminui tamanho da string


		//Execução das diretivas EQU e IF
		if (qtdLinhasLidas != -1)
		{
			listaDeTokensPre = scannerPreprocessamento (listaDeTokensPre, linhaLida, qtdLinhasLidas);
			listaDiretivasEQU = diretivasPreprocessamento (listaDiretivasEQU, listaDeTokens, &flagDiretivas);
			switch (flagDiretivas)
			{
			case 0:
				//Adiciona linha ao arquivo de saída
				if (flagFalsoIF == 0) //Comando anterior NÂO era um if com condição falsa
				{
					fputs (linhaLida, saida);
					fputc ('\n', saida);
				}
				else //Comando anterior ERA um if com condição falsa
				{
					flagFalsoIf = 0;
				}
				break;
			case 1: //IF com condição falsa
				flagFalsoIF = 1;
				break;
			case 2:
				printf ("ERRO_6\nRotulo Invalido\n");
				return 6;
			case 3:
				printf ("ERRO_7\nRotulo Não Declarado\n");
				return 7;
			default:
				break;
			}
		}
	}

	fclose (saida);
	rewind (fp);
	return 0;
}


/*
Função de leitura de linha:
	Recebe arquivo e ponteiro de string, elimina comentário e espaços extras.
	Retorna -1 caso o arquivo TENHA chegado ao fim, caso contrario quantidade lida de
	linhas até encontrar uma que não seja somente comentario.
*/
int leituraDeLinhaPre (FILE *fp, char* linha)
{
	char novoC;
	unsigned short int comentario = 0, contador = 0, quantidadeLinhas = 1;
	do
	{
		novoC = toupper (fgetc(fp));
	}
	while (novoC == ' ' || novoC == '\t');
	while (novoC != EOF && novoC != '\n')  //Enquanto não chegar no fim do arquivo e linha
	{
		if (!comentario) //Se não é comentario
		{
			if (novoC == ';') //Verifica se é inicio de comentario
			{
				comentario = 1; //Set flag de comentario
				novoC = toupper (fgetc(fp));
				continue;
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


			else//Adiciona caracter lido à string
			{
				linha [contador] = novoC;
				contador++;
			}
		}
		novoC = toupper (fgetc(fp)); //Le mais um caracter
	}

	//Se não foi lida nenhuma linha, chamada recursiva até ler uma ou
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
	return quantidadeLinhas; //Retorna quantidade de linhas lidas até achar uma "válida"
}

/*
Insere uma nova linha de codigo na lista de tokens
*/
Tokens* novaLinhaTokens (Tokens *lista, int linhaOriginal)
{
	Tokens *novo = (Tokens *)malloc(sizeof (Tokens));
	novo->linhaOriginal = linhaOriginal;
	novo->token = criaToken ();
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
}

/*
Insere um novo token encontrado em uma linha
*/

int tipoToken (char *string)
{
	char tabelaTokens [][] =
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
	for (i = 0, i<=26; i++){
		if (strcmp (string, tabelaTokens[i]) == 0){
			return i;
		}
		else if (strcmp (string, tabelaTokens[i]) > 0){
			   return 27;	
		}
	}
	return 27;
}

Token* insereToken (Tokens *lista, char *string)
{
	Token *novo = (Token *)malloc(sizeof (Token));
	novo->tipo = tipoToken (string);
	if (string != NULL) //Somente se for um rotulo
	{
		novo->string = (char *)malloc(strlen (string) * sizeof (char));
		stcpy (novo->string, string);
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
		novo->prox = lista->token->prox;
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

void liberaTokens (Tokens *lista)
{
	Tokens *temp;
	Token *temp2;

	while (lista != NULL)
	{
		temp = lista;
		while (temp->token != NULL)
		{
			temp2 = temp->token;
			temp->token = temp->token->prox;
			if (temp2->string != NULL)
				free (temp2->string);
			free (temp2);
		}
		free (temp);
		lista = lista->proximaLinha;
	}
}

