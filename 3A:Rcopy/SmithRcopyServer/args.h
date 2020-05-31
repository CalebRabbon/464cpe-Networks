// Creates a structure to hold all the variables for the arguments passed into
// rcopy
// Created by Caleb Rabbon 5/26/2020

#ifndef __ARGS_H__
#define __ARGS_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct Args{
   char* fromFile;
   char* toFile;
   int windowSize;
   int bufferSize;
   float percentError;
   char* remoteMachine;
   int remotePort;
} Args;

void printArgs(Args* args);

void checkArgs(int argc, char * argv[], Args* args);

#endif
