/*
 * @file bbpeer.c			This file contains all of the function definitions of the functions used in the bulletin
 * board ring peer program. Description of what the individual functions do and their purpose can be found in the
 * bbpeer.h file. This file only contains the code and a basic overlay of what the bulletin board peer program
 * does.
 *
 * The bbpeer program simulates a bulletin board that the peers can write and read from. In order to write or read
 * from the bulletin board, a peer must have a token. This token is passed among the peers to allow all peers to have
 * the chance to use the bulletin board.
 *
 * The bulletin board is actually a shared file between the peers. When the peers read or write to the bulletin board,
 * they are reading or writing to that file that they specified. This program requires that all of the peers to be
 * located on the same host machine, host to host communication is not supported. Because of this, the passing of the
 * actual file between peers is not needed. All of the peers share the same local file.
 *
 * Peers are allowed to join or exit at any time. When a peer wants to join, it sends one of the peers in the ring
 * a join request and that peer sends the joining peer it's next peer and sets it's own next peer to the joining
 * peer. A peer that wants to leave sends an exit notification to the next peer. Peers can only exit when they have the
 * token to prevent problems when more than one peer want to exit at one time. When a peer receives a notification that
 * a peer wants to leave the ring, the peer checks to see if the peer that is wanting to exit is their next peer in the
 * ring. If the peer that is leaving is the next peer, then the exiting peer's next peer is set as the new next peer of
 * the peer that received the exit notification. The peer then sends the exiting peer the same exit notification to
 * notify the exiting peer that they can now exit the ring. If the peer that is exiting the ring is not the current
 * peer's next peer, then the peer simply forwards the exit notification to its next peer.
 *
 * @author Jacob Rook
 * @date 04/17/2018
 * @info Course COP 4635
 * Project 2
 */

#include"bbpeer.h"

