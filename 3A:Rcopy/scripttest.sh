#!/bin/bash


rm ./csmall.txt
time ./rcopy ./small.txt ./csmall.txt 10 1000 .20 unix1.csc.calpoly.edu 4551
diff ./small.txt ./csmall.txt

#rm ./cmed.txt
#time ./rcopy ./med.txt ./cmed.txt 10 1000 .20 unix1.csc.calpoly.edu 4551
#diff ./med.txt ./cmed.txt
#
#rm ./cbig.txt
#time ./rcopy ./big.txt ./cbig.txt 50 1000 .10 unix1.csc.calpoly.edu 6551
#diff ./big.txt ./cbig.txt
#
#rm ./cbig.txt
#time ./rcopy ./big.txt ./cbig.txt 5 1000 .15 unix1.csc.calpoly.edu 7551
#diff ./big.txt ./cbig.txt
