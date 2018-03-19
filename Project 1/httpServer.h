/*
 * @file httpServer.h		This file contains all of the function prototypes and function descriptions for the functions
 * used in the httpServer.c file. All information about specific functions will be found in this file above the individual
 * functions. Description of what the httpServer program does can be found in the header of the httpClient.c file.
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include"headerFiles.h"

//Constants for the server program
#define SERV_PORT 60073							//Port number used by the server
#define MES_MAX 1024							//Maximum size of a client can send to the server
#define RES_MAX 4096							//Maximum size of response buffer
#define WAIT_SIZE 16							//Wait size for the server

//Response status line formats
#define RES_STATUS_OK  "HTTP/1.1 200 OK\r\n\r\n"
#define RES_STATUS_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define RES_STATUS_NOT_FOUND  "HTTP/1.1 404 Not Found\r\n\r\n"
#define RES_STATUS_METH_NOT_ALLOWED "HTTP/1.1 405 Method Not Allowed\r\n\r\n"
#define RES_STATUS_NOT_IMPLEMENTED "HTTP/1.1 501 Not Implemented\r\n\r\n"


int LISTEN_SOCKET_D;							//Listen socket descriptor
int SOCKET_D;									//Client socket descriptor

char SERVER_NAME[32];							//Name of server in dotted decimal form: "255.255.255.255"
struct sockaddr_in SERVER_ADDR;					//Server address
struct sockaddr_in CLIENT_ADDR;					//Client address
socklen_t CLNT_ADDR_LEN;						//Client address length

int SKIP_SEND;									//Variable to indicate wither to skip sending a response

/*
 *	@brief	error		The error() function produces an error message using perror() then terminates the program.
 *
 *	@parm	msg			Message to display as the error message
 */
void error(const char *msg);

/*
 * @brief createSocket	The createSocket() function create the listen socket for the server, binds it to the host, and
 * starts to listen for clients.
 *
 */
void createSocket(void);

/*
 * @brief startServer	The startServer() function begins the process of accepting requests from clients and responding
 * to the requests.
 */

void startServer(void);

/*
 *	@brief	receiveMessage	The receiveMessage function reads the message sent by the client and stores the message in
 *	the message variable.
 */
void receiveMessage(char *message, int mes_max);

/*
 * @brief createResponse	The
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
void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent, char *bytes, int sizeOfBytes);

/*
 *
 */
void sendBytes2Client(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent);

/*
 *
 */
void sendResponse(char *response);

#endif /* HTTPSERVER_H_ */
