/*
 * @file bbserver.c
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#include"bbserver.h"

int main(int argc, char *argv[])
{
	if(!validateArgv(argc, argv))
	{
		printf("Program Terminating.");
		return 0;
	}

	//Declare Variables
	int i;

	int sock;
	int numberHosts = atoi(argv[2]);

	struct sockaddr_in servAddr;
	struct sockaddr_in *peerAddrs = (struct sockaddr_in *)malloc(numberHosts * sizeof(peerAddrs));

	//Build local server socket address
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//Create socket
	sock = socket(PF_INET,SOCK_DGRAM,0);
	if(sock < 0)
	{
		perror("Error: Socket Failed");
		exit(1);
	}

	//Bind socket to local address and port
	if((bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0 )
	{
		perror("Error: Bind Failed");
		exit(1);
	}

	printf("Socket was created and binded to local address and port\n");

	//Receive numberHost peer information
	for(i = 0; i < numberHosts; i++)
	{
		printf("Waiting for peers to join.\n");
		//Receive peer information
		if ((recvfrom(sock, NULL, 0, 0,(struct sockaddr *)&peerAddrs[i], NULL)) < 0)
		{
			perror("Error: Received Message Error");
			exit(1);
		}
		printf("Peer received with IP address: %lu Port address: %hu \n",
				(unsigned long)peerAddrs[i].sin_addr.s_addr, peerAddrs[i].sin_port);
	}

	createRing(peerAddrs,numberHosts, sock);

	printf("Ring created. Server Terminating.");

	return 0;
}


int validateArgv(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Invalid Parameters.\n"
				"Parameter Format: Port_Number Number_of_Hosts\n");
		return 0;
	}

	int portNum = atoi(argv[1]);
	int numberHosts = atoi(argv[2]);

	if(numberHosts <= 0 || portNum < 60000 || portNum > 60099)
	{
		printf("Invalid Parameter Values.\n"
				"Port Number must be between 60,000 and 60,099\n"
				"Must have one or more hosts to join.\n");
		return 0;
	}

	return 1;
}

void createRing(struct sockaddr_in *peerAddresses, int numberOfPeers, int socket)
{
	int i;

	for(i = 0; i < numberOfPeers - 1; i++)
	{
		if ((sendto(socket, &peerAddresses[i + 1], sizeof(peerAddresses[i + 1]), 0,
				(struct sockaddr *)&peerAddresses[i], sizeof(peerAddresses[i]))) < 0 )
		{
			perror("Error: Sending Error");
			exit(1);
		}
	}
	if( (sendto(socket, &peerAddresses[0], sizeof(peerAddresses[0]), 0,
			(struct sockaddr *)&peerAddresses[numberOfPeers - 1],
			sizeof(peerAddresses[numberOfPeers]))) < 0)
	{
		perror("Error: Sending Error");
		exit(1);
	}

}
