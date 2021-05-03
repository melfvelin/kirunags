// This is the TCP/IP server
#include <unistd.h>
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

#ifndef debug
#define debug
#endif /* debug */


int main(int argc, char const *argv[])
{
	// Init of variables
	int server_tm;
	int server_tc;
	int new_socket;
	int tc_socket;
	int tm_socket;
	int valread;
	struct sockaddr_in address_tm;
	struct sockaddr_in address_tc;
	struct in_addr addrptr;
	int opt = 1;
	int addrlen = sizeof(address_tm);

	// Init char array of zeros
	char buffer[1024] = {0};
	char const *hello = "Hello from server";
	char const *telem = "This is a telemetry message";

	// Creating socket file descriptor
	if((server_tm = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	// creating a socket file descriptor for tc socket
	if((server_tc = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("TC socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	// returns 0 on success, the function sets a socket option
	if(setsockopt(server_tm, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// sets a socket option for tc socket
	if(setsockopt(server_tc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("tc setsockopt");
		exit(EXIT_FAILURE);
	}
	/*
	if(inet_aton("10.0.1.5", &addrptr)==0)
	{
		perror("aton error");
		exit(EXIT_FAILURE);
	}
	else
	{
		#ifdef debug
		printf("succesfully converted IP-address\n");
		#endif
	} */


	address_tm.sin_family = AF_INET;
	address_tm.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY should bind the socket to the localhost address
	address_tc.sin_family = AF_INET;
	//address_tc.sin_addr.s_addr = addrptr;
	if(inet_aton("10.0.1.5", &address_tc.sin_addr) == 0)
	{
		perror("aton error");
		exit(EXIT_FAILURE);
	}
	// htons returns the value in TCP/IP network order
	address_tm.sin_port = htons(TM_PORT);
	address_tc.sin_port = htons(TC_PORT);

	#ifdef debug
	printf("Input to htons: %i\n ", TM_PORT);
	printf("Output from htons: %i\n", htons(TM_PORT));
	#endif /* debug */

	// The above things go into the below bind function, I think

	// Attempting to bind
	// The struct sockaddr contains the port number and another struct called in_addr (Internet address)
	// the (struct sockaddr *)&pointer is a typecast from struct sockaddr_in to struct sockaddr
	if(bind(server_tm, (struct sockaddr *)&address_tm, sizeof(address_tm)) < 0 )
	{
		perror("tm bind failed");
		exit(EXIT_FAILURE);
	}
	// Attempting to bind tc address
	if(bind(server_tc, (struct sockaddr *)&address_tc, sizeof(address_tc)) < 0 )
	{
		perror("tc bind failed");
		exit(EXIT_FAILURE);
	}

	if(listen(server_tm, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	else
	{
		#ifdef debug
		printf("\nServer listening for connections on %i\n", TM_PORT);
		#endif /* debug */
	}

	if(listen(server_tc, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	else
	{
		#ifdef debug
		printf("\nServer listening for connections on %i\n", TC_PORT);
		#endif /* debug */
	}

	if((tm_socket = accept(server_tm, (struct sockaddr *)&address_tm, (socklen_t*)&addrlen)) < 0)
	{
		perror("tm accept");
		exit(EXIT_FAILURE);
	}

	if((tc_socket = accept(server_tc, (struct sockaddr *)&address_tc, (socklen_t*)&addrlen)) < 0)
	{
		perror("tc accept");
		exit(EXIT_FAILURE);
	}

	// Reading value from new_socket to buffer
	valread = read(tc_socket, buffer, 1024);
	// printing buffer
	printf("Received TC message: %s\n", buffer);	
	
	

	send(tm_socket, telem, strlen(telem), 0);
	// send(new_socket, s_hello, strlen(hello), 0);
	
	printf("Attempted to send: %s \n", telem);

	return 0;
}


