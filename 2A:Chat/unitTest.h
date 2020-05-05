#include <stdio.h>
#include <string.h>

#define TEST_ENTRY(_ACTUAL, _EXPECT)\
{\
   HTEntry _actual = _ACTUAL;\
   HTEntry _expect = _EXPECT;\
   if (strcmp(_actual.data, _expect.data) || (_actual.frequency != _expect.frequency)) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "Subbstitution %s, value.data %s, expected.data %s\n",\
         #_ACTUAL, (char* )_actual.data, (char* )_expect.data);\
      fprintf(stderr,"Substitution %s, val.frequency %u,expect.frequency %u\n",\
         #_ACTUAL, _actual.frequency, _expect.frequency);\
      }\
}

#define TEST_AGAINST_NULL(_ACTUAL)\
{\
   void * _actual = _ACTUAL;\
   if (_actual != NULL) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %p, value %p, expected NULL\n",\
         #_ACTUAL, _actual);\
      }\
}

#define TEST_NULL(_ACTUAL, _EXPECT)\
{\
   const char * _actual = _ACTUAL;\
   const char * _expect = _EXPECT;\
   if (_actual != NULL && _expect != NULL) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %s, expected %s\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}

#define TEST_ERROR(_FUNCTION_CALL)\
{\
      _FUNCTION_CALL;\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Expected error detection did not occur\n");\
}

#define TEST_STRING(_ACTUAL, _EXPECT)\
{\
   const char * _actual = _ACTUAL;\
   const char * _expect = _EXPECT;\
   if (strcmp(_actual, _expect)) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %s, expected %s\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}

#define TEST_REAL(_ACTUAL, _EXPECT, _EPSILON)\
{\
   double _actual = _ACTUAL, _expect = _EXPECT, _epsilon = _EPSILON;\
   double _diff = _actual - _expect;\
   if(_diff < 0){\
      _diff = _diff * -1;\
   }\
   if (_diff > _epsilon) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %g, expected %g +/-%g\n",\
         #_ACTUAL, _actual, _expect, _epsilon);\
      }\
}

#define TEST_UNSIGNED(_ACTUAL, _EXPECT)\
{\
   unsigned long _actual = _ACTUAL, _expect = _EXPECT;\
   if (_actual != _expect) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %lu, expected %lu\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}

#define TEST_CHAR(_ACTUAL, _EXPECT)\
{\
   char _actual = _ACTUAL, _expect = _EXPECT;\
   if (_actual != _expect) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value '%c', expected '%c'\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}

#define TEST_BOOLEAN(_ACTUAL, _EXPECT)\
{\
   long _actual = _ACTUAL, _expect = _EXPECT;\
   if (_actual == 0 && _expect != 0) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value false, expected true\n",\
         #_ACTUAL);\
      }\
   if (_actual != 0 && _expect == 0) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value true, expected false\n",\
         #_ACTUAL);\
      }\
}

#define TEST_SIGNED(_ACTUAL,_EXPECT)\
{\
   long _actual = _ACTUAL, _expect = _EXPECT;\
   if (_actual != _expect) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %ld, expected %ld\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}

#define TEST_INT(_ACTUAL,_EXPECT)\
{\
   int _actual = _ACTUAL, _expect = _EXPECT;\
   if (_actual != _expect) {\
      fprintf(stderr, "Failed test in %s at line %d:\n", __FILE__, __LINE__);\
      fprintf(stderr, "   Found substitution %s, value %i, expected %i\n",\
         #_ACTUAL, _actual, _expect);\
      }\
}



