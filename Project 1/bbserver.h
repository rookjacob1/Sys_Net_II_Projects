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


int validateArgv(int argc, char *argv[]);

void createRing(struct sockaddr_in *peerAddresses, int numberOfPeers);



#endif /* BBSERVER_H_ */
