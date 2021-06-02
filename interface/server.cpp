// This is the TCP/IP server
/* #include <unistd.h>
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
*/
#include "server.h"
#include "tmtc.h"
#include "tables.h"
//#include "tmtc.h"

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
		int tc_socket = 0;
		int tm_socket = 0;
		int valread;
		struct sockaddr_in m_sAddrSesh;
		struct sockaddr_in m_sAddrStat;
		struct sockaddr_in address_tm;
		struct sockaddr_in address_tc;
		int opt = 1;
		int addrlen = sizeof(address_tm);
		char *tc_ip;
		char *tm_ip;

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

		// with inet_aton it's easy to change from localhost to other IP adresses
		/*
		if(inet_aton("127.0.0.1", &address_tm.sin_addr) == 0)
		{
			perror("[server.cpp:setup] aton error");
			exit(EXIT_FAILURE);
		}
		// with inet_aton it's easy to change from localhost to other IP adresses
		if(inet_aton("127.0.0.1", &address_tc.sin_addr) == 0)
		{
			perror("[server.cpp:setup] aton error");
			exit(EXIT_FAILURE);
		}
		*/
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

		#ifdef debug
		tc_ip = inet_ntoa(m_sAddrSesh.sin_addr);
		printf("[server.cpp:setup] bind successfull on %s:%u\n", tm_ip, ntohs(address_tm.sin_port));
		#endif /* debug */

		// Attempting to bind tc address
		if(bind(m_nStatServ, (struct sockaddr *)&m_sAddrStat, sizeof(m_sAddrStat)) < 0 )
		{
			perror("[server.cpp:setup] tc bind failed:");
			exit(EXIT_FAILURE);
		}

		#ifdef debug
		tm_ip = inet_ntoa(m_sAddrStat.sin_addr);
		printf("[server.cpp:setup] bind successfull on %s:%u\n", tc_ip, ntohs(address_tc.sin_port));
		#endif /* debug */


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


		if((m_nSeshSock = accept(m_nSeshServ, (struct sockaddr *)&address_tm, (socklen_t*)&addrlen)) < 0)
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
		if((m_nStatSock = accept(m_nStatServ, (struct sockaddr *)&address_tc, (socklen_t*)&addrlen)) < 0)
		{
			perror("[server.cpp:setup] tc accept");
			exit(EXIT_FAILURE);
		}
		else if(m_nStatSock != 0)
		{
			#ifdef debug
			printf("[server.cpp:setup] Connection accepted on tc socket!\n");
			#endif /* debug */
		}

		// Reading value from m_nStatSock to buffer, returns bytes read
		valread = read(m_nSeshSock, m_cBuffer, 1024);			// First session frame will arrive
		std::cout << "[server.cpp:setup] received bytes: " << valread << std::endl;
	    uint8_t *pnBuffer = (uint8_t *)malloc(sizeof(m_cBuffer));	// remember to free pnBuffer when done
	    memcpy(pnBuffer, &m_cBuffer, valread);

	    uint32_t m_nMsglen;
	    uint32_t m_nMsgType;
	    uint32_t m_nTabType;

	    tmtc::DecapsulateSession(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
	    //  decision to call setconftable should be here
	    server::SetConfTable(pnBuffer, m_nMsglen, m_nTabType);
	    // this buffer pointer should be sent to the Sync function for preamble detection
	    // the next step would be to determine what type of message has arrived but let's implement that later
	    // function call to sync()
	    // tmtc::DecapsulateSession(pnBuffer, )

	    // next frame to arrive will be the status request

	    valread = read(m_nStatSock, m_cBuffer, 1024);		// assuming a status request frame arrives
	    std::cout << "[server.cpp:setup] received bytes: " << valread << std::endl;
	    memcpy(pnBuffer, &m_cBuffer, valread);
		tmtc::DecapsulateStatus(pnBuffer, m_nMsglen, m_nMsgType, m_nTabType);
	    std::cout << "[server.cpp:setup] MsgType: " << m_nMsgType << std::endl;
	    std::cout << "[server.cpp:setup] TabType: " << m_nTabType << std::endl;

	    if(m_nMsgType == 10 && m_nTabType == 1)
	    {
	    	std::cout << "[server.cpp:setup] Msg 10, Tab 10 lookup " << std::endl;
	    	UL_TABLE *m_spTable;
	    	m_spTable = &sUlTable;
	    	send(m_nStatSock, m_spTable, sizeof(UL_TABLE), 0);		
	    }

	    free(pnBuffer);
		return 0;
	}

	// Called after DecapsulateSession()
	void SetConfTable(uint8_t *pnData, uint32_t nMsglen, uint32_t nTabType)
	{
		switch(nTabType)
		{
			case 0:
			return;

			case 1:
			// UL CONF TABLE RECEIVED
			memcpy(&sUlTable, pnData, nMsglen);
			// safe handling of global variables would be good
			return;
		}
		return;
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


