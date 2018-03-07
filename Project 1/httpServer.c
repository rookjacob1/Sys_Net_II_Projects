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
	printf("\n\nServer started.\n");

	createSocket();

	startServer();

	return 0;
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
	printf("\n\nServer ready.\n");
	for( ; ; )
	{
		printf("Server listening and waiting for client request...\n\n");

		initVariables();

		//Accepting connection requests
		SOCKET_D = accept(LISTEN_SOCKET_D, &CLIENT_ADDR, &CLNT_ADDR_LEN);
		if (SOCKET_D < 0)
		{
			perror("Error: Accepting failed.");
			exit(1);
		}

		while((RECV_SIZE = recv(SOCKET_D, BUFF_PTR, MAX_LEN)) > 0)
		{
			BUFF_PTR += RECV_SIZE;
			MAX_LEN -= RECV_SIZE;
			LEN += RECV_SIZE;
		}

	}
}

void initVariables(void)
{
	BUFF_PTR = BUFFER;
	MAX_LEN = sizeof(BUFFER);
	LEN = 0;
	RECV_SIZE = 0;
}

void processRequest(void)
{

}
