#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

ListaMacro* criaMacros()
{
	return NULL;
}

void liberaMacros (ListaMacro *lista)
{
	ListaMacro *temp = lista;
	Tokens *fim;
	Parametro *aux;
	liberaTokens(lista->inicioMacro);
	while (temp != NULL)
	{
		int i;
		for (i = 0; i < 4; i++)
		{
			aux = temp->parametro[i];
			while (aux != NULL)
			{
				temp->parametro[i] = aux->prox;
				free (aux);
				aux = temp->parametro[i];
			}
		}

        fim = lista->fimMacro;
        fim->proximaLinha = NULL;

        liberaTokens(temp->inicioMacro);

		lista = temp->prox;
		free (temp);
		temp = lista;
	}
}

void imprimiMacro (ListaMacro *lista)
{
printf ("imprimi macro\n");
	ListaMacro *temp = lista;
	Parametro *aux;
	while (temp != NULL)
	{
		int i;
		printf ("Macro: %s\n", temp->nomeMacro);
		printf ("\tInicio: %d\n", temp->inicioMacro->linhaOriginal);
		printTokens(temp->inicioMacro);
		printf ("\tFim: %d\n", temp->fimMacro->linhaOriginal);
		for (i = 0; i < 4; i++)
		{
			printf ("\t\tParametro: %d\n", i);
			aux = temp->parametro[i];
			while (aux != NULL)
			{
				printf ("\t\t\t%s\n", aux->substitui->string);
				aux = aux->prox;
			}
		}
		temp = temp->prox;
	}
printf ("Fim imprimi\n");
}

//Macros------------------------------------------------------------------------

ListaMacro* insereMacro (ListaMacro* lista, Tokens *linha)
{
	ListaMacro* tempLista = lista;
	Tokens *tempLinha = linha;
    Tokens *old = linha;
	Token *tempToken = linha->token->prox;
	while (tempLista != NULL)
    {
        int res = strcmp (tempLista->nomeMacro, tempToken->string);
        if (res == 0)
        {
            printf ("Erro 12 - Rotulo Repetido - Linha: %d\n", linha->linhaOriginal);
            printf ("Erro 13 - Uso inv�lido de MACRO - Linha: %d\n", linha->linhaOriginal);
            return lista;
        }
        tempLista = tempLista->prox;
    }

	ListaMacro *novo = (ListaMacro *)malloc (sizeof (ListaMacro));
	novo->nomeMacro = (char *)malloc ((strlen (tempLinha->token->prox->string)+1)*sizeof (char));
	strcpy (novo->nomeMacro, tempLinha->token->prox->string);
	novo->prox = lista;
	for (int i = 0; i<4; i++){
        novo->parametro[i]=NULL;
	}
    lista = novo;
	novo->inicioMacro = linha;
    tempLinha = tempLinha->proximaLinha;


    while (tempLinha != NULL)
	{
        tempToken = tempLinha->token;
		if (tempToken->tipo == ENDMACRO) //Fim da macro
		{
			break;
		}
		else if (tempToken->tipo == ROTULO)
		{
			tempLinha = substituiMacro(lista, tempLinha);
            Tokens *tempDelete = old->proximaLinha;
            tempDelete->proximaLinha = NULL;
            liberaTokens(tempDelete);
            old->proximaLinha = tempLinha;
            tempToken = tempLinha->token;

		}
		//procura parametros
        while (tempToken!=NULL)
        {
            if (tempToken->tipo == ROTULO && tempToken->string[0] == '&')
            {

                int j;
                if (novo->parametro[0] == NULL)
                {
                    Parametro *novoParametro = (Parametro *)malloc(sizeof (Parametro));
                    novoParametro->substitui = tempToken;
                    novoParametro->prox = novo->parametro[0];
                    novo->parametro[0] = novoParametro;
                }
                else {
                    for (j = 0; j < 4; j++)
                    {
                    if (novo->parametro[j] != NULL)
                        {
                            if (strcmp (tempToken->string, novo->parametro[j]->substitui->string) == 0)
                            {
                                Parametro *novoParametro = (Parametro *)malloc(sizeof (Parametro));
                                novoParametro->substitui = tempToken;
                                novoParametro->prox = novo->parametro[j];
                                novo->parametro[j] = novoParametro;
                            break;
                            }
                        }
                        else {
                            Parametro *novoParametro = (Parametro *)malloc(sizeof (Parametro));
                            novo->parametro[j] = novoParametro;
                            novo->parametro[j]->substitui = tempToken;
                            novo->parametro[j]->prox = NULL;
                            break;
                        }
                    }
                }
            }
            tempToken = tempToken->prox;
        }
		old = tempLinha;
		tempLinha = tempLinha->proximaLinha;
	}

	novo->fimMacro = old;
	return lista;
}

