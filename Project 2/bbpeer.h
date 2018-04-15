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
const char HEADER[] = "<message n=%3d>\n";
const char FOOTER[] = "\n</message>\n";
#define MESSAGE_SIZE 256
#define HEADER_SIZE 16
#define FOOTER_SIZE 13
#define BODY_SIZE (MESSAGE_SIZE - HEADER_SIZE - FOOTERSIZE)

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

struct message_Header_t{
	int token;
	int action;
	int sequenceNumber;
};

struct message_t{

	struct message_Header_t header;
	char messageBody[MESSAGE_SIZE + 1];

};

//Global variables to use inside bulletinBoardRing()
struct sockaddr_in NEXT_PEER_ADDR;
int NEXT_PEER_PORT;
int HOST_PORT;
int SOCKET_D;
char *FILENAME;
struct message_t OUT_MESSAGE;
int HAVE_TOKEN;

//Variables for bulletinBoardEditing() thread to communicate with main thread
int READ_BIT;
int WRITE_BIT;
int LIST_BIT;
int EXIT_BIT;
char WRITE_MESSAGE[MESSAGE_SIZE + 1];

//bulletinBoardEditing() thread variables
pthread_t TID;
pthread_mutex_t PRINT_LOCK;


/*
 *
 */
void error(const char *msg);

/*
 *
 */
void buildSocketAddress(struct sockaddr_in *socketAddress, int socketPort);

/*
 *
 */
void initMessage(struct message_t *message, int token, int action, int sequenceNumber, char *messageText);

/*
 *
 */
void mutexPrint(const char *str);

/*
 *
 */
void cleanStdin(void);

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
void validateArgv(int argc, char *argv[], int *sendPort);

/*
 *
 */
void getNextPeerFromServer(int serverPort);

/*
 *
 */
void getNextPeerFromPeer(int peerPort);

/*
 *
 */
void bulletinBoardRing(void);

/*
 * determineInitiator works properly with print statements.
However, one of the peers was printing 0 for the port address of the recieved port. Not really a problem but something to try to fix later
 */
void determineInitiator(void);

/*
 *
 */
void processNextMessage(void);

/*
 *
 */
void handleJoin(struct sockaddr_in *joiningPeerAddr);

/*
 *
 */
void handleExit(struct message_t *receivedMessage);

/*
 *
 */
void checkUserInput(void);

/*
 *
 */
void bulletinBoardWrite(void);

/*
 *
 */
void bulletinBoardRead(void);

/*
 *
 */
void bulletinBoardList(void);

/*
 *
 */
void bulletinBoardExit(void);

/*
 *
 */
void *bulletinBoardEditing(void *parm);

/*
 *
 */
void userWrite(void);

/*
 *
 */
void userRead(void);

/*
 *
 */
void userList(void);

/*
 *
 */
void userExit(void);






#endif /* BBPEER_H_ */
