/* Caleb Rabbon, crabbon@calpoly.edu
 * Created 4/19/2020 */

/* Uncomment the below lines to turn on print statments for debugging */
/*
#define DEBUG
#define IPDEBUG
*/

#include "trace.h"
#include "checksum.h"
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

/* CHECKSUM_VALEU is found by manually adding each word in IP Header */
#define CHECKSUM_VALUE 0xFFFF

#define PING_REQUEST 8
#define PING_REPLY 0

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

/* Calculates the checksum of an ipHeader* and fills in the ipHeader* */
void findIP_CS(struct ipHeader* iHeader, void* pkt_data){
   uint16_t calc_cs = 0; /* The calculated checksum */
   uint16_t ipWords = 0; 

   /* Round IP Words up */
   ipWords = iHeader->headerLen;

   /* Moving the pkt_data pointer to the IP's header length */
   pkt_data += sizeof(uint8_t) * IP_HEADERLEN_OFFSET;

   calc_cs = in_cksum((unsigned short*) pkt_data, ipWords);

#ifdef IPDEBUG
   printf("CS = %i\n", calc_cs);
   printf("CHECKSUMVALUE = %i\n", CHECKSUM_VALUE);
#endif
   if(calc_cs == 0)
      iHeader->checksum_flg = "Correct";
   else
      iHeader->checksum_flg = "Incorrect";
}

/* Converts an ipHeader lenth from a byte to the "Actual Length"
 * "Actual Length" = lower 4 bits * 4 Bytes (Size of word) 
 */
uint8_t convertLen(uint8_t rawLength){
   uint8_t actLength;
   actLength = (rawLength & 0xF) * 4;
   return actLength;
}

/* Fills in the passed in struct ipHeader pointer with data from the 
 * void* pkt_data and returns void */
void createIpHeader(struct ipHeader* iHeader, void* pkt_data){
   int i = 0;
   void* saved_pkt_data = pkt_data;

   /* Moving the pkt_data pointer to the IP's header length */
   pkt_data += sizeof(uint8_t) * IP_HEADERLEN_OFFSET;
   iHeader->headerLen = convertLen(*((uint8_t*)pkt_data));

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

   /* Filling out the checksum */
   iHeader->checksum[0] = *((uint8_t*)pkt_data);
   pkt_data += sizeof(uint8_t);
   iHeader->checksum[1] = *((uint8_t*)pkt_data);
   pkt_data += sizeof(uint8_t);

   for(i = 0; i < IP_ADDR_SIZE; i ++){
      iHeader->senderIP[i] = ((uint8_t*)(pkt_data))[i];
      iHeader->destIP[i] = ((uint8_t*)(pkt_data))[i+IP_ADDR_SIZE];
   }

   findIP_CS(iHeader, saved_pkt_data);
}

/* Returns the associated protocol name given a struct ipHeader* */
/* TCP = 6, ICMP = 1, UDP = 17 */
char* toWord(struct ipHeader* iHeader){
   if(iHeader->protocol == TCP)
      return "TCP";
   else if(iHeader->protocol == ICMP)
      return "ICMP";
   else if(iHeader->protocol == UDP)
      return "UDP";
   return "Unknown";
}

/* Prints out the IP Header */
void printIpHeader(struct ipHeader* iHeader){
   printf("\tIP Header\n");
   printf("\t\tHeader Len: %u (bytes)\n", iHeader->headerLen);
   printf("\t\tTOS: 0x%x\n", iHeader->tos);
   printf("\t\tTTL: %i\n", iHeader->ttl);
   printf("\t\tIP PDU Len: %i (bytes)\n", iHeader->pduLen);
   printf("\t\tProtocol: %s\n", toWord(iHeader));
   if(iHeader->checksum[1] == 0){
   printf("\t\tChecksum: %s (0x%x)\n", iHeader->checksum_flg, 
      iHeader->checksum[0]);
   }
   else{
      printf("\t\tChecksum: %s (0x%x%02x)\n", iHeader->checksum_flg, 
         iHeader->checksum[1], iHeader->checksum[0]);
   }
   printf("\t\tSender IP: %i.", iHeader->senderIP[0]);
   printf("%i.",  iHeader->senderIP[1]);
   printf("%i.",  iHeader->senderIP[2]);
   printf("%i\n", iHeader->senderIP[3]);
   printf("\t\tDest IP: %i.", iHeader->destIP[0]);
   printf("%i.",  iHeader->destIP[1]);
   printf("%i.",  iHeader->destIP[2]);
   printf("%i\n", iHeader->destIP[3]);
}

