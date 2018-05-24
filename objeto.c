#ifndef OBJETO
#define OBJETO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "preprocessamento.h"

#include "objeto.h"

void analisarotulo(Tokens *ponteiro){//possivel ter que mudar pra tratar x+1 etc
	Tokens *next=ponteiro;
	int i=0;
	
	while(next->token->string[i]!='\n'){
		if(isdigit(next->token->string[0])!=0){
			printf("Erro Lexico.linha %d", next->linhaOriginal);
		}
		else{
			if(isalpha(next->token->string[i])!=0&&isdigit(next->token->string[i])!=0){
				printf("Erro Lexico.linha %d", next->linhaOriginal);
			}
		}
		i++;
	}
}

int trataconst(Token *ponteiro){
	int valorfinal=0;

	if((ponteiro->string[2]=='x')||(ponteiro->string[1]=='x')){
		valorfinal=strtol(ponteiro->string, NULL, 16);	
	}
	else{
		valorfinal=atoi(ponteiro->string);
	}
	return valorfinal;
}

void ChecaTokenSintatico(Tokens *ponteiro){//Utilizado assim que tiver lido uma das funções validas
	Token *aux;
	Tokens *auxiliar;
	int cont=0, flag=6, varmacro=0;//flag indica numero de argumento necessarios ou tipo de comando
	
	auxiliar=ponteiro;
	while(1){
		cont=0;
		aux=auxiliar->token;
		while(1){
			
			if(cont==0){
				if((aux->tipo>=0&&aux->tipo<=3)||(aux->tipo==5)||(aux->tipo==7)||(aux->tipo==10)||(aux->tipo==18)||(aux->tipo==22)||(aux->tipo==26)){
					printf("Erro sintatico leagal, linha %d\n", auxiliar->linhaOriginal);
					break;
				}
				else{
					if(aux->tipo==6){
						flag=2;
					}
					else{
						if((aux->tipo==4)||(aux->tipo==8)||((aux->tipo>=11)&&(aux->tipo<=17))||((aux->tipo>=19)&&(aux->tipo<=20))||(aux->tipo==24)||(aux->tipo==25)){
							flag=1;
						}
						else{
							if(aux->tipo==21){
								flag=3;							
							}
							else{
								if(aux->tipo==23||aux->tipo==9){
									flag=0;
								}
								else{
									//if(aux->tipo==27){
									//	flag = 5;
									//}
									//else{
									//	tam=strlen(aux->string);
										
										if(aux->tipo==27){
												flag=4;
												//printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
												//break;
										}
									//}
								}
							}
						}
					}
				}
			}
			if(cont>0){
				if(flag==2){
					if(((cont==1)&&(aux->tipo<26))||((cont==3)&&(aux->tipo<26))){
						printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
						break;
					}
					else{
						if(cont==2&&aux->tipo!=2){
							printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
							break;
						}
					}
				}
				else{
					if(flag==3){
						if((aux->tipo!=7)&&(aux->tipo!=26)){
							printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
							break;
						}
						else{
							if(aux->prox!=NULL){
								printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
								break;
							}
						}
					}
					if(flag==1){
						if(aux->tipo<=26){
							printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
							break;
						}
						else{
							if(aux->prox!=NULL){
								printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
								break;
							}
						}
					}
					else{
						if(flag==0){
							if(aux!=NULL){
									printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
									break;
							}
						}
						else{
							if(flag==4){
								if((aux->tipo>=0&&aux->tipo<=3)||(aux->tipo==7)||(aux->tipo==9)||(aux->tipo==11)||(aux->tipo==21)||(aux->tipo==26)){
									printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
									break;
								}
								else{
									if(aux->tipo==6){
										flag=2;
									}
									else{
										if((aux->tipo==4)||(aux->tipo==5)||(aux->tipo==8)||((aux->tipo>=12)&&(aux->tipo<=17))||(aux->tipo==19)||(aux->tipo==20)||(aux->tipo==22)||(aux->tipo==24)||(aux->tipo==25)){
											flag=1;
										}
										else{								
											if(aux->tipo==23){
												flag=0;
											}
											/*else{
												if(aux->tipo==18){
													while(aux!=NULL){
														aux=aux->prox;
														varmacro++;
														if(varmacro>3){
															printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
															break;
														}
													}
												}
											}*/
										}
									}
								}
								cont=1;
							}
						}
					}
				}
			}
			if(aux->prox==NULL){
				if(((flag==1)||(flag==3))&&cont==0){
					printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
				}
				else{
					if(flag==2&&cont==2){
						printf("Erro sintatico, linha %d\n", auxiliar->linhaOriginal);
					}
					else{

					}
				}
				break;
			}
			cont++;
			aux=aux->prox;	
		}
		if(auxiliar->proximaLinha==NULL){
			break;
		}
		auxiliar=auxiliar->proximaLinha;
	}
}


#endif