#!/bin/bash

#rm ./ctiny.txt
#echo "Client Windows Size 10"
#echo "       Buffer  Size 1000"
#echo "       Error %      .2"
#time ./rcopy ./tiny.txt ./ctiny.txt 10 1000 .20 unix2.csc.calpoly.edu 4554
#diff ./tiny.txt ./ctiny.txt

rm ./csmall.txt
echo "Client Windows Size 10"
echo "       Buffer  Size 1000"
echo "       Error %      .2"
time ./rcopy ./small.txt ./csmall.txt 10 1000 .20 unix2.csc.calpoly.edu 4554

diff ./small.txt ./csmall.txt

#echo "Client Windows Size 5"
#echo "       Buffer  Size 10"
#echo "       Error %      0.0"
#./rcopy ./small.txt ./csmall.txt 5 10 .0 unix2.csc.calpoly.edu 4554

#echo "Client Windows Size 5"
#echo "       Buffer  Size 10"
#echo "       Error %      0.0"
#./rcopy ./t.txt ./2t.txt 5 10 .0 unix2.csc.calpoly.edu 4554
#
#echo "Client Windows Size 1"
#echo "       Buffer  Size 10"
#echo "       Error %      0.0"
#./rcopy ./t.txt ./2t.txt 1 10 .0 unix2.csc.calpoly.edu 4554
#
#echo "Client Windows Size 1"
#echo "       Buffer  Size 100"
#echo "       Error %      0.0"
#./rcopy ./t.txt ./2t.txt 1 100 .0 unix2.csc.calpoly.edu 4554
