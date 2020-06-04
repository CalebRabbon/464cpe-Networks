#!/bin/bash

#rm ./csmall.txt
#time ./rcopy ./small.txt ./csmall.txt 10 1000 .20 unix1.csc.calpoly.edu 4554
#diff ./small.txt ./csmall.txt

rm ./csmall.txt
time ./rcopy ./big.txt ./cbig.txt 5 1000 .15 unix1.csc.calpoly.edu 4554
diff ./big.txt ./cbig.txt
