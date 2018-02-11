bbserver : bbserver.o
	gcc -pthread -o bbserver bbserver.o
bbserver.o : bbserver.c headerFiles.h
	gcc -c -g -Wall bbserver.c

bbpeer : bbpeer.o
	gcc -pthread -o bbpeer bbpeer.o
bbpeer.o : bbpeer.c headerFiles.h
	gcc -c -g -Wall bbpeer.c

clean :
	rm bbserver bbserver.o bbpeer bbpeer.o 