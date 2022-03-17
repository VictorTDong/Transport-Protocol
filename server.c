/*
Purpose: The purpose of this assignment is to create UDP servers that can communicate with a TCP client through an indirection server
Details: This source file is for the TCP server and UDP server, it acts as the middle man between the various different micro servers
Limitations and assumption: None
Known bugs: None
*/
#include<stdio.h>
#include<string.h>	//strlen
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define MAXLINE 1024
#define KEY 5

char * toMicroserver(int port, char *clientMessage);

int main(int argc, char *argv[]){

	int socket_desc, client_sock;
	struct sockaddr_in server, client;
	char clientMessage[MAXLINE] = "";
	char serverMsg[MAXLINE] = "";
	char * optionMessage = "Hi Client, what service would you like to request?\nT for translate\nC for currency converter\nV for voting\nL for listing voting results\nE for exit\nOption:";
	char * translateMessage = "Please state what word you would like to translate (E to exit)\nAvailable words are:\nhello\nred\napple\nmouse\nchips";
	char * converterMessage = "Please enter the the amount you want to convert in the form <amount> <source> <destination> (E to exit)";
	char * voteMessage = "The available candidates (E to exit)\n<Name>     <Vote ID>\nGreg Smith 1111\nKevin Jay  2222\nJay Layco  3333\nTony Hern  4444\nPlease enter a Vote ID";
	char * temp;

	memset(&server, '\0', sizeof(server));
	memset(&client, '\0', sizeof(client));

	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket\n");
	}
	puts("Socket created\n");
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8890);
	
    int bindStatus = bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
	if( bindStatus == -1){
		perror("Binding failed");
		return 1;
	}
	printf("Binding done.\n");
	
	listen(socket_desc , 3);
	
	printf("Waiting for clients...\n");
	int  clientLen;

	client_sock = accept(socket_desc, (struct sockaddr*)&client, &clientLen);
	if (client_sock < 0){
		perror("Connection failed");
		return 1;
	}
	
	char ipAddressClient[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client.sin_addr, ipAddressClient, INET_ADDRSTRLEN);
	printf("Accepted client from ip: %s\n", ipAddressClient);

    int recvStatus;
	
	memset(serverMsg,0,MAXLINE);
	memset(clientMessage,0,MAXLINE);

	strncpy(serverMsg,optionMessage,strlen(optionMessage));
	write(client_sock , serverMsg , strlen(serverMsg));

	while(1)
	{
		memset(serverMsg,0,MAXLINE);
		memset(clientMessage,0,MAXLINE);

		recvStatus = recv(client_sock, clientMessage, MAXLINE, 0);
        if (recvStatus==-1){
            printf("Error in receiving!\n");
            break;
        }
	
		int port = 0;
		memset(serverMsg,0,MAXLINE);
		int microExitCondition = 0; 
		if(clientMessage[0] == 'T')
		{
			port = 8881;
			strncpy(serverMsg,translateMessage, strlen(translateMessage));
			write(client_sock , serverMsg , strlen(serverMsg));
			while(microExitCondition == 0)
			{
				memset(serverMsg,0,MAXLINE);
				recv(client_sock,clientMessage,MAXLINE,0);
				if (strncmp(clientMessage,"E",strlen("E")) == 0)
				{
					strncpy(serverMsg,optionMessage,strlen(optionMessage));
					microExitCondition = 1;
				}
				else
				{
					temp = toMicroserver(port, clientMessage);
					strncpy(serverMsg, temp, strlen(temp)); 
					strcat(serverMsg,translateMessage);
					free(temp);
				}
				write(client_sock , serverMsg , strlen(serverMsg));
			}
		}
		else if(clientMessage[0] == 'C')
		{
			port = 8882;
			strncpy(serverMsg,converterMessage, strlen(converterMessage));
			write(client_sock , serverMsg , strlen(serverMsg));
			while(microExitCondition == 0)
			{
				memset(serverMsg,0,MAXLINE);
				recv(client_sock,clientMessage,MAXLINE,0);
				if (strncmp(clientMessage,"E",strlen("E")) == 0)
				{
					strncpy(serverMsg,optionMessage,strlen(optionMessage));
					microExitCondition = 1;
				}
				else
				{
					temp = toMicroserver(port, clientMessage);
					strncpy(serverMsg, temp, strlen(temp)); 
					strcat(serverMsg,converterMessage);
					free(temp);
				}
				write(client_sock , serverMsg , strlen(serverMsg));
			}
		}
		else if(clientMessage[0] == 'V')
		{
			port = 8883;
			strncpy(serverMsg,voteMessage, strlen(voteMessage));
			write(client_sock , serverMsg , strlen(serverMsg));

			memset(serverMsg,0,MAXLINE);
			recv(client_sock,clientMessage,MAXLINE,0);

			if (strncmp(clientMessage,"E",strlen("E")) == 0)
			{
				strncpy(serverMsg,optionMessage,strlen(optionMessage));
			}
			else
			{
				int input;
				char ip[15];
				sscanf(clientMessage,"%d", &input);
				input *= KEY;
				sprintf(clientMessage,"%d %s", input, ipAddressClient);

				temp = toMicroserver(port, clientMessage);
				strncpy(serverMsg, temp, strlen(temp)); 
				strcat(serverMsg,optionMessage);
				free(temp);
			}
			write(client_sock , serverMsg , strlen(serverMsg));
		
		}
		else if(clientMessage[0] == 'L')
		{
			port = 8883;
			
			sprintf(clientMessage,"%s %s", "L", ipAddressClient);

			temp = toMicroserver(port, clientMessage);
			strncpy(serverMsg, temp, strlen(temp)); 
			strcat(serverMsg,optionMessage);
			free(temp);
			write(client_sock , serverMsg , strlen(serverMsg));
		}
		else if(clientMessage[0] == 'E')
		{
			break;
		}
		else
		{
			strcpy(serverMsg,"Invalid option, please try again\n");
			strcat(serverMsg,optionMessage);
			write(client_sock , serverMsg , strlen(serverMsg));
		}
	}
    close(client_sock);
    close(socket_desc);
	puts("All sockets closed");
	return 0;
}

/*
Function: toMicroserver
Purpose: To handle the communication between the UDP client and server
Method: The function will send the TCP client message to the UDP server and will perform the function requested. It will then recieve the UDP server response
Input: int for port number of the specified UDP server and a char pointer for the message to be sent to UDP server
Output: char pointer for the UDP server response
*/

char * toMicroserver(int port, char *clientMessage) 
{
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr;
	struct timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 300000;

	char * serverResponse = malloc(sizeof(char));

	memset(serverResponse,0,MAXLINE);
	memset(buffer,0,MAXLINE);

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = INADDR_ANY;
	//servaddr.sin_addr.s_addr = inet_addr("136.159.5.25");
	//Commented line of code is for using CSX and the other is for local IP
	int n, len;
	
	sendto(sockfd, clientMessage, strlen(clientMessage),MSG_CONFIRM, (const struct sockaddr *) &servaddr,sizeof(servaddr));
	printf("Message sent to microserver is: %s\n", clientMessage);
	n = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, (struct sockaddr *) &servaddr,&len);
	if(n <= 0)
	{
		printf("Microserver is offline\n");
		strcpy(serverResponse,"Microserver is offline, please enter E and choose a different option or try again later\n");
	}
	else
	{
		buffer[n] = '\0';
		strncpy(serverResponse,buffer,strlen(buffer));
		printf("Message recieved from microserver is: %s\n", serverResponse);
	}

	close(sockfd);
	return serverResponse;
}
