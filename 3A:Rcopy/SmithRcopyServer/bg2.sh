#!/bin/bash

rm ./c2big.txt
time ./rcopy ./2big.txt ./c2big.txt 5 1000 .15 unix1.csc.calpoly.edu 8551
diff ./2big.txt ./c2big.txt
