#!/bin/bash

g++ -O0 procedure.cpp -o procedure
g++ -pthread -O0 posix.cpp -o posix
g++ -fopenmp -O0 openmp.cpp -o openmp

> logger.txt

START=1000000
END=1002000
TOTAL=$(( $END - $START ))

for (( var=$START; var<=$END; var++ ))
do
	./procedure $var >> logger.txt
    ./posix $var >> logger.txt
    ./openmp $var >> logger.txt
    echo $var
done | tqdm --total $TOTAL >> /dev/null

rm procedure posix openmp