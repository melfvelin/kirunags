/*  This is the TCP/IP client
*   Client side C++ program to transmit and receive TC and TM frames
*
*
*/
#include "client.h"

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
    int m_nTcSock = 0;
    int m_nTmSock = 0;

    int m_nStatSock = 0;
    int m_nSeshSock = 0;
    struct sockaddr_in m_sAddrSesh;
    struct sockaddr_in m_sAddrStat;
    struct sockaddr_in m_sAddrTc;
    struct sockaddr_in m_sAddrTm;
      
    uint8_t buffer[1024] = {0};
    
    if ((m_nTmSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n TM creation error \n");
        return -1;
    }

    if ((m_nTcSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n TC socket creation error \n");
        return -1;
    }

    if ((m_nSeshSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Session Socket creation error \n");
        return -1;
    }

    if ((m_nStatSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Status Socket creation error \n");
        return -1;
    }



    m_sAddrTm.sin_family = AF_INET;
    m_sAddrTm.sin_port = htons(2001);

    m_sAddrTc.sin_family = AF_INET;
    m_sAddrTc.sin_port = htons(2002);

    m_sAddrSesh.sin_family = AF_INET;
    m_sAddrSesh.sin_port = htons(8888);

    m_sAddrStat.sin_family = AF_INET;
    m_sAddrStat.sin_port = htons(8888);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrTm.sin_addr)<=0)
    {
        printf("\nInvalid TM address/ Addrescis not supported \n");
        return -1;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrTc.sin_addr)<=0)
    {
        printf("\nInvalid TC address/ Address not supported \n");
        return -1;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrSesh.sin_addr)<=0)
    {
        printf("\nInvalid TC address/ Address not supported \n");
        return -1;
    }

    if(inet_pton(AF_INET, "127.0.0.1", &m_sAddrStat.sin_addr)<=0)
    {
        printf("\nInvalid TC address/ Address not supported \n");
        return -1;
    }

    
    // if (connect(m_nTmSock, (struct sockaddr *)&m_sAddrTm, sizeof(m_sAddrTm)) < 0)
    // {
    //     printf("\nTM Connection Failed \n");
    //     return -1;
    // }
    
    
    // if (connect(m_nTcSock, (struct sockaddr *)&m_sAddrTc, sizeof(m_sAddrTc)) < 0)
    // {
    //     printf("\nTC Connection Failed \n");
    //     }
    
    if (connect(m_nSeshSock, (struct sockaddr *)&m_sAddrSesh, sizeof(m_sAddrSesh)) < 0)
    {
        printf("\nSession Connection Failed \n");
        return -1;
    }
    
    if (connect(m_nStatSock, (struct sockaddr *)&m_sAddrStat, sizeof(m_sAddrStat)) < 0)
    {
        printf("\nStatus Connection Failed \n");
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
    uint8_t *pnOut;
    uint8_t m_nTC = 'C';
    uint8_t *m_pnData;
    uint32_t m_nAckCode;
    uint32_t m_nMsgLen;

    pnOut = (uint8_t *)malloc(sizeof(STATUS_HEADER));
    ClientFuncs::EncapsulateStatus(pnOut, 1);
    std::cout << "Sending Status frame requesting uplink table" << std::endl;
    send(m_nStatSock, pnOut, sizeof(STATUS_HEADER), 0);        // Step 0.1
    free(pnOut);
    // currently the modscheme always prints as 3, need to check out why
    valread = read(m_nStatSock, buffer, 1024);              // Step 0.2
    memcpy(&m_sUlTableReply, &buffer, valread);
    std::cout << "Status reply, table type: " << m_sUlTableReply.nTabType << std::endl;
    std::cout << "mod scheme: " << m_sUlTableReply.nModScheme << std::endl;


    m_sUlTable.nModScheme = 5;
    send(m_nSeshSock, psUltable, sizeof(UL_TABLE), 0);        // Step 1
    std::cout << "Sending new uplink table (Session frame)" << std::endl;

    pnOut = (uint8_t *)malloc(sizeof(STATUS_HEADER));
    ClientFuncs::EncapsulateStatus(pnOut, 1);
    std::cout << "Sending Status frame requesting uplink table" << std::endl;
    send(m_nStatSock, pnOut, sizeof(STATUS_HEADER), 0);        // Step 2
    free(pnOut);

    // Client only receives conf tables on status socket
    valread = read(m_nStatSock, buffer, 1024);              // Step 3
    memcpy(&m_sUlTableReply, &buffer, valread);
    std::cout << "Status reply, table type: " << m_sUlTableReply.nTabType << std::endl;
    std::cout << "mod scheme: " << m_sUlTableReply.nModScheme << std::endl;

    //  Sending TC     
    m_nMsgLen = sizeof(TC_HEADER) + sizeof(m_nTC) + sizeof(uint32_t);
    m_pnData = &m_nTC;
    pnOut = (uint8_t *)malloc(m_nMsgLen);
    ClientFuncs::EncapsulateTC(pnOut, sizeof(m_nTC), m_pnData);
    std::cout << "Sending TC" << std::endl;
    send(m_nSeshSock, pnOut, m_nMsgLen, 0);                 // Step 4
    free(pnOut);

    // read to see if a TC ACK has arrived
    std::cout << "Waiting for TC ACK" << std::endl;
    valread = read(m_nSeshSock, buffer, 1024);              // Step 5

    pnOut = &buffer[0];
    ClientFuncs::DecapsulateTCACK(pnOut, m_nAckCode, m_pnData);
    if(m_nAckCode == 2)
    {
        std::cout << "TC accepted (client side)" << std::endl;        
    }


    return 0;
}
