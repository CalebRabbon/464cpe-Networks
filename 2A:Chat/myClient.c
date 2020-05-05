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
#include "flags.h"
#include "pollLib.h"
#include "macros.h"
#include "unitTest.h"

void sendToServer(int socketNum);
int getFromStdin(char * stdbuf, char * prompt);
void checkArgs(int argc, char * argv[]);
void login(char* handle, int socketNum, char* loginBuff);
void sendLogin(int socketNum, char* loginBuff, uint16_t sendLen);
int convertCharType(char type);

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

   /*
   int socketNum = 0;         //socket descriptor
   char loginBuff[MAX_LOGIN_SIZE];

   checkArgs(argc, argv);

   // set up the TCP Client socket
   socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);

   // Login to the server with the handle
   login(argv[1], socketNum, loginBuff);

   sendToServer(socketNum);

   close(socketNum);
   */

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

void printServerMsg(uint8_t flag, int messageLen, uint16_t PDU_Len){
   printf("From Server:\n");
   printf("\tmsg Len: %d \tPDU_Len: %d\n", messageLen, PDU_Len);
   switch(flag)
   {
      case FLAG_2:
         printf("\tFlag %i: Good Handle\n", flag);
         break;
      case FLAG_3:
         printf("\tFlag %i: Handle Already Exists\n", flag);
         break;
      case FLAG_4:
         printf("\tFlag %i: Broadcast Message Received\n", flag);
         break;
      case FLAG_5:
         printf("\tFlag %i: Message from Another Client\n", flag);
         break;
      case FLAG_7:
         printf("\tFlag %i: Error When Sending a Message: One or More Clients Don't Exist\n", flag);
         break;
      case FLAG_9:
         printf("\tFlag %i: Message from Server ACKing the FLAG_8 to Terminate\n", flag);
         break;
      case FLAG_11:
         printf("\tFlag %i: Server responding to FLAG_10: Giving the number of clients on the server\n", flag);
         break;
      case FLAG_12:
         printf("\tFlag %i: Following the FLAG_11: Giving a FLAG_12 for each client handle on the server\n", flag);
         break;
      case FLAG_13:
         printf("\tFlag %i: Listing the client handles is finished\n", flag);
         break;
   }
}

// Responds to an incoming packet received from the server
// dataBuf is the buffer of data after the Chat PDU Length
void packetResponse(uint8_t flag, char* dataBuf, int messageLen, uint16_t PDU_Len, int socketNum)
{
   /*
   char* handle = NULL;
   uint8_t handleLen = 0;
   char strHandle[MAX_HANDLE_SIZE + 1];  // Used for the Handle, 101 becuase one is used for \0
   */
   printServerMsg(flag, messageLen, PDU_Len);
   switch(flag)
   {
      case FLAG_2:

         break;
      case FLAG_3:
         exit(0);
         break;
      case FLAG_4:
         break;
      case FLAG_5:
         break;
      case FLAG_8:
         break;
      case FLAG_10:
         break;
   }
}


// Receives a message from the server
void ackFromServer(int socketNum)
{
	char buf[MAXBUF];
	int messageLen = 0;
   
   // Stuff Caleb Added
   char* dataBuf = NULL;
   uint16_t PDU_Len = 0;
   uint8_t flag = 0;
		
	//now get the data from the socketNum (message includes null)
   if ((messageLen = recv(socketNum, buf, HEADER_LEN, MSG_WAITALL)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
	
	if (messageLen == 0)
	{
		// recv() 0 bytes so client is gone
      fprintf(stderr, "Server Terminated\n");
      exit(1);
	}

   PDU_Len = ntohs(((uint16_t*)buf)[0]);

   dataBuf = buf + sizeof(char)*2;

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(socketNum, dataBuf, PDU_Len - HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }
   flag = dataBuf[0];

   packetResponse(flag, dataBuf, messageLen, PDU_Len, socketNum);
}

// Log the client into the server with the handle
void login(char* handle, int socketNum, char* loginBuff)
{
   uint8_t handleLen;
   uint16_t pduLen;
   int offset = CHAT_HEADER_LEN + HANDLE_BYTE;

   handleLen = ((uint16_t)(strlen(handle)));
   pduLen = handleLen + HANDLE_BYTE + CHAT_HEADER_LEN;
   ((uint16_t*)(loginBuff))[0] = htons(pduLen);
   loginBuff[2] = FLAG_1;
   loginBuff[3] = handleLen;
   fillBuff(handleLen, handle, loginBuff, offset);

   sendLogin(socketNum, loginBuff, pduLen);

   ackFromServer(socketNum);
}

void sendLogin(int socketNum, char* loginBuff, uint16_t sendLen)
{
   int sent = 0;            //actual amount of data sent/*
   sent =  send(socketNum, loginBuff, sendLen, 0);
   if (sent < 0)
   {
      perror("send call");
      exit(-1);
   }

   printf("Amount of login data sent is: %d\n", sent);
}

// Converts the character to the correct int type defined by the MACROS
int convertCharType(char type){
   switch(type){
      case 'M':
         return TYPE_M;
         break;
      case 'B':
         return TYPE_B;
         break;
      case 'E':
         return TYPE_E;
         break;
      case 'L':
         return TYPE_L;
         break;
      case 'm':
         return TYPE_M;
         break;
      case 'b':
         return TYPE_B;
         break;
      case 'e':
         return TYPE_E;
         break;
      case 'l':
         return TYPE_L;
         break;
   }
   return -1;
}

// Finds the type of the message and returns an int representing that type or -1
// if the message is incorrectly formatted
int findType(char* stdbuf){
   char* curVal = stdbuf;
   int i = 0;
   int type = 0;
   char charType = '0';

   // Skip over whitespace
   while(curVal[i] == ' '){
      i ++;
   }
   // Found the % symbol
   if (curVal[i] == '%'){
      printf("Found the percent\n");
      i ++;
      charType = curVal[i];
      type = convertCharType(charType);
      return type;
   }

   return -1;
}

void procStdin(int stdlen, char* stdbuf, char* sendbuf){
   int type;

   type = findType(stdbuf);

   switch(type){
      case TYPE_M:
         printf("TYPE_M\n");
         break;
      case TYPE_B:
         printf("TYPE_B\n");
         break;
      case TYPE_E:
         printf("TYPE_E\n");
         break;
      case TYPE_L:
         printf("TYPE_L\n");
         break;
   }
}


void sendToServer(int socketNum)
{
	char stdbuf[MAXBUF];    //data from user input
   // Add 114 to sendbuf to account for 10 bytes of handles 1 byte for number of
   // destinations, 3 bytes for Chat-Header, 100 bytes for max sender handle
	char sendbuf[MAXBUF + 114];   //data sent to server
	int stdlen = 0;         //amount of data to send
	int sent = 0;           //actual amount of data sent/*
			
	memset(stdbuf, 0, MAXBUF);

	while (strcmp(stdbuf, "exit"))
	{

		stdlen = getFromStdin(stdbuf, "$: ");

      // Process the stdbuf and organize it into a packet inside the sendbuf
      procStdin(stdlen, stdbuf, sendbuf);
		
		//printf("read: %s string len: %d (including null)\n", stdbuf, stdlen);
			
		sent =  send(socketNum, stdbuf, stdlen, 0);
		if (sent < 0)
		{
			perror("send call");
			exit(-1);
      }

      printf("Amount of data sent is: %d\n", sent);
   }
}

int getFromStdin(char * stdbuf, char * prompt)
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
			stdbuf[inputLen] = aChar;
			inputLen++;
		}
	}

	stdbuf[inputLen] = '\0';
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
