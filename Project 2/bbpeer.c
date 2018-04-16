/*
 * @file bbpeer.c
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#include"bbpeer.h"

int main(int argc, char *argv[])
{
	int sendPort;

	validateArgv(argc, argv, &sendPort);

	if(argc == 6)
		getNextPeerFromServer(sendPort);
	else
		getNextPeerFromPeer(sendPort);

	bulletinBoardRing();

	close(SOCKET_D);
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void buildSocketAddress(struct sockaddr_in *socketAddress, int socketPort)
{
	memset(socketAddress, 0 , sizeof(struct sockaddr_in));
	(*socketAddress).sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(*socketAddress).sin_addr);
	(*socketAddress).sin_port = htons(socketPort);
}

void initMessage(struct message_t *message, int token, int action, int sequenceNumber, char *messageText)
{
	(*message).header.token = token;
	(*message).header.action = action;
	(*message).header.sequenceNumber = sequenceNumber;

	if(messageText != NULL)
	{
		snprintf((*message).messageBody, MESSAGE_SIZE, "%s", messageText);
	}

	//printf("%d\t%d\t%d\n%s\n\n",(*message).header.token,(*message).header.action, (*message).header.sequenceNumber, (*message).messageBody);
}

void mutexPrint(const char *str)
{
	pthread_mutex_lock(&PRINT_LOCK);
	printf("%s\n", str);
	pthread_mutex_unlock(&PRINT_LOCK);
}

void cleanStdin(void)
{
	int c;
	do
	{
		c = getchar();
	}while(c != '\n' && c != EOF);
}

void validateArgv(int argc, char *argv[], int *sendPort)
{

	if(argc != 5 && argc != 6)
	{
		error("Invalid Number of Parameters. \n"
				"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
	}
	if(argc == 6)
	{
		if( strcmp(argv[1], "-new"))
		{
			error("Invalid Parameter Format. \n"
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		}
		if(strcmp(argv[2], "localhost"))
			error("Program does not support nonlocal message passing\n"
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		*sendPort = atoi(argv[3]);
		HOST_PORT = atoi(argv[4]);
		FILENAME = argv[5];
	}
	else
	{
		if(strcmp(argv[1], "localhost"))
					error("Program does not support nonlocal message passing\n"
							"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		*sendPort = atoi(argv[2]);
		HOST_PORT = atoi(argv[3]);
		FILENAME = argv[4];
	}
	if(*sendPort < 60000 || *sendPort > 60099)
	{
		error("Invalid Port Number.\n"
				"Port Number must be between 60,000 and 60,099\n");
	}
	if(HOST_PORT < 60000 || HOST_PORT > 60099)
	{
		error("Invalid Host Port Number.\n"
				"Host Port Number must be between 60,000 and 60,099\n");
	}
}

void getNextPeerFromServer(int serverPort)
{
	struct sockaddr_in serverAddr;
	struct sockaddr_in hostAddr;

	char message[MESSAGE_SIZE];

	buildSocketAddress(&hostAddr, HOST_PORT);
	buildSocketAddress(&serverAddr, serverPort);

	if ((SOCKET_D = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(SOCKET_D, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", HOST_PORT);


	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	message[strlen(message) - 1] = '\0';
	printf("Sending \"%s\" to server\n\n", message);
	sendto(SOCKET_D, message, strlen(message) + 1, 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));

	recvfrom(SOCKET_D, &NEXT_PEER_ADDR, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	NEXT_PEER_PORT = ntohs((NEXT_PEER_ADDR).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((NEXT_PEER_ADDR).sin_addr), NEXT_PEER_PORT);
}

void getNextPeerFromPeer(int peerPort)
{
	struct sockaddr_in peerAddr;
	struct sockaddr_in hostAddr;
	struct message_t peerRequest;


	char message[MESSAGE_SIZE];

	buildSocketAddress(&hostAddr, HOST_PORT);
	buildSocketAddress(&peerAddr, peerPort);

	if ((SOCKET_D = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(SOCKET_D, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", HOST_PORT);


	sprintf(message, "%d", HOST_PORT);

	initMessage(&peerRequest, NO_TOKEN, JOIN, NO_SEQ, message);

	sendto(SOCKET_D, &peerRequest, sizeof(struct message_t), 0, (struct sockaddr *)&peerAddr, sizeof(struct sockaddr_in));

	recvfrom(SOCKET_D, &NEXT_PEER_ADDR, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	NEXT_PEER_PORT = ntohs((NEXT_PEER_ADDR).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((NEXT_PEER_ADDR).sin_addr), NEXT_PEER_PORT);
}

void bulletinBoardRing(void)
{
	READ_BIT = 0;
	WRITE_BIT = 0;
	LIST_BIT = 0;
	EXIT_BIT = 0;
	HAVE_TOKEN = 0;

	FILE *fp;

	determineInitiator();

	pthread_mutex_init(&PRINT_LOCK, NULL);
	pthread_create(&TID, NULL, bulletinBoardEditing, NULL);

	if(HAVE_TOKEN == 1)
	{
		fp = fopen(FILENAME, "w");//If the file already exists, this will
		if(fp == NULL)
			error("Error opening file");
		fclose(fp);
		checkUserInput();
	}

	while(EXIT_BIT != 1)
	{
		processNextMessage();
		if(HAVE_TOKEN == 1)
		{
			checkUserInput();
			sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));
		}
	}

	pthread_join(TID, NULL);
	pthread_mutex_destroy(&PRINT_LOCK);
}

void determineInitiator(void)
{
	int tmpPortNumber;
	char portNumber[16];

	sprintf(portNumber, "%d", HOST_PORT);
	initMessage(&OUT_MESSAGE, TOKEN_INIT, NO_ACTION, NO_SEQ, portNumber);

	struct message_t inMessage;
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	printf("Beginning to determine the Initiator. Sending host port number %d to next peer with port number %d\n\n",
			HOST_PORT, NEXT_PEER_PORT);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

	while(1)
	{
		printf("Waiting for message from peer\n\n");
		recvfrom(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&peerAddr, &peerAddrLen);

		if(inMessage.header.token == TOKEN_INIT)
		{
			tmpPortNumber = atoi(inMessage.messageBody);
			if(tmpPortNumber < HOST_PORT)
			{
				printf("\"%s\" received from peer with port number %d\n",
						inMessage.messageBody, ntohs(peerAddr.sin_port));
				printf("Host port number: %d > received port: %d\n", HOST_PORT, tmpPortNumber);
				printf("Forwarding received port number: %d to the next peer with port number %d\n\n",
						tmpPortNumber, NEXT_PEER_PORT);
				sendto(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
			}
			else if(tmpPortNumber == HOST_PORT)
			{
				printf("\"%s\" received from peer with port number %d\n",
										inMessage.messageBody, ntohs(peerAddr.sin_port));
				printf("Host port number: %d = received port: %d\n\n", HOST_PORT, tmpPortNumber);
				printf("Host has the lowest port number, therefore is the Initiator!!\n\n");

				printf("Sending notification to next peer to inform the Initiator has been found\n\n");
				initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, NO_SEQ, portNumber);
				sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
				initMessage(&OUT_MESSAGE, PASS_TOKEN , NO_ACTION, 1, NULL);
				HAVE_TOKEN = 1;
				break;
			}
			else
			{
				printf("\"%s\" received from peer with port number %d\n",
												inMessage.messageBody, ntohs(peerAddr.sin_port));
				printf("Host port number: %d < received port: %d\n", HOST_PORT, tmpPortNumber);
				printf("Re-sending host port number %d to next peer with port number %d\n\n", HOST_PORT, NEXT_PEER_PORT);
				sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
			}
		}
		else
		{
			printf("Received notification that Initiator was found from peer with port number %d\n"
					"Forwarding notification to next peer with port number %d\n\n",
					ntohs(peerAddr.sin_port), NEXT_PEER_PORT);
			initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, NO_SEQ, portNumber);
			sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
			break;
		}
	}

}

void processNextMessage(void)
{
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	struct message_t inMessage;

	char printStatement[256];

	recvfrom(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&peerAddr, &peerAddrLen);

	if(inMessage.header.token == PASS_TOKEN)
	{//Can not pass tokens with an action. Pass with current sequence Number though
		snprintf(printStatement, sizeof(printStatement),
				"*************************Token received from peer with port %5d*************************\n",
				ntohs(peerAddr.sin_port));
		mutexPrint(printStatement);

		SEQ_NUM = inMessage.header.sequenceNumber;
		HAVE_TOKEN = 1;
	}//Update sequence Number of OUT_MESSAGE so checkUserInput can know what sequence number BB is at
	else if(inMessage.header.token == NO_TOKEN)
	{
		if(inMessage.header.action == JOIN)
		{
			snprintf(printStatement, sizeof(printStatement),
					"*************************Join request received from peer with port %5d*************************\n",
					ntohs(peerAddr.sin_port));
			mutexPrint(printStatement);

			handleJoin(&peerAddr, &inMessage);
		}
		else if(inMessage.header.action == EXIT)
		{
			snprintf(printStatement, sizeof(printStatement),
					"*************************Exit notification received from peer with port %5d*************************\n",
					ntohs(peerAddr.sin_port));
			mutexPrint(printStatement);

			handleExit(&inMessage);
		}
		else
		{
			return;//Do nothing
		}
	}
	else
	{
		return;//Do nothing
	}
}

void handleJoin(struct sockaddr_in *joiningPeerAddr, struct message_t *receivedMessage)
{
	int joiningPeerPort = atoi((*receivedMessage).messageBody);
	char printStatement[256];
	snprintf(printStatement, sizeof(printStatement), "Peer wanting to join has port address: %d",
			joiningPeerPort);
	mutexPrint(printStatement);

	if(joiningPeerPort != ntohs((*joiningPeerAddr).sin_port))////////NEEED TO CHECK THIS WHEN TESTING
	{
		buildSocketAddress(joiningPeerAddr, joiningPeerPort);
	}
	snprintf(printStatement, sizeof(printStatement), "Sending address of current next peer with port: %d to the joining peer with port %d\n",
			ntohs(NEXT_PEER_ADDR.sin_port), ntohs((*joiningPeerAddr).sin_port));
	mutexPrint(printStatement);

	sendto(SOCKET_D, &NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR), 0, (struct sockaddr *)joiningPeerAddr, sizeof(struct sockaddr_in));

	snprintf(printStatement, sizeof(printStatement), "Setting new next peer address to joining peer with port: %d\n",
			 ntohs((*joiningPeerAddr).sin_port));
	mutexPrint(printStatement);

	memcpy(&NEXT_PEER_ADDR, joiningPeerAddr, sizeof(struct sockaddr_in));
	NEXT_PEER_PORT = ntohs((NEXT_PEER_ADDR).sin_port);

}

void handleExit(struct message_t *receivedMessage)
{
	int exitingPeerPort;
	int exitingPeerNextPort;

	char tmpStr[16];
	char printStatement[256];
	int tmp;

	tmp = strcspn((*receivedMessage).messageBody, "\n");
	strncpy(tmpStr, (*receivedMessage).messageBody, tmp);
	tmpStr[tmp] = '\0';
	exitingPeerPort = atoi(tmpStr);

	sprintf(printStatement, "Exiting peer has port %d\n", exitingPeerPort);
	mutexPrint(printStatement);

	if(exitingPeerPort == NEXT_PEER_PORT)
	{


		strcpy(tmpStr, &(*receivedMessage).messageBody[tmp + 1]);
		exitingPeerNextPort = atoi(tmpStr);


		if(exitingPeerNextPort ==  HOST_PORT)
		{
			sprintf(printStatement, "Host's next peer with port %d is the peer that is exiting\n"
					"Forwarding exit notification to exiting peer to notify that they can exit\n",
					exitingPeerPort);

			sendto(SOCKET_D, receivedMessage, sizeof(*receivedMessage), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

				sprintf(printStatement, "Setting host's new next peer address to the exiting peer's next peer with port %d\n",
					exitingPeerNextPort);

			buildSocketAddress(&NEXT_PEER_ADDR, exitingPeerNextPort);
		}
		else
		{
			sprintf(printStatement, "Host's next peer with port %d is the peer that is exiting\n"
					"Forwarding exit notification to exiting peer to notify that they can exit\n",
					exitingPeerPort);

			sendto(SOCKET_D, receivedMessage, sizeof(*receivedMessage), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

				sprintf(printStatement, "Setting host's new next peer address to the exiting peer's next peer with port %d\n",
					exitingPeerNextPort);

			buildSocketAddress(&NEXT_PEER_ADDR, exitingPeerNextPort);
		}

	}
	else
	{
		sprintf(printStatement, "Host's next peer with port %d is not the exiting peer\n"
				"Forwarding exit notification to next peer\n",
				NEXT_PEER_PORT);
		mutexPrint(printStatement);

		sendto(SOCKET_D, receivedMessage, sizeof(*receivedMessage), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
	}
}

void checkUserInput(void)
{
	//initMessage(&OUT_MESSAGE, PASS_TOKEN , NO_ACTION, OUT_MESSAGE.header.sequenceNumber + 1, NULL);
	if(WRITE_BIT)
	{
		bulletinBoardWrite();
	}
	else if(READ_BIT)
	{
		bulletinBoardRead();
	}
	else if(LIST_BIT)
	{
		bulletinBoardList();
	}
	else if(EXIT_BIT)
	{
		bulletinBoardExit();
	}
	else
	{
		initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, SEQ_NUM, NULL);
		sleep(4);
		sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
	}
	HAVE_TOKEN = 0;
}

void bulletinBoardWrite(void)
{
	char tmpHeader[HEADER_SIZE + 1];
	char tmpWriteMessage[MESSAGE_SIZE + 1];
	FILE *fp;
	int bytesWrote = 0;

	sprintf(tmpHeader, HEADER, SEQ_NUM);

	sprintf(tmpWriteMessage, "%s%s", tmpHeader, WRITE_MESSAGE);

	fp = fopen(FILENAME, "a");
	if (fp == NULL)
		error("Error opening file");
	while(bytesWrote < MESSAGE_SIZE)
	{
		bytesWrote += fwrite(tmpWriteMessage, 1, MESSAGE_SIZE - bytesWrote, fp);
	}
	fclose(fp);

	mutexPrint("Wrote the following message to the bulletin board:");
	mutexPrint(tmpWriteMessage);
}

void bulletinBoardRead(void)
{
	FILE *fp;
	char tmpReadBuffer[MESSAGE_SIZE + 1];
	tmpReadBuffer[MESSAGE_SIZE] = '\0';
	int bytesRead = 0;
	int numMessages;

	struct stat fileStats;

	if(!stat(FILENAME, &fileStats))
		error("Error getting information on file");
	numMessages = fileStats.st_size / MESSAGE_SIZE;

	if(READ_BIT <= numMessages)
	{
		fp = fopen(FILENAME, "r");
		if(fp == NULL)
			error("Error opening file");

		while(bytesRead < MESSAGE_SIZE)
		{
			bytesRead += fread(tmpReadBuffer, 1, MESSAGE_SIZE - bytesRead, fp);
		}

		fclose(fp);

		mutexPrint("The message read is:");
		mutexPrint(tmpReadBuffer);
	}
	else
	{
		sprintf(tmpReadBuffer, "There have only been %d messages wrote, message %d does not exist yet",
				numMessages, READ_BIT);
		mutexPrint(tmpReadBuffer);
	}
}

void bulletinBoardList(void)
{
	FILE *fp;
	char tmpReadBuffer[MESSAGE_SIZE + 1];
	int bytesRead;
	int totalBytesRead = 0;
	int fileSize;


	struct stat fileStats;

	if(!stat(FILENAME, &fileStats))
		error("Error getting information on file");
	fileSize = fileStats.st_size;

	if(fileSize > 0)
	{
		fp = fopen(FILENAME, "r");
		if(fp == NULL)
			error("Error opening file");

		mutexPrint("The message read is:");

		while(totalBytesRead < fileSize)
		{
			bytesRead = fread(tmpReadBuffer, 1, MESSAGE_SIZE, fp);
			totalBytesRead += bytesRead;

			tmpReadBuffer[bytesRead] = '\0';

			pthread_mutex_lock(&PRINT_LOCK);
			printf("%s", tmpReadBuffer);
			pthread_mutex_unlock(&PRINT_LOCK);

		}

		fclose(fp);
	}
	else
	{
		mutexPrint("There has not been any messages written to the bulletin board\n");
	}
}

void bulletinBoardExit(void)
{
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	struct message_t inMessage;

	char printStatement[256];
	sprintf(printStatement, "%d\n%d", HOST_PORT, NEXT_PEER_PORT);

	initMessage(&OUT_MESSAGE, NO_TOKEN, EXIT, NO_SEQ, printStatement);

	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));


	while(1)
	{
		recvfrom(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&peerAddr, &peerAddrLen);

		if(inMessage.header.token == NO_TOKEN)
		{
			if(inMessage.header.action == JOIN)
			{
				snprintf(printStatement, sizeof(printStatement),
						"*************************Join request received from peer with port %5d*************************\n",
						ntohs(peerAddr.sin_port));
				mutexPrint(printStatement);

				sprintf(printStatement, "Waiting to exit, therefore forwarding join request to next peer\n");
				mutexPrint(printStatement);

				sendto(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&peerAddr, peerAddrLen);

			}
			else if(inMessage.header.action == EXIT)
			{
				snprintf(printStatement, sizeof(printStatement),
						"*************************Exit notification received from peer with port %5d*************************\n",
						ntohs(peerAddr.sin_port));
				mutexPrint(printStatement);

				if(!strcmp(OUT_MESSAGE.messageBody, inMessage.messageBody))//Comparing exit notification to the one that was sent
				{
					sprintf(printStatement, "Exit notification was the same notification that was sent.\n"
							"Therefore, it is safe to exit ring.\n");
					mutexPrint(printStatement);

					initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NO_SEQ, NULL);

					sprintf(printStatement, "Passing token to next peer with port %d\n"
							"Exiting ring and terminating\n", NEXT_PEER_PORT);
					mutexPrint(printStatement);

					sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

				}
				else
				{
					sprintf(printStatement, "Exit notification was not the same notification that was sent.\n"
							"Therefore, an error occurred in sending or receiving\n");
					mutexPrint(printStatement);
				}
			}
			else
			{
				return;//Do nothing
			}
		}
		else//If the message is passing a token, that means that there are two tokens which is bad. Discard message.
		{
			return;//Do nothing
		}
	}


}

void *bulletinBoardEditing(void *parm)
{
	char opt;
	char defaultMessage[64];
	while(1)
	{
		if(READ_BIT || WRITE_BIT || LIST_BIT)
			continue;
		if(EXIT_BIT)
			break;
		mutexPrint("\n******************************************************************************************\n\n"
				"Enter w to write to the bulletin board\n"
				"Enter r to read an entry from the bulletin board\n"
				"Enter l to list all entries from the bulletin board\n"
				"Enter e to exit the bulletin board ring\n"
				"\n"
				"The message format is as follows:\n"
				"<message n=sequence Number\n"
				"<body>\n"
				"</message>\n"
				"\n******************************************************************************************\n\n"
				"Please enter one of the options above:");

		opt = fgetc(stdin);
		cleanStdin();
		switch (opt)
		{
			case 'w': case 'W':
				userWrite();
				break;

			case 'r': case 'R':
				userRead();
				break;

			case 'l': case 'L':
				userList();
				break;

			case 'e': case 'E':
				userExit();
				break;

			default:
				sprintf(defaultMessage, "%c is not a valid option. No action taken.\n", opt);
				mutexPrint(defaultMessage);
		}
	}
	mutexPrint("Bulletin board editing thread terminating\n");
	pthread_exit(NULL);
}

void userWrite(void)
{
	char tmpStr[MESSAGE_SIZE - HEADER_SIZE + 1];

	mutexPrint("Please enter the message to write to the bulletin board:");

	fgets(tmpStr, sizeof(tmpStr) - FOOTER_SIZE, stdin);

	tmpStr[strcspn(tmpStr, "\n")] = '\0';

	memset(&tmpStr[strlen(tmpStr)], ' ', sizeof(tmpStr) - strlen(tmpStr));

	sprintf(&tmpStr[sizeof(tmpStr) - FOOTER_SIZE], "%s", FOOTER);

	sprintf(WRITE_MESSAGE, "%s", tmpStr);

	mutexPrint("Setting write bit\n");
	WRITE_BIT = 1;
}

void userRead(void)
{
	char tmpRead[8];
	printf("Please enter the message to read:\n");

	fgets(tmpRead, sizeof(tmpRead), stdin);

	mutexPrint("Setting read bit\n");
	READ_BIT = atoi(tmpRead);
}

void userList(void)
{
	mutexPrint("Setting list bit\n");
	LIST_BIT = 1;
}

void userExit(void)
{
	mutexPrint("Setting exit bit\n");
	EXIT_BIT = 1;
}



