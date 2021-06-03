// This is the TCP/IP server
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>

#include "server.h"
#include "tmtc.h"
#include "tables.h"
#include "SetConf.h"


/*	uncomment for debug output
#ifndef debug
#define debug
#endif */ /* debug */


namespace server{
	int setup(void)
	{
		// Init of variables
		int server_tm;
		int server_tc;
		int m_nSeshServ;
		int m_nStatServ;
		int m_nStatSock = 0;
		int m_nSeshSock = 0;
		int valread;
		struct sockaddr_in m_sAddrSesh;
		struct sockaddr_in m_sAddrStat;
		int opt = 1;
		int addrlen = sizeof(sockaddr_in);
		
		// Init char array of zeros
		char m_cBuffer[1024] = {0};

		// Creating socket file descriptor STEP 1
		if((m_nSeshServ = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("[server.cpp:setup] Socket creation failed:");
			exit(EXIT_FAILURE);
		}

		// creating a socket file descriptor for tc socket
		if((m_nStatServ = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			perror("[server.cpp:setup] TC socket creation failed:");
			exit(EXIT_FAILURE);
		}

		// sets a socket option for tc socket, not necessary but good practice
		if(setsockopt(m_nSeshServ, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		{
			perror("[server.cpp:setup] setsockopt:");
			exit(EXIT_FAILURE);
		}

		// sets a socket option for tc socket, not necessary but good practice
		if(setsockopt(m_nStatServ, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		{
			perror("[server.cpp:setup] tc setsockopt:");
			exit(EXIT_FAILURE);
		}

		m_sAddrSesh.sin_addr.s_addr = INADDR_ANY;
		m_sAddrStat.sin_addr.s_addr = INADDR_ANY;
		m_sAddrSesh.sin_family = AF_INET;
		m_sAddrStat.sin_family = AF_INET;
		m_sAddrSesh.sin_port = htons(2005);
		m_sAddrStat.sin_port = htons(2006);


		if(bind(m_nSeshServ, (struct sockaddr *)&m_sAddrSesh, sizeof(m_sAddrSesh)) < 0 )
		{
			perror("[server.cpp:setup] tm bind failed:");
			exit(EXIT_FAILURE);
		}

		// Attempting to bind tc address
		if(bind(m_nStatServ, (struct sockaddr *)&m_sAddrStat, sizeof(m_sAddrStat)) < 0 )
		{
			perror("[server.cpp:setup] tc bind failed:");
			exit(EXIT_FAILURE);
		}

		if(listen(m_nSeshServ, 3) < 0)
		{
			perror("[server.cpp:setup] listen:");
			exit(EXIT_FAILURE);
		}
		#ifdef debug
			printf("[server.cpp:setup] Server listening for connections on %i\n", TM_PORT);
		#endif /* debug */


		if(listen(m_nStatServ, 3) < 0)
		{
			perror("[server.cpp:setup] listen:");
			exit(EXIT_FAILURE);
		}
		#ifdef debug
			printf("[server.cpp:setup] Server listening for connections on %i\n", TC_PORT);
		#endif /* debug */


		if((m_nSeshSock = accept(m_nSeshServ, (struct sockaddr *)&m_sAddrSesh, (socklen_t*)&addrlen)) < 0)
		{
			perror("[server.cpp:setup] tm accept:");
			exit(EXIT_FAILURE);
		}
		else if(m_nSeshSock != 0)
		{
			#ifdef debug
			printf("[server.cpp:setup] Connection accepted on tm socket!\n");
			#endif /* debug */
		}
		if((m_nStatSock = accept(m_nStatServ, (struct sockaddr *)&m_sAddrStat, (socklen_t*)&addrlen)) < 0)
		{
			perror("[server.cpp:setup] tc accept");
			exit(EXIT_FAILURE);
		}
		
		// Reading value from m_nStatSock to buffer, returns bytes read
		valread = read(m_nSeshSock, m_cBuffer, 1024);			// First session frame will arrive

		std::cout << "[server.cpp:setup] received bytes: " << valread << std::endl;
	    uint8_t *pnBuffer = (uint8_t *)malloc(sizeof(m_cBuffer));	// remember to free pnBuffer when done
	    memcpy(pnBuffer, &m_cBuffer, valread);

	    uint32_t m_nMsglen;
	    uint32_t m_nMsgType;
	    uint32_t m_nTabType;

	    if(tmtc::GetMsgType(pnBuffer, valread) == 10)
	    {
	    	ServFuncs::DecapsulateSession(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);	
	    }
	    
	    if(m_nTabType == 1)
	    {
	    	SetConfTable(pnBuffer, m_nMsglen, m_nTabType);	
	    }

	    //  decision to call setconftable should be here
	    
	    
	    valread = read(m_nStatSock, m_cBuffer, 1024);		// assuming a status request frame arrives
	    std::cout << "[server.cpp:setup] received bytes: " << valread << std::endl;
	    memcpy(pnBuffer, &m_cBuffer, valread);

	    if(tmtc::GetMsgType(pnBuffer, valread) == 11)
	    {
	    	ServFuncs::DecapsulateStatus(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
	    	std::cout << "[server.cpp:setup] MsgType: " << m_nMsgType << std::endl;
	    	std::cout << "[server.cpp:setup] TabType: " << m_nTabType << std::endl;
	    	
	    	switch(m_nTabType)
	    	{
	    		case 0:
	    		return -1;

	    		case 1:
	    		UL_TABLE *m_spTable;
	    		m_spTable = &sUlTable;
	    		send(m_nStatSock, m_spTable, sizeof(UL_TABLE), 0);			
	    		break;
	    	}
	    }
	    else
	    {
	    	std::cout << "getMsgType invalid return value" << std::endl;
	    }
		
	    free(pnBuffer);
		return 0;
	}

	// WIP WIP WIP WIP
	void FindSyncMarker(const uint8_t *in, uint32_t nDataLen)
	{
		uint8_t m_SyncState;
		uint8_t nNewBit;
		uint8_t ones = 0;
		uint32_t nPreamble = 0xA1B2C3D4;
		uint32_t dwWord = 0;
		uint8_t dataByte;

		// nested for loop might be the best solution for TCP interface
		// wip wip wip
		for(int i = 0; i < nDataLen; i++)
		{
			// new byte comes in here

			for(int j = 0; j < 8; j++)
			{
				// bit shifting happens here
				nNewBit = (in[i] & 0x01);
			}
		}

		/*
		for(int i = 0; i < nDataLen * sizeof(uint8_t); i++)
		{
			bit = (in[i] & 0x01);
			dwWord = (dwWord << 1) | bit;
			dataByte = (dataByte << 1) | bit;
			// ones = _mm_popcnt_u32(nPreamble^dwWord);


			// byte implementation doesnt work unless one everything is byte-wise, what happens if packet is preceded by three bits of trash?
			byte = (in[i] & 0xFF);
			dwWord = (dwWord << 8) | byte;
			dataByte = (dataByte << 8) | byte;

			switch(m_SyncState)
			{
				case 0:		// SYNC STATE SEARCH
					if(_mm_popcnt_u32(nPreamble^dwWord) == 0)
					{
						std::cout << "Start sequence found SEARCH -> DECODE" << std::endl;	
					}
					break;
				case 1:		// SYNC STATE DECODE
					std::cout << "State DECODE" << std::endl;
					break;
			}

		}
		*/
		

		return;
	}
} /* server */


