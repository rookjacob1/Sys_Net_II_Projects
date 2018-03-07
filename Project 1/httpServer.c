/*
 * @file httpServer.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */

#include"httpServer.h"

int main(void)
{
	printf("\n\n\nServer Started\n");

	initVariables();

	createSocket();

	startServer();

	return 0;
}

void initVariables(void)
{
	BUFF_PTR = BUFFER;
	LEN = 0;
	RECV_SIZE = 0;
	WAIT_SIZE = 16;
}

void createSocket(void)
{
	LISTEN_SOCKET_D = socket(PF_INET, SOCK_STREAM, 0);
	if (LISTEN_SOCKET_D < 0)
	{
		perror("Error: Listen socket failed.");
		exit(1);
	}

	if( bind(LISTEN_SOCKET_D, &SERVER_ADDR, sizeof(SERVER_ADDR)) < 0)
	{
		perror("Error: Binding failed.");
		exit(1);
	}

	if( listen(LISTEN_SOCKET_D, WAIT_SIZE) < 0 )
	{
		perror("Error: Listening failed");
		exit(1);
	}
}

void startServer(void)
{

}

void processRequest(void)
{

}
