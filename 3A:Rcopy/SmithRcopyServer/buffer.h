#ifndef __BUFFER_H__
#define __BUFFER_H__

#define HEADERLEN 7
#define EMPTY 0
#define FULL 1

typedef struct windowelement WindowElement;
struct windowelement{
   uint8_t data_buf[MAX_LEN];
   int32_t data_len; // Length of the data in the data_buffer
   int flag; // Either EMPTY or FULL
};

WindowElement* createWindow(int32_t windowSize);
void createWindowElement(WindowElement* win, uint8_t* data_buf, int32_t data_len);
void addElement(uint32_t seqNum, WindowElement element, WindowElement* window, int windowSize);
void printWindow(WindowElement* window, int32_t windowSize);
void deleteElement(uint32_t seqNum, WindowElement* window, int windowSize);
int isEmptySpot(int seqNum, WindowElement* window, int windowSize);
int isWindowEmpty(WindowElement* window, int windowSize);
char* convertToString(char* string, uint8_t *databuf, uint32_t data_len);
void getElement(int seqNum, WindowElement* newElement, WindowElement* window, int windowSize);

#endif
