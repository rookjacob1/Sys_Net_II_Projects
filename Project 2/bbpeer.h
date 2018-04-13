/*
 * @file bbpeer.h
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef BBPEER_H_
#define BBPEER_H_

#include"headerFiles.h"

//Message Formats
#define HEADER "<message n=%d>\n"
#define FOOTER "</message>\n"
#define BODYSIZE 256

//Token Modes
#define PASS_TOKEN 1024
#define NO_TOKEN 1025
#define TOKEN_INIT 1026

//Action Modes
#define NO_ACTION 2048
#define JOIN 2049
#define EXIT 2050

//Sequence Modes
#define NO_SEQ -2048





int READBIT;
int WRITEBIT;
int LISTBIT;
int EXITBIT;


typedef struct Header{
	int token;
	int action;
	int sequenceNumber;
}message_Header_t;

typedef struct message{

	struct message_Header_t header;
	char messageBody[BODYSIZE];

}message_t;


/*
 *
 */
void error(const char *msg);

/*
 * @breif	validateArgv	The validateArgv() call checks if the arguments passed to the main
 * function are in the correct format and have appropriate values.
 *
 * @param	argc			Number of arguments passed to main()
 *
 * @param	argv			Argument vector passed to main()
 *
 * @return	int				validateArgv() returns 0 if the arguments indicate a peer joining a
 * new ring, 1 if the arguments indicate a peer joining an existing ring, and -1 if the arguements
 * are in the wrong format or have the wrong corresponding values.
 */
void validateArgv(int argc, char *argv[], int *sendPort, int *hostPort, char *filename);

/*
 *
 */
void getNextPeerFromServer(struct sockaddr_in *nextPeerAddr, int *nextPeerPort, int serverPort, int hostPort, int *socketDescriptor);

/*
 *
 */
void buildSocketAddress(struct sockaddr_in *socketAddress, int socketPort);

/*
 *
 */
void getNextPeerFromPeer(struct sockaddr_in *nextPeerAddr, int *nextPeerPort, int peerPort, int hostPort, int *socketDescriptor);

/*
 *
 */
void initMessage(message_t *message, int messageToken, int messageAction, int messageSequenceNumber, char *messageText);





#endif /* BBPEER_H_ */
