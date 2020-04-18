/* Caleb Rabbon, crabbon@calpoly.edu */

#include "trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <pcap/pcap.h>

#define ETHER_ADDR_SIZE 6
#define REQUEST 1
#define REPLY 2
#define ARP 0x0806
#define IP  0x0800

/* Number of bytes the ARP Opcode is from the beginning of the frame */
#define OPCODE_OFFSET 20

/* ARP_MAC_OFFSET is the number of bytes between the Sender MAC address and the
 * Target MAC address in the ARP Header */
#define ARP_MAC_OFFSET 10

/* ARP_IP_OFFSET is the number of bytes between the Sender IP address and the
 * Target IP address in the ARP Header */
#define ARP_IP_OFFSET 10

/* Number of bytes the IP Header Length is from the beginning of the frame */
#define IP_HEADERLEN_OFFSET 14
#define TCP 6
#define ICMP 1
#define UDP 17

#define ICMP_OFFSET 34   /* ICMP offset */
#define PING_REQUEST 8
#define PING_REPLY 0

#define UDP_OFFSET 34    /* UDP offset */

/* Flags for TCP Header */
#define ACK_FLG 0x0010
#define SYN_FLG 0x0002
#define RST_FLG 0x0004
#define FIN_FLG 0x0001

void printPacketNumLen(int number, int frame){
   printf("\nPacket number: %i  Frame Len: %i\n\n", number, frame);
   printf("\tEthernet Header\n");
}

/* Fills in the passed in struct ethHeader with data from the void* pkt_data */
void createEthHeader(struct ethHeader* eHeader, void* pkt_data){
   int i = 0;

   for(i = 0; i < ETHER_ADDR_SIZE; i ++){
      eHeader->dst.ether_addr_octet[i] = ((uint8_t*)(pkt_data))[i];
      eHeader->src.ether_addr_octet[i] =
         ((uint8_t*)(pkt_data))[i+ETHER_ADDR_SIZE];
   }

   pkt_data += sizeof(uint8_t) * 2 * ETHER_ADDR_SIZE;
   eHeader->type = ntohs(*((uint16_t*)pkt_data));
}

/* uint16_t -> char*
 * Takes in a uint16_t and returns a string representing the ethernet type
 */
char* findEthType(uint16_t i){
   if(i == 0x0800){
      return "IP";
   }
   if(i == 0x0806){
      return "ARP";
   }
   return "Unknown";
}


/* struct ethHeader* -> void
 * Prints out the ethHeader destination address in IPv4 numbers and colon
 * notation
 */
void printEthHeader(struct ethHeader* eHeader){
   char* s;
   s = ether_ntoa(&(eHeader->dst));
   printf("\t\tDest MAC: %s\n", s);
   s = ether_ntoa(&(eHeader->src));
   printf("\t\tSource MAC: %s\n", s);
   printf("\t\tType: %s\n\n", findEthType(eHeader->type));

}

/* Returns an int representing if the next packet was successful */
int findNextHeader(pcap_t* pcap_file, struct pcap_pkthdr** pkt_header, const 
      u_char** pkt_data){

   int pcap_next_ret = 1;

   pcap_next_ret = pcap_next_ex(pcap_file, pkt_header, pkt_data);
   /* From the man pages about pcap_next_ex return value
    *  1 = packet was read without problems
    *  0 = packets are being read from a live capture and the timeout expired
    * -1 = Error occured
    * -2 = Packets being read from a ``savefile'' and no more packets to read
    */
   return pcap_next_ret;
}

/* Finds, creates, and outputs the ethernet header */
/* Takes in a struct ethHeader* , struct pcacp_pkthdr* , void* and outputs and
 * int representing the next type of packet following the Ethernet header */
int outputEthHeader(struct pcap_pkthdr* pkt_header, void* pkt_data, int i){
   struct ethHeader eHeader;

   createEthHeader(&eHeader, pkt_data);
   printPacketNumLen(i, (pkt_header)->len);
   printEthHeader(&eHeader);

   if(eHeader.type == ARP)
      return ARP;
   else if(eHeader.type == IP)
      return IP;
   else
      return 0;
}

/* Converts an Arp header opcode to a string */
/* Takes in a uint16_t and returns a char* */
char* opCodeToStr(uint16_t opcode){
   if(opcode == REQUEST)
      return "Request";
   else if(opcode == REPLY)
      return "Reply";
   else
      return "Unknown";
}

