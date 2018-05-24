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


typedef struct percorredor{
    Token *apontador;
    struct percorredor *prox;
}Ligerinho;

typedef struct passagem {
    Ligerinho *elementos;
    struct passagem *prox;
}Passagem;


typedef struct tabelaDeSimbolo {
    char string[25];
    short int endereco;
    struct tabelaDeSimbolo *prox;
}TS;



TS* insereNaTS (TS *ts, Token *atoken, short int endereco, short int linha);
Tokens* algoritmoDeUmaPassagem (Tokens *listaDeTokens, FILE *arqOUT);
Passagem * colocaEndereco (TS *lista, Token *atoken, Passagem *bla);
void gravaObjeto (Tokens *listaDeTokens, FILE *arqOUTobjeto);
Passagem *inserePassagem (Passagem *lista, Token *atoken);
char* corrigeConst (char *dest, char *in, int linha);
void completaPassagem (Passagem *pass, TS *ts);
void liberaPassagem (Passagem *lista);
void printTS (TS *ts, Passagem *alt);
void liberaToken (Token *atoken);
char* itoa(int val, int base);
void liberaTS (TS *lista);


int main(int argc, char** argv)
{
    FILE *arqOUTmacro, *arqOUTobjeto;

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
            //Preprocessamento
            listaDeTokens = preprocessamento (arqIN, argv[3], listaDeTokens);
            if (arqIN != NULL)
                fclose (arqIN);
            break;


        case 'M':
            printf ("Macros\n");
            //Preprocessamento
            listaDeTokens = preprocessamento (arqIN, argv[3], listaDeTokens);
            if (arqIN != NULL)
                fclose (arqIN);

            //Macros
            strcpy (nomeArquivo, argv[3]);
            strcat (nomeArquivo, ".macro");
            arqOUTmacro = fopen (nomeArquivo, "w");
            if (arqOUTmacro == NULL)
            {
                printf ("ERRO_16\nErro ao gerar arquivo de saida");
                return 2;
            }
            listaDeTokens = macros (listaDeTokens, arqOUTmacro);
            if (arqOUTmacro != NULL)
                fclose (arqOUTmacro);
            break;


        case 'O':
            printf ("Codigo Objeto\n");
            //Preprocessamento
            //Preprocessamento
            listaDeTokens = preprocessamento (arqIN, argv[3], listaDeTokens);
            if (arqIN != NULL)
                fclose (arqIN);

            //Macros
            strcpy (nomeArquivo, argv[3]);
            strcat (nomeArquivo, ".macro");
            arqOUTmacro = fopen (nomeArquivo, "w");
            if (arqOUTmacro == NULL)
            {
                printf ("ERRO_16\nErro ao gerar arquivo de saida");
                return 2;
            }
            listaDeTokens = macros (listaDeTokens, arqOUTmacro);
            if (arqOUTmacro != NULL)
                fclose (arqOUTmacro);

            //CodigoObjeto
            strcpy (nomeArquivo, argv[3]);
            strcat (nomeArquivo, ".0");
            arqOUTobjeto = fopen (nomeArquivo, "w");
            if (arqOUTobjeto == NULL)
            {
                printf ("ERRO_16\nErro ao gerar arquivo de saida");
                return 2;
            }
            listaDeTokens = algoritmoDeUmaPassagem (listaDeTokens, arqOUTobjeto);
            //gravaObjeto (listaDeTokens, arqOUTobjeto);

            if (arqOUTobjeto != NULL)
                fclose (arqOUTobjeto);
            break;
        default:
            printf("Erro_3\nComando de argumento invalido");
            return 3;
	}

	free (nomeArquivo);

	liberaTokens(listaDeTokens);
    return 0;
}
void printTS (TS *ts, Passagem *alt)
{
    TS *temp = ts;
    Passagem *aux;
    short int encontrou = 0;

    aux = alt;
    while (aux!= NULL){
        Ligerinho *aux2 = aux->elementos;
        while (aux2 != NULL)
        {
            if (aux2->apontador != NULL)
                if (aux2->apontador->string!=NULL)
                    printf("Elemento: '%s'\t", aux2->apontador->string);
            aux2 = aux2->prox;
        }
        printf ("\n");
        aux = aux->prox;
    }

    printf("\n\n");
    while (temp != NULL){
        aux = alt;
        while (aux != NULL){
            short int res = strcmp (temp->string, aux->elementos->apontador->string);
            if (res == 0){
                encontrou = 1;
                break;
                }
            aux = aux->prox;
        }
        if (encontrou){
            encontrou = 0;
            if (aux != NULL)
            {
                Ligerinho *temp2 = aux->elementos;
                while (temp2 != NULL)
                {
                    temp2 = temp2->prox;
                }
                printf("\n");
            }
        }

        temp = temp->prox;
    }
}

