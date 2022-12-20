#!/bin/bash

gcc -O0 procedure.c -o procedure
gcc -pthread -O0 posix.c -o posix
gcc -fopenmp -O0 openmp.c -o openmp

> logger.txt

START=1000000
END=1002000
TOTAL=$(( $END - $START ))

for (( var=$START; var<=$END; var++ ))
do
	(./procedure $var; ./posix $var; ./openmp $var)  >> logger.txt
    echo $var
done | tqdm --total $TOTAL >> /dev/null

rm procedure posix openmp