 * @author Jacob Rook
 * @date 03/09/2018
 * @info Course COP 4635
 * Project 1

 * @file httpServer.c		This file contains all of the function definitions for the functions used the the httpServer
 * program. Description on how the function works can be found in this file. Description of what the individual functions
 * do can be found in the httpServer.h file. The httpSever program is a program that is suppose to act as an HTTP/1.1
 * server to respond to clients that connect to the server using TCP. The program creates a listen socket to listen for
 * clients. When a client connects to the server, the server processes the message sent by the client and produces an
 * appropriate response to send to the client.
 *
 * Currently, the only HTTP/1.1 method supported by this server is the GET method. The server sends proper HTTP/1.1
 * status lines, however, the sending of the file requested currently does not work properly.