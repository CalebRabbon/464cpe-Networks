
void testconvertCharType(){
// define TEST_INT(_ACTUAL, _EXPECT)
   int type = 0;

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
}
