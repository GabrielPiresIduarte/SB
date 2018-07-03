all: compilador ligador executar

compilador:
	nasm -f elf -o T2.o T2.asm

ligador: 
	ld -m elf_i386 -o T2 T2.o

executar:
	./T2

clean:
	rm *.o
