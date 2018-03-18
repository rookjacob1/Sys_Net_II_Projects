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

	printf("Client shutting down\n\n");
	close(SOCKET_D);

}

void createMessage(char *message, char *messageFormat, char *input, int messageSize)
{
	if((strlen(messageFormat) + strlen(FILE_NAME)) < messageSize )
		sprintf(message, messageFormat, FILE_NAME);

	else
		error("Error. Message is too large\n");
	printf("Client created message: %s\n\n", message);
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

	struct timeval tv ={10,0};
	setsockopt(SOCKET_D, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	printf("Client waiting for response from server\n\n");

	do
	{
		bytes = read(SOCKET_D, response + received, total - received);
		if(errno == EWOULDBLOCK)
		{
			printf("Timeout occurred, assumed end of response\n\n");
			break;
		}
		if(bytes < 0)
			error("Error. Error receiving response from server");
		if(bytes == 0)
			break;

		received += bytes;

		//Has a large file that needs to be taken in chunks
		if(received > res_max/2)
		{
			downloadLargeFile(response, res_max);
			return;
		}

	} while( received < total);


	printf("Client received:\n%s\nfrom server\n\n", response);
}

void processResponse(char *response)
{
	char *charStatusCode = strtok(response, " ");
	charStatusCode = strtok(NULL, " ");
	int intStatusCode = atoi(charStatusCode);
	char *phrase = strtok(NULL, "\r\n");
	char *file;

	if(intStatusCode == 200)
	{
		printf("Client request was successful\n\n");
		file = findBeginningFile(response);
		downloadSmallFile(file);
	}
	else
	{
		printf("Client request was not successful: %s",phrase);
	}



}

char *findBeginningFile(char *response)
{
	char *ptr = response;
	char test[5] = {"\0\0\0\0\0"};
	while(1)
	{
		if(*ptr != '\r')
		{
			ptr += 1;
		}
		else
		{
			strncpy(test, ptr, 4);
			if(!strcmp(test, "\r\n\r\n"))
			{
				return ptr + 4;
			}
		}
	}

}

void downloadSmallFile(char *file)
{
	int i;
	int fileSize = strlen(file);
	char newFilename[140];
	sprintf(newFilename, "Client_Copy_%s", FILE_NAME);

	FILE *fp = fopen(newFilename, "wb");
	if(fp == NULL)
		error("Client can not download file");

	while(fileSize > 0)
	{
		i = fwrite(file, 1, fileSize, fp);
		fileSize -= i;
		file += i;
	}

	//Closing file
	if(fclose(fp) == EOF)
	{
		perror("Error. Error closing file\n\n");
		printf("\n\n");
	}
}

void downloadLargeFile(char *response, int res_max)
{

}
