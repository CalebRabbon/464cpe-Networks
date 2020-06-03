#ifndef __BUFFER_H__
#define __BUFFER_H__

#define HEADERLEN 7
#define EMPTY 0
#define FULL 1

typedef struct windowelement WindowElement;
struct windowelement{
   uint8_t pdu[MAX_LEN];
   int flag; // Either EMPTY or FULL
};

void createWindowElement(WindowElement* win, uint8_t* pdu, int32_t pdu_size);
void addElement(WindowElement element, WindowElement* window, int windowSize);
void printWindow(WindowElement* window, int32_t windowSize);
void deleteElement(WindowElement element, WindowElement* window, int windowSize);

#endif
