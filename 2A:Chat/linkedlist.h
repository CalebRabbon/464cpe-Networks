#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Node{
   char* handle;
   int socketNum;
   struct Node* next;
} Node;

void printNode(Node* n);

void printLinkedList(Node* head);

void* cMalloc(size_t size);

// Dynaically creates a new node and returns a pointer to it
Node* makeNode(char* handle, int socketNum);

// Checks to see if the given handle is available 
int available(Node* head, Node* node);

// Adds a node to the beginning of the list
Node* addNode(Node* head, Node* node);

// Searches through the list and removes the node
Node* removeNode(Node* head, Node* node);

Node* makeLinkedList();

/*
typedef struct LinkedList{
   Node* head;
}LinkedList;
*/

#endif
