#include "test.h"
#include "macros.h"
#include "flags.h"
#include "myClient.h"

// File used to test the parse functions

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
