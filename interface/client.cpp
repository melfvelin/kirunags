// This is the TCP/IP client
// Client side C/C++ program to demonstrate Socket programming
/* #include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "tables.h"

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2008 */

#include "client.h"

#ifndef debug
#define debug
#endif /* debug */

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    int sock2 = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in tm_server;
    struct sockaddr_in tc_server;
    char const *hello = "Hello from client";
    char const *telec = "This is a telecommand";
    char buffer[1024] = {0};
    char *tc_ip;
    char *tm_ip;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket2 creation error \n");
        return -1;
    }
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(TM_PORT);

    tm_server.sin_family = AF_INET;
    tm_server.sin_port = htons(TM_PORT);

    tc_server.sin_family = AF_INET;
    tc_server.sin_port = htons(TC_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &tm_server.sin_addr)<=0)
    {
        printf("\nInvalid TM address/ Addrescis not supported \n");
        return -1;
    }

    if(inet_pton(AF_INET, "10.0.1.5", &tc_server.sin_addr)<=0)
    {
        printf("\nInvalid TC address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&tm_server, sizeof(tm_server)) < 0)
    {
        printf("\nTM Connection Failed \n");
        return -1;
    }
    #ifdef debug
        tm_ip = inet_ntoa(tm_server.sin_addr);
        printf("Client: attempting to connect on %s:%u\n", tm_ip, ntohs(tm_server.sin_port));
    #endif /* debug */

    if (connect(sock2, (struct sockaddr *)&tc_server, sizeof(tc_server)) < 0)
    {
        printf("\nTC Connection Failed \n");
        return -1;
    }
    #ifdef debug
        tc_ip = inet_ntoa(tc_server.sin_addr);
        printf("Client: attempting to connect on %s:%u\n", tc_ip, ntohs(tc_server.sin_port));
    #endif /* debug */

	if(1)
	{
		TC_HEADER tx_tc_frame;
		tx_tc_frame.preamble = 0xA1B2C3D4;
		const char message[] = "TC TEST MESSAGE";
		uint32_t total_len = (sizeof(TC_HEADER) + sizeof(message));
		tx_tc_frame.total_len = total_len;
		tx_tc_frame.tf_size = sizeof(message);
		// adding message after tc header
		//memcpy((tx_tc_frame + sizeof(TC_HEADER)), &message, tx_tc_frame.tf_size);

		uint8_t *tc_ptr = (uint8_t *)malloc(tx_tc_frame.total_len);
		memcpy(tc_ptr, &tx_tc_frame, sizeof(TC_HEADER));
		memcpy((tc_ptr + sizeof(TC_HEADER)), &message, tx_tc_frame.tf_size);
		if(tmtc::decapsulate(tc_ptr, total_len) == 0)
		{
			std::cout << "Successfully decapsulated tc frame!" << std::endl;
		}
	}

    // this is where we send
    send(sock2, tc_ptr, total_len, 0);

    printf("Attempted to send TC FRAME\n");
    valread = read( sock , buffer, 1024);
    printf("Received TM message: %s\n",buffer );
    return 0;
}
