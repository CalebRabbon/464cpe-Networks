#include "myClient.h"

// Converts the character to the correct int type defined by the MACROS
int convertCharType(char type){
   switch(type){
      case 'M':
         return TYPE_M;
         break;
      case 'B':
         return TYPE_B;
         break;
      case 'E':
         return TYPE_E;
         break;
      case 'L':
         return TYPE_L;
         break;
      case 'm':
         return TYPE_M;
         break;
      case 'b':
         return TYPE_B;
         break;
      case 'e':
         return TYPE_E;
         break;
      case 'l':
         return TYPE_L;
         break;
   }
   return -1;
}


// Finds the type of the message from the stdbuf and returns an int representing that type or -1
// if the message is incorrectly formatted
int findType(char* stdbuf){
   char* curVal = stdbuf;
   int i = 0;
   int type = 0;
   char charType = '0';

   // Skip over whitespace
   while(curVal[i] == ' '){
      i ++;
   }
   // Found the % symbol
   if (curVal[i] == '%'){
      i ++;
      charType = curVal[i];
      type = convertCharType(charType);
      return type;
   }

   return -1;
}

// Takes in the stdbuf and copies the first string into str. It will append
// the \0 character to the end of the string at index len.
// Len is the max length of the str excluding the null character
void findStr(char* stdbuf, char* str, int len){
   int i = 0;
   int j = 0;

   if(stdbuf == NULL){
      memset(str, 0, len);
      return;
   }

   // Move the pointer through the white space
   while (stdbuf[i] == ' '){
      if(stdbuf[i] == '\0'){
         return;
      }
      i ++;
   }

   // Found the beginning of the string
   // Move the pointer through the string
   while (stdbuf[i] != ' '){
      if(stdbuf[i] == '\0' || stdbuf[i] == '\n'){
         // Append a Null key if end of string or new line
         str[j] = '\0';
         return;
      }
      str[j] = stdbuf[i];

      // Got to the end of the str buffer
      if(j == len){
         str[j] = '\0';
         return;
      }

      j ++;
      i ++;
   }

   // Appending the null to the end of the string
   str[j] = '\0';
}

// Returns -1 if the single digit is not a number and default 1 if the strlen is
// greater than 1
int8_t convertStrToInt(char* str){
   if(strlen(str) == 1){
      if(strcmp(str, "1") == 0)
         return 1;
      else if(strcmp(str, "2") == 0)
         return 2;
      else if(strcmp(str, "3") == 0)
         return 3;
      else if(strcmp(str, "4") == 0)
         return 4;
      else if(strcmp(str, "5") == 0)
         return 5;
      else if(strcmp(str, "6") == 0)
         return 6;
      else if(strcmp(str, "7") == 0)
         return 7;
      else if(strcmp(str, "8") == 0)
         return 8;
      else if(strcmp(str, "9") == 0)
         return 9;
      else
         return -1;
   }
   // Staying that the string was greater than 1 character so it is handle
   return 1;
}

// Takes in the stdbuf and returns an int representing the number of clients to
// send to. It also fills in the send buf with the number of handles at the correct
// sendbuf location
// Returns -1 if either stdbuf or sendbuf is NULL
int getHandleNum(char* stdbuf, char* sendbuf, char* sendHandle){
   char* curVal = NULL;
   char str[3];
   int8_t val;

   if(stdbuf == NULL || sendbuf == NULL)
      return -1;

   curVal = stepString(stdbuf);

   // Should be pointing at either a number or a handle
   // leaving room just incase the handle is not a singular value
   findStr(curVal, str, 3);

   val = convertStrToInt(str);

   // Setting the sendbuf to the right spot and setting Num Destination handles
   sendbuf[CHAT_HEADER_LEN + SEND_HANDLE_BYTE  + strlen(sendHandle)] = val;

   return val;
}

// Steps over the string and returns a pointer to the rest of the buffer. A string is
// separated by white space
// Returns NULL if there is no next string
char* stepString(char* stdbuf){
   char* curVal = stdbuf;
   int i = 0;
   if(stdbuf == NULL)
      return NULL;

   // Move the pointer through the white space
   while (stdbuf[i] == ' '){
      if(stdbuf[i] == '\0')
         return NULL;
      i ++;
   }

   // Found the beginning of the string
   // Move the pointer through the string
   while (stdbuf[i] != ' '){
      if(stdbuf[i] == '\0')
         return NULL;
      i ++;
   }

   // Move the pointer through the white space
   while (stdbuf[i] == ' '){
      if(stdbuf[i] == '\0')
         return NULL;
      i ++;
   }

   // Found the beginning of the next string 
   curVal += i;

   return curVal;
}

void procStdin(int stdlen, char* stdbuf, char* sendbuf){
   int type;

   type = findType(stdbuf);

   switch(type){
      case TYPE_M:
         printf("TYPE_M\n");
         break;
      case TYPE_B:
         printf("TYPE_B\n");
         break;
      case TYPE_E:
         printf("TYPE_E\n");
         break;
      case TYPE_L:
         printf("TYPE_L\n");
         break;
   }
}


