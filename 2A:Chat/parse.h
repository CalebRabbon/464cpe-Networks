#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdint.h>

char* stepString(char* stdbuf);
int findType(char* stdbuf);
int convertCharType(char type);
void findStr(char* stdbuf, char* str, int len);
int8_t convertStrToInt(char* str);
int getHandleNum(char* stdbuf, char* sendbuf, char* sendHandle);

#endif
