#include "linkedlist.h"

#define TRUE 1
#define FALSE 0
//#define PRINT

// Caleb's malloc to stop errors
void* cMalloc(size_t size){
   void* val = malloc(size);

   if(val == NULL){
      perror("Error with malloc");
      exit(EXIT_FAILURE);
   }
   return val;
}

int findListLength(Node* head){
   int i = 0;
   if(head == NULL){
      return i;
   }
   while(head != NULL){
      i++;
      // Move head along
      head = head->next;
   }
   return i;
}

void printNode(Node* n){
   if(n == NULL){
      printf("\tNode == NULL\n");
      return;
   }
   printf("\t%s | SN: %i |",n->handle, n->socketNum);
   if(n->next != NULL){
      printf("-> %s\n", n->next->handle);
   }
   else if(n->next == NULL){
      printf("\t-> NULL\n");
   }
}

// Prints all the nodes in the linked list
void printLinkedList(Node* head){
   printf("\nCurrent Linked List of Handles:\n");
   Node* curVal = head;
   if(head == NULL){
      printf("\t-> NULL\n");
      return;
   }
   while(curVal != NULL){
      printNode(curVal);
      // Move curVal along
      curVal = curVal->next;
   }
   printf("\n");
}


// Dynaically creates a new node and returns a pointer to it
Node* makeNode(char* handle, int socketNum){
   Node* n = cMalloc(sizeof(Node));
   strcpy(n->handle,handle);
   n->socketNum = socketNum;
   n->next = NULL;
   return n;
}

// Checks to see if the given handle is available 
int available(Node* head, char* handle){
   Node* curVal = head;
   //char* temp = curVal->handle;


   while(curVal != NULL){
      if(strcmp(curVal->handle,handle) == 0){
         // Found the node
         return FALSE;
      }
      // Move curVal along
      curVal = curVal->next;
   }
   return TRUE;
}

// Adds a node to the beginning of the list and returns the new head of the list
Node* addNode(Node* head, Node* node){
   if(node == NULL){
      printf("Your node is %s. Please add a non NULL node\n", (char*)node);
      return head;
   }

   if(available(head, node->handle)){
      Node* prevHead = head;
      head = node;
      head->next = prevHead;
   }

   return head;
}

// Finds the socket associated with the handle
// Returns 0 if the handle was not found
int findSocket(Node* head, char* handle){
   Node* curVal = head;

   while(curVal != NULL){
      if(strcmp(curVal->handle,handle) == 0){
         // Found the node
         return curVal->socketNum;
      }
      // Move curVal along
      curVal = curVal->next;
   }
   return 0;
}

// Finds the node at the designated index
// Index starts at 0 for the first node
Node* findNodeIndex(Node* head, int index){
   int i = 0;

   if(head == NULL){
#ifdef PRINT
      printf("Your head is %s. Please use a non NULL head\n", (char*)head);
#endif
      return head;
   }

   for(i = 0; i < index; i ++){
      if(head == NULL){
         fprintf(stderr, "Index %i is out of range\n", index);
         return NULL;
      }
      head = head->next;
   }

   return head;
}

// Finds the node associated with the socket number
Node* findNode(Node* head, int socketNum){
   if(head == NULL){
#ifdef PRINT
      printf("Your head is %s. Please use a non NULL head\n", (char*)head);
#endif
      return head;
   }

   while(head != NULL){
      if(head->socketNum == socketNum){
         return head;
      }
      head = head->next;
   }

   return head;
}

// Searches through the list and removes the node and returns the new head of
// the list
Node* removeNode(Node* head, Node* node){
   Node* curVal = head;
   Node* temp = NULL;
   Node* prev = NULL;
   if(node == NULL){
#ifdef PRINT
      printf("Your node is %s. Please remove a non NULL node\n", (char*)node);
#endif
      return head;
   }

   if(head == NULL){
#ifdef PRINT
      printf("Your head is %s. There are no nodes in your list\n", (char*)head);
#endif
      return head;
   }

   if(strcmp(head->handle,node->handle) == 0){
      // Removing the head of list
      temp = curVal;
#ifdef PRINT
      printf("Removing the head: '%s' from the handle list\n", temp->handle);
#endif
      head = node->next;
      free(temp);
#ifdef PRINT
      printf("Head: '%p' \n", (void*)head);
#endif
      return head;
   }

   // Since it is not the head, moving the curVal forward
   curVal = head->next;

   // Setting the previous value
   prev = head;

   while(curVal != NULL){
      // Removing inner node
      if(strcmp(curVal->handle,node->handle) == 0){
         // Found the node
#ifdef PRINT
         printf("Removed %s from the handle list\n", curVal->handle);
#endif
         prev->next = node->next;
#ifdef PRINT
         printNode(node);
#endif
         free(curVal);
         return head;
      }
      // Set prev to curVal
      prev = curVal;

      // Move curVal along
      curVal = curVal->next;
   }
#ifdef PRINT
   printf("Handle could not be removed because it does not exist\n");
#endif
   return head;
}

Node* makeLinkedList(){
   Node* n = cMalloc(sizeof(Node));
   n = NULL;
   return n;
}
