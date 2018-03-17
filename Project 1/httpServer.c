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

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void createSocket(void)
{
	memset(&SERVER_ADDR, 0, sizeof(SERVER_ADDR));
	SERVER_ADDR.sin_family = AF_INET;
	SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);
	SERVER_ADDR.sin_port = htons(SERV_PORT);

	LISTEN_SOCKET_D = socket(PF_INET, SOCK_STREAM, 0);
	if (LISTEN_SOCKET_D < 0)
		error("Error: Listen socket failed.");

	if( bind(LISTEN_SOCKET_D, (struct sockaddr *)&SERVER_ADDR, sizeof(struct sockaddr_in)) < 0)
		error("Error: Binding failed.");

	if( listen(LISTEN_SOCKET_D, WAIT_SIZE) < 0 )
		error("Error: Listening failed");

}

void startServer(void)
{
	char message[MES_MAX], response[RES_MAX];
	char *responseStatusOK = "HTTP/1.1 200 OK\r\n\r\n";
	char *responseStatusNOTFound = "HTTP/1.1 404 Not Found\r\n\r\n";
	char clientName[32];
	printf("\n\nServer ready.\n");
	for( ; ; )
	{
		printf("Server listening and waiting for client request...\n\n");

		//Accepting connection requests
		SOCKET_D = accept(LISTEN_SOCKET_D, (struct sockaddr *)&CLIENT_ADDR, &CLNT_ADDR_LEN);
		if (SOCKET_D < 0)
			error("Error: Accepting failed.");

		strcpy(clientName, inet_ntoa(CLIENT_ADDR.sin_addr));
		printf("Connected to %s\n", clientName);

		receiveMessage(message, MES_MAX);

		createResponse(message, response, RES_MAX);

		close(SOCKET_D);

	}
}

void receiveMessage(char *message, int mes_max)
{
	int total = mes_max - 1;
	int received = 0;
	int bytes;
	memset(message, 0, mes_max);

	printf("Server waiting for message from client\n\n");

	do
	{
		bytes = read(SOCKET_D, message + received, total - received);
		printf("\n%d\n",bytes);
		if(bytes < 0)
			error("Error. Error receiving message from client");
		if(bytes == 0)
			break;

		received += bytes;

	} while( received < total);

	printf("Server received %s from client", message);
}

void createResponse(char *message, char *response, int res_max)
{
	write(SOCKET_D, BUFFER, sizeof(BUFFER));
	printf("%s",BUFFER);

	close(SOCKET_D);

}
