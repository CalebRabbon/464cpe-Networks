#include "linkedlist.h"

#define TRUE 1
#define FALSE 0

// Caleb's malloc to stop errors
void* cMalloc(size_t size){
   void* val = malloc(size);

   if(val == NULL){
      perror("Error with malloc");
      exit(EXIT_FAILURE);
   }
   return val;
}

void printNode(Node* n){
   if(n == NULL){
      printf("Node == NULL\n");
      return;
   }
   printf("Handle: %s/t SocketNum: %i/t",n->handle, n->socketNum);
   if(n->next != NULL){
      printf("Next = %s\n", n->next->handle);
   }
   else if(n->next == NULL){
      printf("Next = NULL\n");
   }
}

// Prints all the nodes in the linked list
void printLinkedList(Node* head){
   Node* curVal = head;
   printf("curVal %p\n", (void*)curVal);

   while(curVal != NULL){
      printNode(curVal);
      // Move curVal along
      curVal = curVal->next;
   }
}


// Dynaically creates a new node and returns a pointer to it
Node* makeNode(char* handle, int socketNum){
   Node* n = cMalloc(sizeof(Node));
   n->handle = handle;
   n->socketNum = socketNum;
   n->next = NULL;
   return n;
}

// Checks to see if the given handle is available 
int available(Node* head, Node* node){
   Node* curVal = head;

   while(curVal != NULL){
      if(strcmp(curVal->handle,node->handle) == 0){
         // Found the node
         printf("Found duplicate handle %s in handle list\n", curVal->handle);
         return FALSE;
      }
      // Move curVal along
      curVal = curVal->next;
   }
   return TRUE;
}

// Adds a node to the beginning of the list
void addNode(Node* head, Node* node){
   if(available(head, node)){
      printf("availabe\n");
      printNode(head);
      Node* prevHead = head;
      head = node;
      node->next = prevHead;
   }
}

// Searches through the list and removes the node
void removeNode(Node* head, Node* node){
   Node* curVal = head;
   Node* temp = NULL;

   while(curVal != NULL){
      if(strcmp(curVal->handle,node->handle) == 0){
         // Found the node
         temp = curVal;
         printf("Removed %s from the handle list\n", temp->handle);
         curVal->next = node->next;
         free(temp);
      }
      // Move curVal along
      curVal = curVal->next;
   }
   printf("Handle could not be removed because it does not exist\n");
}

Node* makeLinkedList(){
   Node* n = cMalloc(sizeof(Node));
   n = NULL;
   return n;
}