/* Converts the ping number (reply or request) to a string */
char* getPingType(uint8_t type){
   if(type == PING_REPLY)
      return "Reply";
   else if(type == PING_REQUEST)
      return "Request";
   return "109";
}

/* Prints the ping packet (ICMP) header */
void printICMPHeader(struct ipHeader* iHeader, void* pkt_data){
   uint8_t type;
   uint16_t icmp_offset;

   icmp_offset = IP_HEADERLEN_OFFSET + iHeader->headerLen;

   pkt_data += sizeof(uint8_t) * icmp_offset;
   type = *((uint8_t*)pkt_data);
   printf("\n\tICMP Header\n");
   printf("\t\tType: %s\n", getPingType(type));
}

/* Prints the UDP Header */
void printUDPHeader(struct ipHeader* iHeader, void *pkt_data){
   uint16_t src;
   uint16_t dst;
   uint16_t udp_offset;

   udp_offset = IP_HEADERLEN_OFFSET + iHeader->headerLen;

   pkt_data += sizeof(uint8_t) * udp_offset;
   src = ntohs(*((uint16_t*)pkt_data));

   pkt_data += sizeof(uint16_t);
   dst = ntohs(*((uint16_t*)pkt_data));

   printf("\n\tUDP Header\n");
   printf("\t\tSource Port: : %u\n", src);
   printf("\t\tDest Port: : %u\n", dst);
}

