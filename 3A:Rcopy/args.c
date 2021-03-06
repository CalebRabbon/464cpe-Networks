#include <string.h>
#include <stdlib.h>
#include "args.h"

void printArgs(Args* args){
   printf("Printing the Arguements\n");
   printf("From file: %s\n", args->fromFile);
   printf("To File: %s\n", args->toFile);
   printf("Window size: %i\n", args->windowSize);
   printf("Buffer size: %i\n", args->bufferSize);
   printf("Percent error: %f\n", args->percentError);
   printf("Remote machine: %s\n", args->remoteMachine);
   printf("Remote port: %i\n", args->remotePort);
}

// Checks the number of arguments and assignems them to the Args*
void checkArgs(int argc, char * argv[], Args* args)
{
	/* check command line arguments  */
	if (argc != 8)
	{
      printf("usage: %s from-filename to-filename window-size buffer-size error-percent remote-machine remote-port\n", argv[0]);
		exit(1);
	}
	if (strlen(argv[1]) > 100)
   {
      printf("FROM filename to long needs to be less than 100 and is: %d\n", (int)strlen(argv[1]));
      exit(-1);
   }
   if (strlen(argv[2]) > 100)
   {
      printf("TO filename to long needs to be less than 100 and is: %d\n", (int)strlen(argv[1]));
      exit(-1);
   }

   if (atoi(argv[4]) < 1 || atoi(argv[4]) > 1400)
   {
      printf("Buffer size needs to be between 400 and 1400 and is: %d\n", atoi(argv[4]));
      exit(-1);
   }

   if (atoi(argv[3]) < 1)
   {
      printf("Window size needs to be greater than 1 and is: %d\n", atoi(argv[3]));
      exit(-1);
   }
   args->fromFile = argv[1];
   args->toFile = argv[2];
   args->windowSize = atoi(argv[3]);
   args->bufferSize = atoi(argv[4]);
   args->percentError = atof(argv[5]);
   args->remoteMachine = argv[6];
   args->remotePort = atoi(argv[7]);

   if((args->percentError >= 1.0) || (args->percentError < 0.0)){
		printf("usage: error-percent: %s, must be between 0 and less than 1\n", argv[3]);
		exit(1);
   }

}
