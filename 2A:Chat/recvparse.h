#ifndef __RECVPARSE_H__
#define __RECVPARSE_H__

void findSender(char* message, char* sendHandle);
int findNumHandles(char* message, char* sendHandle);
char* nextHandle(char* message);
char* findDestHandle(char* message, int num, char* destHandle);
int findTextLen(char* message, int pduLen);
char* findTextStart(char* message);
void getText(char* message, char* text, int pdulen);

#endif
