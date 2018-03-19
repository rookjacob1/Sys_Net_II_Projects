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
	char opt;
	getAddressFile();

	do
	{
	printf("\n\n\nClient Started\n\n");

	connectServer();

	transferData();

	displayFile();

	printf("Would you like to request another file? (y/n): ");
	scanf("\n%c", &opt);

	if(opt == 'Y' || opt == 'y')
	{
		printf("\nPlease enter the file, with file extension, to be requested from server\n");
		fgets(FILE_NAME, sizeof(FILE_NAME) - 1, stdin);
		FILE_NAME[strlen(FILE_NAME) - 1] = '\0';
	}

	} while(opt == 'Y' || opt == 'y');


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


	printf("\nClient shutting down\n\n");
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
			processResponse(response, res_max, received);
			return;
		}

	} while( received < total);


	printf("Client received:\n%s\nfrom server\n\n", response);

	processResponse(response, res_max, 0);

}

void processResponse(char *response, int res_max, int bytesInBuffer)
{
	char *responseCopy = (char *)malloc(res_max*sizeof(char));
	strcpy(responseCopy, response);

	char *charStatusCode = strtok(responseCopy, " ");
	charStatusCode = strtok(NULL, " ");
	int intStatusCode = atoi(charStatusCode);

	char *phrase = strtok(NULL, "\r\n");
	free(responseCopy);

	char *file;
	int sizeOfHeader;


	if(intStatusCode == 200)
	{
		printf("Client request was successful\n\n");
		//Finding the beginning of the file
		file = strstr(response, "\r\n\r\n");
		if(file == NULL)
			error("Error. Response was corrupted");
		file += 4;

		if(bytesInBuffer == 0)//Have all of the bytes
			downloadSmallFile(file);
		else if((bytesInBuffer > 0) && (bytesInBuffer < res_max))//Still have bytes to download
		{
			sizeOfHeader = file - response;
			downloadLargeFile(file, res_max - sizeOfHeader, bytesInBuffer - sizeOfHeader);
		}
		else
			error("Error. Incorrect arguments passed to processResponse");
	}
	else
	{
		printf("Client request was not successful: %s",phrase);
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

	printf("Downloading small file as %s\n\n", newFilename);

	while(fileSize > 0)
	{
		i = fwrite(file, 1, fileSize, fp);
		fileSize -= i;
		file += i;
	}

	printf("%s was completely downloaded\n\n", newFilename);


	//Closing file
	if(fclose(fp) == EOF)
	{
		perror("Error. Error closing file\n\n");
		printf("\n\n");
	}
}

void downloadLargeFile(char *headBuffer, int bufferSize, int receivedBytes)
{
	//Adjusting the timeout to give the large file time to download
	struct timeval tv ={60,0};
	setsockopt(SOCKET_D, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	char newFilename[140];
	sprintf(newFilename, "Client_Copy_%s", FILE_NAME);

	FILE *fp = fopen(newFilename, "wb");
	if(fp == NULL)
		error("Client can not download file");

	char *tailBuffer = headBuffer + bufferSize;
	char *curr = headBuffer;

	int sizeTmpBuffer = bufferSize / 4;
	char *tmpBuffer = (char *)malloc(sizeTmpBuffer* sizeof(char));

	int *readNotDownloaded = (int *)malloc(sizeof(int));
	*readNotDownloaded = bufferSize - receivedBytes;
	int readBytes;

	printf("Downloading large file as %s...\n\n", newFilename);

	do
	{
		if(((bufferSize - *readNotDownloaded) > sizeTmpBuffer))
		{
			readBytes = read(SOCKET_D, tmpBuffer, sizeTmpBuffer);
			if(errno == EWOULDBLOCK)
			{
				printf("Time out occurred, assumed end of message\n\n");
				break;
			}
			if(readBytes < 0)
				error("Error. Error receiving message from client");
			if(readBytes != 0)
				addBytes2Buffer(headBuffer, tailBuffer, curr, readNotDownloaded, tmpBuffer, readBytes);
		}

		writeBytes2File(fp, headBuffer, tailBuffer, curr, readNotDownloaded);

	} while( (*readNotDownloaded > 0));

	printf("%s was completely downloaded\n\n", newFilename);

	free(readNotDownloaded);

}

void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded, char *bytes, int sizeOfBytes)
{
	int dist2Tail = tailBuffer - curr;
	char *tailByte;

	if(dist2Tail >= *readNotDownloaded)
	{
		tailByte = curr + *readNotDownloaded;
		if(strncpy(tailByte, bytes, sizeOfBytes) == NULL)
			error("Error. Error with reading bytes into buffer");
		*readNotDownloaded += sizeOfBytes;
	}
	else
	{
		tailByte = headBuffer + (*readNotDownloaded - dist2Tail);
		if(strncpy(tailByte, bytes, sizeOfBytes) == NULL)
					error("Error. Error with reading bytes into buffer");
		*readNotDownloaded += sizeOfBytes;
	}
}

void writeBytes2File(FILE *fp, char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded)
{
	int dist2Tail = tailBuffer - curr;
	int writeBytes;
	int writeLimit = *readNotDownloaded;
	if(*readNotDownloaded > dist2Tail)
		writeLimit = dist2Tail;


	writeBytes = fwrite(curr, 1, writeLimit, fp);
	*readNotDownloaded -= writeBytes;

	if(writeBytes == dist2Tail)
		curr = headBuffer;
	else
		curr = curr + writeBytes;

}

void displayFile(void)
{
	pid_t pid = fork();
	sprintf(FILE_NAME, "Client_Copy_%s", FILE_NAME);
	if(!pid)
	{
		execlp("sensible-browser", "sensible-browser", FILE_NAME, NULL);
	}
	else
	{
		printf("Displaying %s\n\n", FILE_NAME);
		waitpid(pid, NULL,0);
	}
}
