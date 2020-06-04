#!/bin/bash

rm ./cbig.txt
time ./rcopy ./big.txt ./cbig.txt 5 1000 .15 unix1.csc.calpoly.edu 8551
diff ./big.txt ./cbig.txt