/* Prints out the arpHeader */
void printArpHeader(struct arpHeader* aHeader){
   printf("\tARP header\n");
   printf("\t\tOpcode: %s\n", opCodeToStr(aHeader->opcode));
   printf("\t\tSender MAC: %s\n", ether_ntoa(&(aHeader->senderMac)));
   printf("\t\tSender IP: %i.", aHeader->senderIP[0]);
   printf("%i.",  aHeader->senderIP[1]);
   printf("%i.",  aHeader->senderIP[2]);
   printf("%i\n", aHeader->senderIP[3]);
   printf("\t\tTarget MAC: %s\n", ether_ntoa(&(aHeader->targetMac)));
   printf("\t\tTarget IP: %i.", aHeader->targetIP[0]);
   printf("%i.",  aHeader->targetIP[1]);
   printf("%i.",  aHeader->targetIP[2]);
   printf("%i\n\n", aHeader->targetIP[3]);
}

/* Fills in the passed in struct arpHeader pointer with data from the 
 * void* pkt_data and returns void */
void createArpHeader(struct arpHeader* aHeader, void* pkt_data){
   int i = 0;

   /* Moving the pkt_data pointer to the Opcode */
   pkt_data += sizeof(uint8_t) * OPCODE_OFFSET;
   aHeader->opcode = ntohs(*((uint16_t*)pkt_data));

   /* Move the pkt_data pointer to senderMac */
   pkt_data += sizeof((*aHeader).opcode);

   for(i = 0; i < ETHER_ADDR_SIZE; i ++){
      aHeader->senderMac.ether_addr_octet[i] = ((uint8_t*)(pkt_data))[i];
      aHeader->targetMac.ether_addr_octet[i] =
         ((uint8_t*)(pkt_data))[i+ARP_IP_OFFSET];
   }

   /* Move the pkt_data pointer to sender IP address */
   pkt_data += sizeof((*aHeader).senderMac);

   for(i = 0; i < IP_ADDR_SIZE; i ++){
      aHeader->senderIP[i] = ((uint8_t*)(pkt_data))[i];
      aHeader->targetIP[i] = ((uint8_t*)(pkt_data))[i+ARP_IP_OFFSET];
   }
}

/* Outputs the ARP Header given a void* to the pkt_data */
void outputArpHeader(void* pkt_data){
   struct arpHeader aHeader;

   createArpHeader(&aHeader, pkt_data);
   printArpHeader(&aHeader);
}

/* Fills in the passed in struct ipHeader pointer with data from the 
 * void* pkt_data and returns void */
void createIpHeader(struct ipHeader* iHeader, void* pkt_data){
   int i = 0;

   /* Moving the pkt_data pointer to the IP's header length */
   pkt_data += sizeof(uint8_t) * IP_HEADERLEN_OFFSET;
   iHeader->headerLen = *((uint8_t*)pkt_data);

   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   iHeader->tos = *((uint8_t*)pkt_data);

   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   iHeader->pduLen = ntohs(*((uint16_t*)pkt_data));

   /* Moving the pkt_data pointer to the ttl which is 6 bytes after pduLen */
   pkt_data += sizeof(uint8_t) * 6;
   iHeader->ttl= *((uint8_t*)pkt_data);

   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   iHeader->protocol = *((uint8_t*)pkt_data);

   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   /* Since the bytes need to go from network to computer order I swapped the
    * checksum values. Thus the first value goes to checksum[1] and NOT
    * checksum[0] */
   iHeader->checksum[1] = *((uint8_t*)pkt_data);
   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   iHeader->checksum[0] = *((uint8_t*)pkt_data);

   /* Moving the pkt_data pointer one byte forward */
   pkt_data += sizeof(uint8_t);
   for(i = 0; i < IP_ADDR_SIZE; i ++){
      iHeader->senderIP[i] = ((uint8_t*)(pkt_data))[i];
      iHeader->destIP[i] = ((uint8_t*)(pkt_data))[i+IP_ADDR_SIZE];
   }
}

/* Returns the associated protocol name given a struct ipheader* */
/* TCP = 6, ICMP = 1, UDP = 17 */
char* toWord(struct ipHeader* iHeader){
   if(iHeader->protocol == TCP)
      return "TCP";
   else if(iHeader->protocol == ICMP)
      return "ICMP";
   else if(iHeader->protocol == UDP)
      return "UDP";
   return NULL;
}

/* Calculates the checksum of an ipheader* and returns a string representing if
 * it is "Correct" or "Incorrect" */
char* findCS(struct ipHeader* iHeader){
   return "NULL";
}

