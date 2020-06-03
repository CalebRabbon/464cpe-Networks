// Buffer code for sliding window
// Written by Caleb Rabbon 6/1/2020

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
#include "srej.h"
#include "buffer.h"

/*
uint8_t * createPDU(uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int dataLen)
{
   static uint8_t pduBuffer[MAX_LEN];

   unsigned short checksum = 0;

#ifdef PRINT
   printf("Inside SequencNumber: %i\n", sequenceNumber);
#endif
   (((uint32_t*)pduBuffer)[0]) = htonl(sequenceNumber);

   // zero out the checksum
   (((uint16_t*)pduBuffer)[2]) = 0;

   (((uint8_t*)pduBuffer)[6]) = flag;

   memcpy(&(((uint8_t*)pduBuffer)[7]), payload, dataLen);

   checksum = in_cksum((unsigned short*)pduBuffer, dataLen + HEADERLEN);

#ifdef PRINT
   printf("Inside Checksum: %hu\n", checksum);
#endif

   memcpy(((uint16_t*)pduBuffer) + 2, &checksum, sizeof(unsigned short));

   // Checking to see if the resulting checksum is 0 after you take the checksum
   // of the whole PDU
#ifdef PRINT
   checksum = in_cksum((unsigned short*)pduBuffer, dataLen + HEADERLEN);
   printf("resulting checksum 0 = %i\n", checksum);
#endif

   return pduBuffer;
}

void printPDU(uint8_t * pduBuffer, int pduLength)
{
   unsigned short checksum = 0;
   checksum = in_cksum((unsigned short*)pduBuffer, pduLength);

   printf("\tsequenceNumber: %i\n", ntohl(*((uint32_t*)pduBuffer)));
   printf("\tChecksum Value: %i\n", ((uint16_t*)pduBuffer)[2]);
   printf("\tChecksum of PDU: %i\n", checksum);
   printf("\tflag: %i\n", (uint8_t)pduBuffer[6]);
   printf("\tPayload: %s\n", pduBuffer + HEADERLEN);
   printf("\tPayload Length: %i\n", pduLength - HEADERLEN);
}
*/

void printAvailability(int flag){
   if (flag == EMPTY){
      printf("Window Availability: EMPTY\n");
   }
   else if (flag == FULL){
      printf("Window Availability: FULL\n");
   }
   else{
      printf("Window Availability: UNKNOWN ****SHOULD NOT BE HERE**** \n");
   }
}

char* convertToString(char* string, uint8_t *databuf, uint32_t data_len){
   if(data_len == 0){
      return NULL;
   }
   memcpy(string, databuf, data_len);
   string[data_len] = '\0';
   return string;
}

void printWindowElement(WindowElement* window, int i){
   window += i;
   char str[MAX_LEN];

   printf("Index: %i\n", i);
   printf("Window Buffer: %s\n", convertToString(str, window->data_buf, window->data_len));
   printAvailability(window->flag);
   printf("\n");
}

void printWindow(WindowElement* window, int32_t windowSize){
   int i = 0;
   for (i = 0; i < windowSize; i ++){
      printWindowElement(window, i);
   }
}

uint8_t getPDUFlag(uint8_t* pdu){
   return (uint8_t)pdu[6];
}

// Creates a window element by filling in the WindowElement*
void createWindowElement(WindowElement* win, uint8_t* data_buf, int32_t data_len){
   memcpy(win->data_buf, data_buf, data_len);
   win->flag = FULL;
   win->data_len = data_len;
}

// Caleb's malloc to stop errors
void* cMalloc(size_t size){
   void* val = malloc(size);

   if(val == NULL){
      perror("Error with malloc");
      exit(EXIT_FAILURE);
   }
   return val;
}

// Creates a buffer with windowSize
WindowElement* createWindow(int32_t windowSize){
   WindowElement* window;
   window = cMalloc(windowSize * sizeof(WindowElement));
   memset(window, 0, windowSize * sizeof(WindowElement));

   return window;
}

/*
int getSeqNum(WindowElement element){
   uint32_t seqNum = ntohl(((uint32_t*)(element.pdu))[0]);
   printf("seqNum %i\n", seqNum);
   return seqNum;
}
*/

// Adds overwrites the data at the index in the window.
// The index is a circular queue
void addElement(uint32_t seqNum, WindowElement element, WindowElement* window, int windowSize){
   //uint32_t seqNum = getSeqNum(element);
   int winIndex = seqNum % windowSize;
   printf("winIndex = %i\n", winIndex);

   window += winIndex;
   memcpy(window->data_buf, element.data_buf, MAX_LEN);
   window->flag = element.flag;
   window->data_len = element.data_len;
}

void deleteElement(uint32_t seqNum, WindowElement* window, int windowSize){
   //uint32_t seqNum = getSeqNum(element);
   int winIndex = seqNum % windowSize;

   window += winIndex;
   window->flag = EMPTY;
}

// Returns EMPTY if there is nothing in the buffer at that sequence number
// else returns FULL
int isEmptySpot(int seqNum, WindowElement* window, int windowSize){
   int winIndex = seqNum % windowSize;
   window += winIndex;
   if(window->flag == EMPTY){
      return EMPTY;
   }
   else {
      return FULL;
   }
}

// Returns EMPTY if the whole window is empty
// else returns FULL
int isWindowEmpty(WindowElement* window, int windowSize){
   int i = 0;

   for (i = 0; i < windowSize; i ++){
      if(isEmptySpot(i, window, windowSize) == FULL){
         return FULL;
      }
   }
   return EMPTY;
}

int getElement(int seqNum, WindowElement* window, int windowSize){
   int winIndex = seqNum % windowSize;
   window += winIndex;

   if(window->flag == EMPTY){
      printf("EMPTY");
      return EMPTY;
   }
   else {
      return FULL;
   }
}

int main ( int argc, char *argv[] )
{
   int windowSize = 5;
   uint8_t data_buf[MAX_LEN];
   WindowElement* window = createWindow(windowSize);
   WindowElement element;
   char* payload = "hello";

   printf("ORGININAL\n");
   printf("length = %i\n", (int)(HEADERLEN + strlen(payload)));

   memcpy(data_buf, "HELLO WORLD", 11);

   //pdu = createPDU(1, 99, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, data_buf, (int)sizeof("HELLO WORLD"));
   printWindow(window, windowSize);
   addElement(1, element, window, windowSize);
   printWindow(window, windowSize);

   deleteElement(1, window, windowSize);
   printWindow(window, windowSize);

   /*
   pdu = createPDU(2, 22, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, pdu, HEADERLEN + strlen(payload));
   addElement(element, window, windowSize);
   //printWindow(window, windowSize);

   pdu = createPDU(3, 33, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, pdu, HEADERLEN + strlen(payload));
   addElement(element, window, windowSize);
   //printWindow(window, windowSize);

   pdu = createPDU(4, 44, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, pdu, HEADERLEN + strlen(payload));
   addElement(element, window, windowSize);

   pdu = createPDU(5, 55, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, pdu, HEADERLEN + strlen(payload));
   addElement(element, window, windowSize);
   printWindow(window, windowSize);

   pdu = createPDU(6, 66, (uint8_t*)payload, strlen(payload));
   createWindowElement(&element, pdu, HEADERLEN + strlen(payload));
   addElement(element, window, windowSize);
   printWindow(window, windowSize);
   */

   return 0;
}
