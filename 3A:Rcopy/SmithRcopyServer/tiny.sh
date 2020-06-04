#!/bin/bash

rm ./ctiny.txt
time ./rcopy ./tiny.txt ./ctiny.txt 10 1000 .28 unix1.csc.calpoly.edu 4554
diff ./tiny.txt ./ctiny.txt