int main(int argc, char *argv[])
{
	int sendPort;

	validateArgv(argc, argv, &sendPort);

	if(argc == 6)
	{
		getNextPeerFromServer(sendPort);
		bulletinBoardRing(1);
	}
	else
	{
		getNextPeerFromPeer(sendPort);
		bulletinBoardRing(0);
	}


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

void initMessage(struct message_t *message, int token, int action, char *messageText)
{
	(*message).header.token = token;
	(*message).header.action = action;

	if(messageText != NULL)
	{
		snprintf((*message).messageBody, MESSAGE_SIZE, "%s", messageText);
	}

	//printf("%d\t%d\t\n%s\n\n",(*message).header.token,(*message).header.action, (*message).messageBody);
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

	if(argc != 5 && argc != 6)//Need to have either 5 or 6 arguments, otherwise the command line entry is incorrect
	{
		error("Invalid Number of Parameters. \n"
				"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
	}
	if(argc == 6)//If there are 6 arguments, suppose to be in following form: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>
	{
		if( strcmp(argv[1], "-new"))//Making sure -new is spelled right and is in correct place
		{
			error("Invalid Parameter Format. \n"
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		}
		if(strcmp(argv[2], "localhost"))//Program only handles peers on the same host machine, if something other than localhost is given, program terminates
			error("Program does not support nonlocal message passing\n"
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");

		//Storing the arguments in the appropriate variables
		*sendPort = atoi(argv[3]);
		HOST_PORT = atoi(argv[4]);
		FILENAME = argv[5];
	}
	else//5 arguments given, suppose to be in following form: bbpeer localhost <portNum> <hostPort> <filenameBulletinBoard>
	{
		if(strcmp(argv[1], "localhost"))//Program only handles peers on the same host machine, if something other than localhost is given, program terminates
					error("Program does not support nonlocal message passing\n"
							"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");

		//Storing the arguments in the appropriate variables
		*sendPort = atoi(argv[2]);
		HOST_PORT = atoi(argv[3]);
		FILENAME = argv[4];
	}
	if(*sendPort < 60000 || *sendPort > 60099)//UWF SSH servers only allow ports 60,000 to 60,099 to be used
	{
		error("Invalid Port Number.\n"
				"Port Number must be between 60,000 and 60,099\n");
	}
	if(HOST_PORT < 60000 || HOST_PORT > 60099)//UWF SSH servers only allow ports 60,000 to 60,099 to be used
	{
		error("Invalid Host Port Number.\n"
				"Host Port Number must be between 60,000 and 60,099\n");
	}
}

void getNextPeerFromServer(int serverPort)
{
	struct sockaddr_in serverAddr;			//Socket address to store the server's socket address
	struct sockaddr_in hostAddr;			//Socket address to store the host's socket address

	char message[MESSAGE_SIZE];				//Message to send to the server

	buildSocketAddress(&hostAddr, HOST_PORT);		//Building host's socket address to use for binding the port to the socket
	buildSocketAddress(&serverAddr, serverPort);	//Building server's socket address to use for sending the peer request message

	//Creating socket
	if ((SOCKET_D = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	//Binding socket to the created socket address
	if((bind(SOCKET_D, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", HOST_PORT);

	//Getting message to send to the server from the user
	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	message[strlen(message) - 1] = '\0';			//Getting rid of the extra '\n'
	printf("Sending \"%s\" to server\n\n", message);
	sendto(SOCKET_D, message, strlen(message) + 1, 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));

	//Receiving and storing the next peer's address in NEXT_PEER_ADDR
	recvfrom(SOCKET_D, &NEXT_PEER_ADDR, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	NEXT_PEER_PORT = ntohs((NEXT_PEER_ADDR).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((NEXT_PEER_ADDR).sin_addr), NEXT_PEER_PORT);
}

void getNextPeerFromPeer(int peerPort)
{
	struct sockaddr_in peerAddr;				//Socket address to store the existing peer's socket address
	struct sockaddr_in hostAddr;				//Socket address to store the host's socket address
	struct message_t peerRequest;


	char message[MESSAGE_SIZE];

	buildSocketAddress(&hostAddr, HOST_PORT);		//Building host's socket address to use for binding the port to the socket
	buildSocketAddress(&peerAddr, peerPort);		//Building existing peer's socket address used to send to peer

	//Building socket
	if ((SOCKET_D = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	//Binding socket address to the host's socket address
	if((bind(SOCKET_D, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", HOST_PORT);

	//Creating message with the integer value of the host's port number to send to existing peer
	sprintf(message, "%d", HOST_PORT);

	//Initializing message_t structure to send to the existing peer
	initMessage(&peerRequest, NO_TOKEN, JOIN, message);

	//Sending the message to the existing peer
	sendto(SOCKET_D, &peerRequest, sizeof(struct message_t), 0, (struct sockaddr *)&peerAddr, sizeof(struct sockaddr_in));

	//Receiving and storing the next peer's address in the NEXT_PEER_ADDR variable
	recvfrom(SOCKET_D, &NEXT_PEER_ADDR, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	NEXT_PEER_PORT = ntohs((NEXT_PEER_ADDR).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((NEXT_PEER_ADDR).sin_addr), NEXT_PEER_PORT);
}

void bulletinBoardRing(int init)
{
	if(init)//If the initiator need to be found
		determineInitiator();

	pthread_mutex_init(&PRINT_LOCK, NULL);						//Initialize the mutex print lock
	pthread_create(&TID, NULL, bulletinBoardEditing, NULL);		//Start the bulletinBoardEditing() thread

	while(EXIT_BIT != EXIT)
	{
		processNextMessage();
		if(HAVE_TOKEN == 1)
		{
			checkUserInput();
		}
	}

	pthread_join(TID, NULL);									//Wait for bulletinBoardEditing() thread to terminate
	pthread_mutex_destroy(&PRINT_LOCK);							//Destroy the mutex print lock
}

void determineInitiator(void)
{
	int tmpPortNumber;
	char portNumber[16];

	sprintf(portNumber, "%d", HOST_PORT);
	initMessage(&OUT_MESSAGE, TOKEN_INIT, NO_ACTION, portNumber);

	struct message_t inMessage;
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	memset(&inMessage, 0, sizeof(struct message_t));
	memset(&peerAddr, 0 , sizeof(struct sockaddr_in));

	printf("Beginning to determine the Initiator. Sending host port number %d to next peer with port number %d\n\n",
			HOST_PORT, NEXT_PEER_PORT);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

	READ_BIT = 0;
	WRITE_BIT = 0;
	LIST_BIT = 0;
	EXIT_BIT = 0;
	HAVE_TOKEN = 0;

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
				initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, portNumber);
				sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

				initRing();

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
			initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, portNumber);
			sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
			break;
		}
	}

}

void initRing(void)
{
	FILE *fp;
	HAVE_TOKEN = 1;

	fp = fopen(FILENAME, "w");//If the file already exists, this will
	if(fp == NULL)
		error("Error opening file");
	fclose(fp);

	initMessage(&OUT_MESSAGE, PASS_TOKEN , NO_ACTION, NULL);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

}

void processNextMessage(void)
{
	struct message_t inMessage;
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	memset(&inMessage, 0, sizeof(struct message_t));
	memset(&peerAddr, 0 , sizeof(struct sockaddr_in));

	char printStatement[256];

	recvfrom(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&peerAddr, &peerAddrLen);

	sprintf(printStatement, "Processing next message from %d",ntohs(peerAddr.sin_port));
	mutexPrint(printStatement);

	if(inMessage.header.token == PASS_TOKEN)
	{//Can not pass tokens with an action. Pass with current sequence Number though
		snprintf(printStatement, sizeof(printStatement),
				"*************************Token received from peer with port %5d*************************\n",
				ntohs(peerAddr.sin_port));
		mutexPrint(printStatement);

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
			sprintf(printStatement, "Message Info:\n"
					"Sender's port %d\n"
					"Token Value %d\tAction Value %d\n"
					"Message %s\n\n",ntohs(peerAddr.sin_port), (inMessage).header.token,(inMessage).header.action,
					(inMessage).messageBody);
			mutexPrint(printStatement);
			return;//Do nothing
		}
	}
	else
	{
		sprintf(printStatement, "Message Info:\n"
				"Sender's port %d\n"
				"Token Value %d\tAction Value %d\n"
				"Message %s\n\n",ntohs(peerAddr.sin_port), (inMessage).header.token,(inMessage).header.action,
				(inMessage).messageBody);
		mutexPrint(printStatement);
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

	NEXT_PEER_PORT = ntohs((*joiningPeerAddr).sin_port);
	buildSocketAddress(&NEXT_PEER_ADDR, NEXT_PEER_PORT);
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
			NEXT_PEER_PORT = exitingPeerNextPort;
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
			NEXT_PEER_PORT = exitingPeerNextPort;

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
		initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NULL);
		sleep(SLEEP_TIME);
		sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
	}
	HAVE_TOKEN = 0;
}

int getFileSize(void)
{
	int fileSize;

	struct stat fileStats;
	memset(&fileStats, 0, sizeof(fileStats));

	if(stat(FILENAME, &fileStats) != 0)
		error("Error getting information on file");
	fileSize = fileStats.st_size;

	return fileSize;
}

void bulletinBoardWrite(void)
{
	char tmpHeader[HEADER_SIZE + 1];
	char tmpWriteMessage[MESSAGE_SIZE + 1];
	FILE *fp;
	int bytesWrote = 0;
	int sequenceNumber = getFileSize() / MESSAGE_SIZE + 1;

	sprintf(tmpHeader, HEADER, sequenceNumber);

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


	sprintf(tmpWriteMessage, "Sending token to next peer with port number %d", NEXT_PEER_PORT);
	mutexPrint(tmpWriteMessage);
	initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NULL);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

	WRITE_BIT = 0;
}

void bulletinBoardRead(void)
{
	FILE *fp;
	char tmpReadBuffer[MESSAGE_SIZE + 1];
	tmpReadBuffer[MESSAGE_SIZE] = '\0';
	int bytesRead = 0;
	int numMessages = getFileSize() / MESSAGE_SIZE;

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

	sprintf(tmpReadBuffer, "Sending token to next peer with port number %d", NEXT_PEER_PORT);
	mutexPrint(tmpReadBuffer);

	initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NULL);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
	READ_BIT = 0;
}

void bulletinBoardList(void)
{
	FILE *fp;
	char tmpReadBuffer[MESSAGE_SIZE + 1];
	int bytesRead;
	int totalBytesRead = 0;
	int fileSize = getFileSize();

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

	sprintf(tmpReadBuffer, "Sending token to next peer with port number %d", NEXT_PEER_PORT);
	mutexPrint(tmpReadBuffer);

	initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NULL);
	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));
	LIST_BIT = 0;
}

void bulletinBoardExit(void)
{
	struct message_t inMessage;
	struct sockaddr_in peerAddr;
	socklen_t peerAddrLen;

	memset(&inMessage, 0, sizeof(struct message_t));
	memset(&peerAddr, 0 , sizeof(struct sockaddr_in));

	char printStatement[256];

	sprintf(printStatement, "Starting exiting process, sendingexit notification to next peer with port %d\n", NEXT_PEER_PORT);
	mutexPrint(printStatement);

	sprintf(printStatement, "%d\n%d", HOST_PORT, NEXT_PEER_PORT);

	initMessage(&OUT_MESSAGE, NO_TOKEN, EXIT, printStatement);

	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

	sprintf(printStatement, "%s\n\nWas sent to peer with port number %d", OUT_MESSAGE.messageBody, NEXT_PEER_PORT);
	mutexPrint(printStatement);


	while(1)
	{
		sprintf(printStatement, "Waiting to receive exiting notification\n");
		mutexPrint(printStatement);

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

					initMessage(&OUT_MESSAGE, PASS_TOKEN, NO_ACTION, NULL);

					sprintf(printStatement, "Sending token to next peer with port number %d\n"
							"Exiting ring and terminating\n", NEXT_PEER_PORT);
					mutexPrint(printStatement);

					sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_ADDR, sizeof(NEXT_PEER_ADDR));

					EXIT_BIT = EXIT;
					close(SOCKET_D);
					return;
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

				sprintf(printStatement, "%d\n%d\t%d\n%s\n\n",ntohs(peerAddr.sin_port), (inMessage).header.token,(inMessage).header.action,
						(inMessage).messageBody);
				mutexPrint(printStatement);
				continue;//Do nothing
			}
		}
		else//If the message is passing a token, that means that there are two tokens which is bad. Discard message.
		{
			sprintf(printStatement, "%d\n%d\t%d\n%s\n\n",ntohs(peerAddr.sin_port), (inMessage).header.token,(inMessage).header.action,
									(inMessage).messageBody);
			mutexPrint(printStatement);
			continue;//Do nothing
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



