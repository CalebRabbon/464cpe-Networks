#!/bin/bash

rm ./cbig.txt
time ./rcopy ./big.txt ./cbig.txt 50 1000 .10 unix1.csc.calpoly.edu 7551
diff ./big.txt ./cbig.txt
