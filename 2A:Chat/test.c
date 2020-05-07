#include "test.h"
#include "macros.h"
#include "flags.h"
#include "myClient.h"
#include "recvparse.h"

// File used to test the parse functions

// TESTING the recvparse.c


void testgetText(){
	char stdbuf[MAXBUF];    //data from user input
   char sendbuf[MAX_SEND_LEN];
   char text[200];
   char* databuf;
   int pdulen;

   printf("TEST: getText\n");

//   stdbuf = "%M Caleb Text";
   memset(stdbuf, '\0', MAXBUF);
   memset(sendbuf, '\0', MAX_SEND_LEN);

   /*
   getFromStdin(stdbuf, "");
   pdulen = procStdin(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   TEST_INT(databuf[0], 6);
   len = findTextLen(databuf, pdulen);
   TEST_INT(len, 4);
   getText(databuf, text, pdulen);
   TEST_STRING(text, "Text");
   */

   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 1 Caleb Text", 15);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "Text");

   memset(text, 0, 200);
   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 2 Caleb Text", 15);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "\0");

   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 1 Caleb ", 11);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "\0");

   memset(text, 0, 200);
   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 1 Caleb", 10);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "\0");

   memset(text, 0, 200);
   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 1 Caleb    ", 14);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "\0");

   memset(text, 0, 200);
   memset(stdbuf, '\0', MAXBUF);
   memcpy(stdbuf,"%M 2 Caleb Text Aa", 18);
   memset(sendbuf, 0, MAX_SEND_LEN);
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   getText(databuf, text, pdulen);
   TEST_STRING(text, "Aa");

   printf("Finish: getText\n");
}

void testfindTextStart(){
   char* stdbuf = NULL;
   char sendbuf[MAX_SEND_LEN];
   char* databuf;
   char* text;

   printf("TEST: findTextStart\n");

   stdbuf = "%M Caleb Text";
   memset(sendbuf, 0, MAX_SEND_LEN);
   proc_M(stdbuf, sendbuf, "sender");
   databuf = (char*)sendbuf + 2;
   text = findTextStart(databuf);
   TEST_CHAR(text[0], 'T');

   stdbuf = "%M 1 Caleb Text";
   memset(sendbuf, 0, MAX_SEND_LEN);
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   text = findTextStart(databuf);
   TEST_CHAR(text[0], 'T');

   stdbuf = "%M 2 Caleb Text";
   memset(sendbuf, 0, MAX_SEND_LEN);
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   text = findTextStart(databuf);
   TEST_CHAR(text[0], '\n');

   stdbuf = "%M 2 Caleb Text A";
   memset(sendbuf, 0, MAX_SEND_LEN);
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   text = findTextStart(databuf);
   TEST_CHAR(text[0], 'A');

   printf("Finish: findTextStart\n");
}
void testfindTextLen(){
   char* stdbuf = NULL;
   char sendbuf[MAX_SEND_LEN];
   char* databuf = NULL;
   int len;
   int pdulen;

   printf("TEST: findTextLen\n");

   stdbuf = "%M Caleb Text";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   len = findTextLen(databuf, pdulen);
   TEST_INT(len, 4);

   memset(sendbuf, 0, MAX_SEND_LEN);
   stdbuf = "%M 2 Caleb Text";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   len = findTextLen(databuf, pdulen);
   TEST_INT(len, 0);

   memset(sendbuf, 0, MAX_SEND_LEN);
   stdbuf = "%M 1 Caleb      Text";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   len = findTextLen(databuf, pdulen);
   TEST_INT(len, 4);

   memset(sendbuf, 0, MAX_SEND_LEN);
   stdbuf = "%M 2 Caleb Text 12345678";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   len = findTextLen(databuf, pdulen);
   TEST_INT(len, 8);
   /*
   */

   printf("Finish: findTextLen\n");
}

