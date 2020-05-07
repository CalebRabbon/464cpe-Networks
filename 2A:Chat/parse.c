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
// the \0 character to the end of the string. The string ends either by
// whitespace, \0, \n, or len.
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

// Takes in the stdbuf, the sendbuf, and a sendHandle which is a string
// reresenting the name of the sender and returns an int representing the number of clients to
// send to. It also fills in the sendbuf with the number of handles at the correct
// sendbuf location.
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

#ifdef PRINT
   printf("val %i\n",CHAT_HEADER_LEN + SEND_HANDLE_BYTE  + (int)strlen(sendHandle));
#endif

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

// Returns a pointer to the beginning of the destination handles for the sendbuf
char* fillSender(char* sendbuf, char* sendHandle){
   sendbuf[3] = strlen(sendHandle);

   // Set buffer to beginning of sendHandle
   sendbuf += 4;

   memcpy(sendbuf, sendHandle, strlen(sendHandle));

   // Set to beginning of destination handles 
   // +1 for Num Desination handles
   sendbuf += (strlen(sendHandle) + 1);

   return sendbuf;
}

// Returns true if the string is a number and false if not
int isNumber(char* str){
   if(strcmp(str,"1") == 0 || strcmp(str,"2") == 0 || strcmp(str,"3") == 0 || strcmp(str,"4") == 0
      || strcmp(str,"5") == 0 || strcmp(str,"6") == 0 || strcmp(str,"7") == 0 || strcmp(str,"8") == 0 
      || strcmp(str,"9") == 0 || strcmp(str,"0") == 0 ){
      return TRUE;
   }
   else{
      return FALSE;
   }
}

// Finds the first handle in the sendbuf
char* findFirstHandle(char* stdbuf){
   char* curVal = stdbuf;

   // Plus one for the
   int len = MAX_HANDLE_SIZE + 1;

   char str[len];

   // Skip over the flag
   curVal = stepString(curVal);

   // Find the string could either be a number or a flag
   findStr(curVal, str, len);

   if(isNumber(str)){
      // Steping over the number
      curVal = stepString(curVal);
   }

   // curVal is pointed to the location of the first string
   return curVal;
}

// Fills the handle inside the sendbuf and returns a pointer to the next
// available spot in the sendbuf
char* fillHandle(char* sendbuf, char* handle){
   sendbuf[0] = strlen(handle);
   // Set buffer to beginning of handle
   sendbuf += 1;

   memcpy(sendbuf, handle, strlen(handle));

   sendbuf += strlen(handle);

   return sendbuf;
}

// Fills the sendbuf with the rest of the text and returns a pointer to one
// after the last address
char* fillText(char* sendbuf, char* text){
   int i = 0;

   if(text == NULL){
      sendbuf[0] = '\n';
      return sendbuf;
   }

   while(1){
      if(text[i] == '\n'){
         sendbuf += i;
         return sendbuf;
      }
      if(text[i] == '\0'){
         sendbuf += i;
         return sendbuf;
      }
      if(i == MAX_SEND_TXT){
         // Max buffer length
         sendbuf += i;
         return sendbuf;
      }
      // Copy data to sendbuf
      sendbuf[i] = text[i];
      i++;
   }
   sendbuf += i;

   return sendbuf;
}

// Fills out the pdu chat header
void fillChatHeader(char* sendbuf, int flag, int pduLen){
   (((uint16_t*)sendbuf)[0]) = htons(pduLen);
   (((uint8_t*)sendbuf)[2]) = flag;
}

// Fills out the sendbuf with a packet and returns the pdu packet length
int proc_M(char* stdbuf, char* sendbuf, char* sendHandle){
   char* curVal = NULL;
   char* startSendBuf = sendbuf;
   char* end = NULL;
   char handle[MAX_HANDLE_SIZE + 1];
   int handleNum = 0;
   uint16_t totalLength = 0;
   int i = 0;

   handleNum = getHandleNum(stdbuf, sendbuf, sendHandle);

   sendbuf = fillSender(sendbuf, sendHandle);

   curVal = findFirstHandle(stdbuf);

   for(i = 0; i < handleNum; i++){
      findStr(curVal, handle, MAX_HANDLE_SIZE);
      sendbuf = fillHandle(sendbuf, handle);
      curVal = stepString(curVal);
   }

   end = fillText(sendbuf, curVal);

#ifdef PRINT
   printf("sendbuf beginning %lu\n", (uintptr_t)sendbuf);
   printf("end %lu\n", (uintptr_t)end);
   printf("adding sender %lu\n",(uintptr_t)sendbuf - (uintptr_t)startSendBuf);
#endif

   totalLength = (uintptr_t)end - (uintptr_t)startSendBuf;

   fillChatHeader(startSendBuf, FLAG_5, totalLength);

   return totalLength;
}

int proc_E(char* sendbuf){
   fillChatHeader(sendbuf, FLAG_8, DEFAULT_PDULEN);
   return DEFAULT_PDULEN;
}

int procStdin(char* stdbuf, char* sendbuf, char* sendHandle){
   int type;

   type = findType(stdbuf);

   switch(type){
      case TYPE_M:
         printf("TYPE_M\n");
         return proc_M(stdbuf, sendbuf, sendHandle);
         break;
      case TYPE_B:
         printf("TYPE_B\n");
         break;
      case TYPE_E:
         printf("TYPE_E\n");
         return proc_E(sendbuf);
         break;
      case TYPE_L:
         printf("TYPE_L\n");
         break;
   }
   return 0;
}

int getFromStdin(char * stdbuf, char * prompt)
{
	// Gets input up to MAXBUF-1 (and then appends \0)
	// Returns length of string including null
	char aChar = 0;
	int inputLen = 0;       
	
	// Important you don't input more characters than you have space 
	printf("%s ", prompt);
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			stdbuf[inputLen] = aChar;
			inputLen++;
		}
	}

	stdbuf[inputLen] = '\0';
	inputLen++;  //we are going to send the null
	
	return inputLen;
}
