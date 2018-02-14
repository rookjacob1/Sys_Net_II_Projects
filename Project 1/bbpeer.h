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

#include"bulletinBoard.h"

	int READBIT;
	int WRITEBIT;
	int LISTBIT;
	int EXITBIT;


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
int validateArgv(int argc, char *argv[]);


#endif /* BBPEER_H_ */