void testfindDestHandle(){
   char* stdbuf = NULL;
   char* databuf = NULL;
   char sendbuf[MAX_SEND_LEN];
   char destHandle[101];
   char* test;

   printf("TEST: findDestHandle\n");

   stdbuf = "%M Caleb Text";
   proc_M(stdbuf, sendbuf, "sender");
   // Adding 2 to sendbuf to remove the chat_header
   databuf = sendbuf + 2;
   memset(destHandle, 0, 101);
   test = findDestHandle(databuf, 1, destHandle);
   TEST_STRING(destHandle, "Caleb");
   TEST_CHAR(test[0], 'T');

   stdbuf = "%M 2 Caleb Text";
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   memset(destHandle, 0, 101);
   test = findDestHandle(databuf, 1, destHandle);
   TEST_STRING(destHandle, "Caleb");
   TEST_INT(test[0], 4);

   memset(destHandle, 0, 101);
   findDestHandle(databuf, 2, destHandle);
   TEST_STRING(destHandle, "Text");

   stdbuf = "%M 3 Caleb Text Man adf";
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   memset(destHandle, 0, 101);
   findDestHandle(databuf, 1, destHandle);
   TEST_STRING(destHandle, "Caleb");

   memset(destHandle, 0, 101);
   findDestHandle(databuf, 2, destHandle);
   TEST_STRING(destHandle, "Text");

   memset(destHandle, 0, 101);
   findDestHandle(databuf, 3, destHandle);
   TEST_STRING(destHandle, "Man");

   memset(destHandle, 0, 101);
   findDestHandle(databuf, 4, destHandle);
   TEST_NULL(destHandle, NULL);

   printf("Finish: findDestHandle\n");

}
void testfindNumHandles(){
   char* stdbuf = NULL;
   char* databuf = NULL;
   char sendbuf[MAX_SEND_LEN];

   printf("TEST: findNumHandles\n");

   stdbuf = "%M Caleb Text";
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   TEST_INT(findNumHandles(databuf, "sender"), 1);

   stdbuf = "%M 2 Caleb Text";
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   TEST_INT(findNumHandles(databuf, "sender"), 2);

   stdbuf = "%M 2 Caleb Text adf";
   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;
   TEST_INT(findNumHandles(databuf, "sender"), 2);

   printf("Finish: findNumHandles\n");
}

void testfindSender(){
   char* stdbuf = NULL;
   char* databuf = NULL;
   char sendbuf[MAX_SEND_LEN];
   char sendHandle[MAX_HANDLE_SIZE + 1];

   printf("TEST: findSender\n");
   stdbuf = "%M Caleb Text";

   proc_M(stdbuf, sendbuf, "sender");
   databuf = sendbuf + 2;

   findSender(databuf, sendHandle);
   TEST_STRING(sendHandle, "sender");

   printf("Finish: findSender\n");
}

//TESTING the parse.c functions
void testconvertCharType(){
// define TEST_INT(_ACTUAL, _EXPECT)
   int type = 0;
   printf("TEST: convertCharType\n");

   type = convertCharType('M');
   TEST_INT(type, TYPE_M);

   type = convertCharType('m');
   TEST_INT(type, TYPE_M);

   type = convertCharType('E');
   TEST_INT(type, TYPE_E);

   type = convertCharType('E');
   TEST_INT(type, TYPE_E);

   type = convertCharType('B');
   TEST_INT(type, TYPE_B);

   type = convertCharType('b');
   TEST_INT(type, TYPE_B);

   type = convertCharType('L');
   TEST_INT(type, TYPE_L);

   type = convertCharType('l');
   TEST_INT(type, TYPE_L);

   type = convertCharType(' ');
   TEST_INT(type, -1);

   type = convertCharType('\n');
   TEST_INT(type, -1);

   printf("Finish: convertCharType\n");
}

void testfindType(){
   char* stdbuf = NULL;
   int type = 0;

   printf("TEST: findType\n");

   stdbuf = "  %M  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%M  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "   %M";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%M";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = " %M   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%M   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "  %m  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%m  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "   %m";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%m";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = " %m   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "%m   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_M);

   stdbuf = "  %B  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%B  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "   %B";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%B";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = " %B   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%B   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "  %b  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%b  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "   %b";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%b";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = " %b   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "%b   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_B);

   stdbuf = "  %E  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%E  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "   %E";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%E";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = " %E   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%E   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "  %e  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%e  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "   %e";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%e";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = " %e   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "%e   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_E);

   stdbuf = "  %L  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%L  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "   %L";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%L";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = " %L   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%L   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "  %l  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%l  ";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "   %l";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%l";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = " %l   asdf";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "%l   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, TYPE_L);

   stdbuf = "l   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, -1);

   stdbuf = "   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, -1);

   stdbuf = " %a   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, -1);

   stdbuf = " a%   afdaa\n";
   type = findType(stdbuf);
   TEST_INT(type, -1);

   printf("Finish: findType\n");
}

