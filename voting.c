/*
Purpose: The purpose of this assignment is to create UDP servers that can communicate with a TCP client through an indirection server
Details: This will either recieve the "L" command from the UDP server in which it checks against the IP and if the user has voted to see if user is eligible to 
		 see the list. If command is not "L" the program will check to see if the message from UDP client is one of the keys of the candidate. It will recieve 
		 an encrypted key and the server will decrypt it to compare with hard coded user keys
Limitations and assumption: None
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

#define PORT 8883
#define MAXLINE 1024
#define STRINGMAX 15
//Vote ID for each candidates
#define CANDIDATE1 1111
#define CANDIDATE2 2222
#define CANDIDATE3 3333
#define CANDIDATE4 4444

int checkIP(char *clientIPLog[], char *clientIP);

int main() {
	int sockfd;
	char buffer[MAXLINE];
	char key[STRINGMAX];
	char serverMessage[MAXLINE];
	char *clientIPLog[STRINGMAX]; 
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
	
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	int len, recvSize;
	//votes of each candidates
	int c1 = 312;
	int c2 = 430;
	int c3 = 397;
	int c4 = 423;
	
	len = sizeof(cliaddr); 
	for(int i = 0; i < STRINGMAX; i++)
	{
		clientIPLog[i] = malloc(INET_ADDRSTRLEN);
	}

	int didVote = 0;
	int ipCheck;
	while(1)
	{	
		char * input = malloc(sizeof(char));
		char * ip = malloc(INET_ADDRSTRLEN);
		
		recvSize = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, ( struct sockaddr *) &cliaddr,&len);
		buffer[recvSize] = '\0';
		sscanf(buffer,"%s %s", input, ip);
		ipCheck = checkIP(clientIPLog, ip);
		printf("Client IP is: %s\n", ip);

		if (strcmp(input, "L") == 0)
		{	
			if(didVote == 1 &&  ipCheck == 1)
			{
				sprintf(serverMessage, "%s %d %s %d %s %d %s %d %s","Here are the results\n <Name>     <# Votes>\n Greg Smith", c1, "\n Kevin Jay ",c2, "\n Jay Layco ", c3,"\n Tony Hern ", c4,"\n");
			}
			else
			{
				strcpy(serverMessage,"You have not voted. Please vote before you see results\n");
			}
			
		}
		
		else if(didVote == 0)
		{
			int key = atoi(input);
			printf("This is the encrypted key %d\n", key);
			key = key/5;
			printf("This is the decrypted key %d\n", key);
			if(key == CANDIDATE1)
			{
				c1++;
				strcpy(serverMessage,"Thank you for voting\n");
				didVote = 1; 
			}
			else if(key == CANDIDATE2)
			{
				c2++;
				strcpy(serverMessage,"Thank you for voting\n");
				didVote = 1; 
			}
			else if(key == CANDIDATE3)
			{
				c3++;
				strcpy(serverMessage,"Thank you for voting\n");
				didVote = 1; 
			}
			else if(key == CANDIDATE4)
			{
				c4++;
				strcpy(serverMessage,"Thank you for voting\n");
				didVote = 1; 
			}
			else 
			{
				strcpy(serverMessage,"The Vote ID is incorrect\n");
				didVote = 0;
			}
		}
		else
		{
			strcpy(serverMessage,"You have already voted. Please vote before you see results\n");
		}
		sendto(sockfd, serverMessage, strlen(serverMessage),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
		free(input);
		free(ip);
	}
	return 0;
}

/*
Function: checkIP
Purpose: To check if the client IP is already on the list
Method: This will traverse through a list and checks to see if the IP is already added, if it is not, it will add the IP. If the IP is found, then it will
		stop and return a 1
Input: Pointer to the char array and char pointer to the client IP
Output: int for telling whether of not the IP is found
*/

int checkIP(char *clientIPLog[], char *clientIP)
{
	int voted = 0;
	int count = 0;
	char *temp = malloc(INET_ADDRSTRLEN);

	for(int i = 0; i < STRINGMAX; i++)
	{
		if(strcmp(clientIPLog[i],clientIP) == 0)
		{
			return 1;
		}
		count++;
	}
	if(count == STRINGMAX)
	{
		int i = 0;
		while (clientIPLog[i] == 0)
		{
			i++;
		}
		memcpy(clientIPLog[i],clientIP,strlen(clientIP));
		printf("This is the ip saved: %s\n",clientIPLog[i]);
		return 0;
	}
	return 1;
}
