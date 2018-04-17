/*
 * @file bbserver.c			This file contains all of the function definitions for the bulletin board server program.
 * Description what the individual functions do and their purpose can be found in the bbserver.h file. This file only
 * contains the code for the functions.
 *
 * The bbserver program listens for peers, using UDP datagrams, trying to connect with other peers to create a ring to
 * share a common file to read and write to. When the server is started, the user will specify how many peers must be
 * contact the server before the server creates the ring. Once the number of received peers meet the allotted number,
 * the server wil create the ring by sending the individual peers their next member in the ring. Once the peers are
 * sent their next peers, the server shuts down and does not aid the peers anymore.
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#include"bbserver.h"

int main(int argc, char *argv[])
{
	//Declare Variables
	int sockD;				//Socket Descriptor
	int serverPort;			//Server Port Number
	int numberHosts;		//Number of peers to accept to make the ring


	struct sockaddr_in serverAddr;			//Server socket address
	struct sockaddr_in *peerAddrs;			//Array of peer addresses to store the joining peers

	validateArgv(argc, argv, &serverPort, &numberHosts);

	createBindSocket(&serverAddr, &serverPort, &sockD);

	peerAddrs = (struct sockaddr_in *)malloc(numberHosts * sizeof(struct sockaddr_in));

	acceptPeers(peerAddrs, numberHosts, sockD);

	createRing(peerAddrs, numberHosts, sockD);

	printf("Ring created. Server Terminating.\n\n");

	free(peerAddrs);			//freeing allocated memory
	close(sockD);				//Closing socket

	return 0;
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void validateArgv(int argc, char *argv[], int *serverPort, int *numHosts)
{
	if (argc != 3)
		error("Invalid Parameters.\n"
				"Parameter Format: bbserver <PortNum> <numberHosts>\n");


	*serverPort = atoi(argv[1]);
	*numHosts = atoi(argv[2]);

	if(*numHosts <= 0 || *serverPort < 60000 || *serverPort > 60099)
		error("Invalid Parameter Values.\n"
				"Port Number must be between 60,000 and 60,099\n"
				"Must have one or more hosts to join.\n");

}

void createBindSocket(struct sockaddr_in *serverAddr, int *serverPort, int *socketDescriptor)
{
	//Build local server socket address
	memset(serverAddr, 0, sizeof(struct sockaddr_in));
	(*serverAddr).sin_family = AF_INET;
	(*serverAddr).sin_port = htons(*serverPort);
	(*serverAddr).sin_addr.s_addr = htonl(INADDR_ANY);

	//Create socket
	*socketDescriptor = socket(AF_INET ,SOCK_DGRAM,0);
	if(*socketDescriptor < 0)
	{
		error("Error: Socket Failed");
		exit(1);
	}

	//Bind socket to local address and port
	if((bind(*socketDescriptor, (struct sockaddr *)serverAddr, sizeof(struct sockaddr_in))) < 0 )
	{
		error("Error: Bind Failed");
		exit(1);
	}

	printf("Socket was created and binded to local address and port\n\n");
}

void acceptPeers(struct sockaddr_in *peerAddresses, int numberOfPeers, int socketDescriptor)
{
	int i = 0;
	char buffer[256];
	buffer[255] = '\0';
	socklen_t len = sizeof(struct sockaddr_in);

	//Receive number of peer information
	while(i < numberOfPeers)
	{
		printf("Waiting for peers to join.\n");
		//Receive peer information
		if ((recvfrom(socketDescriptor, buffer, sizeof(buffer) - 1, 0,(struct sockaddr *)&peerAddresses[i], &len)) < 0)
		{
			perror("Error: Received Message Error");
			exit(1);
		}
		printf("Peer received with IP address: %s Port address: %d \n",
				inet_ntoa((peerAddresses[i]).sin_addr), ntohs((peerAddresses[i]).sin_port));
		printf("Peer's message: %s\n\n", buffer);

		i++;
	}
}

void createRing(struct sockaddr_in *peerAddresses, int numberOfPeers, int socketDescriptor)
{
	int i;

	for(i = 0; i < numberOfPeers - 1; i++)
	{
		printf("%d -> %d\n\n", ntohs((peerAddresses[i]).sin_port), ntohs((peerAddresses[i + 1]).sin_port));
		if ((sendto(socketDescriptor, &peerAddresses[i + 1], sizeof(struct sockaddr_in), 0,
				(struct sockaddr *)&peerAddresses[i], sizeof(struct sockaddr_in))) < 0 )
		{
			error("Error: Sending Error");
		}
	}
	printf("%d -> %d\n\n", ntohs((peerAddresses[numberOfPeers - 1]).sin_port), ntohs((peerAddresses[0]).sin_port));
	if( (sendto(socketDescriptor, &peerAddresses[0], sizeof(struct sockaddr_in), 0,
			(struct sockaddr *)&peerAddresses[numberOfPeers - 1],
			sizeof(struct sockaddr_in))) < 0)
	{
		error("Error: Sending Error");
	}

}
