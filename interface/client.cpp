/*  This is the TCP/IP client
*   Client side C++ program to transmit and receive TC and TM frames
*
*
*/
#include "client.h"
#include "tmtc.h"
#include "tables.h"

#ifndef debug
#define debug
#endif /* debug */

namespace client{
    
} /* client */


int main(int argc, char const *argv[])
{
    
    int valread;       
    int m_nSeshServ;
    int m_nStatServ;
    int m_nStatSock = 0;
    int m_nSeshSock = 0;
    struct sockaddr_in m_sAddrSesh;
    struct sockaddr_in m_sAddrStat;
      
    char buffer[1024] = {0};
    
    if ((m_nSeshSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    if ((m_nStatSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket2 creation error \n");
        return -1;
    }

    m_sAddrSesh.sin_family = AF_INET;
    m_sAddrSesh.sin_port = htons(2005);

    m_sAddrStat.sin_family = AF_INET;
    m_sAddrStat.sin_port = htons(2006);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrSesh.sin_addr)<=0)
    {
        printf("\nInvalid TM address/ Addrescis not supported \n");
        return -1;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrStat.sin_addr)<=0)
    {
        printf("\nInvalid TC address/ Address not supported \n");
        return -1;
    }

    if (connect(m_nSeshSock, (struct sockaddr *)&m_sAddrSesh, sizeof(m_sAddrSesh)) < 0)
    {
        printf("\nTM Connection Failed \n");
        return -1;
    }
    
    if (connect(m_nStatSock, (struct sockaddr *)&m_sAddrStat, sizeof(m_sAddrStat)) < 0)
    {
        printf("\nTC Connection Failed \n");
        return -1;
    }
      
    UL_TABLE m_sUlTable;
    UL_TABLE m_sUlTableReply;
    UL_TABLE *psUltable = &m_sUlTable;

    m_sUlTable.nPreamble = PREAMBLE;
    m_sUlTable.nMsglen = sizeof(UL_TABLE);
    m_sUlTable.nMsgType = 10;
    m_sUlTable.nTabType = 1;
    m_sUlTable.nScrambler = 0;
    m_sUlTable.nFEC = 0;
    m_sUlTable.nFrameFormat = 0;
    m_sUlTable.nLineCode = 0;
    m_sUlTable.nModScheme = 3;
    m_sUlTable.fBitRate = 1200;
    m_sUlTable.nPlopVersion = 0;
    m_sUlTable.nPostamble = POSTAMBLE;

    send(m_nSeshSock, psUltable, sizeof(UL_TABLE), 0);        // Step 1
    std::cout << "[client.cpp:main] Attempted to send session frame: " << std::endl;

    uint8_t *pnOut = (uint8_t *)malloc(sizeof(STATUS_HEADER));
    ClientFuncs::EncapsulateStatus(pnOut, 1);
    send(m_nStatSock, pnOut, sizeof(STATUS_HEADER), 0);        // Step 2
    free(pnOut);

    // Client only receives conf tables on status socket
    valread = read(m_nStatSock, buffer, 1024);              // Step 3
    std::cout << "[client.cpp:main] received bytes: " << valread << std::endl;
    memcpy(&m_sUlTableReply, &buffer, valread);
    std::cout << "Status reply:" << std::endl << "Table type: " << m_sUlTableReply.nTabType << std::endl;
    std::cout << "Mod scheme: " << m_sUlTableReply.nModScheme << std::endl;

    return 0;
}
