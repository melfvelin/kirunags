// This is the TCP/IP server
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstring>

#define TM_PORT 2001
#define TC_PORT 2002
#define DO_PORT 2007
#define DE_PORT 2009

int main(int argc, char const *argv[])
{
	// Init of variables
	int server_fd;
	int new_socket;
	int valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	// Init char array of zeros
	char buffer[1024] = {0};
	char hello[32] = "Hello from server";
	std::string s_hello = "Hello from serverString";

	// Creating socket file descriptor
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	// No idea what this does so far
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY should bind the socket to the localhost address
	address.sin_port = htons(TM_PORT);
	// The above things go into the below bind function, I think

	if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if(listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		// Reading value from new_socket to buffer
		valread = read(new_socket, buffer, 1024);
		// printing buffer
		printf("%s\n", buffer);	
	}
	

	send(new_socket, hello, strlen(hello), 0);
	// send(new_socket, s_hello, strlen(hello), 0);
	// sending strings does not work
	printf("Attempted to send: %s \n", hello);

	return 0;
}


