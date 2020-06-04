#!/bin/bash

rm ./csmall.txt
time ./rcopy ./small.txt ./csmall.txt 100 1000 .20 unix1.csc.calpoly.edu 4551
#diff ./small.txt ./csmall.txt