/* Converts an ipHeader lenth from a byte to the "Actual Length"
 * "Actual Length" = lower 4 bits * 4 Bytes (Size of word) 
 */
uint8_t convertLen(uint8_t rawLength){
   uint8_t actLength;
   actLength = (rawLength & 0xF) * 4;
   return actLength;
}

/* Prints out the IP Header */
void printIpHeader(struct ipHeader* iHeader){
   printf("\tIP Header\n");
   printf("\t\tHeader Len: %u (bytes)\n", convertLen(iHeader->headerLen));
   printf("\t\tTOS: 0x%x\n", iHeader->tos);
   printf("\t\tTTL: %i\n", iHeader->ttl);
   printf("\t\tIP PDU Len: %i (bytes)\n", iHeader->pduLen);
   printf("\t\tProtocol: %s\n", toWord(iHeader));
   printf("\t\tChecksum: %s (0x%02x%02x)\n", findCS(iHeader), 
      iHeader->checksum[0], iHeader->checksum[1]);
   printf("\t\tSender IP: %i.", iHeader->senderIP[0]);
   printf("%i.",  iHeader->senderIP[1]);
   printf("%i.",  iHeader->senderIP[2]);
   printf("%i\n", iHeader->senderIP[3]);
   printf("\t\tDest IP: %i.", iHeader->destIP[0]);
   printf("%i.",  iHeader->destIP[1]);
   printf("%i.",  iHeader->destIP[2]);
   printf("%i\n\n", iHeader->destIP[3]);
}

/* Converts the ping number (reply or request) to a string */
char* getPingType(uint8_t type){
   if(type == PING_REPLY)
      return "Reply";
   else if(type == PING_REQUEST)
      return "Request";
   return NULL;
}

/* Prints the ping packet (ICMP) header */
void printICMPHeader(void* pkt_data){
   uint8_t type;

   pkt_data += sizeof(uint8_t) * ICMP_OFFSET;
   type = *((uint8_t*)pkt_data);
   printf("\tICMP Header\n");
   printf("\t\tType: %s\n", getPingType(type));
}

/* Prints the UDP Header */
void printUDPHeader(void *pkt_data){
   uint16_t src;
   uint16_t dst;

   pkt_data += sizeof(uint8_t) * UDP_OFFSET;
   src = ntohs(*((uint16_t*)pkt_data));

   pkt_data += sizeof(uint16_t);
   dst = ntohs(*((uint16_t*)pkt_data));

   printf("\tUDP Header\n");
   printf("\t\tSource Port: : %u\n", src);
   printf("\t\tDest Port: : %u\n", dst);
}

/* Print TCP Ack Number */
void printACKNum(uint32_t ackNum){
   if(ackNum == 0)
      printf("\t\tACK Number: <not valid>\n");
   else
      printf("\t\tACK Number: %lu\n", (unsigned long)ackNum);
}

/* Print TCP flags */
void printFlags(uint16_t flgs){
   if(flgs & ACK_FLG)
      printf("\t\tACK Flag: Yes\n");
   else
      printf("\t\tACK Flag: No\n");

   if(flgs & SYN_FLG)
      printf("\t\tSYN Flag: Yes\n");
   else
      printf("\t\tSYN Flag: No\n");

   if(flgs & RST_FLG)
      printf("\t\tRST Flag: Yes\n");
   else
      printf("\t\tRST Flag: No\n");

   if(flgs & FIN_FLG)
      printf("\t\tFIN Flag: Yes\n");
   else
      printf("\t\tFIN Flag: No\n");
}

/* Finds the TCP checksum */
char* findTCP_CS(uint32_t checksum){
   return NULL;
}

