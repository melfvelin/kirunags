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

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2009

namespace server
{
	int setup(void);
	void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType);
}

// global variable declaration 
extern UL_TABLE sUlTable;

#endif /* SERVER_H */