TS* insereNaTS (TS *ts, Token *atoken, short int endereco, short int linha){
    TS *novo = (TS *)malloc(sizeof (TS));
    novo->endereco = endereco;
    if (atoken->string[strlen(atoken->string)-1] == ':')
        atoken->string[strlen(atoken->string)-1] = '\0';
    else
        printf ("Erro lexico, formacao de rotulo - linha: %d", linha);
    strcpy (novo->string, atoken->string);
    novo->prox = ts;
    ts = novo;
    return ts;
}

void liberaTS (TS *lista){
    TS *temp = lista;
    while (temp != NULL)
    {
        lista = lista->prox;
        free (temp);
        temp = lista;
    }
}

Passagem * colocaEndereco (TS *lista, Token *atoken, Passagem *bla){
    TS *temp = lista;
    while (temp != NULL)
    {
        short int res = strcmp (atoken->string, temp->string);
        if (res == 0)
        {
            strcpy (atoken->string, itoa (temp->endereco, 10));
            return bla;
        }
        temp = temp->prox;
    }
    bla = inserePassagem(bla, atoken);
    return bla;
}

Passagem *inserePassagem (Passagem *lista, Token *atoken){
    Passagem *aux = lista;
    if (lista == NULL)
    {
        Ligerinho *novoPer = (Ligerinho *)malloc(sizeof (Ligerinho));
        novoPer->apontador = atoken;
        novoPer->prox = NULL;
        Passagem *novo = (Passagem *)malloc(sizeof (Passagem));
        novo->elementos = novoPer;
        novo->prox = lista;
        lista = novo;
        return lista;
    }
    Ligerinho *temp;
    while (aux != NULL)
    {
        temp = aux->elementos;
        short int res = strcmp (atoken->string, temp->apontador->string);
        if (res == 0)
        {
            Ligerinho *novoPer = (Ligerinho *)malloc(sizeof (Ligerinho));
            novoPer->apontador = atoken;
            novoPer->prox = aux->elementos;
            aux->elementos = novoPer;
            return lista;
        }
        aux = aux->prox;
    }

    Ligerinho *novoPer = (Ligerinho *)malloc(sizeof (Ligerinho));
    novoPer->apontador = atoken;
    novoPer->prox = NULL;
    Passagem *novo = (Passagem *)malloc(sizeof (Passagem));
    novo->elementos = novoPer;
    novo->prox = lista;
    lista = novo;

    return lista;
}

void liberaPassagem (Passagem *lista){
    Passagem *aux = lista;
    Ligerinho *temp;
    while (aux != NULL)
    {
       temp = aux->elementos;
       while (temp != NULL)
       {
            aux->elementos = aux->elementos->prox;
            free (temp);
            temp = aux->elementos;
       }
       lista = lista->prox;
       free (aux);
       aux = lista;
    }
}

void liberaToken (Token *atoken){
    Token *temp = atoken;
    while (temp != NULL){
        if (atoken != NULL)
            atoken = atoken->prox;
        if (temp != NULL){
            if (temp->string != NULL){
                free (temp->string);
            }
            free (temp);
        }
        temp = atoken;
    }
}

char* itoa(int val, int base){

	static char buf[32] = {0};

	int i = 30;

	for(; val && i ; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];

}


char* corrigeConst (char *dest, char *in, int linha){
        if (strlen (in)<2){
        printf ("Erro sintatico, uso invalido de operando em CONST - Linha: %d\n", linha);
        strcpy (dest, "00");
        return dest;
    }
    else
    {
        int i;
        for (i = 0; i<strlen (in); i++)
        {
            if (isxdigit (in[i]) == 0)
            {
                if (i != 1 && in[1] != 'X'){
                    printf ("Erro sintatico, uso invalido de operando em CONST - Linha: %d\n", linha);
                    strcpy (dest, "00");
                    return dest;
                }
            }
        }
        char *end;
        int tempNum = (int) strtol (in, &end, 16);
        dest = itoa (tempNum, 10);
    }
    return dest;
}


void completaPassagem (Passagem *pass, TS *ts)
{
    Passagem *temp = pass;
    TS *aux;

    Ligerinho *lig;
    while (temp != NULL){
        lig = temp->elementos;
        if (lig->apontador != NULL)
            if (lig->apontador->string != NULL)
               // printf ("lig:%s\n", lig->apontador->string);
        aux = ts;
        while (aux != NULL){
            short int res = strcmp (lig->apontador->string, aux->string);
            if (res == 0){
                while (lig!=NULL){
                    strcpy (lig->apontador->string, itoa (aux->endereco, 10));
                    lig = lig->prox;
                }
                //printTS(ts, pass);
                break;
            }
            aux = aux->prox;
        }
        temp = temp->prox;
    }
}

