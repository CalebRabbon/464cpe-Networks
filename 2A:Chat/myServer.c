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
#include "linkedlist.h"
#include "flags.h"
#include "pollLib.h"
#include "macros.h"
#include "recvparse.h"
#include "test.h"
#include "shared.h"
#include "myClient.h"
#include "parse.h"

//#define PRINT
//#define TEST

void processSockets(int mainServerSocket, Node** head);
char* recvFromClient(int clientSocket, Node** head);
void addNewClient(int mainServerSocket);
void removeClient(int clientSocket);
int checkArgs(int argc, char *argv[]);
void addToList(Node** head, char* strHandle, int socketNum);
void createStrHandle(char* handle, uint8_t handleLen, char* strHandle);
int checkClient(int socketNum, char* strHandle, Node** head);

/*
void test(Node** head, char* name, int i){
   addToList(head, name, i);
}

void t(Node** head){
   printLinkedList(*head);
}
*/

int main(int argc, char *argv[])
{
#ifdef TEST
   testfindSender();
   testfindNumHandles();
   testfindDestHandle();
   testfindTextLen();
   testfindTextStart();
   testgetText();
#endif

#ifndef TEST
	int mainServerSocket = 0;   //socket descriptor for the server socket
	int portNumber = 0;
   Node* head = makeLinkedList();
	
	setupPollSet();
	portNumber = checkArgs(argc, argv);

   while(1){
	   //create the server socket
	   mainServerSocket = tcpServerSetup(portNumber);

	   // Main control process (clients and accept())
	   processSockets(mainServerSocket, &head);
   }
	
	// close the socket - never gets here but nice thought
	close(mainServerSocket);
#endif
	
	return 0;
}

void processSockets(int mainServerSocket, Node** head)
{
	int socketToProcess = 0;
	
	addToPollSet(mainServerSocket);
		
	while (1)
	{
		if ((socketToProcess = pollCall(POLL_WAIT_FOREVER)) != -1)
		{
			if (socketToProcess == mainServerSocket)
			{
				addNewClient(mainServerSocket);
			}
			else
			{
				recvFromClient(socketToProcess, head);
            printLinkedList(*head);
			}
		}
		else
		{
			// Just printing here to let me know what is going on
			printf("Poll timed out waiting for client to send data\n");
		}
		
	}
}

// Prints the text inside a buffer
void printText(char* buff, uint8_t len)
{
   int i = 0;
   printf("Data: ");
   for (i = 0; i < len; i ++)
      printf("%c", buff[i]);
   printf("\n");
}

// Creates a Null terminated handle by copying the handle to the strHandle
// buffer
void createStrHandle(char* handle, uint8_t handleLen, char* strHandle)
{
   int i = 0;
#ifdef PRINT
   printf("Create String:\n");
#endif

   // Fill in the buffer up to the MAX_HANDLE_SIZE then add the Null
   if (handleLen <= MAX_HANDLE_SIZE){
      for (i = 0; i < handleLen; i ++){
         strHandle[i] = handle[i];
#ifdef PRINT
         printf("%c", strHandle[i]);
#endif
      }
      strHandle[handleLen] = '\0';
#ifdef PRINT
      printf("|%c|\n", strHandle[handleLen]);
#endif
   }
   else {
      for (i = 0; i < MAX_HANDLE_SIZE; i ++)
         strHandle[i] = handle[i];
      strHandle[MAX_HANDLE_SIZE] = '\0';
      fprintf(stderr,"Your handle is more than 100 characters, truncating the remaining characters\n");
   }
}

// Returns a header with a PDU Length of 3 and the designated flag
void createDefaultChatHeader(char* header, int flag){
   (((uint16_t*)header)[0]) = htons(DEFAULT_PDULEN);
   (((uint8_t*)header)[2]) = flag;
}


// Sends a designated flag
void sendFlag(int socketNum, uint8_t flag){
   char header[CHAT_HEADER_LEN];
   createDefaultChatHeader(header, flag);
   safeSend(socketNum, header, DEFAULT_PDULEN);
}

// Adds the handle to the list
void addToList(Node** head, char* strHandle, int socketNum){
   Node* node = makeNode(strHandle, socketNum);
   *head = addNode(*head, node);
}

// Checks to see if the new handle is taken. If it is send flag 3, if not add
// to list and send flag 2
int checkClient(int socketNum, char* strHandle, Node** head){

   if(!(available(*head, strHandle))){
      sendFlag(socketNum, FLAG_3);
      removeClient(socketNum);
      return 3;
   }
   else
   {
      printf("checkClient stlen(strHandle): %i\n", (int)strlen(strHandle));
      printf("checkClient strHandle: %s\n", strHandle);
      addToList(head, strHandle, socketNum);
      sendFlag(socketNum, FLAG_2);
      return 2;
   }
}