Tokens* copiaMacro (Tokens *lista, Tokens *inicioMacro, Tokens *fimMacro)
{
    Tokens *temp = inicioMacro->proximaLinha, *oldLinha = NULL, *novo, *primeiraLinha = NULL;
    //Enquanto não chegar ao fim da macro
    while (temp != NULL){
        //Cria novo tokens
        novo = (Tokens *)malloc(sizeof (Tokens));
        novo->linhaOriginal = temp->linhaOriginal;
        novo->qtdToken = temp->qtdToken;
        novo->proximaLinha = lista->proximaLinha;

        //Para retornar o primeiro elemento
        if (primeiraLinha == NULL){
            primeiraLinha = novo;
        }

        //Linka o tokens criado anteriormente com o atual
        if (oldLinha != NULL){
            oldLinha->proximaLinha = novo;
        }
        oldLinha = novo;


        Token *aux = temp->token, *oldToken = NULL, *primeiroToken = NULL;
        //Enquanto não copiar todos os tokens da linha
        while (aux!=NULL){
            //Cria novo token
            Token *novoToken = (Token*) malloc (sizeof (Token));
            novoToken->tipo = aux->tipo;
            novoToken->prox = NULL;
            //Se o token é um rotulo
            if (aux->tipo == ROTULO)
            {
                int qtd = strlen (aux->string);
                novoToken->string = (char *)malloc((qtd+1)* sizeof(char));
                strcpy (novoToken->string, aux->string);
            }
            else{
                novoToken->string = NULL;
            }
            //Prepara para retornar o primeiro token
            if (primeiroToken == NULL){
                primeiroToken = novoToken;
            }

            //Linka token anterior com o novo
            if (oldToken != NULL){
                oldToken->prox = novoToken;
            }

            oldToken = novoToken;
            aux = aux->prox;
        }
        novo->token = primeiroToken;
        temp = temp->proximaLinha;
    }
    lista = primeiraLinha;
    return lista;
}

Tokens* substituiMacro (ListaMacro* listaMacro, Tokens *listaDeTokens)
{
    Token *auxToken = listaDeTokens->token;
    ListaMacro *lista = listaMacro;

    while (lista != NULL){
        //confere se é macro
        short int res = strcmp(lista->nomeMacro, auxToken->string);

        //Se a macro corresponde com o que ta na lista
        if (res == 0)
        {
            short int i, contador = 0;

            //Conta quantos parametros a declaração tem
            for (i = 0; i<4; i++){
                if (lista->parametro[i]!=NULL)
                {
                    contador ++;
                }
            }

            //Substitui parametros
            for (i = 0; i<contador; i++)
            {
                auxToken = auxToken->prox;
                if (auxToken == NULL) {printf ("2Erro 15 - Quantidade invalida de parametros para macro"); break;}

                //Se token é virgula
                if (auxToken->tipo <=3) {
                    //Pula pro proximo
                    auxToken = auxToken->prox;
                }

                //Token é rotulo
                if (auxToken->tipo == ROTULO){
                    //Variavel para percorrer lista de parametros
                    Parametro *tempParametro = lista->parametro[i];

                    //Enquanto não percorrer todos os parametros
                    while (tempParametro != NULL){
                        //Substitui parametro
                        if (tempParametro->substitui->string != NULL)
                            tempParametro->substitui->string = (char *)malloc((strlen (auxToken->string) + 1)*sizeof (char));
                        strcpy (tempParametro->substitui->string, auxToken->string);
                        //Segue para o proximo parametro
                        tempParametro = tempParametro->prox;
                    }
                 }
            }

            //Ainda tinha tokens, ou seja, mais parametros do que o declarado
            if (auxToken->prox != NULL)  {printf ("Erro 15 - Quantidade invalida de parametros para macro"); break;}

            //Chama função para copiar a macro
            listaDeTokens = copiaMacro (listaDeTokens, lista->inicioMacro, lista->fimMacro);
            break;
        }
        lista = lista->prox;
    }

    return listaDeTokens;
}

