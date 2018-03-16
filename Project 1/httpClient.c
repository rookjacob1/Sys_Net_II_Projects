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
	if(argc != 3)
	{
		printf("\nPlease enter the server address/name and the file name:\n"
				"\n"
				"usage: ./httpClient <address/name> <content>\n");
		exit(1);
	}
	SERVER_NAME = argv[1];
	strcpy(FILE_NAME,argv[2]);
}

void connectServer(void)
{
	memset(&SERVER_ADDR, 0 , sizeof(SERVER_ADDR));
	SERVER_ADDR.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_NAME, &SERVER_ADDR.sin_addr);
	SERVER_ADDR.sin_port = htons(SERV_PORT);
	if((SOCKET_D = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error. Socket Creation Failed.");
		exit(1);
	}

	if(connect(SOCKET_D, (struct sockaddr *)&SERVER_ADDR, sizeof(SERVER_ADDR)) < 0)
	{
		perror("Error. Connection Failed");
		exit(1);
	}

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
