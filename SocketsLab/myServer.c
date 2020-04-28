/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
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
#define HEADER_LEN 2

char* recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
   int serverSocket = 0;   //socket descriptor for the server socket
   int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
   char* data = "";
   
   portNumber = checkArgs(argc, argv);
   
   //create the server socket
   serverSocket = tcpServerSetup(portNumber);

   while(1){
      data = "";
      // wait for client to connect
      clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
   
      // Must have data != NULL before the strcmp. If it is after it will
      // segfaut becuase you can't call strcmp on NULL
      while(data != NULL && strcmp(data,"exit")){
         data = recvFromClient(clientSocket);
      }

      /* close the sockets */
   	close(clientSocket);
   }
	close(serverSocket);


   return 0;
}

char* recvFromClient(int clientSocket)
{
   char buf[MAXBUF];
   char* dataBuf = NULL;
   int messageLen = 0;
   uint16_t PDU_Len = 0;
   
   memset(buf, 0, MAXBUF);

   // Use a time value of 1 second (so time is not null)
   // Set time is null to have unlimited time
   while (selectCall(clientSocket, 1, 0, TIME_IS_NULL) == 0)
   {
      printf("Select timed out waiting for client to send data\n");
   }

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(clientSocket, buf, HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }
   if (messageLen == 0){
      // Client ^C
      return NULL;
   }

   PDU_Len = ntohs(((uint16_t*)buf)[0]);

   dataBuf = buf + sizeof(char)*2;

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(clientSocket, dataBuf, PDU_Len - HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }

   printf("Recv Len: %d, Header Len: %d, Data: %s\n", messageLen + HEADER_LEN, PDU_Len, dataBuf);

   return dataBuf;
}

int checkArgs(int argc, char *argv[])
{
   // Checks args and returns port number
   int portNumber = 0;

   if (argc > 2)
   {
      fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
      exit(-1);
   }
   
   if (argc == 2)
   {
   portNumber = atoi(argv[1]);
   }

   return portNumber;
}

