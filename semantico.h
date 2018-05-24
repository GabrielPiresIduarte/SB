#ifdef SEMANTICO_H
#define SEMANTICO_H
#include <stdio.h>

void PrimeiraPassagem(Tokens *ponteiro, Simbolos *inicio);
void SegundaPassagem(Tokens *ponteiro, Simbolos *inicio);
void InsereTabelaSimbolos(Tokens *ponteiro, Token *iniciatoken, Simbolos *inicio, int rot, int pcounter);
void ChecaDivisao(Tokens *lista, Token *ponteiro, Simbolos *inicio);
void ChecaSemantica(Tokens *ponteiro, Simbolos *simbolo);

#endif