#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessamento.h"
#include "semantico.h"	

void CriaTabelaSimbolos(Token *iniciatoken, Simbolos **inicio, int rot, int pcounter){
		Simbolos *novono = (Simbolos *)malloc(sizeof(Simbolos));;
		Token *aux=iniciatoken;

		//printf("Q LOUCURA MEU JOVEM\n");
	//if(inicio==NULL){
		printf("DOIDERA TOTAL\n");
		*inicio=novono;
		(*inicio)->prox=NULL;
		(*inicio)->nome=iniciatoken->string;
		if(rot==0){
			(*inicio)->declarado=0;
			(*inicio)->valor=0;
			(*inicio)->constante=2;
			//printf("%d\n", inicio->valor);
		}
		else{
			if(rot==1){
				(*inicio)->declarado=1;
				if(aux->prox->tipo==5){
					(*inicio)->constante=1;
					(*inicio)->valor=aux->prox->prox->string;
				}
				else{
					(*inicio)->valor=pcounter;
				}
			}
		}
	//}
}
void InsereTabelaSimbolos(Tokens *ponteiro, Token *iniciatoken, Simbolos *inicio, int rot, int pcounter){
	Tokens *auxiliar=ponteiro;
	Token *aux=iniciatoken;
	Simbolos *simbolo=inicio, *novono;

	printf("Q LOUCURA MEU JOVEM 2\n");
	/*if(inicio==NULL){
		printf("DOIDERA TOTAL\n");
		novono = (Simbolos *)malloc(sizeof(Simbolos));
		inicio=novono;
		inicio->prox=NULL;
		inicio->nome=iniciatoken->string;
		if(rot==0){
			inicio->declarado=0;
			inicio->valor=0;
			inicio->constante=2;
			printf("%d\n", inicio->valor);
		}
		else{
			if(rot==1){
				inicio->declarado=1;
				if(aux->prox->tipo==5){
					inicio->constante=1;
					inicio->valor=aux->prox->prox->string;
				}
				else{
					inicio->valor=pcounter;
				}
			}
		}
	}
	else{*/
		printf("SAFADEZ TOTAL\n");
		while(simbolo->prox!=NULL&&simbolo->nome!=aux->string){
			printf("LOUCURA LOUCURA LOUCURA\n");
			simbolo=simbolo->prox;
		}
		if(simbolo->nome==aux->string&&rot==1&&simbolo->declarado==1){
			printf("Erro Semantico, linha %d\n", auxiliar->linhaOriginal);
		}
		else{
			printf("MAIS SAFADO\n");
			if(simbolo->nome==aux->string&&rot==1){
				printf("QQTAROLANDAOSOASOND\n");
				simbolo->declarado=1;//Muda Declaração para 1
				if(aux->prox->tipo==5){
					simbolo->constante=1;
					simbolo->valor=aux->prox->prox;
				}
				else{
					printf("SERA Q VAI DAR BOM\n");
					simbolo->valor=pcounter;//coloca o valor
				}
			}
			else{
				printf("APARENTEMENTE NAO\n");
				if(rot==1){
					printf("CAEDE\n");
					novono = (Simbolos *)malloc(sizeof(Simbolos)); //InsereTabelaSimbolos com declarado==1
					simbolo->prox=novono;
					simbolo->prox=NULL;
					simbolo->declarado=1;
					if(aux->prox->tipo==5){//checa se é const
						simbolo->constante=1;
						simbolo->valor=aux->prox->prox;
					}
					else{
						simbolo->valor=pcounter;//coloca o valor
					}
				}
				else{
					printf("Q DOIASIRANSdOAKSFHasdf\n");
					novono = (Simbolos *)malloc(sizeof(Simbolos)); //InsereTabelaSimbolos com declarado==0
					simbolo->prox=novono;
					simbolo->prox=NULL;
					simbolo->declarado=0;
				}
			}
		}
	//}
}

