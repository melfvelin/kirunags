#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include "tables.h"
#include "tmtc.h"

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2008

namespace client
{
	uint8_t *makeTC_frame(const char *message, uint32_t message_len);
	void grc_connect(void);
}

#endif /* CLIENT_H */
