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
 * @breif	validateArgv	The validateArg() function checks to make sure that the command line arguments are
 * valid for the server to use. The port number must be within 60,000 and 60,099 because the UWF ssh servers
 * only allow those ports to be used by the users. Also the number of hosts has to be greater than 0.
 *
 * The program will then store the server port and number of hosts in the locations of the serverPort and
 * numHosts locations.
 *
 * @param	argc			Number of parameters in the command line vector
 *
 * @param	argv			Command line vector of user input
 *
 * @param	serverPort		Pointer to the location to store the value of the server's port
 *
 * @param	numHosts		Pointer to the location to store the number of hosts
 */
void validateArgv(int argc, char *argv[], int *serverPort, int *numHosts);

/*
 * @brief	createBindSocket		The createBindSocket() function creates and binds a socket given the parameters
 * passed into the function. The serverPort parameter stores the location of the server port number to bind to.
 * The address of the server is stored at the serverAddr pointer location and the socket descriptor of the socket
 * is stored at the location pointed to at socketDescriptor.
 *
 * @param	serverAddr				Pointer to the location to store the server's socket address
 *
 * @param	serverPort				Pointer to the location that stores the server port number, used to create the
 * socket
 *
 * @param	socketDescriptor		Pointer to the location to store the value of the socket descriptor for the
 * created socket
 */
void createBindSocket(struct sockaddr_in *serverAddr, int *serverPort, int *socketDescriptor);

/*
 *	@brief	acceptPeers			The acceptPeers() function listens for peers wishing to join the bulletin board ring.
 *	When a peer contacts the server, the server stores the peer's socket address in the array of socket addresses
 *	that is passed to the function as the peerAddresses parameter.
 *
 *	@param	peerAddresses		Pointer to the location of the start of the array of peer socket addresses. The
 *	function assumes that the array has already been allocated enough memory for the number of peers to join.
 *
 *	@param	numberOfPeers		This variable tells the function how many peers to accept before ending
 *
 *	@param	socketDescriptor	This variabel tells the function which socket to use
 */
void acceptPeers(struct sockaddr_in *peerAddresses, int numberOfPeers, int socketDescriptor);

/*
 * @breif	createRing			This function sends out the addresses of the peer's next peer in the ring.
 *
 * @param	peerAddresses		Array of peer addresses that will consist of the ring
 *
 * @param	numberOfPeers		Number of peers that will be in the ring
 *
 * @param	socket				Socket descriptor for the socket of the server
 */
void createRing(struct sockaddr_in *peerAddresses, int numberOfPeers, int socket);



#endif /* BBSERVER_H_ */
