/*
 * @file httpClient.c		This file contains all of the function prototypes and function descriptions for the functions
 * used in the httpClient.c file. All information about specific functions will be found in this file above the individual
 * functions. Description of what the httpServer program does can be found in the header of the httpClient.c file.
 *
 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include"headerFiles.h"
//Constants for the client program
#define SERV_PORT 60073						//Port number used by the server
#define MES_MAX 1024						//Maximum size of a client can send to the server
#define RES_MAX 4096						//Maximum size of response buffer

int SOCKET_D;								//Socket Descriptor
char SERVER_NAME[17];						//IP address of the server in character form
struct sockaddr_in SERVER_ADDR;				//Actual IP address of the server
char FILE_NAME[128];						//Name of the file to get from the server


/*
 *	@brief	error		The error() function produces an error message using perror() then terminates the program.
 *
 *	@parm	msg			Message to display as the error message
 */
void error(const char *msg);

/*
 *	@brief	getAddressFile		The getAddressFile() function asks the user for the IP address in dotted decimal form and
 *	the name of the file to receive from the server and stores that information into SERVER_NAME and FILE_NAME respectively
 */
void getAddressFile(void);

/*
 *	@brief	connectServer		The connectServer() function uses the character IP address to find the server.
 *	NOTE: Currently the client and server has only been able to connect when the client and server are on the same device.
 *	When using the UWF SSH server and a VM, the client could not find the server given the character IP address.
 *
 */
void connectServer(void);

/*
 *	@brief	transferData		The transferData() function creates a HTTP/1.1 request based on the user input using the
 *	createMessage() function. The function the calls the sendMessage() function to send the HTTP/1.1 request to the server.
 *	Then the function receives the response with help of receieveResponse() function, which receives and handles the
 *	response.
 */
void transferData(void);

/*
 * @brief	createMessage		The createMessage function creates a HTTP/1.1 request based on the file specified by the
 * user. The function uses the messageFormat and input, which is the name of the file, to create an HTTP/1.1 GET request.
 * The GET request is stored in a character array starting at the message pointer.
 *
 * @parm	message				Pointer to the beginning of the message buffer
 *
 * @parm	messageFormat		Format of the HTTP/1.1 GET request
 *
 * @parm	input				Name of file to be requested
 *
 * @parm	messageSize			Max size of the message
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
void processResponse(char *response, int res_max, int bytesInBuffer);

/*
 *
 */
void downloadSmallFile(char *file);

/*
 *
 */
void downloadLargeFile(char *headBuffer, int bufferSize, int receievedBytes);

/*
 *
 */
void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded, char *bytes, int sizeOfBytes);

/*
 *
 */
void writeBytes2File(FILE *fp, char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded);

/*
 *
 */
void displayFile(void);


#endif /* HTTPCLIENT_H_ */
