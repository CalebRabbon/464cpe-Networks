//
// Written Hugh Smith, Updated: April 2020
// Use at your own risk.  Feel free to copy, just leave my name in it.
//

#include "myClient.h"
//#define TEST
//#define PRINT

void checkArgs(int argc, char * argv[]);

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

#ifdef TEST
   testconvertCharType();
   testfindType();
   teststepString();
   testfindStr();
   testconvertStrToInt();
   testgetHandleNum();
   testfillSender();
   testisNumber();
   testfindFirstHandle();
   testfillHandle();
   testfillText();
   testproc_M();
   testproc_E();
#endif

#ifndef TEST
   int socketNum = 0;         //socket descriptor
   char loginBuff[MAX_LOGIN_SIZE];

   setupPollSet();

   checkArgs(argc, argv);

   // set up the TCP Client socket
   socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);

   // Login to the server with the handle
   if(strlen(argv[1]) > 99){
      printf("Invalid handle, handle longer than 100 characters %s\n", argv[1]);
      exit(1);
   }
   login(argv[1], socketNum, loginBuff);

   // Runs the client
   runClient(socketNum, argv[1]);

   close(socketNum);
#endif

   return 0;
}

void runClient(int serverSocket, char* clientHandle)
{
	int socketToProcess = 0;
	int flag = 0;
	
	addToPollSet(serverSocket);
	addToPollSet(STDIN_FILENO);

	while (1)
	{
      printf("$:");
      fflush(stdout);
		if ((socketToProcess = pollCall(POLL_WAIT_FOREVER)) != -1)
		{
			if (socketToProcess == STDIN_FILENO)
			{
            sendToServer(serverSocket, clientHandle);
			}
			else
			{
            // Receiving from the server
#ifdef PRINT
            printf(" Receiving from server\n");
#endif
            flag = ackFromServer(serverSocket, clientHandle);
            // Process the %L without STDIN interruption
            if(flag == 11){
               while(flag != 13){
                  flag = ackFromServer(serverSocket, clientHandle);
               }
            }
#ifdef PRINT
            printf(" Done Receiving from server\n");
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

void flag11Response(char* databuf){
   uint32_t clientNum;
   databuf += sizeof(char);
   clientNum = ntohl(*((uint32_t*)databuf));
   printf("\nNumber of clients: %i\n", clientNum);
}

void flag12Response(char* databuf){
   char sendHandle[MAX_HANDLE_SIZE + 1];
   findSender(databuf, sendHandle);
   printf("\t%s\n", sendHandle);
}

int findTextLen4(uint16_t pdulen, int handleLen){
   return pdulen - handleLen - CHAT_HEADER_LEN - HANDLE_BYTE;
}

// Responds to a flag 4 from the server
void flag4resp(char* sendHandle, char* databuf, uint16_t pdulen){
   char text[MAX_SEND_TXT];
   int textlen;

   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(text, 0, MAX_SEND_TXT);
   findSender(databuf, sendHandle);
#ifdef PRINT
   printf("sendHandle %s\n", sendHandle);
   printf("databuf[0] flag %i\n", databuf[0]);
   printf("databuf[1] sendhandle len%i\n", databuf[1]);
   printf("databuf[2] first letter %c\n", databuf[2]);
   printf("databuf[3] letter %c\n", databuf[3]);
   printf("databuf[4] letter %c\n", databuf[4]);
   printf("databuf[5] letter %c\n", databuf[5]);
   printf("databuf[6] letter %c\n", databuf[6]);
   printf("databuf[7] letter %c\n", databuf[7]);
   printf("databuf[8] letter %c\n", databuf[8]);
#endif
   // Point databuf at string
   databuf +=  FLAGBYTE + SEND_HANDLE_BYTE + strlen(sendHandle);
   textlen = findTextLen4(pdulen, strlen(sendHandle));
   memcpy(text, databuf, textlen);
   text[textlen] = '\0';
#ifdef PRINT
   printf("text %c\n", databuf[0]);
   printf("strlen(sendhandle) %i\n", (int)strlen(sendHandle));
#endif
   printf("\n%s: %s\n", sendHandle, text);
}

// Responds to an incoming packet received from the server
// dataBuf is the buffer of data after the Chat PDU Length
// Returns the flag that was processed
int packetResponse(uint8_t flag, char* databuf, char* handle, uint16_t pdulen, int socketNum)
{
   char sendHandle[MAX_HANDLE_SIZE + 1];
   char text[MAX_SEND_TXT];
   /*
   char* handle = NULL;
   uint8_t handleLen = 0;
   char strHandle[MAX_HANDLE_SIZE + 1];  // Used for the Handle, 101 becuase one is used for \0
   */
#ifdef PRINT
   printServerMsg(flag, pdulen - 2, pdulen);
#endif
   switch(flag)
   {
      case FLAG_3:
         printf("Handle already in use: %s\n", handle);
         exit(0);
         break;
      case FLAG_4:
         flag4resp(sendHandle,  databuf, pdulen);
         break;
      case FLAG_5:
         memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);
         memset(text, 0, MAX_SEND_TXT);
         findTextLen(databuf, pdulen);
         findSender(databuf, sendHandle);
         getText(databuf, text, pdulen);
         printf("\n%s: %s\n", sendHandle, text);
         break;
      case FLAG_7:
         findSender(databuf, sendHandle);
         printf("\nClient with handle %s does not exist.\n", sendHandle);
         break;
      case FLAG_9:
         exit(0);
         break;
      case FLAG_11:
         flag11Response(databuf);
         break;
      case FLAG_12:
         flag12Response(databuf);
         break;
   }
   return flag;
}


// Receives a message from the server 
// Returns an int representing the flag that was processed
int ackFromServer(int socketNum, char* handle)
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

   return packetResponse(flag, dataBuf, handle, PDU_Len, socketNum);
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

   ackFromServer(socketNum, handle);
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
#ifdef PRINT
   printf("Amount of login data sent is: %d\n", sent);
#endif
}

void sendToServer(int socketNum, char* sendHandle)
{
	char stdbuf[MAXBUF];    //data from user input
   // Add 114 to sendbuf to account for 10 bytes of handles 1 byte for number of
   // destinations, 3 bytes for Chat-Header, 100 bytes for max sender handle
	char sendbuf[MAX_SEND_LEN];   //data sent to server
	int sendlen = 0;         //amount of data to send

	memset(stdbuf, '\0', MAXBUF);

	if(getFromStdin(stdbuf) == -1){
      fprintf(stderr,"Error: Input is too large\n");
   }

   // Process the stdbuf and organize it into a packet inside the sendbuf
   sendlen = procStdin(stdbuf, sendbuf, sendHandle);
	
	//printf("read: %s string len: %d (including null)\n", stdbuf, sendlen);

   safeSend(socketNum, sendbuf, sendlen);
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
