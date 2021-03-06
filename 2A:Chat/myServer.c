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

void fillFlag11(char* message, int len);

void testfillFlag11(){
   char flag11msg[5];
   void* data;
   uint32_t length;

   printf("TEST: fillFlag11\n");

   fillFlag11(flag11msg, 1);
   TEST_INT(flag11msg[0], 11);

   data = flag11msg;
   data += sizeof(char);
   length = ntohl(*((uint32_t*)data));
   TEST_UNSIGNED(length, 1);

   fillFlag11(flag11msg, 2);
   TEST_INT(flag11msg[0], 11);

   data = flag11msg;
   data += sizeof(char);
   length = ntohl(*((uint32_t*)data));
   TEST_UNSIGNED(length, 2);

   fillFlag11(flag11msg, 3);
   TEST_INT(flag11msg[0], 11);

   data = flag11msg;
   data += sizeof(char);
   length = ntohl(*((uint32_t*)data));
   TEST_UNSIGNED(length, 3);

   fillFlag11(flag11msg, 0);
   TEST_INT(flag11msg[0], 11);

   data = flag11msg;
   data += sizeof(char);
   length = ntohl(*((uint32_t*)data));
   TEST_UNSIGNED(length, 0);

   printf("Finish: fillFlag11\n");
}

int main(int argc, char *argv[])
{
#ifdef TEST
   testfindSender();
   testfindNumHandles();
   testfindDestHandle();
   testfindTextLen();
   testfindTextStart();
   testgetText();
   testfillFlag11();
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
#ifdef PRINT
            printLinkedList(*head);
#endif
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
#ifdef PRINT
      printf("checkClient stlen(strHandle): %i\n", (int)strlen(strHandle));
      printf("checkClient strHandle: %s\n", strHandle);
#endif
      addToList(head, strHandle, socketNum);
      sendFlag(socketNum, FLAG_2);
      return 2;
   }
}


void printPacketData(uint8_t flag, char* databuf, uint16_t PDU_Len, int socketNum, char* handle){
   printf("Packet from Client:\n");
   printf("\tFlag: %i\tHandle: %s\tHandle Len: %i\tPDU_Len: %d\tSocket: %i\n\n", flag, handle, databuf[1], PDU_Len, socketNum);
}


// Fills in the new message with chat header then the additional message
// components
void createNewMessage(uint8_t flag, int PDU_Len, char* message, char* newMessage){
   fillChatHeader(newMessage, flag, PDU_Len);
   newMessage += 3;

   // Move message to Send Handle length part
   message += 1;
   memcpy(newMessage, message, PDU_Len - 3);
}

// Prints out a message
void printMessage(char* msg, int len){
   int i = 0;

   for (i = 0; i < len; i ++){
      printf("%x", msg[i]);
   }
   printf("\n");;
}

void sendFlag7(int sendSocket, char* destHandle){
   int pdulen = 0;
   uint8_t destLen = strlen(destHandle);
   char databuf[MAX_HANDLE_SIZE + HANDLE_BYTE];
   char* mesPtr = databuf;
   char newMessage[MAX_HANDLE_SIZE + HANDLE_BYTE];

   //databuf[0] = Flag which will be filled in by createNewMessage
   databuf[1] = destLen;
   mesPtr += 2;
   memcpy(mesPtr, destHandle, destLen);
   mesPtr[destLen] = '\0';

   pdulen = CHAT_HEADER_LEN + HANDLE_BYTE + destLen;
#ifdef PRINT
   printf("Pdulen %i\n", pdulen);
#endif

   createNewMessage(FLAG_7, pdulen, databuf, newMessage);

   safeSend(sendSocket, newMessage, pdulen);
}

