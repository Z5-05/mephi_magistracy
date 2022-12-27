#!/bin/bash

g++ -o procedure procedure.cpp
g++ -fopenmp -o openmp openmp.cpp
g++ -pthread -o posix posix.cpp

> logger.txt

for var in `seq 1000000 1 1000200`
do
	./procedure $var >> logger.txt
done

for var in `seq 1000000 1 1000200`
do
	./posix $var >> logger.txt
done

for var in `seq 1000000 1 1000200`
do
	./openmp $var >> logger.txt
done

rm procedure posix openmp
