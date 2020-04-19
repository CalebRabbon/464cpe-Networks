#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>
#include <string.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#define IP_ADDR_SIZE 4

__attribute__((packed))

struct ethHeader
{
   struct ether_addr dst;  /* Destination address */
   struct ether_addr src;  /* Source address */
   uint16_t          type; /* Type of next packet */
}__attribute__((packed));

struct arpHeader
{
   uint16_t          opcode;             /* Opcode as a 2 byte unsigned int */
   struct ether_addr senderMac;          /* Destination address */
   unsigned char senderIP[IP_ADDR_SIZE]; /* Sender IP broken into 4 bytes */
   struct ether_addr targetMac;          /* Source address */
   unsigned char targetIP[IP_ADDR_SIZE]; /* Target IP broken into 4 bytes */
}__attribute__((packed));

struct ipHeader
{
   uint8_t  headerLen;   /* Header length=lower 4 bits x 4bytes (word length)*/
   uint8_t  tos;         /* Terms of service */
   uint8_t  ttl;         /* Time to live */
   uint16_t pduLen;      /* IP Protocol Data Unit length in bytes */
   uint8_t  protocol;    /* IP protocol: TCP = 6, ICMP = 1, UDP = 17 */
   uint8_t  checksum[2]; /* 2 Bytes representing the checksum */
   char* checksum_flg; /* String representing if the checksum is correct */
   unsigned char senderIP[IP_ADDR_SIZE]; /* Sender IP broken into 4 bytes */
   unsigned char destIP[IP_ADDR_SIZE];   /* Target IP broken into 4 bytes */
}__attribute__((packed));

#endif
