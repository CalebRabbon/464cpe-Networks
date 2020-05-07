#include "unitTest.h"
#include "myClient.h"
#include "recvparse.h"
#include "macros.h"

// Finds the sender of a message
void findSender(char* message, char* sendHandle){
   uint8_t sendHandleLen;
   sendHandleLen = message[1];

   message += 2;

   memcpy(sendHandle, message, sendHandleLen);

   // Add NULL character
   sendHandle[sendHandleLen] = '\0';
}

// Finds the number of destination handles from a received message
int findNumHandles(char* message, char* sendHandle){
   int val = 0;
   int flagbyte = 1;

   val = message[flagbyte + SEND_HANDLE_BYTE  + strlen(sendHandle)];

   return val;
}

// Gets to the next destHandle if you are on a dest handle
char* nextHandle(char* message){
   int len = 0;
   len = message[0];
   message += (len + 1);   // Add one to get to next handle
   return message;
}

// Finds the handle and returns the next byte after the handle in the char* message
char* getHandle(char* message, char* destHandle){
   int len = 0;
   len = message[0];
   message += 1;
   memcpy(destHandle, message, len);
   destHandle[len] = '\0';

   message += len;
   return message;
}

// Takes an int representing which destination handle and finds the corresponding destination handle
// The int is 1 indexed
// Returns the address right after the handle
char* findDestHandle(char* message, int num, char* destHandle){
   char sendHandle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;
   int i = 0;

   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);

   findSender(message, sendHandle);

   handleNum = findNumHandles(message, sendHandle);

   if(num > handleNum){
      printf("Handle number %i is too large\n", num);
      destHandle = NULL;
      return NULL;
   }

   // First Dest Handle Length
   message += FLAGBYTE + SEND_HANDLE_BYTE  + strlen(sendHandle) + NUM_DEST_BYTE;

   for(i = 1; i < num; i++){
      message = nextHandle(message);
   }

   return getHandle(message, destHandle);
}

// Returns a char* to the start of the text portion of the %m message
char* findTextStart(char* message){
   char* textStart = 0;
   char destHandle[MAX_HANDLE_SIZE + 1];
   char sendHandle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;

   memset(destHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);

   findSender(message, sendHandle);

   handleNum = findNumHandles(message, sendHandle);

   textStart = findDestHandle(message, handleNum, destHandle);

   return textStart;
}

// Finds the length of the text message
int findTextLen(char* message, int pduLen){
   char destHandle[MAX_HANDLE_SIZE + 1];
   char sendHandle[MAX_HANDLE_SIZE + 1];
   uintptr_t textStart = 0;
   uintptr_t dataStart = 0;
   uintptr_t temp = 0;
   int handleNum = 0;
   int textLen = 0;

   memset(destHandle, 0, MAX_HANDLE_SIZE + 1);
   memset(sendHandle, 0, MAX_HANDLE_SIZE + 1);

   dataStart = (uintptr_t)((void*)message);

   findSender(message, sendHandle);
   /*
   printf("sender %s\n", sendHandle);
   */

   handleNum = findNumHandles(message, sendHandle);
   /*
   printf("handleNum %i\n", handleNum);
   */

   textStart = (uintptr_t)(findDestHandle(message, handleNum, destHandle));

   temp = textStart - dataStart;
   textLen = pduLen - temp - CHAT_HEADER_LEN + FLAGBYTE;
   /*
   printf("temp %lu\n", temp);
   printf("pduLen %i\n", pduLen);
   printf("textStart %lu\n", textStart);
   printf("dataStart %lu\n", dataStart);
   printf("CHAT_HEADER_LEN %i\n", CHAT_HEADER_LEN);
   printf("textLen %i\n", textLen);
   */

   return textLen;
}

// Fills in the text buffer with the text from the message and appends a '\0' to
// the end
void getText(char* message, char* text, int pdulen){
   int textlen = findTextLen(message, pdulen);
   char* textStart = findTextStart(message);

   memcpy(text, textStart, textlen);

   text[textlen] = '\0';
}


