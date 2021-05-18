/*  This is the TCP/IP client
*   Client side C++ program to transmit and receive TC and TM frames
*
*
*/
#include "client.h"
#include "tmtc.h"
#include "tables.h"

#ifndef debug
#define debug
#endif /* debug */

namespace client{
    /* *maleTC_Frame - creates a TC packet made of a TC_HEADER and a data field
    *   input: payload data string and payload data size
    *   output: pointer to TC packet in memory
    *   returns uint8_t *tc_ptr
    */
    uint8_t *makeTC_frame(const char *message, uint32_t message_len)
    {
        
        TC_HEADER tx_tc_frame;
        tx_tc_frame.preamble = PREAMBLE;
        uint32_t total_len = (sizeof(TC_HEADER) + message_len);
        tx_tc_frame.total_len = total_len;
        tx_tc_frame.tf_size = message_len;
        
        // allocate memory space for TC packet
        uint8_t *tc_ptr = (uint8_t *)malloc(tx_tc_frame.total_len);
        // copy TC header to allocated memory
        if(memcpy(tc_ptr, &tx_tc_frame, sizeof(TC_HEADER)) == NULL)
        {
            std::cout << "memcpy returned empty address!" << std::endl;
        }

        // adding message after tc header
        if(memcpy((tc_ptr + sizeof(TC_HEADER)), message, tx_tc_frame.tf_size) == NULL)
        {
            std::cout << "memcpy returned empty address!" << std::endl;
        }

        // return the pointer containing the adress to the constructed TC frame
        return tc_ptr; 
    }
} /* client */


int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    int sock2 = 0;
    char tc_address[16] = {0};
    char tm_address[16] = {0};
    int tc_port = 0;
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
    //tc_server.sin_port = htons(TC_PORT);

    // To be continued
    std::cout << "Please input what port to connect to (2001 for TM or 52001 for GRC)" << std::endl;
    std:cin >> tc_port;
    std::cout << "Port: " << tc_port << std::endl;
    tc_server.sin_port = htons(tc_port);

    std::cout << "Please input ipv4 address to connect to TM port: " << std::endl;
    std::cin >> tm_address;

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, tm_address, &tm_server.sin_addr)<=0)
    {
        printf("\nInvalid TM address/ Addrescis not supported \n");
        return -1;
    }

    
    std::cout << "Please input ipv4 address to connect to TC port: " << std::endl;
    std::cin >> tc_address;

    if(inet_pton(AF_INET, tc_address, &tc_server.sin_addr)<=0)
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
    
    
    // for user input
    // std::cin >> key_buffer;
    // or hardcoded input:
    char key_buffer[128] = {0};
    std::cout << "Please enter telecommand: " << std::endl;
    std::cin >> key_buffer;

    uint8_t *tc_ptr = tmtc::telecommand::encapsulate(key_buffer, strlen(key_buffer));
    // char test[128];
    // memcpy(&test, (tc_ptr + sizeof(TC_HEADER)), strlen("NEW MESSAGE"));
    // std::cout << "test = " << test << std::endl;
    send(sock2, tc_ptr, (sizeof(TC_HEADER) + strlen(key_buffer)), 0);
    //printf("Attempted to send TC FRAME\n");
    std::cout << "[client.cpp:main] Attempted to send TC: " << key_buffer << std::endl;
    // read returns bytes read
    valread = read(sock, buffer, 1024);
    std::cout << "[client.cpp:main] received bytes: " << valread << std::endl;
    uint8_t *tm_ptr = (uint8_t *)malloc(valread * sizeof(uint8_t));
    
    memcpy(tm_ptr, &buffer, valread);
    tmtc::telemetry::decapsulate(tm_ptr);
    //printf("Received TM message: %s\n",buffer );
    return 0;
}
