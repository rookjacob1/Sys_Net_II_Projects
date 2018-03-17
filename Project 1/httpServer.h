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
#define MES_MAX 1024
#define RES_MAX 4096
#define WAIT_SIZE 16

int LISTEN_SOCKET_D;
int SOCKET_D;

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
 *
 */
void receiveMessage(char *message, int mes_max);

/*
 * @brief processRequest
 */
void createResponse(char *message, char *response, int res_max);


#endif /* HTTPSERVER_H_ */
