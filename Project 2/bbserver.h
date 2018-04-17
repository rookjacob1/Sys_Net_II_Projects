/*
 * @file bbserver.h			This file contains all of the function prototypes and descriptions of the functions for the
 * bulletin board server program. The code of the functions can be found in bbserver.c.
 *
 * The bbserver program takes in two arguments from the command line:
 *
 * bbserver <PortNumber> <NumberHosts>
 *
 * The port number is used to bind the server to that port number, so the peers know which port to use. The number of
 * hosts is used to tell the server how many peers to wait for before starting the ring. The main function will take
 * these arguments and send them to the validateArgv() function to make sure that the arguments can be used. From
 * the port number the user provided, the server will bind to a socket with that given port number.
 *
 * Then the server will start waiting for peers to join, using the acceptPeers() function. Once the number of peers
 * specified by the user has joined, the creteRing() function will be called, which will send each peer it's next
 * peer to send messages to in the ring.
 *
 * The server will then shut down and will not assist the peers anymore.
 *
 * @author Jacob Rook
 * @date 04/17/2018
 * @info Course COP 4635
 * Project 2
 */

#ifndef BBSERVER_H_
#define BBSERVER_H_

#include"headerFiles.h"


/*
 *	@brief	error		The error() function produces an error message using perror() then terminates the program.
 *
 *	@parm	msg			Message to display as the error message
 */
void error(const char *msg);

/*
 * @breif	validateArgv
 *
 * @param	argc
 *
 * @param	argv
 */
void validateArgv(int argc, char *argv[], int *serverPort, int *numHosts);

/*
 *
 */
void createBindSocket(struct sockaddr_in *serverAddr, int *serverPort, int *socketDescriptor);

/*
 *
 */
void acceptPeers(struct sockaddr_in *peerAddresses, int numberOfPeers, int socketDescriptor);

/*
 * @breif	createRing
 *
 * @param	peerAddresses
 *
 * @param	numberOfPeers
 *
 * @param	socket
 */
void createRing(struct sockaddr_in *peerAddresses, int numberOfPeers, int socket);



#endif /* BBSERVER_H_ */
