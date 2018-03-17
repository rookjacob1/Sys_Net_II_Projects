/*
 * @file httpClient.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */
#include"httpClient.h"

int main (void)
{
	getAddressFile(argc, argv);

	printf("\n\n\nClient Started\n\n");

	connectServer();

	transferData();


	return 0;
}

void getAddressFile(void)
{
	if(argc != 3)
	{
		printf("\nPlease enter the server address/name and the file name:\n"
				"\n"
				"usage: ./httpClient <address/name> <content>\n");
		exit(1);
	}
	SERVER_NAME = argv[1];
	strcpy(FILE_NAME,"GET /user/bin/image.html HTTP/1.1 \r\n\r\n");
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

	write(SOCKET_D, FILE_NAME, strlen(FILE_NAME));

	printf("Client Data Sent to Server\n\n");

	printf("Client Receiving Data from Server\n\n");

	read(SOCKET_D, BUFFER, sizeof(BUFFER));


	BUFFER[strlen(BUFFER)] = '\0';
	printf("Echoed string received: %s\n\n", BUFFER);

	close(SOCKET_D);

	printf("Client Shutting Down");
}
