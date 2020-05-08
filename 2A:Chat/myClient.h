#ifndef __MYCLIENT_H__
#define __MYCLIENT_H__

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

#include "networks.h"
#include "flags.h"
#include "pollLib.h"
#include "macros.h"
#include "unitTest.h"
#include "parse.h"
#include "recvparse.h"
#include "test.h"
#include "shared.h"

void sendToServer(int socketNum, char* sendHandle);
void login(char* handle, int socketNum, char* loginBuff);
void sendLogin(int socketNum, char* loginBuff, uint16_t sendLen);
void runClient(int serverSocket, char* clientHandle);
int ackFromServer(int socketNum, char* handle);

#endif
