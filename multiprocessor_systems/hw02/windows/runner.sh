#!/bin/bash

g++ -O0 -o win32 win32.cpp 
> logger_win32.txt

for var in `seq 1000000 1 1000200`
do
	./win32 $var >> logger_win32.txt
done

rm win32