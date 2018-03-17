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
#define MAX_BUFFER_SIZE 257



int SOCKET_D;
int RECV_SIZE;
char *SERVER_NAME;
int LEN;
int MAX_LEN;
char BUFFER[MAX_BUFFER_SIZE];
char *BUFF_PTR;
struct sockaddr_in SERVER_ADDR;
char FILE_NAME[128];

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

#endif /* HTTPCLIENT_H_ */