void teststepString(){
//TEST_STRING(_ACTUAL, _EXPECT)
   printf("TEST: stepString\n");
   char* str = NULL;

   str = "   %M  hello";
   TEST_STRING(stepString(str), "hello");

   str = "   fewa%M  hello";
   TEST_STRING(stepString(str), "hello");

   str = "fewa%M  hello";
   TEST_STRING(stepString(str), "hello");

   str = "fewa%M   hello   ";
   TEST_STRING(stepString(str), "hello   ");

   str = "  fewa%M   hello   ";
   TEST_STRING(stepString(str), "hello   ");

   str = "  fewa%M   hello   fewa";
   TEST_STRING(stepString(str), "hello   fewa");

   str = "     ";
   TEST_NULL(stepString(str), NULL);

   str = "  fdasf   ";
   TEST_NULL(stepString(str), NULL);

   str = "fdasf   ";
   TEST_NULL(stepString(str), NULL);

   str = "fdasf";
   TEST_NULL(stepString(str), NULL);

   str = NULL;
   TEST_NULL(stepString(str), NULL);

   printf("Finish: stepString\n");
}

void testfindStr(){
   char* str = NULL;
   char buf[101];

   printf("TEST: findStr\n");

   str = "  fewa%M   hello   fewa";
   findStr(str, buf, 100);
   TEST_STRING(buf, "fewa%M");

   str = "  1   hello   fewa";
   findStr(str, buf, 100);
   TEST_STRING(buf, "1");

   str = "  12   hello   fewa";
   findStr(str, buf, 100);
   TEST_STRING(buf, "12");

   str = "  1   hello   fewa";
   findStr(str, buf, 100);
   TEST_STRING(buf, "1");

   str = "1   hello   fewa";
   findStr(str, buf, 100);
   TEST_STRING(buf, "1");

   str = "caleb";
   findStr(str, buf, 100);
   TEST_STRING(buf, "caleb");

   str = "caleb\n";
   findStr(str, buf, 100);
   TEST_STRING(buf, "caleb");

   str = "";
   findStr(str, buf, 100);
   TEST_STRING(buf, "");

   str = "\n";
   findStr(str, buf, 100);
   TEST_STRING(buf, "");

   str = "hello";
   findStr(str, buf, 1);
   TEST_STRING(buf, "h");

   printf("Finish: findStr\n");
}
void testgetHandleNum(){
   char sendbuf[MAX_SEND_LEN];
   int numHand = 0;

   printf("TEST: getHandleNum\n");

   memset(sendbuf, 0, MAX_SEND_LEN);

   numHand = getHandleNum("  %M  asdf", sendbuf, "Caleb");
   TEST_INT(numHand, 1);
   TEST_INT(sendbuf[9], 1);

   numHand = getHandleNum("  %M  a", sendbuf, "CALEb");
   TEST_INT(numHand, -1);
   TEST_INT(sendbuf[9], -1);

   numHand = getHandleNum("  %M  a ", sendbuf, "asdfa");
   TEST_INT(numHand, -1);
   TEST_INT(sendbuf[9], -1);

   numHand = getHandleNum("  %M  1 ", sendbuf, "Ca13d");
   TEST_INT(numHand, 1);
   TEST_INT(sendbuf[9], 1);

   numHand = getHandleNum("  %M  13 ", sendbuf, "caleb12");
   TEST_INT(numHand, 1);
   TEST_INT(sendbuf[11], 1);

   numHand = getHandleNum("  %M  3 ", sendbuf, "a");
   TEST_INT(numHand, 3);
   TEST_INT(sendbuf[5], 3);
   TEST_INT(sendbuf[4], 0);
   TEST_INT(sendbuf[3], 0);
   TEST_INT(sendbuf[6], 0);


   printf("Finish: getHandleNum\n");
}

