section .data
puta db 'puta '
puta1 db 'puta1'
puta2 db 'puta2'
vadia db 'vadia'
piroca db 'piroca'
buceta db 'buceta'
separador db '[>]'
negativo db '-'

intro db 'POR FAVOR ESCREVA SEU NOME', 10
hola db 'HOLA, '
TAMHOLA EQU $ - hola
comprimento db ' bem vindo ao programa de CALC IA-32', 10
TAM EQU $ - comprimento
msg1 db 'ESCOLHA UMA OPÇÂO:',10, '-1: SOMA',10, '-2: SUBTRAÇÂO', 10, '-3: MULTIPLICAÇÂO', 10, '-4: DIVISÂO', 10, '-5: MOD', 10, '-6: SAIR', 10
.len:	equ	$ - msg1
NOMETAM equ 20


section .bss
aux resb 4
aux1 resb 4
dez resb 1
nome resb 20
resposta resb 11
arg resb 11; local intermediario para utilizar função de ler string
arg1 resb 11; numero maximo de caracteres que podem ser representados por 32 bits com sinal
arg2 resb 11
argint resb 11; local intermediario pra funçao de inteiro
buffer resb 1;lugar para salvar enter
blackmagic resb 1
sinal resb 1
backspace resb 1
digitos resb 1

section .text,
global _start
_start:
	mov [backspace], byte 0x8
	mov [blackmagic], byte 1
	mov eax, 4;Trecho para imprimir mensagem inicial
	mov ebx, 1
	mov ecx, intro
	mov edx, 27
	int 80h

	push nome
	call lestring

	mov eax, 4
	mov ebx, 1
	mov ecx, hola
	mov edx, TAMHOLA
	int 80h

	push nome
	call escrevestring	

	mov eax, 4
	mov ebx, 1
	mov ecx, comprimento
	mov edx, TAM
	int 80h

menu:	
	cmp [blackmagic], byte 1
	je bm1
	mov eax, 4;Trecho para imprimir menu
	mov ebx, 1
	mov ecx, msg1
	mov edx, msg1.len
	int 80h

	mov eax, 3
	mov ebx, 1
	mov ecx, resposta
	mov edx, 2
	int 80h;Le opção

	mov al, [resposta]
	sub al, 0x30
	cmp al, 6
	je end

;leitura dos argumentos

	push arg1
	call leinteiro


	push arg2
	call leinteiro

	jmp bm2
;if else para o tipo de operação
bm1:
	mov [arg1], dword 1
	mov [arg2], dword 1
	mov [resposta], dword 1

bm2:
	mov al, [resposta]
	sub al, 0x30
	cmp al, 1
	je soma
	cmp al, 2
	je subtrai
	cmp al, 3
	je multiplica
	cmp al, 4
	je divide
	cmp al, 5
	je mod

soma:
	mov eax, [arg1]
	add eax, [arg2]
	mov [resposta], eax
	jmp imprimeresposta

subtrai:
	mov eax, [arg1]
	sub eax, [arg2]
	mov [resposta], eax
	jmp imprimeresposta

multiplica:
	mov eax, [arg1]
	mov ebx, [arg2]
	imul ebx
	mov [resposta], eax
	jmp imprimeresposta

divide:
	mov eax, [arg1]
	mov ebx, [arg2]
	cdq
	idiv ebx
	mov [resposta], eax
	jmp imprimeresposta

mod:
	mov eax, [arg1]
	mov ebx, [arg2]
	cdq
	idiv ebx
	mov [resposta], edx
	jmp imprimeresposta


imprimeresposta:
	push resposta
	call escreveinteiro
	cmp [blackmagic], byte 1
	je  bm4
	mov eax, 3
	mov ebx, 1
	mov ecx, buffer
	mov edx, 1
	int 80h
bm4:
	mov [blackmagic], byte 0
	jmp menu

end:
	mov eax, 1
	mov ebx, 0
	int 80h

lestring:
	push ebp
	mov ebp, esp
	mov eax, 3
	mov ebx, 1
	mov ecx, arg
	mov edx, 20
	int 80h
	mov ebx, [arg]
	mov eax, [ebp+8]
	mov [eax], ebx
	mov ebx, [arg+4]
	mov [eax+4], ebx
	mov bl,[arg+8]
	mov [eax+8], bl
	pop ebp
	ret

escrevestring:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov ebx, [eax]
	mov [arg], ebx
	mov ebx, [eax+4]
	mov [arg+4], ebx
	mov bl, [eax+8]
	mov [arg+8], bl
	mov eax, 4
	mov ebx, 1
	mov ecx, arg
	mov edx, 20
	int 80h
	pop ebp
	ret

leinteiro:
	push ebp
	mov ebp, esp
	mov eax, 3
	mov ebx, 1
	mov ecx, argint
	mov edx, 11
	int 80h
	mov ecx, 11
	mov eax, 0
	mov [sinal], byte 0
loop1:
	mov ebx, 11
	sub ebx, ecx
	movzx dx, [argint+ebx]
	cmp dx, word 0x2D
	jne positivo
	mov [sinal], byte 1
	loop loop1
positivo:
	movzx edx, dx
	sub edx, 0x30
	add eax, edx
	movzx dx, [argint+ebx+1]
	movzx edx, dx
	cmp edx, 0xA
	je fimleinteiro
	mul	 dword[dez]
	loop loop1
fimleinteiro:
	cmp [sinal], byte 1
	jne positivo2
	imul eax, dword -1
positivo2:
	mov edx, [ebp+8]
	mov [edx], eax
	pop ebp	
	ret

escreveinteiro:
	push ebp
	mov ebp, esp
	;mov [sinal], byte 0
	mov ebx, [ebp+8]
	mov eax, [ebx]
	cmp eax, dword 0
	jge positivo3
	;mov [sinal], byte 1
	imul eax, dword -1
	
	pushad
	pusha
	mov eax, 4
	mov ebx, 1
	mov ecx, negativo
	mov edx, 1
	int 80h
	popa
	popad

positivo3: 
	mov ecx, 11
	mov [digitos], dword 0
loop2:
	cdq
	mov [dez], dword 0xA
	div dword[dez]
	add dl, 0x30
	add [digitos], dword 1
	mov [ebx], dl
	cmp eax, 0
	je fimescreveinteiro

	
	push ecx; coloca valor do contador de loop para utilizar ecx para outro loop
	mov edx, ecx
	mov ecx, 12
	sub ecx, edx
miniloop:
	mov dl,[ebx+ecx-1]
	mov [ebx+ecx], dl
	loop miniloop	
	pop ecx
naoguarda:
	loop loop2

fimescreveinteiro:

	mov eax, [ebx]
	mov [argint], eax
	mov eax, [ebx+4]
	mov [argint+4], eax
	mov ax,[ebx+8]
	mov [argint+8], ax
	mov al, [ebx+10]
	mov [argint+10], al

	cmp [blackmagic], byte 1
	je bm3

	mov eax, 4
	mov ebx, 1
	mov ecx, argint
	mov edx, [digitos]
	int 80h


bm3:	
	pop ebp
	ret