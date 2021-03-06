#ifndef PREPROCESSAMENTO_H
#define PREPROCESSAMENTO_H
#include <stdio.h>
//#include <list>
//Preprocessamento-------------------------------------------------------------------

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
	char nomeRotulo [25];
	char operando [25];
	struct diretivaPre *prox;
} DiretivaEQU;

/*
std::list<DiretivaEQU> diretivasPreprocessamento (std::list<DiretivaEQU> lista, std::list<Tokens> listaDeTokens, short int *flagDiretivas);
std::list<DiretivaEQU> adicionaDiretivaPre (std::list<DiretivaEQU> lista, std::list<DiretivaEQU>::iterator i, short int *flag);
std::list<Tokens> scannerPreprocessamento (std::list<Tokens> listaDeTokens, char* linhaLida, int quantia);
std::list<Tokens> preprocessamento (FILE *fp, char *arquivoSaida, std::list<Tokens> listaDeTokensPre);
std::list<Tokens> novaLinhaTokens (std::list<Tokens> lista, int linhaOriginal);
short int procuraEQU (std::list<DiretivaEQU> lista, char *string);
std::list<Tokens> insereToken (std::list<Tokens> lista, char * string);
int leituraDeLinhaPre (FILE * fp, char * linha);
void printDiretivaEQU (std::list<DiretivaEQU> lista);
void printTokens (std::list<Tokens> lista)
int tipoToken (char * string);
int isNumber (char *string);
*/

//*
DiretivaEQU* diretivasPreprocessamento (DiretivaEQU *lista, Tokens *listaDeTokens, short int *flagDiretivas);
DiretivaEQU* adicionaDiretivaPre (DiretivaEQU *lista, char *nome, char *operando, short int *flag);
Tokens* preprocessamento (FILE *fp, char *arquivoSaida, Tokens *listaDeTokensPre);
Tokens* scannerPreprocessamento (Tokens* lista, char* linhaLida, int quantia);
Tokens* novaLinhaTokens (Tokens *lista, int linhaOriginal);
short int procuraEQU (DiretivaEQU *lista, char *string);
Tokens* insereToken (Tokens *lista, char *string);
void gravaMacro (Tokens *lista, FILE *arqOUT2);
int leituraDeLinhaPre (FILE *fp, char *linha);
void printDiretivaEQU (DiretivaEQU* lista);
void liberaEQU (DiretivaEQU *lista);
void liberaTokens (Tokens *lista);
void printTokens (Tokens *lista);
DiretivaEQU* criaDiretivaEQU ();
int isNumber (char *string);
Tokens* criaTokens ();
Token* criaToken ();
//*/

#endif