/* Print TCP Ack Number */
void printACKNum(uint32_t ackNum, uint16_t flgs){
   if(ackNum == 0)
      printf("\t\tACK Number: <not valid>\n");
   else if(ackNum != 0 && ((flgs & ACK_FLG) == 0))
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

/* Adds the value to the checksum and adjusts for overflow */
void addVal(uint16_t val, uint16_t* calc_cs){
   /* If you overflow add one to the checksum */
   uint16_t prev_calc_cs = *calc_cs;

   *calc_cs += val;
   if(*calc_cs < prev_calc_cs)
      *calc_cs += 1;
}

/* Determins if the TCP checksum is correct */
char* findTCP_CS(void* pkt_data, struct ipHeader* iHeader){
   uint16_t calc_cs = 0; /* The calculated checksum */
   uint16_t IpWord = 0;
   uint16_t base_cs = 0;
   uint16_t tcpLength = 0;
   uint16_t tcpWords = 0;
   uint16_t tcp_offset;

   /* Moving the pkt_data pointer to the beginning of the TCP's header length */
   tcp_offset = IP_HEADERLEN_OFFSET + iHeader->headerLen;
   pkt_data += sizeof(uint8_t) * tcp_offset;

   /* tcpLength in bytes
    * This is found by taking the total IP Length - IP Header length */
   tcpLength = iHeader->pduLen - iHeader->headerLen;

#ifdef DEBUG
   printf("tcpLength %i\n", tcpLength);
#endif

   /* Must divide by 2 to get words since it is set to tcpLength which is in
    * bytes */
   tcpWords = tcpWords/2;

   base_cs = in_cksum((unsigned short*) pkt_data, tcpLength);

   /* Must invert the checksum then convert it to host order to add pseudo
    * header */
   base_cs = ~base_cs;
   base_cs = ntohs(base_cs);

#ifdef DEBUG
   printf("base_cs 0x%x\n", base_cs);
   printf("tcpWords %i\n", tcpWords);
#endif

#ifdef DEBUG
   printf("BASE base_cs 0x%06x\n", base_cs);
   printf("myCS = 0x%06x\n", calc_cs);
#endif

   calc_cs = base_cs;

   /* Adding the TCP Pseudo Header */
   addVal(TCP, &calc_cs);
#ifdef DEBUG
   printf("TCP = 0x%x\n", TCP);
   printf("calc_cs = 0x%x\n", calc_cs);
#endif

   addVal(tcpLength, &calc_cs);
#ifdef DEBUG
   printf("tcpLength = 0x%x\n", tcpLength);
   printf("calc_cs = 0x%x\n", calc_cs);
#endif

   /* Multiply by 256 to shift over the first char by one byte */
   IpWord = iHeader->senderIP[0] * 256 + iHeader->senderIP[1];
   addVal(IpWord, &calc_cs);
#ifdef DEBUG
   printf("senderIP 1 = 0x%x\n", IpWord);
#endif

   IpWord = iHeader->senderIP[2] * 256 + iHeader->senderIP[3];
   addVal(IpWord, &calc_cs);
#ifdef DEBUG
   printf("senderIP 2 = 0x%x\n", IpWord);
#endif

   IpWord = iHeader->destIP[0] * 256 + iHeader->destIP[1];
   addVal(IpWord, &calc_cs);
#ifdef DEBUG
   printf("destIP 1 = 0x%x\n", IpWord);
#endif

   IpWord = iHeader->destIP[2] * 256 + iHeader->destIP[3];
   addVal(IpWord, &calc_cs);
#ifdef DEBUG
   printf("destIP 2 = 0x%x\n", IpWord);
   printf("calc_cs Integer = %i\n", calc_cs);
   printf("CHECKSUMVALUE = %i\n", CHECKSUM_VALUE);
#endif

   if(calc_cs == CHECKSUM_VALUE)
      return "Correct";
   else
      return "Incorrect";
   return "Incorrect";
}

/* Prints the TCP Header */
void printTCPHeader(struct ipHeader* iHeader, void *pkt_data){
   uint16_t src, dst, flgs, winSize, tcp_offset;
   uint32_t sqNum, ackNum;
   uint8_t  checksum[2]; /* 2 Bytes representing the checksum */
   void* saved_pkt_data = pkt_data;

   tcp_offset = IP_HEADERLEN_OFFSET + iHeader->headerLen;

   pkt_data += sizeof(uint8_t) * tcp_offset;

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

   /* Filling out the checksum */
   checksum[0] = *((uint8_t*)pkt_data);
   pkt_data += sizeof(uint8_t);
   checksum[1] = *((uint8_t*)pkt_data);
   pkt_data += sizeof(uint8_t);

   printf("\n\tTCP Header\n");

   if(src == 80)
      printf("\t\tSource Port:  HTTP\n");
   else
      printf("\t\tSource Port: : %u\n", src);
   if(dst == 80)
      printf("\t\tDest Port:  HTTP\n");
   else
      printf("\t\tDest Port: : %u\n", dst);
   printf("\t\tSequence Number: %lu\n", (unsigned long)sqNum);
   printACKNum(ackNum, flgs);
   printFlags(flgs);
   printf("\t\tWindow Size: %u\n", winSize);
   if(checksum[0] == 0){
      printf("\t\tChecksum: %s (0x%02x)\n", findTCP_CS(saved_pkt_data, iHeader),
         checksum[1]);
   }
   else{
      printf("\t\tChecksum: %s (0x%x%02x)\n", findTCP_CS(saved_pkt_data, iHeader),
         checksum[0], checksum[1]);
   }
}


/* Outputs the IP Header given a void* to the pkt_data */
void outputIpHeader(void* pkt_data){
   struct ipHeader iHeader;

   createIpHeader(&iHeader, pkt_data);
   printIpHeader(&iHeader);

   if(iHeader.protocol == TCP){
      printTCPHeader(&iHeader, pkt_data);
   }
   else if (iHeader.protocol == ICMP){
      printICMPHeader(&iHeader, pkt_data);
   }
   else if (iHeader.protocol == UDP){
      printUDPHeader(&iHeader, pkt_data);
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

   return 0;
}
