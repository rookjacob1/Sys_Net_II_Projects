/*
 * @file httpServer.c		This file contains all of the function definitions for the functions used the the httpServer
 * program. Description on how the function works can be found in this file. Description of what the individual functions
 * do can be found in the httpServer.h file. The httpSever program is a program that is suppose to act as an HTTP/1.1
 * server to respond to clients that connect to the server using TCP. The program creates a listen socket to listen for
 * clients. When a client connects to the server, the server processes the message sent by the client and produces an
 * appropriate response to send to the client.
 *
 * Currently, the only HTTP/1.1 method supported by this server is the GET method. The server sends proper HTTP/1.1
 * status lines, however, the sending of the file requested currently does not work properly.
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
	SERVER_ADDR.sin_addr.s_addr = htonl(INADDR_ANY);		//Default IP address
	SERVER_ADDR.sin_port = htons(SERV_PORT);				//Default port

	LISTEN_SOCKET_D = socket(PF_INET, SOCK_STREAM, 0);		//Create listen socket
	if (LISTEN_SOCKET_D < 0)
		error("Error: Listen socket failed.");
	//Bind listen socket to the local socket address
	if( bind(LISTEN_SOCKET_D, (struct sockaddr *)&SERVER_ADDR, sizeof(struct sockaddr_in)) < 0)
		error("Error: Binding failed.");
	//Listen to connection requests
	if( listen(LISTEN_SOCKET_D, WAIT_SIZE) < 0 )
		error("Error: Listening failed");

}

void startServer(void)
{
	char message[MES_MAX], response[RES_MAX];		//Variables to hold message and responses

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
		//Printing the clients address to console
		strcpy(clientName, inet_ntoa(CLIENT_ADDR.sin_addr));
		printf("Connected to %s\n", clientName);
		//Receive message sent by client
		receiveMessage(message, MES_MAX);
		//Create response
		createResponse(message, response, RES_MAX);

		if(!SKIP_SEND)		//Some functions send the response automatically. If SKIP_SEND is set, the response has been sent
		{
			sendResponse(response);
		}

		printf("Server closing connection with client\n\n");
		//Close the socket
		close(SOCKET_D);

	}
}

void receiveMessage(char *message, int mes_max)
{
	int total = mes_max - 1;		//Total amount of bytes capable to receive
	int received = 0;				//Count of how mant bytes received
	int bytes;						//Variable to store how many bytes were received in one call
	memset(message, 0, mes_max);
	//Setting max time to wait for a subsequent packet to 5 seconds
	struct timeval tv ={5,0};
	setsockopt(SOCKET_D, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	printf("Server waiting for message from client\n\n");
	//Receiving bytes from client
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
	memset(response, 0 , res_max);				//Initializing response to 0
	char *method = strtok(message, " ");		//First token should be method in HTTP/1.1 protocol
	//Determining method
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
	int fd = open(URL, O_RDONLY);
	if(fd == -1) //If fp is NULL the file was not found
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
	GET_SendFile(fd,response,res_max);

	SKIP_SEND = 1;

	//Closing file
	if(close(fd) < 0)
	{
		perror("Error. Error closing file\n\n");
		printf("\n\n");
	}

}

void GET_SendFile(int fd, char *response, int res_max)
{
	int offset = 0;
	int remainData;
	int sentBytes;
	struct stat fileStat;

	if(fstat(fd, &fileStat) < 0)
		error("Error. Error with fstat");

	remainData = fileStat.st_size;
	printf("%d\n\n", remainData);

	while( (remainData > 0))
	{
		sentBytes = sendfile(SOCKET_D, fd,NULL,BUFSIZ);
		if(sentBytes < 0)
			error("Error sending file\n\n");
		printf("Sending %d Bytes\n\n",sentBytes);
		remainData -= sentBytes;
	}

	/*
	int *readNotSent = (int *)malloc(sizeof(int));
	*readNotSent = 0;
	int fileBytes;

	char *headBuffer = response, *tailBuffer = response + res_max;
	char *curr = response;

	int sizeTmpBuffer = res_max / 4;
	char *tmpBuffer = (char *)malloc(sizeTmpBuffer);

	printf("Server sending file to client..\n\n");

	do
	{
		if(!feof(fp) && ((res_max - *readNotSent) > sizeTmpBuffer))
		{//If not at the end of file and the avaiable space left in the buffer is greater than the temporary buffer size
			fileBytes = fread(tmpBuffer, 1, sizeTmpBuffer, fp);
			addBytes2Buffer(headBuffer, tailBuffer, curr, readNotSent, tmpBuffer, fileBytes);
		}
		//Sending bytes to the client
		sendBytes2Client(headBuffer, tailBuffer, curr, readNotSent);


	}while((readNotSent != 0)  && !feof(fp));

	printf("Server sent file to client\n\n");*/

}

void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent, char *bytes, int sizeOfBytes)
{
	char *tailByte = curr + *readNotSent;			//Last byte to start adding to
	if(tailBuffer - tailByte < 0)
	{
		tailByte = headBuffer + (tailByte - tailBuffer);
	}

	int dist2Tail = tailBuffer - tailByte;			//How many bytes are in between tailBuffer and tailByte


	if(dist2Tail >= sizeOfBytes)
	{
		if(strncpy(tailByte, bytes, sizeOfBytes) == NULL)
			error("Error. Error with reading bytes into buffer");
		*readNotSent += sizeOfBytes;
	}
	else
	{

		if(strncpy(tailByte, bytes, sizeOfBytes - dist2Tail) == NULL)
					error("Error. Error with reading bytes into buffer");
		if(strncpy(tailByte, bytes + (sizeOfBytes - dist2Tail), dist2Tail) == NULL)
					error("Error. Error with reading bytes into buffer");
		*readNotSent += sizeOfBytes;
	}
}

void sendBytes2Client(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent)
{
	int dist2Tail = tailBuffer - curr;
	int sentBytes;
	int sendLimit = *readNotSent;
	if(*readNotSent > dist2Tail)
		sendLimit = dist2Tail;

	sentBytes = write(SOCKET_D, curr, sendLimit);
	if(sentBytes < 0)
		error("Error. Error sending file");
	*readNotSent -= sentBytes;

	if(sentBytes == dist2Tail)
		curr = headBuffer;
	else
		curr = curr + sentBytes;

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
