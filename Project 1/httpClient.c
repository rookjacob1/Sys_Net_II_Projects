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
	findServer();

}

void findServer(void)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0 , sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	char tmpServerPort[16];

	sprintf(tmpServerPort, "%d", SERV_PORT);


	if (getaddrinfo(SERVER_NAME, tmpServerPort, &hints, &result) != 0)
	{
		perror("Error. Address info not obtained");
		exit(1);
	}

	for(rp = result; rp != NULL; rp = rp->ai_next)
	{
		SOCKET_D = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(SOCKET_D == -1)
			continue;

		if(connect(SOCKET_D, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(SOCKET_D);
	}

	SERVER_ADDR = *(rp->ai_addr);
	if(rp == NULL)
	{
		perror("Error: Server could not be found");
		exit(1);
	}

	freeaddrinfo(result);
}

void connectServer(void)
{

}

void transferData(void)
{

}
