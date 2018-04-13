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

	int sockD;
	int sendPort;
	int nextPeerPort;
	int hostPort;

	char *filename = NULL;

	struct sockaddr_in nextPeerAddr;

	validateArgv(argc, argv, &sendPort, &hostPort, filename);

	if(argc == 6)
		getNextPeerFromServer(&nextPeerAddr, &nextPeerPort, sendPort, hostPort, &sockD);
	else
		getNextPeerFromPeer(&nextPeerAddr, &nextPeerPort, sendPort, hostPort, &sockD);




	close(sockD);
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void validateArgv(int argc, char *argv[], int *sendPort, int *hostPort, char *filename)
{


	if(argc != 5 && argc != 6)
	{
		error("Invalid Number of Parameters. \n"
				"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
	}
	if(argc == 6)
	{
		if(!strcmp(argv[1], "[-new]"))
		{
			error("Invalid Parameter Format."
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		}
		if(!strcmp(argv[2], "localhost"))
			error("Program does not support nonlocal message passing\n"
					"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		*sendPort = atoi(argv[3]);
		*hostPort = atoi(argv[4]);
		filename = argv[5];
	}
	else
	{
		if(!strcmp(argv[2], "localhost"))
					error("Program does not support nonlocal message passing\n"
							"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		*sendPort = atoi(argv[2]);
		*hostPort = atoi(argv[3]);
		filename = argv[4];
	}
	if(*sendPort < 60000 || *sendPort > 60099)
	{
		error("Invalid Port Number.\n"
				"Port Number must be between 60,000 and 60,099\n");
	}
	if(*hostPort < 60000 || *hostPort > 60099)
	{
		error("Invalid Host Port Number.\n"
				"Host Port Number must be between 60,000 and 60,099\n");
	}
}

void getNextPeerFromServer(struct sockaddr_in *nextPeerAddr, int *nextPeerPort, int serverPort, int hostPort, int *socketDescriptor)
{
	struct sockaddr_in serverAddr;
	struct sockaddr_in hostAddr;

	char message[BODYSIZE];

	buildSocketAddress(&hostAddr, hostPort);
	buildSocketAddress(&serverAddr, serverPort);

	if ((*socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(*socketDescriptor, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", hostPort);


	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	message[strlen(message) - 1] = '\0';
	printf("Sending \"%s\" to server\n\n", message);
	sendto(*socketDescriptor, message, strlen(message) + 1, 0, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));

	recvfrom(*socketDescriptor, nextPeerAddr, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	*nextPeerPort = ntohs((*nextPeerAddr).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((*nextPeerAddr).sin_addr), *nextPeerPort);
}

void buildSocketAddress(struct sockaddr_in *socketAddress, int socketPort)
{
	memset(socketAddress, 0 , sizeof(struct sockaddr_in));
	(*socketAddress).sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(*socketAddress).sin_addr);
	(*socketAddress).sin_port = htons(socketPort);
}

void getNextPeerFromPeer(struct sockaddr_in *nextPeerAddr, int *nextPeerPort, int peerPort, int hostPort, int *socketDescriptor)
{
	struct sockaddr_in peerAddr;
	struct sockaddr_in hostAddr;
	struct message_t peerRequest;


	char message[BODYSIZE];

	buildSocketAddress(&hostAddr, hostPort);
	buildSocketAddress(&peerAddr, peerPort);

	if ((*socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(*socketDescriptor, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %d\n\n", hostPort);


	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	message[strlen(message) - 1] = '\0';
	printf("Sending \"%s\" to server\n\n", message);

	initMessage(&peerRequest, NO_TOKEN, JOIN, NO_SEQ, message);

	sendto(*socketDescriptor, &peerRequest, sizeof(struct message_t), 0, (struct sockaddr *)&peerAddr, sizeof(struct sockaddr_in));

	recvfrom(*socketDescriptor, nextPeerAddr, sizeof(struct sockaddr_in), 0 , NULL, NULL);

	*nextPeerPort = ntohs((*nextPeerAddr).sin_port);

	printf("Next Peer Information:\n"
			"IP Address: %s\n"
			"Port Number: %d\n\n",
			inet_ntoa((*nextPeerAddr).sin_addr), *nextPeerPort);
}

void initMessage(struct message_t *message, int token, int action, int sequenceNumber, char *messageText)
{
	char tmpHeader[32];
	(*message).header.token = token;
	(*message).header.action = action;
	(*message).header.sequenceNumber = sequenceNumber;

	if(sequenceNumber == NO_SEQ && messageText != NULL)
	{
		snprintf((*message).messageBody, BODYSIZE, "%s", messageText);
	}
	else if(messageText != NULL)
	{
		sprintf(tmpHeader, HEADER, sequenceNumber);
		sprintf((*message).messageBody, tmpHeader, "%s\n%s", messageText, FOOTER);
	}

	printf("%d\t%d\t%d\n%s\n\n",(*message).header.token,(*message).header.action, (*message).header.sequenceNumber, (*message).messageBody);
}




