/*  server.h
*   includes for server.cpp
*/
#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2009

int main(int argc, char const *argv[]);

#endif /* SERVER_H */
