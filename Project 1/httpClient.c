/*
 * @file httpClient.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */
#include"httpClient.h"
#include<stdio.h>

int main (void)
{
	getAddressFile();

	printf("\n\n\nClient Started\n\n");

	connectServer();

	transferData();


	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void getAddressFile(void)
{

	printf("\nPlease enter the server IP address in dotted decimal form:\n");
	fgets(SERVER_NAME, sizeof(SERVER_NAME) - 1 , stdin);
	SERVER_NAME[strlen(SERVER_NAME) - 1] = '\0';
	printf("\nPlease enter the file, with file extension, to be requested from server\n");
	fgets(FILE_NAME, sizeof(FILE_NAME) - 1, stdin);
	FILE_NAME[strlen(FILE_NAME) - 1] = '\0';

}

void connectServer(void)
{
	memset(&SERVER_ADDR, 0 , sizeof(SERVER_ADDR));
	SERVER_ADDR.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_NAME, &SERVER_ADDR.sin_addr);
	SERVER_ADDR.sin_port = htons(SERV_PORT);
	if((SOCKET_D = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		error("Error. Socket Creation Failed.");


	if(connect(SOCKET_D, (struct sockaddr *)&SERVER_ADDR, sizeof(SERVER_ADDR)) < 0)
		error("Error. Connection Failed");


}

void transferData(void)
{
	char message[MES_MAX], response[RES_MAX];
	char *messageFormat = "GET /%s HTTP/1.1\r\n\r\n";

	createMessage(message, messageFormat, FILE_NAME, MES_MAX);

	sendMessage(message);

	receiveResponse(response, RES_MAX);

	processResponse(response);
	/*

	LEN = 0;
	MAX_LEN = sizeof(BUFFER);

	printf("Client Sending Data to Server\n\n");

	write(SOCKET_D, FILE_NAME, strlen(FILE_NAME));

	printf("Client Data Sent to Server\n\n");

	printf("Client Receiving Data from Server\n\n");

	read(SOCKET_D, BUFFER, sizeof(BUFFER));


	BUFFER[strlen(BUFFER)] = '\0';
	printf("Echoed string received: %s\n\n", BUFFER);

	close(SOCKET_D);

	printf("Client Shutting Down");
	*/
}

void createMessage(char *message, char *messageFormat, char *input, int messageSize)
{
	if((strlen(messageFormat) + strlen(FILE_NAME)) < messageSize )
		sprintf(message, messageFormat, FILE_NAME);

	else
		error("Error. Message is too large\n");
	printf("Client created message: %s\n\n", message);
	//message[strlen(message)] = EOF;
}

void sendMessage(char *message)
{

	int sent = 0;
	int total = strlen(message);
	int bytes;

	printf("Client sending:\n%s\nto server\n\n", message);

	do
	{
		bytes = write(SOCKET_D, message + sent, total - sent);
		if(bytes < 0)
			error("Error. Error sending message");
		if(bytes == 0)
			break;

		sent += bytes;
	} while(sent < total);

	printf("Client sent message to Server\n\n");
}

void receiveResponse(char *response, int res_max)
{
	int total = res_max - 1;
	int received = 0;
	int bytes;
	memset(response, 0, res_max);

	struct timeval tv ={5,0};
	setsockopt(SOCKET_D, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	printf("Client waiting for response from server\n\n");

	do
	{
		bytes = read(SOCKET_D, response + received, total - received);
		printf("\n%d\n",bytes);
		if(bytes < 0)
			error("Error. Error receiving response from server");
		if(bytes == 0)
			break;

		received += bytes;

	} while( received < total);

	printf("Client received %s from server\n\n", response);
}

void processResponse(char *response)
{
	printf("Client shutting down\n\n");
	close(SOCKET_D);
}
