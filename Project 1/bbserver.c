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
	int sock;

	struct sockaddr_in servAddr;
	struct sockaddr_in *peerAddrs = (struct sockaddr_in *)malloc(atoi(argv[2]) * sizeof(peerAddrs));

	//Build local server socket address
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

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