void printPacketData(uint8_t flag, char* dataBuf, uint16_t PDU_Len, int socketNum, char* handle){
   printf("Packet from Client:\n");
   printf("\tFlag: %i\tHandle: %s\tHandle Len: %i\tPDU_Len: %d\tSocket: %i\n\n", flag, handle, dataBuf[1], PDU_Len, socketNum);
}


// Fills in the new message with the data
void createNewMessage(uint8_t flag, int PDU_Len, char* message, char* newMessage){
   fillChatHeader(newMessage, flag, PDU_Len);
   newMessage += 3;
   memcpy(newMessage, message, PDU_Len - 3);
}

// Sends the %M packet
void sendM(char* message, Node** head, int PDU_Len, uint8_t flag){
   char destHandle[MAX_HANDLE_SIZE + 1];
   char sendHandle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;
   int sendLen = 0;
   int socketNum = 0;
   int i = 0;
   char newMessage[PDU_Len];

   memset(destHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);

   sendLen = findTextLen(message, PDU_Len);
   findSender(message, sendHandle);
   handleNum = findNumHandles(message, sendHandle);

   for(i = 1; i <= handleNum; i ++){
      findDestHandle(message, i, destHandle);
      socketNum = findSocket(*head, destHandle);
      printf("Socket %i\n", socketNum);
      printf("Handle %s\n", destHandle);
      createNewMessage(flag, PDU_Len, message, newMessage);
      if(socketNum == 0){
         printf("User %s doesn't exist\n", destHandle);
      }
      else{
         safeSend(socketNum, message, sendLen);
         printf("Finished sending\n");
      }
   }
}

// Responds to an incoming packet received from the client
// dataBuf is the buffer of data after the Chat PDU Length
// I removed the messagelength since it is really only used for debugging
void packetResponse(uint8_t flag, char* dataBuf, uint16_t PDU_Len, int socketNum, Node** head)
{
   char* handle = NULL;
   uint8_t handleLen = 0;
   char strHandle[MAX_HANDLE_SIZE + 1];  // Used for the Handle, 101 becuase one is used for \0
   Node* node;

   switch(flag)
   {
      case FLAG_1:
         handleLen = dataBuf[1];

#ifdef PRINT
         printf("handleLen = %i\n", handleLen);
#endif
         // Setting dataBuf to handle
         handle = dataBuf + sizeof(char)*2;

         // Copy handle data to strHandle and add a Null character
         createStrHandle(handle, handleLen, strHandle);

#ifdef PRINT
         printf("handle = %s\t handleLen = %i\n", handle, handleLen);
         printf("strHandle = %s\t strlen(strHandle) = %i\n", strHandle, (int)strlen(strHandle));
#endif
         printPacketData(flag, dataBuf, PDU_Len, socketNum, strHandle);

#ifdef PRINT
         printf("handle: %s, strHandle: %s\n", handle, strHandle);
#endif
         checkClient(socketNum, strHandle, head);
         break;
      case FLAG_4:
         break;
      case FLAG_5:
         printf("M Flag 5 received\n");
         //printf("Data buf len %i\n", findTextLen(dataBuf, PDU_Len));
         sendM(dataBuf, head, PDU_Len, flag);
         break;
      case FLAG_8:
         printf("E Flag 8 received\n");
         sendFlag(socketNum, FLAG_9);
         node = findNode(*head, socketNum);
         *head = removeNode(*head, node);
         removeClient(socketNum);
         break;
      case FLAG_10:
         break;
   }
}

char* recvFromClient(int clientSocket, Node** head)
{
	char buf[MAXBUF];
	int messageLen = 0;
   
   // Stuff Caleb Added
   char* dataBuf = NULL;
   uint16_t PDU_Len = 0;
   uint8_t flag = 0;
   Node* node = NULL;
		
	//now get the data from the clientSocket (message includes null)
   if ((messageLen = recv(clientSocket, buf, HEADER_LEN, MSG_WAITALL)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
	
	if (messageLen == 0)
	{
		// recv() 0 bytes so client is gone
      node = findNode(*head, clientSocket);
      *head = removeNode(*head, node);
		removeClient(clientSocket);
      return NULL;
	}

   PDU_Len = ntohs(((uint16_t*)buf)[0]);
   printf("PDU_LEN %i\n", PDU_Len);

   dataBuf = buf + sizeof(char)*2;

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(clientSocket, dataBuf, PDU_Len - HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }
   flag = dataBuf[0];

#ifdef PRINT
   printf("flag %i\n", flag);
#endif

   printf("messageLen %i\n", messageLen);
   packetResponse(flag, dataBuf, PDU_Len, clientSocket, head);
   return dataBuf;
}

void addNewClient(int mainServerSocket)
{
	int newClientSocket = tcpAccept(mainServerSocket, DEBUG_FLAG);
	
	addToPollSet(newClientSocket);
	
}

void removeClient(int clientSocket)
{
	printf("Client on socket %d terminted\n", clientSocket);
	removeFromPollSet(clientSocket);
	close(clientSocket);
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

