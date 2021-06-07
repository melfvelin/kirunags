// This is the TCP/IP server
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/time.h>

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
			printf("[server.cpp:setup] Server listening for connections \n");

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
			printf("[server.cpp:setup] Server accepted connections \n");
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
				nNewBit = (in[i] & 0x01); // 0xFF to extract one byte at a time
				// dwWord << 8 instead
				// STATE DECODE
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

	int SetupMulti()
	{

		int m_nOpt = 1;
		int m_nMasterSock;
		int m_nAddrlen = sizeof(sockaddr_in);
		int m_nNewSock;
		int m_naClientSock[10];
		int m_nMaxClients = 10;
		int m_nActivity;
		int m_nIndex;
		int m_nValread;
		int m_nSockDesc[4];
		int m_nServDesc[4];
		int m_nMaxSockDesc;
		int m_nResult;
		struct sockaddr_in m_sAddr[4];
		uint8_t m_nBuff[1024];
		uint32_t m_nMsgType;

		struct pollfd m_sPollfds[4];

		memset(m_sPollfds, 0, sizeof(m_sPollfds));
		
		// assign port numbers to port array
		for(int i = 0; i < 4; i++)
		{
			m_sAddr[i].sin_family = AF_INET;  
    		m_sAddr[i].sin_addr.s_addr = INADDR_ANY;  
    		m_sAddr[i].sin_port = htons((2001 + i)); 
		}

		

		fd_set readfds;

		// initialize all clients to 0
		for(int i = 0; i < m_nMaxClients; i++)
		{
			m_naClientSock[i] = 0;
		}

		
		for(int i = 0; i < 4; i++)
		{
			if( (m_nServDesc[i] = socket(AF_INET , SOCK_STREAM , 0)) == 0 )
			{
				perror("socket failed");  
        		exit(EXIT_FAILURE);
			}
		}
		//if( setsockopt(m_nMasterSock, SOL_SOCKET, SO_REUSEADDR, (char *)&m_nOpt, sizeof(m_nOpt)) < 0 )  
    	//{  
       // 	perror("setsockopt");  
       // 	exit(EXIT_FAILURE);  
    	//}  

    	// bind all sockets in a loop
    	for(int i = 0; i <4; i++)
    	{
    		if (bind(m_nServDesc[i], (struct sockaddr *)&m_sAddr[i], sizeof(m_sAddr[0]))<0)  
    		{  
        		std::cout << "Index = " << i << std::endl;
        		perror("bind failed");  
        		exit(EXIT_FAILURE);  
    		}	
    	}

    	// listen on all sockets in a loop
    	for(int i = 0; i <4; i++)
    	{
    		if(listen(m_nServDesc[i], 4) < 0)
    		{
    			std::cout << "Index = " << i << std::endl;
    			perror("listen failed");  
        		exit(EXIT_FAILURE); 
    		}
    	}

		for(int i = 0; i <4; i++)
    	{
    		if( m_nSockDesc[i] = (accept(m_nServDesc[i], (struct sockaddr *)&m_sAddr[i], (socklen_t *)&m_nAddrlen)) < 0 )
    		{
    			std::cout << "Index = " << i << std::endl;
    			perror("accept failed");  
        		exit(EXIT_FAILURE); 
    		}
       	}
    	std::cout << "All connections accepted " << std::endl;
    	// read() here
    	std::cout << "Successfully received:" << m_nValread << std::endl;


    	
    	// specify what to listen for in poll

    	// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    	/*
    	m_nResult = poll(m_sPollfds, 4, 10000);
    	std::cout << "poll finished" << std::endl;	
    	if(m_nResult == 0)
    	{
    		std::cout << "poll timeout" << std::endl;	
    	}
    	else if(m_nResult < 0)
    	{
    		std::cout << "poll error" << std::endl;		
    	}
    	else
    	{
    		std::cout << "Successful call to poll" << std::endl;	
    		m_nIndex = 0;
    		while ((m_nValread = read(m_nSockDesc[m_nIndex], m_nBuff, 1024)) == 0)
    		{
    			std::cout << "Empty m_nValread" << std::endl;		
    			m_nIndex++;
    		}
    		m_nMsgType = tmtc::GetMsgType(m_nBuff, m_nValread);
		} */
    		
    		/*
    		m_nIndex = 0;
    		int m_nNewRead = 0;

    		for(int i = 0; i < 4; i++)
    		{
    			if((m_nValread = read(m_nSockDesc[i], m_nBuff, 1024)) == -1)
    			{
    				perror("read");
					exit(EXIT_FAILURE);
    			}
    			std::cout << "Valread: " << m_nValread << std::endl;
    		} */

    		switch(m_nMsgType)
    		{
    			case 0:
    				std::cout << "Decoded MsgType: " << m_nMsgType << std::endl;		
    				break;
    			case 1:
    				std::cout << "Decoded MsgType: " << m_nMsgType << std::endl;
    				break;
    			case 2:
    				std::cout << "Decoded MsgType: " << m_nMsgType << std::endl;
    				break;
    			case 10:
    				std::cout << "Decoded MsgType: " << m_nMsgType << std::endl;
    				break;
    			case 11:
    				std::cout << "Decoded MsgType: " << m_nMsgType << std::endl;
    				break;	
    			default:
    			break;
    		}
    	

    	

    	// valread = read(m_nSeshSock, m_cBuffer, 1024);			// First session frame will arrive
    	return 0;
	}

	void SetupTest(void)
	{

		int opt = TRUE;  
	    int master_socket , addrlen , new_socket , client_socket[30] , 
	    max_clients = 30 , activity, i , sd;  
	    int max_sd;  
	    uint32_t valread;
	    struct sockaddr_in address;  
	         
	    // char buffer[1025];  //data buffer of 1K 
	    uint8_t buffer[1025];  
	    //set of socket descriptors 
	    fd_set readfds;  
	         
	    //a message 
	    char *message = "ECHO Daemon v1.0 \r\n";  
	     
	    //initialise all client_socket[] to 0 so not checked 
	    for (i = 0; i < max_clients; i++)  
	    {  
	        client_socket[i] = 0;  
	    }  
	         
	    //create a master socket 
	    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
	    {  
	        perror("socket failed");  
	        exit(EXIT_FAILURE);  
	    }  
	     
	    //set master socket to allow multiple connections , 
	    //this is just a good habit, it will work without this 
	    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
	    {  
	        perror("setsockopt");  
	        exit(EXIT_FAILURE);  
	    }  
	     
	    //type of socket created 
	    address.sin_family = AF_INET;  
	    address.sin_addr.s_addr = INADDR_ANY;  
	    address.sin_port = htons(8888);  
	         
	    //bind the socket to localhost port 8888 
	    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
	    {  
	        perror("bind failed");  
	        exit(EXIT_FAILURE);  
	    }  
	    printf("Listener on port %d \n", PORT);  
	         
	    //try to specify maximum of 3 pending connections for the master socket 
	    if (listen(master_socket, 3) < 0)  
	    {  
	        perror("listen");  
	        exit(EXIT_FAILURE);  
	    }  
	         
	    //accept the incoming connection 
	    addrlen = sizeof(address);  
	    puts("Waiting for connections ...");  
	         
	    while(TRUE)  
	    {  
	        //clear the socket set 
	        FD_ZERO(&readfds);  
	     
	        //add master socket to set 
	        FD_SET(master_socket, &readfds);  
	        max_sd = master_socket;  
	             
	        //add child sockets to set 
	        for ( i = 0 ; i < max_clients ; i++)  
	        {  
	            //socket descriptor 
	            sd = client_socket[i];  
	                 
	            //if valid socket descriptor then add to read list 
	            if(sd > 0)  
	                FD_SET( sd , &readfds);  
	                 
	            //highest file descriptor number, need it for the select function 
	            if(sd > max_sd)  
	                max_sd = sd;  
	        }  
	     
	        //wait for an activity on one of the sockets , timeout is NULL , 
	        //so wait indefinitely 
	        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
	       
	        if ((activity < 0) && (errno!=EINTR))  
	        {  
	            printf("select error");  
	        }  
	             
	        //If something happened on the master socket , 
	        //then its an incoming connection 
	        if (FD_ISSET(master_socket, &readfds))  
	        {  
	            if ((new_socket = accept(master_socket, 
	                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
	            {  
	                perror("accept");  
	                exit(EXIT_FAILURE);  
	            }  
	             
	            //inform user of socket number - used in send and receive commands 
	            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket ,inet_ntoa(address.sin_addr) ,ntohs(address.sin_port));  
	           	                           
	            //add new socket to array of sockets 
	            for (i = 0; i < max_clients; i++)  
	            {  
	                //if position is empty 
	                if( client_socket[i] == 0 )  
	                {  
	                    client_socket[i] = new_socket;  
	                    printf("Adding to list of sockets as %d\n" , i);  
	                         
	                    break;  
	                }  
	            }  
	        }  
	             
	        //else its some IO operation on some other socket
	        for (i = 0; i < max_clients; i++)  
	        {  
	            sd = client_socket[i];  
	                 
	            if (FD_ISSET( sd , &readfds))  
	            {  
	                //Check if it was for closing , and also read the 
	                //incoming message 
	                if ((valread = read( sd , buffer, 1024)) == 0)  
	                {  
	                    //Somebody disconnected , get his details and print 
	                    getpeername(sd , (struct sockaddr*)&address , \
	                        (socklen_t*)&addrlen);  
	                    printf("Host disconnected , ip %s , port %d \n" , 
	                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
	                         
	                    //Close the socket and mark as 0 in list for reuse 
	                    close( sd );  
	                    client_socket[i] = 0;  
	                }  
	                     
	                //Echo back the message that came in 
	                else 
	                {  
	                	// This is where  received messages are handled	                    	                    
	                    if((ServFuncs::MsgHandler(sd, buffer, valread) ) == 3)
						{
							send(sd, buffer, valread, 0);
						}
						std::cout << "Uplink table set modeschem: " << sUlTable.nModScheme << std::endl;	                    
							
	                }  
	            }  
	        }  
	    }  
	         
	    return;
	}  

}	/* server */















