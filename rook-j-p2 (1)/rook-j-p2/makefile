make : bbserver.o bbpeer.o
	gcc -pthread -o bbserver bbserver.o
	gcc -pthread -o bbpeer bbpeer.o

bbserver : bbserver.o
	gcc -pthread -o bbserver bbserver.o
bbserver.o : bbserver.c bbserver.h headerFiles.h
	gcc -c -g -Wall bbserver.c

bbpeer : bbpeer.o
	gcc -pthread -o bbpeer bbpeer.o
bbpeer.o : bbpeer.c bbpeer.h headerFiles.h
	gcc -c -g -Wall bbpeer.c

clean :
	rm bbserver bbserver.o bbpeer bbpeer.o 

clean_bbserver :
	rm bbserver bbserver.o
clean_bbpeer : bbpeer
	rm bbpeer bbpeer.o