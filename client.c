/*
Purpose: The purpose of this assignment is to create UDP servers that can communicate with a TCP client through an indirection server
Details: This source file is the client code in which it will connect to the TCP indirection server and communicate with it
Limitations and assumption: None
Known bugs: None

Some parts of the code was taken from example code posted on D2L
*/
#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

#define MESSAGESIZE 1024

int main(int argc , char *argv[])
{
	int serverSocket;
	struct sockaddr_in server;
	char clientMessage[MESSAGESIZE] , serverMessage[MESSAGESIZE];
	
	serverSocket = socket(AF_INET , SOCK_STREAM , 0);
	if (serverSocket == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(8890);

	if (connect(serverSocket , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");

	while(1)
	{
		memset(serverMessage,0,MESSAGESIZE);
		if(recv(serverSocket , serverMessage , MESSAGESIZE , 0) < 0)
		{
			puts("recv failed");
			break;
		}
		puts(serverMessage);
		fgets(clientMessage,MESSAGESIZE,stdin);
		
		if(send(serverSocket , clientMessage , strlen(clientMessage) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
	}
	
	close(serverSocket);
	return 0;
}