/* Prints the TCP Header */
void printTCPHeader(void *pkt_data){
   uint16_t src;
   uint16_t dst;
   uint32_t sqNum;
   uint32_t ackNum;
   uint16_t flgs;
   uint16_t winSize;
   uint8_t  checksum[2]; /* 2 Bytes representing the checksum */

   pkt_data += sizeof(uint8_t) * UDP_OFFSET;

   src = ntohs(*((uint16_t*)pkt_data));
   pkt_data += sizeof(uint16_t);

   dst = ntohs(*((uint16_t*)pkt_data));
   pkt_data += sizeof(uint16_t);

   sqNum = ntohl(*((uint32_t*)pkt_data));
   pkt_data += sizeof(uint32_t);

   ackNum = ntohl(*((uint32_t*)pkt_data));
   pkt_data += sizeof(uint32_t);

   flgs = ntohs(*((uint16_t*)pkt_data));
   pkt_data += sizeof(uint16_t);

   winSize = ntohs(*((uint16_t*)pkt_data));
   pkt_data += sizeof(uint16_t);

   /* Since the bytes need to go from network to computer order I swapped the
    * checksum values. Thus the first value goes to checksum[1] and NOT
    * checksum[0] */
   checksum[1] = *((uint8_t*)pkt_data);
   pkt_data += sizeof(uint8_t);

   checksum[0] = *((uint8_t*)pkt_data);

   printf("\tTCP Header\n");
   printf("\t\tSource Port: : %u\n", src);
   printf("\t\tDest Port: : %u\n", dst);
   printf("\t\tSequence Number: %lu\n", (unsigned long)sqNum);
   printACKNum(ackNum);
   printFlags(flgs);
   printf("\t\tWindow Size: %u\n", winSize);
   printf("\t\tChecksum: %s (0x%02x%02x)\n", findTCP_CS(1), checksum[0], 
      checksum[1]);
}


/* Outputs the IP Header given a void* to the pkt_data */
void outputIpHeader(void* pkt_data){
   struct ipHeader iHeader;

   createIpHeader(&iHeader, pkt_data);
   printIpHeader(&iHeader);

   if(iHeader.protocol == TCP){
      printTCPHeader(pkt_data);
   }
   else if (iHeader.protocol == ICMP){
      printICMPHeader(pkt_data);
   }
   else if (iHeader.protocol == UDP){
      printUDPHeader(pkt_data);
   }
}

/* Prints the data stored in the header and all designated fields */
/* struct pcap_pkthdr* pkt_header, u_char* pkt_data, int i -> void */
void printHeader(const u_char* pkt_data, struct pcap_pkthdr* pkt_header, int i){
   uint16_t type;

   type = outputEthHeader(pkt_header, (void*)pkt_data, i);

   if(type == ARP){
      outputArpHeader((void*)pkt_data);
   }
   else if(type == IP){
      outputIpHeader((void*)pkt_data);
   }
}

/* Find all the Ethernet Headers */
void printAllHeaders(pcap_t* pcap_file){
   struct pcap_pkthdr* pkt_header;
   const u_char* pkt_data;
   //struct ethHeader eHeader;
   int pcap_next_ret = 1;
   int i = 1;

   while(pcap_next_ret == 1){
      pcap_next_ret = findNextHeader(pcap_file, &pkt_header, &pkt_data);

      if(pcap_next_ret == -2){
         return;
      }

      printHeader(pkt_data, pkt_header, i);
      i++;
   }
}

int main(int argc, char * argv[])
{
   char errbuf[PCAP_ERRBUF_SIZE];
   const char* fname;
   pcap_t* pcap_file;

   /* Error checking if no input file */
   if(argc < 2){
      fprintf(stderr, "No input file\n");
      exit(EXIT_FAILURE);
   }

   /* Saving input file */
   fname = argv[1];

   /* Error checking for pcap */
   if((pcap_file = pcap_open_offline(fname, errbuf)) == NULL){
      /* Error occured while opening the file */
      fprintf(stderr, "Error while opening the pcap file: %s\n", errbuf);
      exit(EXIT_FAILURE);
   }

   printAllHeaders(pcap_file);
   /*
   printAllHeaders(pcap_file, &pkt_header, &pkt_data, &eHeader);
   */

   /*
   pcap_next_ret = pcap_next_ex(pcap_file, &pkt_header, &pkt_data);
   */
   /* From the man pages about pcap_next_ex return value
    *  1 = packet was read without problems
    *  0 = packets are being read from a live capture and the timeout expired
    * -1 = Error occured
    * -2 = Packets being read from a ``savefile'' and no more packets to read
   createEthHeader(&eHeader, (void*)pkt_data);
   printEthHeader(&eHeader);
    */

   /*
   pcap_next_ret = findNextHeader(pcap_file, &pkt_header, &pkt_data, &eHeader,9);
   createEthHeader(&eHeader, (void*)pkt_data);
   printEthHeader(&eHeader);
   */

   /*
   printf("pcap_next_ret      %i\n", pcap_next_ret);
   printf("pkt_header->caplen %i\n", pkt_header->caplen);
   printf("pkt_header->len    %i\n", pkt_header->len);
   */
   /*
   createEthHeader(&eHeader, (void*)pkt_data);
   printEthHeader(&eHeader);
   */
   return 0;
}