Tokens* algoritmoDeUmaPassagem (Tokens *listaDeTokens, FILE *arqOUT){
    //printTokens(listaDeTokens);
    short int endereco = 0;
    Tokens *lista = listaDeTokens;
    Token *atoken, *prevToken;
    TS *ts = NULL;
    Passagem *nomeQueEsqueci = NULL;
    while (lista != NULL)
    {
        if (lista->token == NULL)
            break;
        //printf ("Iniciio Tipo: %d: %d\n", lista->token->tipo, lista->linhaOriginal);
        //printTokens(listaDeTokens);

        //getchar();
        atoken = lista->token;
        prevToken = atoken;
        while (atoken != NULL)
        {
           switch (atoken->tipo)
           {
               case SPACE:
                    endereco++;
                    prevToken->prox = NULL;
                    strcpy (prevToken->string, "00");
                    liberaToken (atoken);
                    //lista = lista->proximaLinha;
                    //prevToken = lista->token;
                    atoken = NULL;
                    break;;

               case CONST:
                    endereco++;
                    if (atoken->prox != NULL)
                    {
                        if (atoken->prox->tipo != NUMERO && atoken->prox->tipo != ROTULO)
                        {
                            printf ("Erro sintatico, uso invalido de operando em CONST - Linha: %d", lista->linhaOriginal);
                            liberaToken(atoken);
                            strcpy (prevToken->string, "00");
                            lista->token = NULL;
                        }
                        else
                            prevToken->string = corrigeConst (prevToken->string, atoken->prox->string, lista->linhaOriginal);
                    }
                    else
                    {
                        liberaToken(atoken);
                        strcpy (prevToken->string, "00");
                        lista->token = NULL;
                        printf ("Erro sintatico, uso invalido de CONST - Linha: %d", lista->linhaOriginal);
                    }
                    prevToken->prox = NULL;
                    liberaToken(atoken);
                    lista->token = prevToken;
                    liberaToken(atoken);
                    break;


               case ROTULO:
                   if (atoken != lista->token)
                   {
                        endereco++;
                        nomeQueEsqueci = colocaEndereco (ts, atoken, nomeQueEsqueci);
                   }
                   else if (atoken == lista->token)
                   {
                        ts = insereNaTS (ts, atoken, endereco, lista->linhaOriginal);
                        if (atoken->prox != NULL)
                        {
                            if (atoken->prox->tipo != SPACE && atoken->prox->tipo != CONST)
                            {
                                lista->token = lista->token->prox;
                                atoken->prox = NULL;
                                liberaToken(atoken);
                                atoken = lista->token;
                            }
                        }
                   }
                   break;
               default:
                   endereco++;
                   break;
           }
           if (atoken != NULL){
               prevToken = atoken;
               atoken = atoken->prox;
           }
        }
        lista = lista->proximaLinha;
        if (lista != NULL)
        {
            //printf ("---------------------\n");
            //printTS (ts, nomeQueEsqueci);
            //printf ("---------------------: %d\t", lista->linhaOriginal);
            //if (lista->token->tipo==ROTULO)
                //printf ("%s", lista->token->string);
            //printf ("----------\n");
            //if (lista->linhaOriginal>40){
            //    getchar();
           // }
            //printf ("----------\n");
         }
         else {
            //printTS(ts, nomeQueEsqueci);
            completaPassagem (nomeQueEsqueci, ts);
            //gravaObjeto(listaDeTokens,arqOUT);
            //printTokens(listaDeTokens);
            //printTS(ts, nomeQueEsqueci);
            break;
         }
   }
   //printTS(ts, nomeQueEsqueci);
   completaPassagem (nomeQueEsqueci, ts);
   gravaObjeto(listaDeTokens,arqOUT);
   //printTokens(listaDeTokens);
   //printTS(ts, nomeQueEsqueci);
   liberaPassagem(nomeQueEsqueci);
   return listaDeTokens;
}

void gravaObjeto (Tokens *listaDeTokens, FILE *arqOUTobjeto){
    Tokens *lista = listaDeTokens;
    Token *atoken;
    if (arqOUTobjeto == NULL){
   }
    char tabelaTokens [27][13] =
	{
		" ,", /*0*/
		" , ", /*1*/
		",", /*2*/
		", ", /*3*/
		"1", /*4*/
		"CONST", /*5*/
		"9", /*6*/
		"DATA", /*7*/
		"4", /*8*/
		"ENDMACRO", /*9*/
		"EQU", /*10*/
		"IF", /*11*/
		"12", /*12*/
		"5", /*13*/
		"6", /*14*/
		"7", /*15*/
		"8", /*16*/
		"10", /*17*/
		"MACRO", /*18*/
		"3", /*19*/
		"13", /*20*/
		"SECTION", /*21*/
		"SPACE", /*22*/
		"14", /*23*/
		"11", /*24*/
		"2", /*25*/
		"TEXT", /*26*/
	};
    while (lista != NULL)
    {   atoken = lista->token;
        while (atoken != NULL)
        {
            if (atoken->tipo == ROTULO){
                fputs(atoken->string, arqOUTobjeto);
            }
            else{
                fputs(tabelaTokens [atoken->tipo], arqOUTobjeto);
                }
            fputc(' ', arqOUTobjeto);
            atoken = atoken->prox;
        }
        lista = lista->proximaLinha;
     }
}
