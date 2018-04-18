/*
 * @file bbpeer.h
 *
 * @author Jacob Rook
 * @date 04/17/2018
 * @info Course COP 4635
 * Project 2
 */

#ifndef BBPEER_H_
#define BBPEER_H_

#include"headerFiles.h"

//Bulletin Board Message Formating Information
/*
 * These constant variables are used in the formating of the messages written to the shared file between the
 * peers. The messages written to the file are of fixed size (MESSAGE_SIZE). The format of the messages written
 * to the file follow the following format:
 *
 * <message n=number>
 * <message body>
 * </message>
 *
 */

const char HEADER[] = "<message n=%3d>\n";			//Format of the headers of messages posted to the bulletin board
const char FOOTER[] = "\n</message>\n";				//Format of the footers of messages posted to the bulletin board
#define MESSAGE_SIZE 256							//Size of the messages posted to the bulletin board
#define HEADER_SIZE 16								//Size of the bulletin board headers
#define FOOTER_SIZE 13								//Size of the bulletin board footers
#define BODY_SIZE (MESSAGE_SIZE - HEADER_SIZE - FOOTERSIZE)		//Size of the bulletin board body

//END Bulletin Board Message Formating Information

//Peer Message Header Structure
struct message_Header_t{
	int token;
	int action;
};

//Peer Message Structure
struct message_t{

	struct message_Header_t header;
	char messageBody[MESSAGE_SIZE + 1];

};

//Valid Peer Message Header Values
/*
 * 	The message header values are used to communicate among the peers in the ring. Communicate is based on passing of
 * 	the token and actions that other peers need to take. When a message is sent the token and action value of the header
 * 	is set as one of the token and action modes shown below. The description of the token and action modes can be found
 * 	below.
 *
 * 	There are a couple of rules for sending message among the peers, the rules are as follows:
 *
 * 	RULES:
 * 	Tokens can not be passed with actions.
 * 	Messages with invalid modes are discarded.
 */

//Token Modes
#define PASS_TOKEN 1024					//Indication that the token is being passed
#define NO_TOKEN 1025					//Indication that the token is not being passed
#define TOKEN_INIT 1026					//Indication that the initiation process is occurring

//Action Modes
#define NO_ACTION 2048					//Indication that there is no action to be taken
#define JOIN 2049						//Indication that a peer is wishing to join the ring
#define EXIT 2050						//Indication that a peer is wishing to exit the ring
//END Valid Peer Message Header Values



//Scope global variables used inside bulletinBoardRing()
/*
 * These variable are global to the bbpeer files to avoid passing many arguments between functions. Short descriptions
 * of what the variables are used for are given below.
 */
struct sockaddr_in NEXT_PEER_ADDR;				//Socket address of the next peer in the ring to send messages to
int NEXT_PEER_PORT;								//Integer port number of the next peer in the ring to send messages to
int HOST_PORT;									//Integer host port number
int SOCKET_D;									//Integer of the socket descriptor
char *FILENAME;									//Pointer to the filename argument passed by the user
struct message_t OUT_MESSAGE;					//Peer to peer message variable for sending messages to other peers
int HAVE_TOKEN;									//Variable to indicate that the peer has the token
//END Scope global variables used inside bulletinBoardRing()



//Variables for bulletinBoardEditing() thread

