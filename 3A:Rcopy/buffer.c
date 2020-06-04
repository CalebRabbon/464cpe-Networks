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
   printf("\n----------------- WINDOW START ------------------\n");
   for (i = 0; i < windowSize; i ++){
      printWindowElement(window, i);
   }
   printf("----------------- WINDOW END ------------------\n\n");
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

// Adds overwrites the data at the index in the window.
// The index is a circular queue
void addElement(uint32_t seqNum, WindowElement element, WindowElement* window, int windowSize){
   //uint32_t seqNum = getSeqNum(element);
   int winIndex = seqNum % windowSize;

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

// Returns FULL if the whole window is full
// else returns Empty
int isWindowFull(WindowElement* window, int windowSize){
   int i = 0;

   for (i = 0; i < windowSize; i ++){
      if(isEmptySpot(i, window, windowSize) == EMPTY){
         return EMPTY;
      }
   }
   return FULL;
}

// Gets the window element at the given seqNum and fills in the values to the
// passed in newElement
void getElement(int seqNum, WindowElement* newElement, WindowElement* window, int windowSize){
   int winIndex = seqNum % windowSize;
   window += winIndex;

   memcpy(newElement->data_buf, window->data_buf, window->data_len);
   newElement->flag = window->flag;
   newElement->data_len = window->data_len;
}