// Sends the %B packet
void sendB(char* message, Node** head, int PDU_Len, uint8_t flag){
   char destHandle[MAX_HANDLE_SIZE + 1];
   char sendHandle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;
   int sendSocket = 0;
   int i = 0;
   char newMessage[PDU_Len];
   Node* node;

   memset(destHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(newMessage, 0, PDU_Len);

   findSender(message, sendHandle);
#ifdef PRINT
   printf("Sender %s\n", sendHandle);
#endif
   handleNum = findListLength(*head);
#ifdef PRINT
   printf("Handle Numbers %i\n", handleNum);
#endif

   // Set the message to the right spot
   //message +=  FLAGBYTE + SEND_HANDLE_BYTE + strlen(sendHandle);

   sendSocket = findSocket(*head, sendHandle);

   // One indexed for the findNodeIndex
   for(i = 0; i < handleNum; i ++){
      node = findNodeIndex(*head, i);
#ifdef PRINT
      printf("Socket %i\n", node->socketNum);
      printf("Handle %s\n", node->handle);
#endif
      createNewMessage(flag, PDU_Len, message, newMessage);
      if(node->socketNum == 0){
#ifdef PRINT
         printf("User %s doesn't exist\n", node->handle);
#endif
         sendFlag7(sendSocket, node->handle);
      }
      else if(node->socketNum == sendSocket){
         //Skip sending message back to sender
      }
      else{
         safeSend(node->socketNum, newMessage, PDU_Len);
      }
   }
}

// Sends the %M packet
void sendM(char* message, Node** head, int PDU_Len, uint8_t flag){
   char destHandle[MAX_HANDLE_SIZE + 1];
   char sendHandle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;
   int destSocket = 0;
   int sendSocket = 0;
   int i = 0;
   char newMessage[PDU_Len];

   memset(destHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);

   findTextLen(message, PDU_Len);
   findSender(message, sendHandle);
   handleNum = findNumHandles(message, sendHandle);

   sendSocket = findSocket(*head, sendHandle);

   for(i = 1; i <= handleNum; i ++){
      findDestHandle(message, i, destHandle);
      destSocket = findSocket(*head, destHandle);
#ifdef PRINT
      printf("Socket %i\n", destSocket);
      printf("Handle %s\n", destHandle);
#endif
      createNewMessage(flag, PDU_Len, message, newMessage);
      if(destSocket == 0){
#ifdef PRINT
         printf("User %s doesn't exist\n", destHandle);
#endif
         sendFlag7(sendSocket, destHandle);
      }
      else{
         safeSend(destSocket, newMessage, PDU_Len);
      }
   }
}

void sendE(int socketNum, Node** head){
   Node* node;
   sendFlag(socketNum, FLAG_9);
   node = findNode(*head, socketNum);
   *head = removeNode(*head, node);
   removeClient(socketNum);
}

void fillFlag11(char* message, int len){
   uint32_t length = htonl(len);
   message[0] = FLAG_11;
   message += 1;
   memcpy(message, &length, 4);
}

void sendFlag11(int socketNum, Node** head){
   int len;
   char flag11msg[FLAG_11_LEN - 2];
   char newflag11msg[FLAG_11_LEN];

   len = findListLength(*head);
   fillFlag11(flag11msg, len);

   // Create Flag_11 Message
   createNewMessage(FLAG_11, FLAG_11_LEN, flag11msg, newflag11msg);
   safeSend(socketNum, newflag11msg, FLAG_11_LEN);
}

// Fills the flag12msg buffer and returns an int representing the packet size
int fillFlag12(char* message, char* handle){
   int len = strlen(handle);
   message[0] = FLAG_12;
   message[1] = len;
   message += 2;

   memcpy(message, handle, len);
   return (FLAGBYTE + HANDLE_BYTE + len);
}

// Sends a flag 12 packet for each client in the head list
void sendFlag12(int socketNum, Node** head){
   int listlen;
   int i = 0;
   int sendlen = 0;
   Node* node;
   char flag12msg[MAX_HANDLE_SIZE + HANDLE_BYTE + FLAGBYTE];
   char newflag12msg[MAX_HANDLE_SIZE + HANDLE_BYTE + CHAT_HEADER_LEN];

   memset(flag12msg, 0, MAX_HANDLE_SIZE + HANDLE_BYTE + FLAGBYTE);

   listlen = findListLength(*head);

   for(i = 0; i < listlen; i ++){
#ifdef PRINT
      printf("list len %i\n", listlen);
#endif
      node = findNodeIndex(*head, i);

      sendlen = fillFlag12(flag12msg, node->handle);

      // Create Flag_12 Message
      createNewMessage(FLAG_12, sendlen + HEADER_LEN, flag12msg, newflag12msg);
      safeSend(socketNum, newflag12msg, sendlen + HEADER_LEN);
   }
}

void sendL(int socketNum, Node** head){
   sendFlag11(socketNum, head);
   sendFlag12(socketNum, head);
   sendFlag(socketNum, FLAG_13);
}

// Responds to an incoming packet received from the client
// databuf is the buffer of data after the Chat PDU Length
void packetResponse(uint8_t flag, char* databuf, uint16_t PDU_Len, int socketNum, Node** head)
{
   char* handle = NULL;
   uint8_t handleLen = 0;
   char strHandle[MAX_HANDLE_SIZE + 1];  // Used for the Handle, 101 becuase one is used for \0

   switch(flag)
   {
      case FLAG_1:
         handleLen = databuf[1];
#ifdef PRINT
         printf("handleLen = %i\n", handleLen);
#endif
         // Setting databuf to handle
         handle = databuf + sizeof(char)*2;
         // Copy handle data to strHandle and add a Null character
         createStrHandle(handle, handleLen, strHandle);
#ifdef PRINT
         printf("handle = %s\t handleLen = %i\n", handle, handleLen);
         printf("strHandle = %s\t strlen(strHandle) = %i\n", strHandle, (int)strlen(strHandle));
         printPacketData(flag, databuf, PDU_Len, socketNum, strHandle);
         printf("handle: %s, strHandle: %s\n", handle, strHandle);
#endif
         checkClient(socketNum, strHandle, head);
         break;
      case FLAG_4:
#ifdef PRINT
         printf("M Flag 4 received\n");
#endif
         sendB(databuf, head, PDU_Len, flag);
         break;
      case FLAG_5:
#ifdef PRINT
         printf("M Flag 5 received\n");
#endif
         sendM(databuf, head, PDU_Len, flag);
         break;
      case FLAG_8:
#ifdef PRINT
         printf("E Flag 8 received\n");
#endif
         sendE(socketNum, head);
         break;
      case FLAG_10:
#ifdef PRINT
         printf("L Flag 10 received\n");
#endif
         sendL(socketNum, head);
         break;
   }
}

char* recvFromClient(int clientSocket, Node** head)
{
	char buf[MAXBUF];
	int messageLen = 0;
   
   // Stuff Caleb Added
   char* databuf = NULL;
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

   databuf = buf + sizeof(char)*2;

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(clientSocket, databuf, PDU_Len - HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }
   flag = databuf[0];

#ifdef PRINT
   printf("PDU_LEN %i\n", PDU_Len);
   printf("flag %i\n", flag);
   printf("messageLen %i\n", messageLen);
#endif

   packetResponse(flag, databuf, PDU_Len, clientSocket, head);
   return databuf;
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