/*
 * These variables are used for the bulletinBoardEditing() thread to communicate with the main thread. When these
 * variables are not 0, the main thread knows that the user has input a command. The user can make four commands:
 * read, write, list, and exit.
 *
 * The bulletinBoardEditing() thread only allows the user to place one command at a time. The bulletinBoardEditing()
 * thread will allow the user to input another command when the main thread handles the last command and sets the
 * given command bit variable to 0.
 *
 * READ:
 * When the user wants to read from the bulletin board file, the bulletinBoardEditing() thread asks which message
 * the user would like to read and stores that message index into the READ_BIT variable. The next time the main thread
 * checks to see if the user has input a command, the main thread will see that the READ_BIT variable is non-zero and
 * will use the value of the READ_BIT to read the specified message.
 *
 * WRITE:
 * When the user wants to write to the bulletin board file, the bulletingBoardEditing() thread reads the message that
 * the user wants to write to the file and stores that message with the FOOTER string appended to the end of the message
 * in the WRITE_MESSAGE array, then sets the WRITE_BIT to 1. The next time the main thread checks to see if the user has
 * input a command, the main thread will see that the WRITE_BIT variable is non-zero and will handle the write command.
 *
 * LIST:
 * When the user wants to list all of the messages that have been written to the bulletin board file, the bulletinBoardEditing()
 * thread sets the LIST_BIT to a value of 1 to notify the main thread that the user wants to read all of the message from the
 * bulletin board file. When the main thread has printed out all of the messages that have been written to the bulletin board
 * file, the main thread will set the LIST_BIT to zero, to let the bulletinBoardEditing() thread can allow the user to make
 * another command.
 *
 * EXIT:
 * When the user wants to exit the ring, the bulletinBoardEditing thread sets the EXIT_BIT variable to a value of 1, then
 * terminates. When the main thread sees that the EXIT_BIT has been set to 1, the main thread handles the peer exiting the
 * ring. Once it is safe for the peer to exit, the main thread sets the EXIT_BIT equal to the value of the action mode
 * for EXIT to break it's self out of the continuous loop of handling messages.
 *
 */
int READ_BIT;									//Variable to
int WRITE_BIT;
int LIST_BIT;
int EXIT_BIT;
char WRITE_MESSAGE[MESSAGE_SIZE + 1];

//bulletinBoardEditing() thread variables
pthread_t TID;
pthread_mutex_t PRINT_LOCK;

//END Variables for bulletinBoardEditing() thread

#define SLEEP_TIME 10




//SUPPLEMENTARY FUNCTIONS
/*
 *	Generic functions that help multiple functions.
 */

/*
 *	@brief	error		The error() function produces an error message using perror() then terminates the program.
 *
 *	@param	msg			Message to display as the error message
 */
void error(const char *msg);

/*
 *
 */
void buildSocketAddress(struct sockaddr_in *socketAddress, int socketPort);

/*
 *
 */
void initMessage(struct message_t *message, int token, int action, char *messageText);

/*
 *
 */
void mutexPrint(const char *str);

/*
 *
 */
void cleanStdin(void);
//END SUPLEMENTARY FUNCTIONS



//SET UP FUNCTIONS
/*
 * 	Functions that help start the ring.
 */

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
//END SET UP FUNCTIONS



//BULLETIN BOARD RING FUNCTIONS
/*
 *	These functions are used to handle the operation of the ring structure between all of the peers on the main
 *	thread. At this	point the peers have been sent their next member in the ring, and are ready to start the ring.
 *
 */

/*
 *
 */
void bulletinBoardRing(int init);



//RING INITIATION FUNCTIONS
/*
 * determineInitiator works properly with print statements.
However, one of the peers was printing 0 for the port address of the recieved port. Not really a problem but something to try to fix later
 */
void determineInitiator(void);

/*
 *
 */
void initRing(void);
//END RING INITIATION FUNCTIONS



//PEER TO PEER COMMUNICATION HANDLING FUNCTIONS
/*
 *
 */
void processNextMessage(void);

/*
 *
 */
void handleJoin(struct sockaddr_in *joiningPeerAddr, struct message_t *receivedMessage);

/*
 *
 */
void handleExit(struct message_t *receivedMessage);
//END PEER TO PEER COMMUNICATION HANDLING FUNCTIONS



//USER COMMAND HANDLING FUNCTIONS
/*
 *
 */

/*
 *
 */
void checkUserInput(void);

/*
 *
 */
int getFileSize(void);

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
//END USER COMMAND HANDLING FUNCTIONS
//END BULLETIN BOARD RING FUNCTIONS



//BULLETINBOARDEDITING() THREAD FUNCTIONS
/*
 *	These functions are for the bulletinBoardEditing(), that handles the users input.
 */

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
//END BULLETINBOARDEDITING() THREAD FUNCTIONS






#endif /* BBPEER_H_ */
