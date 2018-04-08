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
 * @breif	validateArgv
 *
 * @param	argc
 *
 * @param	argv
 */
int validateArgv(int argc, char *argv[], int *serverPort, int *numHosts);

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