void testconvertStrToInt(){
   printf("TEST: convertStrToInt\n");

   TEST_INT(convertStrToInt("1"), 1);
   TEST_INT(convertStrToInt("2"), 2);
   TEST_INT(convertStrToInt("3"), 3);
   TEST_INT(convertStrToInt("4"), 4);
   TEST_INT(convertStrToInt("5"), 5);
   TEST_INT(convertStrToInt("6"), 6);
   TEST_INT(convertStrToInt("a"), -1);
   TEST_INT(convertStrToInt("1a"), 1);
   TEST_INT(convertStrToInt("df"), 1);
   TEST_INT(convertStrToInt("1f"), 1);

   printf("Finish: convertStrToInt\n");
}

void testfillSender(){
   char sendbuf[MAX_SEND_LEN];
   char* str;

   printf("TEST: fillSender\n");

   memset(sendbuf, 0, MAX_SEND_LEN);
   fillSender(sendbuf, "Caleb");
   TEST_INT(sendbuf[3], 5);
   str = sendbuf + 4;
   TEST_STRING(str, "Caleb");

   memset(sendbuf, 0, MAX_SEND_LEN);
   fillSender(sendbuf, "Caleb1");
   TEST_INT(sendbuf[3], 6);
   str = sendbuf + 4;
   TEST_STRING(str, "Caleb1");

   memset(sendbuf, 0, MAX_SEND_LEN);
   fillSender(sendbuf, "b1");
   TEST_INT(sendbuf[3], 2);
   str = sendbuf + 4;
   TEST_STRING(str, "b1");

   printf("Finish: fillSender\n");
}

void testisNumber(){
   printf("TEST: isNumber\n");
   TEST_BOOLEAN(isNumber("1"), TRUE);
   TEST_BOOLEAN(isNumber("0"), TRUE);
   TEST_BOOLEAN(isNumber("2"), TRUE);
   TEST_BOOLEAN(isNumber("3"), TRUE);
   TEST_BOOLEAN(isNumber("3"), TRUE);
   TEST_BOOLEAN(isNumber("4"), TRUE);
   TEST_BOOLEAN(isNumber("5"), TRUE);
   TEST_BOOLEAN(isNumber("6"), TRUE);
   TEST_BOOLEAN(isNumber("asdf"), FALSE);
   TEST_BOOLEAN(isNumber("3324"), FALSE);
   printf("Finish: isNumber\n");
}

void testfindFirstHandle(){
   char* str = NULL;
   str = "%M 1 handle";

   printf("TEST: findFirstHandle\n");

   TEST_STRING(findFirstHandle(str), "handle");

   str = "   %M 2 a";
   TEST_STRING(findFirstHandle(str), "a");

   str = "   %M 2 aa aab";
   TEST_STRING(findFirstHandle(str), "aa aab");

   str = "   %M aa aab";
   TEST_STRING(findFirstHandle(str), "aa aab");

   str = "%L aa aab";
   TEST_STRING(findFirstHandle(str), "aa aab");

   str = "%L bab";
   TEST_STRING(findFirstHandle(str), "bab");

   printf("Finish: findFirstHandle\n");
}

void testfillHandle(){
   char sendbuf[MAX_SEND_LEN];
   char* curVal = NULL;
   char* test = sendbuf;

   printf("TEST: fillHandle\n");

   memset(sendbuf, 0, MAX_SEND_LEN);

   curVal = fillHandle(sendbuf, "Handle");
   TEST_INT(sendbuf[0], 6);
   test += 1;
   TEST_STRING(test, "Handle");

   curVal = fillHandle(curVal, "handle2");
   TEST_INT(sendbuf[7], 7);
   test += strlen("Handle");
   test += 1;
   TEST_STRING(test, "handle2");

   curVal = fillHandle(curVal, "cal");
   TEST_INT(sendbuf[15], 3);
   test += strlen("handle2");
   test += 1;
   TEST_STRING(test, "cal");

   printf("Finish: fillHandle\n");

}

