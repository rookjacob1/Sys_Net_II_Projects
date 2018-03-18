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

#define RES_STATUS_OK  "HTTP/1.1 200 OK\r\n\r\n"
#define RES_STATUS_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define RES_STATUS_NOT_FOUND  "HTTP/1.1 404 Not Found\r\n\r\n"
#define RES_STATUS_METH_NOT_ALLOWED "HTTP/1.1 405 Method Not Allowed\r\n\r\n"
#define RES_STATUS_NOT_IMPLEMENTED "HTTP/1.1 501 Not Implemented\r\n\r\n"


int LISTEN_SOCKET_D;
int SOCKET_D;

char SERVER_NAME[32];
struct sockaddr_in SERVER_ADDR;
struct sockaddr_in CLIENT_ADDR;
socklen_t CLNT_ADDR_LEN;

int SKIP_SEND;

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

/*
 *
 */
void GET_Method(char *message, char *response, int res_max);

/*
 *
 */
void GET_Header_Lines(char *message, char *response, int res_max);

/*
 *
 */
void GET_AttachFile(char *URL, char *response, int res_max);

/*
 *
 */
void GET_SendFile(FILE *fp, char *response, int res_max);

/*
 *
 */
void sendResponse(char *response);

#endif /* HTTPSERVER_H_ */
