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
	char tmpHeader[32];
	(*message).header.token = token;
	(*message).header.action = action;
	(*message).header.sequenceNumber = sequenceNumber;

	if(messageText != NULL)
	{
		snprintf((*message).messageBody, BODYSIZE, "%s", messageText);
	}

	printf("%d\t%d\t%d\n%s\n\n",(*message).header.token,(*message).header.action, (*message).header.sequenceNumber, (*message).messageBody);
}

void mutexPrint(const char *str)
{
	pthread_mutex_lock(&PRINT_LOCK);
	printf("%s\n");
	pthread_mutex_unlock(&PRINT_LOCK);
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

	char message[BODYSIZE];

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


	char message[BODYSIZE];

	buildSocketAddress(&hostAddr, HOST_PORT);
	buildSocketAddress(&peerAddr, peerPort);

	if ((SOCKET_D = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(SOCKET_D, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", HOST_PORT);


	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	message[strlen(message) - 1] = '\0';
	printf("Sending \"%s\" to peer with port %d\n\n", message, peerPort);

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

	determineInitiator();

	pthread_mutex_init(&PRINT_LOCK, NULL);
	pthread_create(TID, NULL, bulletinBoardEditing, NULL);

	if(HAVE_TOKEN == 1)
	{
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

	exitRing();
	pthread_mutex_destroy(&PRINT_LOCK);
}

void determineInitiator(void)
{
	int tmpPortNumber;
	char portNumber[16];
	sprintf(portNumber, "%d", HOST_PORT);
	initMessage(&OUT_MESSAGE, TOKEN_INIT, NO_ACTION, NO_SEQ, portNumber);

	struct message_t inMessage;

	sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));

	while(1)
	{
		recvfrom(SOCKET_D, *inMessage, sizeof(inMessage), 0, NULL, NULL);

		if(inMessage.header.token == TOKEN_INIT)
		{
			tmpPortNumber = atoi(inMessage.messageBody);
			if(tmpPortNumber < HOST_PORT)
			{
				sendto(SOCKET_D, &inMessage, sizeof(inMessage), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));
			}
			else if(tmpPortNumber == HOST_PORT)
			{
				printf("Initiator!\n\n");
				initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, NO_SEQ, portNumber);
				sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));
				initMessage(&OUT_MESSAGE, PASS_TOKEN , NO_ACTION, 1, NULL);
				HAVE_TOKEN = 1;
				break;
			}
			else
			{
				sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));
			}
		}
		else
		{
			initMessage(&OUT_MESSAGE, NO_TOKEN , NO_ACTION, NO_SEQ, portNumber);
			sendto(SOCKET_D, &OUT_MESSAGE, sizeof(OUT_MESSAGE), 0, (struct sockaddr *)&NEXT_PEER_PORT, sizeof(NEXT_PEER_PORT));
			break;
		}
	}

}

void processNextMessage(void)
{

}

void checkUserInput(void)
{

	initMessage(&OUT_MESSAGE, PASS_TOKEN , NO_ACTION, OUT_MESSAGE.header.sequenceNumber + 1, NULL);
}

void exitRing(void)
{

}

void *bulletinBoardEditing(void *parm)
{

}



