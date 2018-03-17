/*
 * @file httpClient.c
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include"headerFiles.h"

#define SERV_PORT 60073
#define MES_MAX 1024
#define RES_MAX 4096
#define MAX_BUFFER_SIZE 257

int SOCKET_D;
char SERVER_NAME[17];
struct sockaddr_in SERVER_ADDR;
char FILE_NAME[128];


int RECV_SIZE;
int LEN;
int MAX_LEN;
char BUFFER[MAX_BUFFER_SIZE];
char *BUFF_PTR;

/*
 *
 */
void error(const char *msg);

/*
 *
 */
void getAddressFile(void);

/*
 *
 */
void connectServer(void);

/*
 *
 */
void transferData(void);

/*
 *
 */
void createMessage(char *message, char *messageFormat, char *input, int messageSize);

/*
 *
 */
void sendMessage(char *message);

/*
 *
 */
void receiveResponse(char *response, int res_max);

/*
 *
 */
void processResponse(char *response);

#endif /* HTTPCLIENT_H_ */
