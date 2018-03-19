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
 * @brief	createMessage		The createMessage() function creates a HTTP/1.1 request based on the file specified by the
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
 *	@brief	sendMessage		The sendMessage() function sends the HTTP/1.1 GET request created by createMessage() to the
 *	server.
 *
 *	@parm	message			Pointer to the beginning of the HTTP/1.1 GET request
 */
void sendMessage(char *message);

/*
 *	@brief	receiveResponse		The receiveResponse() function receives the response of the server. The function initially
 *	assumes that the response will be short. However, if the response becomes greater than half of the size of the buffer,
 *	the function assumes that response includes a large file that will need to be taken in chunks and it handle appropriately.
 *	The function also calls the processResponse() function to handle the servers response.
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 */
void receiveResponse(char *response, int res_max);

/*
 *	@brief	processResponse		The processResponse() function handles the response message sent by the server by printing
 *	out the status line of the response message and downloading the file, if it is attached. processResponse() handles
 *	small and large file by calling downloadSmallFile() and downloadLargeFile() respectively.
 *
 * @parm	response		Pointer to beginning of response buffer
 *
 * @parm	res_max			Size of response buffer
 *
 * @parm	bytesInBuffer	This variable determines wither there is a large or small file. If the value is greater than 0
 * there is a large file and the variable tells how many bytes are in the response buffer currently
 */
void processResponse(char *response, int res_max, int bytesInBuffer);

/*
 * @brief	downloadSmallFile		The downloadSmallFile() function assumes that the whole file is in the response buffer
 * and downloads the file into a file with the same name as the file which was requested with "Client_Copy_" attached to
 * the beginning to distinguish between the original files and the transfered files.
 *
 * @parm	file					Pointer to the beginning of the file in the response buffer
 */
void downloadSmallFile(char *file);

/*
 * @brief	downloadLargeFile		The downloadLargeFile() function assumes that the file to be downloaded is going to be
 * larger than the response buffer, so the function reads from the server and writes to the file simultaneously. Similar to
 * downloadSmallFile(), the function downloads the file into a file with the same name as the file request with "Client_Copy_"
 * attached to the beginning of the file name to distinguish between the original files and the transfered files.
 *
 * @parm	headBuffer				Pointer to the beginning of the buffer used to receive the file
 *
 * @parm	bufferSize				Size of the buffer used to receive the file
 *
 * @parm	receivedBytes			Number of bytes of already received from the server
 */
void downloadLargeFile(char *headBuffer, int bufferSize, int receievedBytes);

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
 *	@parm	readNotDownloaded	Pointer to the location where the number of bytes that have been read but not downloaded is stored
 *
 *	@parm	bytes				Pointer to the beginning of the temporary buffer to be added the the response buffer
 *
 *	@parm	sizeOfBytes			Number of bytes in the temporary buffer
 */
void addBytes2Buffer(char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded, char *bytes, int sizeOfBytes);

/*
 * @brief	writeBytes2File		The writeBytes2File() function writes the bytes in the response buffer to the file sent
 * by the server
 *
 * @parm	fp					FILE pointer of the file writing the file to
 *
 * @parm	headBuffer			Pointer to the head of the response buffer used to download the file
 *
 * @parm	tailBuffer			Pointer to the end of the response buffer used to download the file
 *
 * @parm	curr				Pointer to the current position in the response buffer
 *
 * @parm	readNotDownloaded	Pointer to the location where the number of bytes that have been downloaded but not writen to the file
 *
 */
void writeBytes2File(FILE *fp, char *headBuffer, char *tailBuffer, char *curr, int *readNotDownloaded);

/*
 * @brief	displayFile			The displayFile() function displays the downloaded file in the browser sensed by the OS.
 */
void displayFile(void);


#endif /* HTTPCLIENT_H_ */
