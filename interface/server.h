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
#include "tables.h"
#include "SetConf.h"
#include "ServFuncs.h"

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2009

namespace server
{
	int setup(void);
}

// global variable declaration 
extern UL_TABLE sUlTable;
extern DL_TABLE sDlTable;

#endif /* SERVER_H */
