// Code to create a user level packet
// By Caleb Rabbon 5/24/2020

#include "packet.h"
#include "checksum.h"

//#define PRINT

#define MAX_BUFFER 80

uint8_t * createPDU(uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int dataLen)
{
   static uint8_t pduBuffer[MAX_BUFFER];
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

void outputPDU(uint8_t * pduBuffer, int pduLength)
{
   unsigned short checksum = 0;
   checksum = in_cksum((unsigned short*)pduBuffer, pduLength);

   printf("sequenceNumber: %i\n", ntohl(*((uint32_t*)pduBuffer)));
   printf("Checksum Value: %i\n", ((uint16_t*)pduBuffer)[2]);
   printf("Checksum of PDU: %i\n", checksum);
   printf("flag: %i\n", (uint8_t)pduBuffer[6]);
   printf("Payload: %s\n", pduBuffer + HEADERLEN);
   printf("Payload Length: %i\n", pduLength - HEADERLEN);
}
