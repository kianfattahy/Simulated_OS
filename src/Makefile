CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c
	gcc -D FRAMEMEMSIZE=$(framesize) -D VARMEMSIZE=$(varmemsize) -g -c shell.c interpreter.c shellmemory.c kernel.c pcb.c ready_queue.c
	gcc -o mysh shell.o interpreter.o shellmemory.o kernel.o pcb.o ready_queue.o

clean: 
	rm mysh; rm *.o
