/*  server.h
*   includes for server.cpp
* author: Martin Elfvelin
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
#include <sys/time.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <vector>

#include "tables.h"
#include "ServerFuncs.h"

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2009

namespace server
{
	void FindAsmOne(const uint8_t *in, uint32_t& nMsgType);
	void FindAsmTwo(const uint8_t *in, const uint32_t nFrameLen, uint32_t& nMsgType);
	void FindAsmThree(const uint8_t *in, uint32_t& nMsgType);
	void SetupServer(void);
}

// global variable declaration 
extern UL_TABLE sUlTable;
extern DL_TABLE sDlTable;

#endif /* SERVER_H */
