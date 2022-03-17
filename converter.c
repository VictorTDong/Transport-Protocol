/*
Purpose: The purpose of this assignment is to create UDP servers that can communicate with a TCP client through an indirection server
Details: This server will read in user input and checks to see if the format of the input is correct and if the input matches one of the eligible conversions,
		 after it performs the comparison, it will convert the CAD amount into one of the eligible currencies
Limitations and assumption: Can only convert from CAD
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

#define PORT 8882
#define MAXLINE 1024
#define STRINGMAX 15

// conversion rates from cad
#define USD 0.81
#define EURO 0.70
#define POUND 0.59
#define BC 0.000013

double convert(double amount, char *source, char *destination);

int main() {
	int sockfd;
	char buffer[MAXLINE];
	char amount[STRINGMAX];
	char source[STRINGMAX];
	char destination[STRINGMAX];
	char serverMessage[MAXLINE]; 
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
		memset(buffer, 0, MAXLINE);
		memset(serverMessage, 0, MAXLINE);
		double convertedAmount = 0;
		recvSize = recvfrom(sockfd, (char *)buffer, MAXLINE,MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
		buffer[recvSize] = '\0';
		int contentsRead = sscanf(buffer,"%s %s %s", amount, source, destination);
		if (contentsRead == 3)
		{
			double amountLong = atof(amount);
			convertedAmount = convert(amountLong,source,destination);
			destination[strlen(destination)] = '\0';
			if(strcmp(destination,"BITCOIN") == 0)
			{
				snprintf(serverMessage,MAXLINE,"%s %.2lf %s %s %.9lf %s\n","Converted",amountLong, source,"to", convertedAmount, destination);
			}
			else if(convertedAmount != -1)
			{
				snprintf(serverMessage,MAXLINE,"%s %.2lf %s %s %.2lf %s\n","Converted",amountLong, source,"to", convertedAmount, destination);
			}
			else
			{
				memset(serverMessage, 0, MAXLINE);
				strcpy(serverMessage,"Conversion not supported\n");
			}
		}
		else 
		{
			strcpy(serverMessage,"Invalid input\n");
			printf("%s",serverMessage);
		}
		sendto(sockfd, serverMessage, strlen(serverMessage),MSG_CONFIRM, (const struct sockaddr *) &cliaddr,len);
		printf("%s",serverMessage);
	}
	return 0;
}

/*
Function: convert
Purpose: Converts CAD to specified currency
Method: Will multiply the CAD with the various exhange rates to get currency
Input: Pointers the source and destination currency and a double for ammount
Output: Double for the converted amount
*/

double convert(double amount, char *source, char *destination)
{
	if(strcmp(source,"CAD") == 0)
	{
		if (strcmp(destination,"USD") == 0)
		{
			return amount * USD;
		}
		if (strcmp(destination,"EURO") == 0)
		{
			return amount * EURO;
		}
		if (strcmp(destination,"POUND") == 0)
		{
			return amount * POUND;
		}
		if (strcmp(destination,"BITCOIN") == 0) 
		{
			return amount * BC;
		}
	}
	else
	{
		return -1;
	}
}
