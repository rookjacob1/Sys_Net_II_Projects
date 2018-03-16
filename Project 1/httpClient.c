/*
 * @file httpClient.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */
#include"httpClient.h"

int main (int argc, char *argv[])
{
	checkArguements(argc, argv);

	printf("\n\n\nClient Started\n\n");

	connectServer();

	transferData();


	return 0;
}

void checkArguements(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("\nPlease enter the server address/name and the file name:\n"
				"%d\n"
				"usage: ./httpClient <address/name> <content>\n",argc);
		exit(1);
	}
	SERVER_NAME = argv[1];
	strcpy(FILE_NAME,argv[2]);
}

void connectServer(void)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0 , sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;

	char tmpServerPort[16];

	sprintf(tmpServerPort, "%d", SERV_PORT);

	printf("Client searching for %s Server\n\n", SERVER_NAME);

	if (getaddrinfo(SERVER_NAME, tmpServerPort, &hints, &result) != 0)
	{
		perror("Error. Address info not obtained");
		exit(1);
	}

	for(rp = result; rp != NULL; rp = rp->ai_next)
	{
		SOCKET_D = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(SOCKET_D == -1)
			continue;

		if(connect(SOCKET_D, rp->ai_addr, rp->ai_addrlen) != -1)
			break;
		close(SOCKET_D);
	}

	if(rp == NULL)
	{
		perror("Error: Server could not be found");
		exit(1);
	}

	SERVER_ADDR = *(struct sockaddr_in *)(rp->ai_addr);

	printf("Client connected to Server.\n\n");

	freeaddrinfo(result);
}

void transferData(void)
{
	LEN = 0;
	MAX_LEN = sizeof(BUFFER);

	printf("Client Sending Data to Server\n\n");

	send(SOCKET_D, FILE_NAME, strlen(FILE_NAME), 0);

	printf("Client Data Sent to Server\n\n");

	printf("Client Receiving Data from Server\n\n");

	while((RECV_SIZE = recv(SOCKET_D, BUFF_PTR, (size_t)(MAX_BUFFER_SIZE), 0)) > 0)
	{
		BUFF_PTR += RECV_SIZE;
		MAX_LEN -= RECV_SIZE;
		LEN += RECV_SIZE;

	}

	BUFFER[LEN] = '\0';
	printf("Echoed string received: %s\n\n", BUFFER);

	close(SOCKET_D);

	printf("Client Shutting Down");
}
