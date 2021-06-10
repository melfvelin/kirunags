// This is the TCP/IP server

#include "server.h"

/*	uncomment for debug output
#ifndef debug
#define debug
#endif */ /* debug */


namespace server{
	
	void FindAsmOne(const uint8_t *in, uint32_t& nMsgType)
	{
		uint8_t m_nNewByte;
		uint8_t m_nOnes = 0;
		uint32_t m_nPreamble = 0xA1B2C3D4;
		uint32_t m_nWord = 0;
		uint8_t m_nDataByte;
		uint32_t m_nInvWord;	
		uint32_t m_nMsglen;
		uint32_t m_nMsgType;
		uint8_t m_testVec[1000];	// use this to verify data contents
		int m_nDecodedBytes = 0;
		int m_nIndex = 0;
		std::vector<uint8_t> datavec;
		SYNC_STATE m_SyncState = SYNC_SEARCH;
		uint32_t nDataLen = 1000;	// Max data lengt, will be changed when nFramelen is decoded
		

		for(int i = 0; i < nDataLen; i++)
		{
				
				switch(m_SyncState)		// use enum instead of numbers here
				{
					case SYNC_SEARCH:		// STATE SEARCH
						// new byte comes in here
						m_nNewByte = in[i];
						m_nWord = (m_nWord << 8) | m_nNewByte;		// works but get backwards
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;

						// swap byte order to check for inverted bit sequence
						m_nInvWord = __builtin_bswap32(m_nWord);
						
						// do-while could be used here?
						if((_mm_popcnt_u32(m_nPreamble^m_nWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
						m_SyncState = SYNC_LENGTH;
						}
						else if((_mm_popcnt_u32(m_nPreamble^m_nInvWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
							m_SyncState = SYNC_LENGTH;
						}
						// std::cout << "Switch state 0, i = " << i << std::endl;
						break;	

					case SYNC_LENGTH:		// STATE DECODE LENGTH FIELD
						// store data somehow
						m_nNewByte = in[i];
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;
						datavec.push_back(m_nDataByte);
						++m_nDecodedBytes;

						// message length decoder
						if(m_nDecodedBytes <= 4)
						{
							m_nMsglen = (m_nMsglen << 8) | m_nDataByte;

							if(m_nDecodedBytes == 4)
							{
								m_nMsglen = __builtin_bswap32(m_nMsglen);
								// nDataLen is changed (affects for-loop)
								nDataLen = i + m_nMsglen - 7;	// (-7 because we loop 8 times while reading preamble and msglen)
								m_SyncState = SYNC_DECODE;
							}	
						}
						// std::cout << "Switch state 1, i = " << i << std::endl;
						break;

					case SYNC_DECODE:		// STATE DECODE REMAINING FRAME
						m_nNewByte = in[i];
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;
						datavec.push_back(m_nDataByte);
						m_testVec[m_nIndex] = in[i];
						// std::cout << "Switch state 2, i = " << i << std::endl;
						++m_nDecodedBytes;
						++m_nIndex;
						if(m_nDecodedBytes == (m_nMsglen - 4))		// replace with a do-while?
						{
							m_SyncState = SYNC_SEARCH;
							std::cout << "Sync state set to 0 " << std::endl;
						}
						break;
				}
		}
		std::cout << "Decoded bytes:" << m_nDecodedBytes << std::endl;

		// TROUBLESHOOTING BELOW
		
		GEN_HEADER m_sGenHeader;
		memcpy(&m_sGenHeader.nMsgType, m_testVec, (2 * sizeof(uint32_t)));
		std::cout << "Msg type: " << m_sGenHeader.nMsgType << std::endl;
		std::cout << "Tab type: " << m_sGenHeader.nTabType << std::endl;
		// writing message type to parent function
		nMsgType = m_sGenHeader.nMsgType;	
		return;
	}

	void FindAsmTwo(const uint8_t *in, const uint32_t nFrameLen, uint32_t& nMsgType)
	{
		uint8_t m_nNewByte;
		uint8_t m_nOnes = 0;
		uint32_t m_nPreamble = 0xA1B2C3D4;
		uint32_t m_nWord = 0;
		uint8_t m_nDataByte;
		uint32_t m_nInvWord;	
		uint32_t m_nMsglen;
		uint32_t m_nMsgType;
		uint8_t m_testVec[1000];	// use this to verify data contents
		int m_nDecodedBytes = 0;
		int m_nIndex = 0;
		std::vector<uint8_t> datavec;
		SYNC_STATE m_SyncState = SYNC_SEARCH;
		uint32_t nDataLen = 1024;	// Max data lengt, will be changed when preamble is found
		
		std::cout << "############### ASM 2 SYNC FUNCTION ###############" << std::endl;

		for(int i = 0; i < nDataLen; i++)
		{
				
				switch(m_SyncState)		// use enum instead of numbers here
				{
					case SYNC_SEARCH:		// STATE SEARCH
						// new byte comes in here
						m_nNewByte = in[i];
						m_nWord = (m_nWord << 8) | m_nNewByte;		// works but get backwards
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;

						// swap byte order to check for inverted bit sequence
						m_nInvWord = __builtin_bswap32(m_nWord);
						
						// do-while could be used here?
						if((_mm_popcnt_u32(m_nPreamble^m_nWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
							m_SyncState = SYNC_DECODE;
							nDataLen = i + nFrameLen - 3;
						}
						else if((_mm_popcnt_u32(m_nPreamble^m_nInvWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
							m_SyncState = SYNC_DECODE;
							nDataLen = i + nFrameLen - 3;
						}
						// std::cout << "Switch state 0, i = " << i << std::endl;
						break;	

					case SYNC_DECODE:		// STATE DECODE REMAINING FRAME
						m_nNewByte = in[i];
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;
						datavec.push_back(m_nDataByte);
						m_testVec[m_nIndex] = in[i];
						// std::cout << "Switch state 2, i = " << i << std::endl;
						++m_nDecodedBytes;
						++m_nIndex;
						if(m_nDecodedBytes == (m_nMsglen - 4))		// replace with a do-while?
						{
							m_SyncState = SYNC_SEARCH;
							std::cout << "Sync state set to 0 " << std::endl;
						}
						break;
				}
		}
		std::cout << "Decoded bytes:" << m_nDecodedBytes << std::endl;

		// DATA VALIDATION BELOW
		
		GEN_HEADER m_sGenHeader;
		memcpy(&m_sGenHeader.nMsgType, (m_testVec + 4), (2 * sizeof(uint32_t)));
		std::cout << "Msg type: " << m_sGenHeader.nMsgType << std::endl;
		std::cout << "Tab type: " << m_sGenHeader.nTabType << std::endl;
		// writing message type to parent function
		nMsgType = m_sGenHeader.nMsgType;	

		return;
	}

	void FindAsmThree(const uint8_t *in, uint32_t& nMsgType)
	{
		uint8_t m_nNewByte;
		uint8_t m_nOnes = 0;
		uint32_t m_nPreamble = 0xA1B2C3D4;
		uint32_t m_nPostamble = POSTAMBLE;
		uint32_t m_nWord = 0;
		uint8_t m_nDataByte;
		uint32_t m_nInvWord;	
		uint32_t m_nMsglen;
		uint32_t m_nMsgType;
		uint8_t m_testVec[1000];	// use this to verify data contents
		int m_nDecodedBytes = 0;
		int m_nIndex = 0;
		std::vector<uint8_t> datavec;
		SYNC_STATE m_SyncState = SYNC_SEARCH;
		uint32_t nDataLen = 128;	// Max data lengt, will be changed when preamble is found
		
		std::cout << "############### ASM 3 SYNC FUNCTION ###############" << std::endl;

		for(int i = 0; i < nDataLen; i++)
		{
				
				switch(m_SyncState)		// use enum instead of numbers here
				{
					case SYNC_SEARCH:		// STATE SEARCH
						// new byte comes in here
						m_nNewByte = in[i];
						m_nWord = (m_nWord << 8) | m_nNewByte;		// works but get backwards
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;

						// swap byte order to check for inverted bit sequence
						m_nInvWord = __builtin_bswap32(m_nWord);
						
						// do-while could be used here?
						if((_mm_popcnt_u32(m_nPreamble^m_nWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
							m_SyncState = SYNC_DECODE;
						}
						else if((_mm_popcnt_u32(m_nPreamble^m_nInvWord)) == 0)
						{
							std::cout << "SYNC -> DECODE" << std::endl;
							m_SyncState = SYNC_DECODE;
						}
						// std::cout << "Switch state 0, i = " << i << std::endl;
						break;	

					case SYNC_DECODE:		// STATE DECODE REMAINING FRAME
						m_nNewByte = in[i];
						m_nDataByte = (m_nDataByte << 8) | m_nNewByte;
						m_nWord = (m_nWord << 8) | m_nNewByte;
						datavec.push_back(m_nDataByte);
						m_testVec[m_nIndex] = in[i];
						// std::cout << "Switch state 2, i = " << i << std::endl;
						
						m_nInvWord = __builtin_bswap32(m_nWord);
						
						// do-while could be used here?
						if((_mm_popcnt_u32(m_nPostamble^m_nWord)) == 0)
						{
							std::cout << "SYNC -> SEARCH" << std::endl;
							m_SyncState = SYNC_SEARCH;
						}
						else if((_mm_popcnt_u32(m_nPostamble^m_nInvWord)) == 0)
						{
							std::cout << "SYNC -> SEARCH" << std::endl;
							m_SyncState = SYNC_SEARCH;
						}
						++m_nDecodedBytes;
						++m_nIndex;
						
						break;
				}
		}
		std::cout << "Decoded bytes:" << m_nDecodedBytes << std::endl;

		// DATA VALIDATION BELOW
		
		GEN_HEADER m_sGenHeader;
		memcpy(&m_sGenHeader.nMsgType, (m_testVec + 4), (2 * sizeof(uint32_t)));
		std::cout << "Msg type: " << m_sGenHeader.nMsgType << std::endl;
		std::cout << "Tab type: " << m_sGenHeader.nTabType << std::endl;
		// writing message type to parent function
		nMsgType = m_sGenHeader.nMsgType;	

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
		uint32_t m_nMsgType;
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
	    std::cout << "Listening on port " << PORT << std::endl;
	         
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
	     
	        // wait for an activity on one of the sockets , timeout is NULL , 
	        // so wait indefinitely 
	        m_nActivity = select( m_nMaxSock + 1 , &readfds , NULL , NULL , NULL);  
	       
	        if ((m_nActivity < 0) && (errno!=EINTR))  
	        {  
	            std::cout << "select error" << std::endl;
	        }  
	             
	        // If something happened on the master socket its an incoming connection 
	        if (FD_ISSET(m_nMasterSock, &readfds))  
	        {  
	            if ((m_nNewSock = accept(m_nMasterSock, 
	                    (struct sockaddr *)&m_sAddress, (socklen_t*)&m_nAddrlen))<0)  
	            {  
	                perror("accept");  
	                exit(EXIT_FAILURE);  
	            }  
	            
	            //inform user of socket number - used in send and receive commands 
	           	std::cout << "New connection, socket fd: " << m_nNewSock << " ip: " << inet_ntoa(m_sAddress.sin_addr) << " port: " << ntohs(m_sAddress.sin_port)
	           	<< std::endl;
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
	                    std::cout << "Host disconnected, ip: " << inet_ntoa(m_sAddress.sin_addr) << " port: " << ntohs(m_sAddress.sin_port) <<  std::endl;
	                    // Close the socket and mark as 0 in list for reuse 
	                    close( m_nSockDesc );
	                    m_nClientSock[i] = 0;  
	                }
	                     
	                // Last option is data arrived, we need to take care of it
	                else 
	                {  
	                	// This is where received messages are dispatched to a message handler or sync function	                    	                    
	                    // server::FindAsmOne(buffer, m_nMsgType);
	                    ServerFuncs::MsgHandler(m_nSockDesc, buffer, m_nValread);
	                }  
	            }  
	        }  
	    }  
	         
	    return;
	}  

}	/* server */















