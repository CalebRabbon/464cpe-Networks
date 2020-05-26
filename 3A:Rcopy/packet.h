
#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HEADERLEN 7

uint8_t * createPDU(uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int dataLen);

void outputPDU(uint8_t * pduBuffer, int pduLength);

#endif
