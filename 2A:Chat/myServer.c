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


void processSockets(int mainServerSocket);
void recvFromClient(int clientSocket);
void addNewClient(int mainServerSocket);
void removeClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int main(int argc, char *argv[])
{
   Node* head = makeLinkedList();
   Node* item = makeNode("Caleb", 4);
   Node* item2 = makeNode("Caleb2", 2);
   Node* C1 = makeNode("C1", 4);
   Node* C2 = makeNode("C2", 4);
   Node* C3 = makeNode("C3", 4);

   head = addNode(head, item);
   printLinkedList(head);
   head = addNode(head, item2);
   printLinkedList(head);
   head = addNode(head, C1);
   printLinkedList(head);
   head = addNode(head, C2);
   printLinkedList(head);
   head = addNode(head, C3);
   printLinkedList(head);
   printf("Removing Item\n");
   head = removeNode(head, C1);
   printLinkedList(head);


   /*
	int mainServerSocket = 0;   //socket descriptor for the server socket
	int portNumber = 0;
	
	setupPollSet();
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	mainServerSocket = tcpServerSetup(portNumber);

	// Main control process (clients and accept())
	processSockets(mainServerSocket);
	
	// close the socket - never gets here but nice thought
	close(mainServerSocket);
   */
	
	return 0;
}

void processSockets(int mainServerSocket)
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
				recvFromClient(socketToProcess);
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

   // Fill in the buffer up to the MAX_HANDLE_SIZE then add the Null
   if (handleLen <= MAX_HANDLE_SIZE){
      for (i = 0; i < handleLen; i ++)
         strHandle[i] = handle[i];
      strHandle[handleLen + 1] = '\0';
   }
   else {
      for (i = 0; i < MAX_HANDLE_SIZE; i ++)
         strHandle[i] = handle[i];
      strHandle[MAX_HANDLE_SIZE] = '\0';
      fprintf(stderr,"Your handle is more than 100 characters, truncating the remaining characters\n");
   }
   printf("STRING: %s\n", strHandle);
}

// Returns a header with a PDU Length of 3 and the designated flag
void createDefaultChatHeader(char* header, int flag){
   (((uint16_t*)header)[0]) = htons(DEFAULT_PDULEN);
   (((uint8_t*)header)[2]) = flag;
}

// Prints out a default chat header packet
void printPacket(char* header){
   printf("Chat PDULen:\t%i\n", ntohs(((uint16_t*)header)[0]));
   printf("Flag:\t\t%u\n", ((uint8_t*)header)[2]);
}

// Sends the packet to the socket number with sendLen
void sendPacket(int socketNum, char* packet, uint8_t sendLen){
	int sent = 0;            //actual amount of data sent/* get the data and send it   */

   printPacket(packet);

	sent =  send(socketNum, packet, sendLen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
}

// Sends a designated flag
void sendFlag(int socketNum, uint8_t flag){
   char header[CHAT_HEADER_LEN];
   createDefaultChatHeader(header, flag);
   sendPacket(socketNum, header, DEFAULT_PDULEN);
}

// Adds the handle to the list
void addToList(char* strHandle){

}

// Sends an error flag of 3 saying the handle is taken
void sendFlag3(){
   return;
}

// Checks to see if the strHandle is taken
int taken(char* strHandle){
   return 0;
}

// Checks to see if the new handle is taken. If it is send flag 3, if not add
// to list and send flag 2
void addClient(int socketNum, char* strHandle){
   if(taken(strHandle)){
      sendFlag3();
   }
   else
   {
      addToList(strHandle);
      sendFlag(socketNum, FLAG_2);
   }
}



// Responds to an incoming packet received from the client
// dataBuf is the buffer of data after the Chat PDU Length
void packetResponse(uint8_t flag, char* dataBuf, int messageLen, uint16_t PDU_Len, int socketNum)
{
   char* handle = NULL;
   uint8_t handleLen = 0;
   char strHandle[MAX_HANDLE_SIZE + 1];  // Used for the Handle, 101 becuase one is used for \0
   switch(flag)
   {
      case FLAG_1:
         printf("Flag: %i\n", flag);
         printf("Handle Len: %i\n", dataBuf[1]);
         handleLen = dataBuf[1];

         // Setting dataBuf to handle
         handle = dataBuf + sizeof(char)*2;

         printText(handle, handleLen);

         printf("Message Len: %d, PDU_Len: %d\n", messageLen, PDU_Len);

         createStrHandle(handle, handleLen, strHandle);
         printf("handle: %s, strHandle: %s\n", handle, strHandle);

         addClient(socketNum, strHandle);


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

void recvFromClient(int clientSocket)
{
	char buf[MAXBUF];
	int messageLen = 0;
   
   // Stuff Caleb Added
   char* dataBuf = NULL;
   uint16_t PDU_Len = 0;
   uint8_t flag = 0;
		
	//now get the data from the clientSocket (message includes null)
   if ((messageLen = recv(clientSocket, buf, HEADER_LEN, MSG_WAITALL)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
	
	if (messageLen == 0)
	{
		// recv() 0 bytes so client is gone
		removeClient(clientSocket);
	}

   PDU_Len = ntohs(((uint16_t*)buf)[0]);

   dataBuf = buf + sizeof(char)*2;

   //now get the data from the client_socket (message includes null)
   if ((messageLen = recv(clientSocket, dataBuf, PDU_Len - HEADER_LEN, MSG_WAITALL)) < 0)
   {
      perror("recv call");
      exit(-1);
   }
   flag = dataBuf[0];

   packetResponse(flag, dataBuf, messageLen, PDU_Len, clientSocket);
//   return dataBuf;
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

