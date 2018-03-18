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

	char clientName[32];
	printf("\n\nServer ready.\n");
	for( ; ; )
	{
		SKIP_SEND = 0;
		printf("Server listening and waiting for client request...\n\n");

		//Accepting connection requests
		SOCKET_D = accept(LISTEN_SOCKET_D, (struct sockaddr *)&CLIENT_ADDR, &CLNT_ADDR_LEN);
		if (SOCKET_D < 0)
			error("Error: Accepting failed.");

		strcpy(clientName, inet_ntoa(CLIENT_ADDR.sin_addr));
		printf("Connected to %s\n", clientName);

		receiveMessage(message, MES_MAX);

		createResponse(message, response, RES_MAX);

		if(!SKIP_SEND)
		{
			sendResponse(response);
		}

		printf("Server closing connection with client\n\n");

		close(SOCKET_D);

	}
}

void receiveMessage(char *message, int mes_max)
{
	int total = mes_max - 1;
	int received = 0;
	int bytes;
	memset(message, 0, mes_max);

	struct timeval tv ={5,0};
	setsockopt(SOCKET_D, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	printf("Server waiting for message from client\n\n");

	do
	{
		bytes = read(SOCKET_D, message + received, total - received);
		if(errno == EWOULDBLOCK)
		{
			printf("Time out occurred, assumed end of message\n\n");
			break;
		}
		if(bytes < 0)
			error("Error. Error receiving message from client");
		if(bytes == 0)
			break;

		received += bytes;

	} while( received < total);

	printf("Server received:\n%s\nfrom client\n\n", message);
}

void createResponse(char *message, char *response, int res_max)
{
	memset(response, 0 , res_max);
	char *method = strtok(message, " ");

	if(!strcmp(method, "GET"))
	{
		printf("Server detected GET Method\n\n");
		GET_Method(message, response, res_max);
	}
	else
	{
		printf("Server detected invalid method\n\n");
		sprintf(response, "%s", RES_STATUS_NOT_IMPLEMENTED);
	}

}

void GET_Method(char *message, char *response, int res_max)
{
	char *URL = strtok(NULL, " ");
	char *version = strtok(NULL, "\r");

	if((URL == NULL) || (version == NULL) || (strcmp(version, "HTTP/1.1")))
	{
		printf("Server detected bad request\n\n");
		sprintf(response, "%s", RES_STATUS_BAD_REQUEST);
		return;
	}

	GET_Header_Lines(message, response, res_max);

	//URL + 1 gets rid of the first "/"
	GET_AttachFile(URL + 1,response, res_max);
}

void GET_Header_Lines(char *message, char *response, int res_max)
{

}

void GET_AttachFile(char *URL, char *response, int res_max)
{
	FILE *fp = fopen(URL, "rb");
	if(fp == NULL)
	{
		perror("Server does not have file");
		printf("\n\n");
		sprintf(response, "%s", RES_STATUS_NOT_FOUND);
		return;
	}
	//Sending Request line with no headerline
	sprintf(response, "%s", RES_STATUS_OK);
	sendResponse(response);

	//Sending File
	GET_SendFile(fp,response,res_max);

	SKIP_SEND = 1;

	//Closing file
	if(fclose(fp) == EOF)
	{
		perror("Error. Error closing file\n\n");
		printf("\n\n");
	}

}

void GET_SendFile(FILE *fp, char *response, int res_max)
{
	int readNotSent = 0;
	int responseLimit = res_max;
	int responseBytes, fileBytes;

	printf("Server sending file to client..\n\n");

	while((readNotSent == 0)  || feof(fp))
	{
		printf("In loop\n");
		if(!feof(fp))
		{
			fileBytes = fread(response + readNotSent, 1, responseLimit, fp);
			readNotSent += fileBytes;
			responseLimit -= fileBytes;
			printf("%s", response + readNotSent);
		}

		responseBytes = write(SOCKET_D, response, responseLimit);
		response += responseBytes;
		readNotSent -= responseBytes;
		responseLimit += responseBytes;

		if(responseBytes < 0)
			error("Error. Error sending message");

	}

	printf("Server sent file to client\n\n");

}

void sendResponse(char *response)
{

	int sent = 0;
	int total = strlen(response);
	int bytes;

	printf("Server sending:\n%s\nto client \n\n", response);

	do
	{
		bytes = write(SOCKET_D, response + sent, total - sent);
		if(bytes < 0)
			error("Error. Error sending message");
		if(bytes == 0)
			break;

		sent += bytes;
	} while(sent < total);

	printf("Server sent response to client\n\n");
}
