/*
 * @file httpClient.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */
#include"httpClient.h"

int main (int argc, char *argv[])
{


	printf("\n\n\nClient Started\n");





	return 0;
}

void checkArguements(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("\nPlease enter the server address/name and the file name:\n"
				"\n"
				"usage: ./httpClient <address/name> <content>");
		exit(1);
	}
	SERVER_NAME = argv[1];
	strcpy(FILE_NAME,argv[2]);
}

void createSocket(void)
{

}

void connectServer(void)
{

}

void transferData(void)
{

}
