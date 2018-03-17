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
#define SERV_PORT 60073
#define MAX_BUFFER_SIZE 256

int LISTEN_SOCKET_D;
int SOCKET_D;
char BUFFER[MAX_BUFFER_SIZE];
char *BUFF_PTR;
int LEN;
int MAX_LEN;
int RECV_SIZE;

#define WAIT_SIZE 16

char SERVER_NAME[32];
struct sockaddr_in SERVER_ADDR;
struct sockaddr_in CLIENT_ADDR;
socklen_t CLNT_ADDR_LEN;

/*
 *
 */
void error(const char *msg);

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
 * @brief initVariables
 */
void initVariables(void);

/*
 * @brief processRequest
 */
void processRequest(void);


#endif /* HTTPSERVER_H_ */
