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
	int i = validateArgv(argc, argv);

	if(i < 0)
	{
		printf("Program Terminating");
		exit(1);
	}
	else if(i)
	{

	}

	return 0;
}

int validateArgv(int argc, char *argv[])
{
	int portNum;
	int hostPort;
	int ret;

	if(argc != 5 || argc != 6)
	{
		printf("Invalid Number of Parameters. \n"
				"Parameter Format: [-new] portNum hostIP hostPort filenameBulletinBoard\n");
		return -1;
	}
	if(argc == 6)
	{
		if(!strcmp(argv[1], "[-new]"))
		{
			printf("Invalid Parameter Format."
					"Parameter Format: [-new] portNum hostIP hostPort filenameBulletinBoard\n");
			return -1;
		}
		portNum = atoi(argv[2]);
		hostPort = atoi(argv[4]);
		ret = 0;
	}
	else
	{
		portNum = atoi(argv[1]);
		hostPort = atoi(argv[3]);
		ret = 1;
	}
	if(portNum < 60000 || portNum > 60099)
	{
		printf("Invalid Parameter Value.\n"
				"Port Number must be between 60,000 and 60,099\n");
		return 0;
	}
	if(hostPort < 60000 || hostPort > 60099)
	{
		printf("Invalid Parameter Value.\n"
				"Host Port Number must be between 60,000 and 60,099\n");
		return 0;
	}
	return ret;
}
