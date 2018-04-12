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

	getNextPeer(&nextPeerAddr, &nextPeerPort, sendPort, hostPort, &sockD);


	close(sockD);
	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void validateArgv(int argc, char *argv[], int *serverPort, int *hostPort, char *filename)
{


	if(argc != 5 || argc != 6)
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
		*serverPort = atoi(argv[3]);
		*hostPort = atoi(argv[4]);
		filename = argv[5];
	}
	else
	{
		if(!strcmp(argv[2], "localhost"))
					error("Program does not support nonlocal message passing\n"
							"Parameter Format: bbpeer [-new] localhost <portNum> <hostPort> <filenameBulletinBoard>\n");
		*serverPort = atoi(argv[2]);
		*hostPort = atoi(argv[3]);
		filename = argv[4];
	}
	if(*serverPort < 60000 || *serverPort > 60099)
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

void getNextPeer(struct sockaddr_in *nextPeerAddr, int *nextPeerPort, int sendingPort, int hostPort, int *socketDescriptor)
{
	struct sockaddr_in sendingAddr;
	struct sockaddr_in hostAddr;

	char message[256];

	buildSocketAddress(&hostAddr, hostPort);
	buildSocketAddress(&sendingAddr, sendingPort);

	if ((*socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("Error creating socket\n");

	if((bind(*socketDescriptor, (struct sockaddr *)&hostAddr, sizeof(hostAddr))) <0)
		error("Error binding socket\n");

	printf("Socket bound with port number %s\n\n", hostPort);


	printf("Please enter a message:\n");
	fgets(message, sizeof(message), stdin);

	printf("Sending %s to server\n\n");
	sendto(*socketDescriptor, message, strlen(message), 0, (struct sockaddr *)&sendingAddr, sizeof(struct sockaddr_in));

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



