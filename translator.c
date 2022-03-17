/*
Purpose: The purpose of this assignment is to create UDP servers that can communicate with a TCP client through an indirection server
Details: This server will translate pre determined english words into french. It will perform comparisons with user input in which it will translate
Limitations and assumption: Only can translate the hardcoded english words
Known bugs: None
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 8881
#define MAXLINE 1024

char hello[] = "bonjour\n";
char red[] = "rouge\n";
char apple[] = "pomme\n";
char mouse[] = "souris\n";
char chips[] = "frites\n";

int main() {
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
	
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	int len, recvSize;

	len = sizeof(cliaddr);

	while(1)
	{
		char * translatedWord = malloc(50);
        memset(translatedWord,0,50);
		recvSize = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
		buffer[recvSize] = '\0';
		printf("Word to be translated is %s\n", buffer);
		strncpy(translatedWord,"The word you requested in french is ",37);
		if (strncmp(buffer,"hello",strlen("hello")) == 0)
		{
			strncat(translatedWord, hello, strlen(hello));
		}
		else if (strncmp(buffer,"red",strlen("red")) == 0)
		{
			strncat(translatedWord, red, strlen(red));
		}
		else if (strncmp(buffer,"apple",strlen("apple")) == 0)
		{
			strncat(translatedWord, apple,strlen(apple));
		}
		else if (strncmp(buffer,"mouse",strlen("mouse")) == 0)
		{
			strncat(translatedWord, mouse,strlen(mouse));
		}
		else if (strncmp(buffer,"chips",strlen("chips")) == 0)
		{
			strncat(translatedWord, chips, strlen(chips));
		}
		else 
		{
			strncat(translatedWord,"not in the list\n",17);
		}
		sendto(sockfd, translatedWord, strlen(translatedWord),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
		printf("%s",translatedWord);
		free(translatedWord);
	}
	return 0;
}