void PrimeiraPassagem(Tokens *ponteiro){
	Tokens *auxiliar=ponteiro;
	Token *aux;
	Simbolos *inicio=NULL, *paradapraprintar;
	int cont;
	int pcounter=0;

	while(1){//loop pra percorrer lista Tokens
		aux=auxiliar->token;
		cont=0;
		
		printf("\nSERA QUE ENTROU AQUI????\n");
		while(1){//loop pra percorrer lista de Token
			printf("E AQUI?????\n");
			if(aux==NULL){
				break;
			}
			printf("AUX->TIPO 27\n");
			if(cont==0&&aux->tipo==27){
				printf("Q Q TA ACONTECENDO\n");
				if(inicio==NULL){
					CriaTabelaSimbolos(aux, &inicio, 1, pcounter);
				}
				else{
					InsereTabelaSimbolos(auxiliar,aux, inicio, 1, pcounter);
				}
			}
			else{
				//printf("COMO ASSIM MEU AMIGO\n");
				if(cont!=0&&aux->tipo==27){
					printf("TA AQUI O SAFADO\n");
					if(inicio==NULL){
						CriaTabelaSimbolos(aux, &inicio, 0, pcounter);
					}
					else{
						InsereTabelaSimbolos(auxiliar, aux, inicio,0, pcounter);
					}
				}
				else{ 	
					//printf("COMO Q ESTAMO\n");
					if(aux->tipo==6){
						pcounter+=3;
					}
					else{
						//printf("COMO Q ESTAMO2\n");
				 		if(aux->tipo==23){
							pcounter+=1;
						}
						else{ 
							//printf("COMO Q ESTAMO3\n");
							if(aux->tipo==4||aux->tipo==5||aux->tipo==8||((aux->tipo>=12)&&(aux->tipo<=17))||aux->tipo==19||aux->tipo==20||aux->tipo==24||aux->tipo==25){
								pcounter+=2;
								
							}
							else{
								//printf("COMO Q ESTAMO4\n");
								if(aux->tipo==22){	
									if(aux->prox==NULL){
										pcounter+=1;
									}
									else{
										//printf("HUEHUEHUE\n");
										pcounter+=atoi(aux->prox->string);
									}
								}
							}
						}
					}
				}
			}
			//printf("ONDE ESTAMOS AGORA\n");
			cont++;
			aux=aux->prox;
		}
		printf("\n\n\nONDE EU TO \n");
		paradapraprintar=inicio;
		while(1){
			if(paradapraprintar==NULL){
				break;
			}
			printf("\n valor %d\n", paradapraprintar->valor);
			paradapraprintar=paradapraprintar->prox;
		}
		printf("QUEDOIDDODIDODIDO\n");
		if(auxiliar==NULL){
			break;
		}
		auxiliar=auxiliar->proximaLinha;
	}
}

void SegundaPassagem(Tokens *ponteiro, Simbolos *inicio){
	Tokens *auxiliar=ponteiro;
	Token *aux;
	int cont;

	while(1){
		aux=auxiliar->token;
		cont=0;
		while(1){
			if(cont==0&&aux->tipo==27){
				cont=-1;
			}
			else if(aux->tipo==4){
					//escrever no arquivo de saida na linha atual 1(opcode) e espaço
			}
			else if(aux->tipo==25){
					//escrever no arquivo de saida na linha atual 2(opcode) e espaço
			}
			else if(aux->tipo==19){
						//escrever no arquivo de saida na linha atual 3(opcode) e espaço
			}
			else if(aux->tipo==	8){
				//escrever no arquivo de saida na linha atual 3(opcode) e espaço
			}
			/*else if(){

			}
			}*/
			cont++;
			aux=aux->prox;
		}
		auxiliar=auxiliar->proximaLinha;
	}

}


void ChecaDivisao(Tokens *lista, Token *ponteiro, Simbolos *inicio){
	Token *aux=ponteiro->prox;
	Simbolos *auxiliar=inicio;

	while(aux->string!=auxiliar->nome){
		auxiliar=auxiliar->prox;
	}
	if(auxiliar->valor==0){
		printf("Erro Semantico, linha %d\n", lista->linhaOriginal);
	}
}

void AlteraConstante(Tokens *lista, Token *ponteiro, Simbolos *inicio){
	Token *aux=ponteiro;
	Simbolos *auxiliar=inicio;

	if(ponteiro->tipo==6){
		aux=aux->prox;
		aux=aux->prox;
	}
	else{
		aux=aux->prox;
	}
	while(aux->string!=auxiliar->nome){
			auxiliar=auxiliar->prox;
	}
	if(auxiliar->constante==1){
		printf("Erro Semantico, linha %d\n", lista->linhaOriginal);
	}
}

void ChecaSemantica(Tokens *ponteiro, Simbolos *simbolo){
	Tokens *aux=ponteiro;
	Token  *auxiliar;
	int indicasecao=2, existetext=0;

	while(aux->proximaLinha!=NULL){
		auxiliar=aux->token;
		while(auxiliar->prox!=NULL){
			if(auxiliar->tipo=26){
				indicasecao=0;
				existetext=1;
			}
			else{
				if(auxiliar->tipo=7){
					indicasecao=1;
				}
				else{
					if(auxiliar->tipo==8){
						ChecaDivisao(aux, auxiliar, simbolo);
					}
					else{
						if(auxiliar->tipo==6||auxiliar->tipo==12||auxiliar->tipo==24){
							AlteraConstante(aux, auxiliar, simbolo);
						}
						if(auxiliar->tipo==4||auxiliar->tipo==6||auxiliar->tipo==8||auxiliar->tipo==9||(auxiliar->tipo>=12&&auxiliar->tipo<=20)||(auxiliar->tipo>=23&&auxiliar->tipo<=25)){
							if(indicasecao==1){
								prinf("Erro Semantico, linha %d\n", aux->linhaOriginal);
							}
						}
						else{
							if(auxiliar->tipo==5||auxiliar->tipo==22){
								if(indicasecao==0){
									prinf("Erro Semantico, linha %d\n", aux->linhaOriginal);
								}
							}
						}
					}
				}
			}
			auxiliar=auxiliar->prox;
		}
		aux=aux->proximaLinha;
	}
	if(existetext==0){
		printf("Erro Semantico, falta secao text\n");
	}
}