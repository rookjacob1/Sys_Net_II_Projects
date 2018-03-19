/*
 * @file httpServer.h		This file contains all of the function prototypes and function descriptions for the functions
 * used in the httpServer.c file. All information about specific functions will be found in this file above the individual
 * functions. Description of what the httpServer program does can be found in the header of the httpServer.c file.
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
int SOCKET_D;									//Socket descriptor

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
 * @brief createResponse	The createResponse function parses the message received from the client and determines
 * which method is requested and calls that method. If the method is not supported, an error message is sent to the client.
 * Currently, only the GET method is supported.
 *
 * @parm	message			Pointer to message received from client
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void createResponse(char *message, char *response, int res_max);

/*
 *	@brief	GET_Method		The GET_Method() function implements the HTTP/1.1 GET method which requests a document from
 *	the server. The function parses the message even further to get the URL of the file requested and the version of
 *	HTTP. The format of the request line of the message sent by the client. Then, GET_Header_Lines() is called to handle
 *	the header line of the message, currently this function is not implemented. So, header lines are not handled.
 *	The file requested is then found and sent to the client using the GET_Attach_File() function.
 *
 * @parm	message			Pointer to message received from client
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void GET_Method(char *message, char *response, int res_max);

/*
 * @brief	GET_Header_Lines		The GET_Header_Lines() function would be used to handle the header lines after the
 * request line of the HTTP/1.1 request message. Currently, the program does not handle the header line, so they are
 * ignored.
 *
 * @parm	message			Pointer to message received from client
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void GET_Header_Lines(char *message, char *response, int res_max);

/*
 *	@brief	GET_Attach_File		The GET_Attach_File() function attempts to find the file the client requests. If the file
 *	is not found, a Not Found status line is sent to the client with no body. If the file is found, an OK status line is
 *	sent to the client. Note, the response is sent here so the SKIP_SEND variable is set to indicate that the response has
 *	been sent and the startServer() function does not need to send the response. Then, the GET_SendFile() function is called
 *	to send the data of the file to the client.
 *
 * @parm	URL				Pointer to the URL of the file requested
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void GET_AttachFile(char *URL, char *response, int res_max);

/*
 *	@brief	GET_SendFile		The GET_SendFile() function is used to read the requested file and send it to the client.
 *	The function creates an extra buffer for the file to be read into. This is due to the help the problem of the original
 *	response buffer wrap around, also to make reading more efficient by not restricting the amount of bytes able to be read
 *	by the position of the end of the buffer at a specific time. The file is read into the temporary buffer and the
 *	temporary buffer and the response buffer are sent to the addBytes2Buffer() function to add the temporary buffer to the
 *	response buffer. The sendBytes2Client() function is then called to send bytes of the response buffer to the client.
 *	This cycle continues until the end of the file is reached.
 *
 * @parm	fp				File pointer to the file to be sent
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void GET_SendFile(FILE *fp, char *response, int res_max);

/*
 *	@brief	addBytes2Buffer		The addBytes2Buffer() function adds the bytes in the temporary buffer to the response
 *	buffer.
 *
 *	@parm	headBuffer			Pointer to the start of the response buffer
 *
 *	@parm	tailBuffer			Pointer to the end of the response buffer
 *
 *	@parm	curr				Pointer to the current position in the response buffer
 *
 *	@parm	readNotSent			Pointer to the location where the number of bytes that have been read but not sent is stored
 *
 *	@parm	bytes				Pointer to the beginning of the temporary buffer to be added the the response buffer
 *
 *	@parm	sizeOfBytes			Number of bytes in the temporary buffer
 */
void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent, char *bytes, int sizeOfBytes);

/*
 *	@breif	sendBytes2Client	The sendBytes2Client() function sends the bytes in the response buffer to the client
 *
 *	@parm	headBuffer			Pointer to the start of the response buffer
 *
 *	@parm	tailBuffer			Pointer to the end of the response buffer
 *
 *	@parm	curr				Pointer to the current position in the response buffer
 *
 *	@parm	readNotSent			Pointer to the location where the number of bytes that have been read but not sent is stored
 */
void sendBytes2Client(char *headBuffer, char *tailBuffer, char *curr, int *readNotSent);

/*
 *	@brief	sendResponse		The sendResponse() function sends the bytes in the response buffer to the client
 *
 *  @parm	response			Pointer to beginning of response buffer
 */
void sendResponse(char *response);

#endif /* HTTPSERVER_H_ */
