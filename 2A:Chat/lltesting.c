#include "linkedlist.h"

int main(int argc, char *argv[])
{
   Node* head = makeLinkedList();
   Node* C1 = makeNode("C1", 4);
   Node* C2 = makeNode("C2", 4);
   Node* C3 = makeNode("C3", 4);

   head = addNode(head, head);
   printLinkedList(head);
   head = addNode(head, C1);
   printLinkedList(head);
   head = addNode(head, C1);
   printLinkedList(head);
   head = addNode(head, C2);
   printLinkedList(head);
   head = removeNode(head, head);
   printLinkedList(head);
   head = removeNode(head, head);
   printLinkedList(head);
   head = removeNode(head, head);
   printLinkedList(head);
   head = removeNode(head, C1);
   printLinkedList(head);
   head = addNode(head, C2);
   printLinkedList(head);
   head = addNode(head, C2);
   printLinkedList(head);
   head = addNode(head, C1);
   printLinkedList(head);
   head = addNode(head, C3);
   printLinkedList(head);
   head = removeNode(head, C2);
   printLinkedList(head);
   head = removeNode(head, C1);
   printLinkedList(head);
   head = removeNode(head, C3);
   printLinkedList(head);
   head = removeNode(head, C3);
   printLinkedList(head);
   head = removeNode(head, C3);
   printLinkedList(head);
   head = addNode(head, C3);
   printLinkedList(head);
   head = removeNode(head, C2);
   printLinkedList(head);
   head = removeNode(head, head);
   printLinkedList(head);
   return;
}
