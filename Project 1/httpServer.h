/*
 * @file httpServer.h
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include"headerFiles.h"

	//Variables for the server program
#define MAX_BUFFER_SIZE 256
int LISTEN_SOCKET_D;
int SOCKET_D;
char BUFFER[MAX_BUFFER_SIZE];
char *BUFF_PRT;
int LEN;
int MAX_LEN = sizeof(BUFFER);
int RECV_SIZE;
int WAIT_SIZE;
struct sockaddrr_in SERVER_ADDR;
struct sockaddrr_in CLIENT_ADDR;
int CLNT_ADDR_LEN;

/*
 * @brief initVariables
 */
void initVariables(void);

/*
 * @brief createSocket
 *
 */
void createSocket(void);

/*
 * @brief startServer
 */

void startServer(void);

/*
 * @brief processRequest
 */
void processRequest(void);


#endif /* HTTPSERVER_H_ */
