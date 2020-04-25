/******************************************************************************
* myClient.c
*
*****************************************************************************/

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

#define DEBUG_FLAG 1

char* sendToServer(int socketNum);
void checkArgs(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
   char* dataBuf = NULL;
	
	checkArgs(argc, argv);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	
//while(strcmp(dataBuf, "exit") != 0){
	dataBuf = sendToServer(socketNum);
   printf("dataBut %s\n", dataBuf);
	
	close(socketNum);
	
	return 0;
}

char* sendToServer(int socketNum)
{
   char sendBuf[MAXBUF];   //total buffer PDU
   char *dataBuf = NULL;   //just the data buffer
   char aChar = 0;
   int sendLen = 0;        //length of total buffer
   int dataLen = 0;        //length of just the data segment
   int sent = 0;           //actual amount of data sent

   // Append additional 2 bytes to sendLen
   sendLen += 2;
   dataBuf = sendBuf + 2;

   memset(sendBuf, 0, MAXBUF);

   while(strcmp(dataBuf, "exit"))
   {
      // Important you don't input more characters than you have space 
      printf("Enter data: ");
      aChar = 0;
      sendLen = 2;
      while ((sendLen) < (MAXBUF - 1) && aChar != '\n')
      {
         aChar = getchar();
         if (aChar != '\n')
         {
            sendBuf[sendLen] = aChar;
            sendLen++;
         }
      }

      // Setting the dataBuf to point to the second byte of the buffer by 2 bytes
      dataBuf = sendBuf + 2;
      dataLen = sendLen - 2;
      dataLen++;  //Including the null inIncluding the null in  the data length

      sendBuf[sendLen] = '\0';
      sendLen++;  //we are going to send the null

      // Setting the first two bytes to the packet length
      ((uint16_t*)sendBuf)[0] = htons(sendLen);
      printf("read: %s string len: %d (including null), PDU Len: %d \n", dataBuf, dataLen, ntohs(((uint16_t*)sendBuf)[0]));

      sent =  send(socketNum, sendBuf, sendLen, 0);
      if (sent < 0)
      {
         perror("send call");
         exit(-1);
      }

      printf("Amount of data sent is: %d\n", sent);
   }
   return dataBuf;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}