Tokens* macros (Tokens *listaDeTokens, FILE *arqOUT)
{
	Tokens *temp = listaDeTokens, *anterior = listaDeTokens;
	Token *aux;
	ListaMacro *lista;
	lista = criaMacros();
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


	while (temp != NULL)
	{
        //printf ("\n");
        //printTokens (listaDeTokens);
        //printf ("while\n");
		aux = temp->token;

		short int flagErro = 0;
		if (aux->tipo == ROTULO)//Se é macro, vai conferir a sintaxw
		{
            if (aux->prox != NULL)
			{
                Token *tempOld = aux, *proxTok;
                proxTok = aux->prox;
                if (proxTok->tipo == MACRO)
                {
                    tempOld->prox = proxTok->prox;
                    proxTok->prox = tempOld;
                    aux = proxTok;
                    if (aux->prox->string[strlen (aux->prox->string)-1]== ':'){
                            aux->prox->string[strlen (aux->prox->string)-1] = '\0';
                    }
                    else {
                        printf ("Erro lexico: linha: %d", temp->linhaOriginal);
                    }
                    temp->token = aux;
                }
            }
		}
		if (aux->tipo == MACRO)//ROTULO)//Se é macro, vai conferir a sintaxw
		{
                if (temp->qtdToken <= 9 && temp->qtdToken>=3) //Confere por quantidade de tokens na linha
                {
                    Token *aux2 = aux->prox;
                    if (aux2->tipo != ROTULO){ //Segundo token não é rotulo
                            printf("1Erro 13 - Uso invalido de MACRO - Linha: %d\n", temp->linhaOriginal);
                            printf("Erro 9 - Uso invalido de instrucao - Linha: %d\n", temp->linhaOriginal);
                            flagErro = 1;
                    }
                    else {
                        while (aux2->prox!=NULL)//Confere sintaxe
                        {
                            aux2 = aux2->prox;

                            if (aux2->tipo <= 3) //Token não é virgula
                            {
                                aux2 = aux2->prox;
                            }
                            if (aux2->tipo != ROTULO)//Token não é argumento, é erro, tem que ser rotulo ou virgulo
                            {
                                flagErro = 1;
                                printf("Erro 13 - Uso invalido de MACRO - Linha: %d\n", temp->linhaOriginal);
                                printf("Erro 9 - Uso invalido de instrucao - Linha: %d\n", temp->linhaOriginal);
                            }
                            else if  (aux2->string[0] != '&')//Confere erro lexico do token
                            {
                                flagErro = 1;
                                printf("Erro 13 - Uso invalido de MACRO - Linha: %d\n", temp->linhaOriginal);
                                printf("Erro 14 - Parametro invalido - Linha: %d\n", temp->linhaOriginal);
                            }

                        }
                    }
                }
                else
                {
                flagErro = 1;
				printf("Erro 13 - Uso inv�lido de MACRO - Linha: %d\n", temp->linhaOriginal);
			}
		}
		else if (aux->tipo == ENDMACRO)
		{
            flagErro = 1;
			printf("Erro 13 - Uso inv�lido de MACRO - Linha: %d\n", temp->linhaOriginal);
		}

		else if (aux->tipo == ROTULO){
            temp = substituiMacro (lista, temp);
            if (temp!=anterior)
                anterior->proximaLinha = temp;
		}

		if (aux->tipo == MACRO && flagErro == 0)
		{
            //printf ("antes da macro\n");
            //getchar ();
            //printTokens (listaDeTokens);
            lista = insereMacro (lista, temp);
            //printf ("\nantes da 'correcao'\n");
            //printTokens (listaDeTokens);
            //getchar ();

            if (temp!=anterior)
                anterior->proximaLinha = lista->fimMacro->proximaLinha->proximaLinha; //Elimina

            temp = anterior->proximaLinha; //temp continua com linha apos endmacro
            Tokens *tempDelete = lista->fimMacro->proximaLinha; //Linha endmacro
            lista->fimMacro->proximaLinha = NULL;
            tempDelete->proximaLinha = NULL; //Retira o endmacro da lista de macros
            liberaTokens(tempDelete); //Libera esse espaço de memoria (endmacro)

            /*
            anterior = lista->fimMacro->proximaLinha; //Linha endmacro
            lista->fimMacro->proximaLinha = NULL;
            anterior->proximaLinha = NULL; //Retira o endmacro da lista de macros
            liberaTokens(anterior); //Libera esse espaço de memoria (endmacro)
            */
            flagErro = 0;
            //printf ("pos correcao\n");
            //printTokens (listaDeTokens);
            //printf ("\n%d\n", temp->linhaOriginal);
            //getchar();
            continue;
		}
		Token *auxToken = temp->token;
		while (auxToken!=NULL)
		{
            if (auxToken->tipo == ROTULO || auxToken->tipo == NUMERO)
            {
                fputs (auxToken->string, arqOUT);
            }
            else
            {
                fputs (tabelaTokens[auxToken->tipo], arqOUT);
            }
            fputc (' ', arqOUT);
            auxToken = auxToken->prox;
        }
        fputc ('\n', arqOUT);

        anterior = temp;
		temp = temp->proximaLinha;
		flagErro = 0;


	}
    liberaMacros(lista);
	return listaDeTokens;
}
