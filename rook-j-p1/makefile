make : httpServer.o httpClient.o
	gcc -o httpServer httpServer.o
	gcc -o httpClient httpClient.o

httpServer : httpServer.o
	gcc -o httpServer httpServer.o
httpServer.o : httpServer.c httpServer.h headerFiles.h
	gcc -c -g -Wall httpServer.c

httpClient : httpClient.o
	gcc -o httpClient httpClient.o
httpClient.o : httpClient.c httpClient.h headerFiles.h
	gcc -c -g -Wall httpClient.c

clean :
	rm httpServer httpServer.o httpClient httpClient.o 

clean_httpServer :
	rm httpServer httpServer.o
clean_bbpeer : bbpeer
	rm httpClient httpClient.o