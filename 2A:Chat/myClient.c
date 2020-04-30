//
// Written Hugh Smith, Updated: April 2020
// Use at your own risk.  Feel free to copy, just leave my name in it.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "networks.h"
#include "pollLib.h"

#define DEBUG_FLAG 1
#define HANDLE_BYTE 1
#define CHAT_HEADER 3
#define MAX_LOGIN_SIZE 104 //Max size of 100 Character handle + 4 Bytes Header
#define FLAG_1 1

void sendToServer(int socketNum);
int getFromStdin(char * sendBuf, char * prompt);
void checkArgs(int argc, char * argv[]);
void login(char* handle, int socketNum, char* loginBuff);
void sendLogin(int socketNum, char* loginBuff, uint16_t sendLen);
/*
void checkHandle(char* handle, int socketNum);
*/

void printLoginBuff(char* loginBuff)
{
   int i = 0;

   printf("Chat PDULen:\t%i\n", ntohs(((uint16_t*)loginBuff)[0]));
   printf("Flag:\t\t%u\n", ((uint8_t*)loginBuff)[2]);
   printf("Handle Length:\t%u\n", ((uint8_t*)loginBuff)[3]);
   printf("Handle:\t\t");
   for (i = 0; i < loginBuff[3]; i ++){
      printf("%c", loginBuff[i + 4]);
   }
   printf("\n");
}


int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
   char loginBuff[MAX_LOGIN_SIZE];
	
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);

   /* Login to the server with the handle */
   login(argv[1], socketNum, loginBuff);

   /*
   checkHandle(argv[1], socketNum);
   */
	
	sendToServer(socketNum);
	
	close(socketNum);
	
	return 0;
}

// Copies the string from the handle into the buffer starting at the offset
void fillBuff(uint8_t len, char* handle, char* buff, int offset)
{
   int i;

   for (i = 0; i < len; i ++){
      buff[i + offset] = handle[i];
   }
}

// Log the client into the server with the handle
void login(char* handle, int socketNum, char* loginBuff)
{
   uint8_t handleLen;
   uint16_t pduLen;
   int offset = CHAT_HEADER + HANDLE_BYTE;

   handleLen = ((uint16_t)(strlen(handle)));
   pduLen = handleLen + HANDLE_BYTE + CHAT_HEADER;
   ((uint16_t*)(loginBuff))[0] = htons(pduLen);
   loginBuff[2] = FLAG_1;
   loginBuff[3] = handleLen;
   fillBuff(handleLen, handle, loginBuff, offset);

   sendLogin(socketNum, loginBuff, pduLen);
}

void sendLogin(int socketNum, char* loginBuff, uint16_t sendLen)
{
   int sent = 0;            //actual amount of data sent/* get the data and send it   */
   sent =  send(socketNum, loginBuff, sendLen, 0);
   if (sent < 0)
   {
      perror("send call");
      exit(-1);
   }

   printf("Amount of data sent is: %d\n", sent);
}



void sendToServer(int socketNum)
{
	char sendBuf[MAXBUF];   //data buffer
	int sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
			
	memset(sendBuf, 0, MAXBUF);

	while (strcmp(sendBuf, "exit"))
	{

		sendLen = getFromStdin(sendBuf, "$: ");

		
		//printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
			
		sent =  send(socketNum, sendBuf, sendLen, 0);
		if (sent < 0)
		{
			perror("send call");
			exit(-1);
		}

		printf("Amount of data sent is: %d\n", sent);
	}
}

int getFromStdin(char * sendBuf, char * prompt)
{
	// Gets input up to MAXBUF-1 (and then appends \0)
	// Returns length of string including null
	char aChar = 0;
	int inputLen = 0;       
	
	// Important you don't input more characters than you have space 
	printf("%s ", prompt);
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			sendBuf[inputLen] = aChar;
			inputLen++;
		}
	}

	sendBuf[inputLen] = '\0';
	inputLen++;  //we are going to send the null
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s handle host-name port-number \n", argv[0]);
		exit(1);
	}
}