void testfillText(){
   char sendbuf[MAX_SEND_TXT];
   char* curVal = "Hello world";
   char* ret = NULL;

   printf("TEST: fillText\n");

   memset(sendbuf, 0, MAX_SEND_TXT);
   curVal = "Hello world";
   ret = fillText(sendbuf, curVal);
   TEST_STRING(sendbuf, curVal);
   //TEST_CHAR(ret[0], 'd');

   memset(sendbuf, 0, MAX_SEND_TXT);
   curVal = "    another time";
   ret = fillText(sendbuf, curVal);
   TEST_STRING(sendbuf, curVal);
   //TEST_CHAR(ret[0], 'e');

   memset(sendbuf, 0, MAX_SEND_TXT);
   curVal = "    another time\n";
   ret = fillText(sendbuf, curVal);
   TEST_STRING(sendbuf, "    another time");
   //TEST_CHAR(ret[0], 'e');

   memset(sendbuf, 0, MAX_SEND_TXT);
   curVal = "    another time\0";
   ret = fillText(sendbuf, curVal);
   TEST_STRING(sendbuf, curVal);
   //TEST_CHAR(ret[0], 'e');

   ret ++;

   printf("Finish: fillText\n");
}

void testproc_M(){
   char* stdbuf = NULL;
   char sendbuf[MAX_SEND_LEN];
   int pdulen = 0;

   printf("TEST: proc_M\n");

   memset(sendbuf, 0, MAX_SEND_LEN);
   stdbuf = "%M Caleb Text";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   TEST_INT(pdulen, 21);
   TEST_INT(ntohs(((uint16_t*)sendbuf)[0]), pdulen);
   TEST_INT(sendbuf[2], FLAG_5);
   TEST_INT(sendbuf[3], 6);
   TEST_CHAR(sendbuf[4], 's');
   TEST_CHAR(sendbuf[5], 'e');
   TEST_CHAR(sendbuf[6], 'n');
   TEST_CHAR(sendbuf[7], 'd');
   TEST_CHAR(sendbuf[8], 'e');
   TEST_CHAR(sendbuf[9], 'r');
   TEST_INT(sendbuf[10], 1);
   TEST_INT(sendbuf[11], 5);
   TEST_CHAR(sendbuf[12], 'C');
   TEST_CHAR(sendbuf[13], 'a');
   TEST_CHAR(sendbuf[14], 'l');
   TEST_CHAR(sendbuf[15], 'e');
   TEST_CHAR(sendbuf[16], 'b');
   TEST_CHAR(sendbuf[17], 'T');
   TEST_CHAR(sendbuf[18], 'e');
   TEST_CHAR(sendbuf[19], 'x');
   TEST_CHAR(sendbuf[20], 't');
   TEST_CHAR(sendbuf[21], '\0');

   memset(sendbuf, 0, MAX_SEND_LEN);
   stdbuf = "  %M   Caleb   Text";
   pdulen = proc_M(stdbuf, sendbuf, "sender");
   TEST_INT(pdulen, 21);
   TEST_INT(ntohs(((uint16_t*)sendbuf)[0]), pdulen);
   TEST_INT(sendbuf[2], FLAG_5);
   TEST_INT(sendbuf[3], 6);
   TEST_CHAR(sendbuf[4], 's');
   TEST_CHAR(sendbuf[5], 'e');
   TEST_CHAR(sendbuf[6], 'n');
   TEST_CHAR(sendbuf[7], 'd');
   TEST_CHAR(sendbuf[8], 'e');
   TEST_CHAR(sendbuf[9], 'r');
   TEST_INT(sendbuf[10], 1);
   TEST_INT(sendbuf[11], 5);
   TEST_CHAR(sendbuf[12], 'C');
   TEST_CHAR(sendbuf[13], 'a');
   TEST_CHAR(sendbuf[14], 'l');
   TEST_CHAR(sendbuf[15], 'e');
   TEST_CHAR(sendbuf[16], 'b');
   TEST_CHAR(sendbuf[17], 'T');
   TEST_CHAR(sendbuf[18], 'e');
   TEST_CHAR(sendbuf[19], 'x');
   TEST_CHAR(sendbuf[20], 't');
   TEST_CHAR(sendbuf[21], '\0');
   TEST_CHAR(sendbuf[22], '\0');
   TEST_CHAR(sendbuf[23], '\0');

   printf("Finish: proc_M\n");
}

void testproc_E(){
   char sendbuf[100];
   int ret;

   printf("TEST: proc_E\n");
   ret = proc_E(sendbuf);
   TEST_INT(ret, 3);

   TEST_INT(ntohs(((uint16_t*)sendbuf)[0]), 3);
   TEST_INT(sendbuf[2], FLAG_8);

   printf("Finish: proc_E\n");
}
