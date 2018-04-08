/*
 * @file bbserver.h
 *
 * @author Jacob Rook
 * @date 02/10/2018
 * @info Course COP 4635
 * Project 1
 */

#ifndef BBSERVER_H_
#define BBSERVER_H_

#include"headerFiles.h"
#include"bulletinBoard.h"

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
