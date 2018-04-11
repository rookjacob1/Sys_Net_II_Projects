/*
 * @file bbpeer.c
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#include"bbpeer.h"

int main(int argc, char *argv[])
{
	int sockD;
	int serverPort;
	int hostPort;
	char *filename;




	validateArgv(argc, argv, &serverPort, &hostPort, filename);

	if(i < 0)
	{
		printf("Program Terminating");
		exit(1);
	}

	if(argc == 5)
	{

	}
	else
	{

	}


	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void validateArgv(int argc, char *argv[], int *serverPort, int *hostPort, char *filename)
{


	if(argc != 5 || argc != 6)
	{
		error("Invalid Number of Parameters. \n"
				"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
	}
	if(argc == 6)
	{
		if(!strcmp(argv[1], "[-new]"))
		{
			error("Invalid Parameter Format."
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		}
		*serverPort = atoi(argv[2]);
		*hostPort = atoi(argv[4]);
		filename = argv[5];
	}
	else
	{
		*serverPort = atoi(argv[1]);
		*hostPort = atoi(argv[3]);
		filename = argv[4];
	}
	if(*serverPort < 60000 || *serverPort > 60099)
	{
		error("Invalid Port Number.\n"
				"Port Number must be between 60,000 and 60,099\n");
	}
	if(*hostPort < 60000 || *hostPort > 60099)
	{
		error("Invalid Host Port Number.\n"
				"Host Port Number must be between 60,000 and 60,099\n");
	}
}



