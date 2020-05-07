#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>

char* stepString(char* stdbuf);
int findType(char* stdbuf);
int convertCharType(char type);
void findStr(char* stdbuf, char* str, int len);
int8_t convertStrToInt(char* str);
int getHandleNum(char* stdbuf, char* sendbuf, char* sendHandle);
char* fillSender(char* sendbuf, char* sendHandle);
int isNumber(char* str);
char* findFirstHandle(char* stdbuf);
char* fillHandle(char* sendbuf, char* handle);
int proc_M(char* stdbuf, char* sendbuf, char* sendHandle);
char* fillText(char* sendbuf, char* text);
int procStdin(char* stdbuf, char* sendbuf, char* sendHandle);
int proc_E(char* sendbuf);
int getFromStdin(char * stdbuf, char * prompt);
void fillChatHeader(char* sendbuf, int flag, int pduLen);

#endif
