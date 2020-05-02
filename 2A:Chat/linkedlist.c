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
   printf("%s | SN: %i |",n->handle, n->socketNum);
   if(n->next != NULL){
      printf("-> %s\n", n->next->handle);
   }
   else if(n->next == NULL){
      printf("-> NULL\n");
   }
}

// Prints all the nodes in the linked list
void printLinkedList(Node* head){
   Node* curVal = head;
   printf("\n");
   if(head == NULL){
      printf("-> NULL\n");
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
int available(Node* head, Node* node){
   Node* curVal = head;

   while(curVal != NULL){
      if(strcmp(curVal->handle,node->handle) == 0){
         // Found the node
         printf("Could not add Handle: '%s' becuase it already Exists\n", curVal->handle);
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

   if(available(head, node)){
      /*
      printf("availabe\n");
      printNode(head);
      printNode(node);
      */
      Node* prevHead = head;
      head = node;
      /*
      printf("printing the new head\n");
      printNode(head);
      */
      head->next = prevHead;
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
      printf("Your node is %s. Please remove a non NULL node\n", (char*)node);
      return head;
   }

   if(head == NULL){
      printf("Your head is %s. There are no nodes in your list\n", (char*)head);
      return head;
   }

   if(strcmp(head->handle,node->handle) == 0){
      // Removing the head of list
      temp = curVal;
      printf("Removing the head: '%s' from the handle list\n", temp->handle);
      head = node->next;
      free(temp);
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
         printf("Removed %s from the handle list\n", curVal->handle);
         prev->next = node->next;
         printNode(node);
         free(curVal);
         return head;
      }
      // Set prev to curVal
      prev = curVal;

      // Move curVal along
      curVal = curVal->next;
   }
   printf("Handle could not be removed because it does not exist\n");
   return head;
}

Node* makeLinkedList(){
   Node* n = cMalloc(sizeof(Node));
   n = NULL;
   return n;
}
