// File used for shared functions between the server and the client
#include "shared.h"

// Prints out a default chat header packet
void printSentPacket(char* header, int socketNum){
   printf("Sending Packet Data to Socket %i:\n", socketNum);
   printf("\tChat PDULen:\t%i\n", ntohs(((uint16_t*)header)[0]));
   printf("\tFlag:\t\t%u\n", ((uint8_t*)header)[2]);
}

void safeSend(int socketNum, char* sendbuf, int sendlen){
	int sent = 0;           //actual amount of data sent/*

   printSentPacket(sendbuf, socketNum);

	sent = send(socketNum, sendbuf, sendlen, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
   }
   printf("Amount of data sent is: %d\n", sent);
}
