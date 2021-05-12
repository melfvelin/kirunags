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

#ifndef debug
#define debug
#endif /* debug */


namespace server{
int setup(void)
{
	// Init of variables
	int server_tm;
	int server_tc;
	int new_socket;
	int tc_socket = 0;
	int tm_socket = 0;
	int valread;
	struct sockaddr_in address_tm;
	struct sockaddr_in address_tc;
	struct in_addr addrptr;
	int opt = 1;
	int addrlen = sizeof(address_tm);
	char *tc_ip;
	char *tm_ip;

	// Init char array of zeros
	char buffer[1024] = {0};

	// Creating socket file descriptor STEP 1
	if((server_tm = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("[server.cpp:setup] Socket creation failed:");
		exit(EXIT_FAILURE);
	}

	// creating a socket file descriptor for tc socket
	if((server_tc = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("[server.cpp:setup] TC socket creation failed:");
		exit(EXIT_FAILURE);
	}

	// returns 0 on success, the function sets a socket option STEP 2 GOOD PRACTICE
	if(setsockopt(server_tm, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("[server.cpp:setup] setsockopt:");
		exit(EXIT_FAILURE);
	}

	// sets a socket option for tc socket
	if(setsockopt(server_tc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("[server.cpp:setup] tc setsockopt:");
		exit(EXIT_FAILURE);
	}

	address_tm.sin_family = AF_INET;
	address_tm.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY should bind the socket to the localhost address
	address_tc.sin_family = AF_INET;
	//address_tc.sin_addr.s_addr = addrptr;
	if(inet_aton("127.0.0.1", &address_tc.sin_addr) == 0)
	{
		perror("[server.cpp:setup] aton error");
		exit(EXIT_FAILURE);
	}
	// htons returns the value in TCP/IP network order
	address_tm.sin_port = htons(TM_PORT);
	address_tc.sin_port = htons(TC_PORT);

	#ifdef debug

	#endif /* debug */

	// The above things go into the below bind function, I think

	// Attempting to bind
	// The struct sockaddr contains the port number and another struct called in_addr (Internet address)
	// the (struct sockaddr *)&pointer is a typecast from struct sockaddr_in to struct sockaddr

	// STEP 3
	if(bind(server_tm, (struct sockaddr *)&address_tm, sizeof(address_tm)) < 0 )
	{
		perror("[server.cpp:setup] tm bind failed:");
		exit(EXIT_FAILURE);
	}

	#ifdef debug
	tc_ip = inet_ntoa(address_tm.sin_addr);
	printf("[server.cpp:setup] bind successfull on %s:%u\n", tm_ip, ntohs(address_tm.sin_port));
	#endif /* debug */

	// Attempting to bind tc address
	if(bind(server_tc, (struct sockaddr *)&address_tc, sizeof(address_tc)) < 0 )
	{
		perror("[server.cpp:setup] tc bind failed:");
		exit(EXIT_FAILURE);
	}

	#ifdef debug
	tm_ip = inet_ntoa(address_tc.sin_addr);
	printf("[server.cpp:setup] bind successfull on %s:%u\n", tc_ip, ntohs(address_tc.sin_port));
	#endif /* debug */


	if(listen(server_tm, 3) < 0)
	{
		perror("[server.cpp:setup] listen:");
		exit(EXIT_FAILURE);
	}
	#ifdef debug
		printf("[server.cpp:setup] Server listening for connections on %i\n", TM_PORT);
	#endif /* debug */


	if(listen(server_tc, 3) < 0)
	{
		perror("[server.cpp:setup] listen:");
		exit(EXIT_FAILURE);
	}
	#ifdef debug
		printf("[server.cpp:setup] Server listening for connections on %i\n", TC_PORT);
	#endif /* debug */


	if((tm_socket = accept(server_tm, (struct sockaddr *)&address_tm, (socklen_t*)&addrlen)) < 0)
	{
		perror("[server.cpp:setup] tm accept:");
		exit(EXIT_FAILURE);
	}
	else if(tm_socket != 0)
	{
		printf("[server.cpp:setup] Connection accepted on tm socket!\n");
	}
	if((tc_socket = accept(server_tc, (struct sockaddr *)&address_tc, (socklen_t*)&addrlen)) < 0)
	{
		perror("[server.cpp:setup] tc accept");
		exit(EXIT_FAILURE);
	}
	else if(tc_socket != 0)
	{
		printf("[server.cpp:setup] Connection accepted on tc socket!\n");
	}

	// Reading value from new_socket to buffer
	valread = read(tc_socket, buffer, 1024);
    uint8_t *buff_ptr = (uint8_t *)malloc(sizeof(buffer));
    memcpy(buff_ptr, &buffer, sizeof(buffer));


    tmtc::decapsulate(buff_ptr, 0);

    char tm_buff[] = "12345678910";
    uint8_t *tm_ptr = (uint8_t *)malloc(strlen(tm_buff) + 0 * sizeof(uint8_t));
    // uint8_t *
    tm_ptr = tmtc::encapsulate(tm_buff, (strlen(tm_buff)+0));
    tmtc::parse_tm_header(tm_ptr);

	send(tm_socket, tm_ptr, (sizeof(TM_HEADER) + strlen(tm_buff) + 0 + sizeof(POSTAMBLE)), 0);
	// send(new_socket, s_hello, strlen(hello), 0);

	// printf("Attempted to send: %s \n", telem);

	return 0;
}
} /* server */


