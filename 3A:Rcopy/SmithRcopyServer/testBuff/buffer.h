#ifndef __NETWORKS_H__
#define __NETWORKS_H__


void createWindowElement(WindowElement* win, uint8_t* pdu, int32_t pdu_size);
void addElement(WindowElement element, WindowElement* window, int windowSize);
void printWindow(WindowElement* window, int32_t windowSize);
void deleteElement(WindowElement element, WindowElement* window, int windowSize);

#endif
