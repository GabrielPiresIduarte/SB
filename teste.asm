add bas
teste: equ 1
if teste
naoAparece: SPACE

macro teste1 &a
output &a
endmacro

macro teste2 &a, &b
add &a
div &b
store &a
ENDMACRO

teste1 pa1

teste2 pa1, pa3

add: sepador

teste2 npa1, npa3

add: pa1


pa1: SPACE
FIM: space 
