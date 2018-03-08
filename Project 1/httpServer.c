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
	memset(&SERVER_ADDR, 0, sizeof(SERVER_ADDR));
	SERVER_ADDR.sin_family = AF_INET;
	SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);
	SERVER_ADDR.sin_port(SERV_PORT);

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
	char clientName[32];
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
		strcpy(clientName, inet_ntoa(CLIENT_ADDR.sin_addr));
		printf("Connected to %s\n", clientName);

		while((RECV_SIZE = recv(SOCKET_D, BUFF_PTR, MAX_LEN)) > 0)
		{
			BUFF_PTR += RECV_SIZE;
			MAX_LEN -= RECV_SIZE;
			LEN += RECV_SIZE;
		}

		processRequest();

		close(SOCKET_D);

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
