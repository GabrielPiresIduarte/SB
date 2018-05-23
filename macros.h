#ifndef MACRO_H
#define MACRO_H

#include "preprocessamento.h"
#include <stdio.h>
//Macros -------------------------------------------------------------------
typedef struct parametroMacro
{
	Token *substitui;
	struct parametroMacro *prox;
} Parametro;

typedef struct structMacro
{
	char *nomeMacro;
	Tokens *inicioMacro, *fimMacro;
	Parametro *parametro[4];
	struct structMacro *prox;
} ListaMacro;

Tokens* copiaMacro (Tokens *lista, Tokens *inicioMacro, Tokens *fimMacro);
Tokens* substituiMacro (ListaMacro* lista, Tokens *listaDeTokens);
ListaMacro* insereMacro (ListaMacro* lista, Tokens *linha);
Tokens* macros (Tokens *listaDeTokens, FILE *arqOUT);
void liberaMacros (ListaMacro *Lista);
void imprimiMacro (ListaMacro *lista);
ListaMacro* criaMacros();

#endif
