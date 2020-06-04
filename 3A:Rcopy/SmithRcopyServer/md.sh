#!/bin/bash

rm ./cmed.txt
time ./rcopy ./med.txt ./cmed.txt 10 1000 .20 unix1.csc.calpoly.edu 6551
diff ./med.txt ./cmed.txt
