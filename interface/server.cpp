// This is the TCP/IP server

#include "server.h"

/*	uncomment for debug output
#ifndef debug
#define debug
#endif */ /* debug */


namespace server{
	
	// WIP WIP WIP WIP
	void FindSyncMarker(const uint8_t *in, uint32_t nDataLen)
	{
		uint8_t m_SyncState;
		uint8_t m_nNewByte;
		uint8_t ones = 0;
		uint32_t nPreamble = 0xA1B2C3D4;
		uint32_t dwWord = 0;
		uint8_t dataByte;


		// processor architecture is big endian?
		// 0xa1b2c3d4 is stored as d4c3b2a1

		std::cout << "Preamble is: " << std::hex << nPreamble << std::endl;


		utils::PrintBinary(nPreamble, 0);
		uint8_t *testptr = (uint8_t *)malloc(sizeof(uint32_t));
		memcpy(testptr, &nPreamble, sizeof(uint32_t));
		uint32_t testdw;
		uint32_t newbyte = 0;
		for(int i = 0; i < 4; i++)
		{
				// new byte comes in here
				// bit shifting happens here
				m_nNewByte = testptr[i];
				dwWord = (dwWord << 8) | m_nNewByte;		// works but get backwards
				testdw = __builtin_bswap32(dwWord);
				// dataByte = (dataByte << 8) | m_nNewByte; payload relevant
				
				// do-while could be used here?
				if((_mm_popcnt_u32(nPreamble^dwWord)) == 0)
				{
					std::cout << "popcount is zero (match) in the loop" << std::endl;
				}
				else if((_mm_popcnt_u32(nPreamble^testdw)) == 0)
				{
					std::cout << "popcount is zero (match) after bswap in the loop" << std::endl;	
				}

				// STATE SEARCH
		}
	
		
		/*
		if(!_mm_popcnt_u32(dwWord^nPreamble))
		{
			std::cout << "popcount = 0 match ffs!" << test3 << std::endl;	
		}

		uint32_t pOut = 0;
		uint32_t input = 255;
		input = 0x80010101;
		// utils::PrintBinary(input, 0);
	*/
		

		// utils::PrintBinary(input, 1);
		
		/*
		for(int i = 0; i < nDataLen; i++)
		{
				// new byte comes in here
				// bit shifting happens here
				m_nNewByte = (in[i] & 0xFF); // 0xFF to extract one byte at a time
				// shifting in 8 zeros and then putting the new byte there
				dwWord = (dwWord << 8) | m_nNewByte;
				dataByte = (dataByte << 8) | m_nNewByte;
				ones = _mm_popcnt_u32(nPreamble^dwWord);		// do-while could be used here?
				// ones = _mm_popcnt_epi32(nPreamble^dwWord);
				// all 0s is match, any ones is mismatch
				if(ones == 0)
				{
					std::cout << "popcount is zero" << std::endl;
				}
				std::cout << "i is: " << i << std::endl;
				std::cout << "m_nNewByte is: " << std::hex << m_nNewByte << std::endl;
				// STATE SEARCH
		} */

		/*
		for(int i = 0; i < nDataLen * sizeof(uint8_t); i++)
		{
			bit = (in[i] & 0x01);
			dwWord = (dwWord << 1) | bit;
			dataByte = (dataByte << 1) | bit;
			// ones = _mm_popcnt_u32(nPreamble^dwWord);


			// byte implementation doesnt work unless one everything is byte-wise, what happens if packet is preceded by three bits of trash? It isnt, take one byte at the time
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
	/* SetupServer() - Creates a TCP server that listens to port 8888. The server can handle up to 10 connections at any time
	*	inputs: none
	*	outputs: none
	*	function calls: ServerFuncs::MsgHandler() when data arrives on the TCP socket
	*/
	void SetupServer(void)
	{

		int m_nOpt = TRUE;  
		int m_nMasterSock;
		int m_nAddrlen;
		int m_nNewSock;
		int m_nClientSock[10];
		int m_nMaxClients = 10;
		int m_nActivity;
		int m_nIndex;
		int m_nSockDesc;
		int m_nMaxSock;
		uint32_t m_nValread;
		struct sockaddr_in m_sAddress;
		uint8_t buffer[1025];  

	    	            
	    // init a set of socket descriptors 
	    fd_set readfds;  
	     
	    // init all client sockets to 0
	    for (int i = 0; i < m_nMaxClients; i++)  
	    {  
	        m_nClientSock[i] = 0;  
	    }  
	         
	    // init a master socket 
	    if( (m_nMasterSock = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
	    {  
	        perror("socket failed");  
	        exit(EXIT_FAILURE);  
	    }  
	     
	    // set master socket to allow multiple connections , 
	    // good practice but it will work without this 
	    if( setsockopt(m_nMasterSock, SOL_SOCKET, SO_REUSEADDR, (char *)&m_nOpt, sizeof(m_nOpt)) < 0 )  
	    {  
	        perror("setsockopt");  
	        exit(EXIT_FAILURE);  
	    }  
	     
	    // set connection type, ip address and port of socket
	    m_sAddress.sin_family = AF_INET;  
	    m_sAddress.sin_addr.s_addr = INADDR_ANY;  
	    m_sAddress.sin_port = htons(8888);  
	         
	    //bind the socket to localhost port 8888 
	    if (bind(m_nMasterSock, (struct sockaddr *)&m_sAddress, sizeof(m_sAddress))<0)  
	    {  
	        perror("bind failed");  
	        exit(EXIT_FAILURE);  
	    }  
	    printf("Listener on port %d \n", PORT);  
	         
	    //try to specify maximum of 3 pending connections for the master socket 
	    if (listen(m_nMasterSock, 3) < 0)  
	    {  
	        perror("listen");  
	        exit(EXIT_FAILURE);  
	    }  
	         
	    //accept the incoming connection
	    m_nAddrlen = sizeof(m_sAddress);  
	    puts("Waiting for connections ...");  
	         
	    while(TRUE)  
	    {  
	        //clear the socket set 
	        FD_ZERO(&readfds);  
	     
	        //add master socket to set 
	        FD_SET(m_nMasterSock, &readfds);  
	        m_nMaxSock = m_nMasterSock;  
	             
	        //add child sockets to set 
	        for ( int i = 0 ; i < m_nMaxClients ; i++)  
	        {  
	            //socket descriptor 
	            m_nSockDesc = m_nClientSock[i];  
	                 
	            //if valid socket descriptor then add to read list 
	            if(m_nSockDesc > 0)  
	                FD_SET( m_nSockDesc , &readfds);  
	                 
	            //highest file descriptor number, need it for the select function 
	            if(m_nSockDesc > m_nMaxSock)  
	                m_nMaxSock = m_nSockDesc;  
	        }  
	     
	        //wait for an activity on one of the sockets , timeout is NULL , 
	        //so wait indefinitely 
	        m_nActivity = select( m_nMaxSock + 1 , &readfds , NULL , NULL , NULL);  
	       
	        if ((m_nActivity < 0) && (errno!=EINTR))  
	        {  
	            printf("select error");  
	        }  
	             
	        //If something happened on the master socket , 
	        //then its an incoming connection 
	        if (FD_ISSET(m_nMasterSock, &readfds))  
	        {  
	            if ((m_nNewSock = accept(m_nMasterSock, 
	                    (struct sockaddr *)&m_sAddress, (socklen_t*)&m_nAddrlen))<0)  
	            {  
	                perror("accept");  
	                exit(EXIT_FAILURE);  
	            }  
	             
	            //inform user of socket number - used in send and receive commands 
	            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , m_nNewSock ,inet_ntoa(m_sAddress.sin_addr) ,ntohs(m_sAddress.sin_port));  
	           	                           
	            //add new socket to array of sockets 
	            for (int i = 0; i < m_nMaxClients; i++)  
	            {  
	                //if position is empty 
	                if( m_nClientSock[i] == 0 )  
	                {  
	                    m_nClientSock[i] = m_nNewSock;  
	                    std::cout << "Adding to list of sockets as" << i << std::endl;
	                    break;  
	                }  
	            }  
	        }  
	             
	        // otherwise its some IO operation on some other socket
	        for (int i = 0; i < m_nMaxClients; i++)  
	        {  
	            m_nSockDesc = m_nClientSock[i];  
	                 
	            if (FD_ISSET( m_nSockDesc , &readfds))  
	            {  
	                // check if it was for closing , and also read the incoming message 
	                if ((m_nValread = read( m_nSockDesc , buffer, 1024)) == 0)  
	                {  
	                    // Client disconnected, print details
	                    getpeername(m_nSockDesc , (struct sockaddr*)&m_sAddress , (socklen_t*)&m_nAddrlen);  
	                    std::cout << "Host disconnected, ip: " << inet_ntoa(m_sAddress.sin_addr) << "port: " << ntohs(m_sAddress.sin_port) <<  std::endl;
	                    // Close the socket and mark as 0 in list for reuse 
	                    close( m_nSockDesc );
	                    m_nClientSock[i] = 0;  
	                }
	                     
	                // Last option is data arrived, we need to take care of it
	                else 
	                {  
	                	// This is where received messages are dispatched to a message handler or sync function	                    	                    
	                    ServerFuncs::MsgHandler(m_nSockDesc, buffer, m_nValread);
	                }  
	            }  
	        }  
	    }  
	         
	    return;
	}  

}	/* server */















