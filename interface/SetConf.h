/*  SetConf.h
*   
*/
#ifndef SETCONF_H
#define SETCONF_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>

#include "tables.h"

void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType);

// global variable declaration 
extern UL_TABLE sUlTable;
extern DL_TABLE sDlTable;

#endif /* SETCONF_H */
