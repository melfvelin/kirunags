// This is the TCP/IP client
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2008
   
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
        printf("\nInvalid TM address/ Address not supported \n");
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

    if (connect(sock2, (struct sockaddr *)&tc_server, sizeof(tc_server)) < 0)
    {
        printf("\nTC Connection Failed \n");
        return -1;
    }

    send(sock2 , telec , strlen(telec) , 0 );
    printf("Attempted to send: %s\n", telec);
    valread = read( sock , buffer, 1024);
    printf("Received TM message: %s\n",buffer );
    return 0;
}