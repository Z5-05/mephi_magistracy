#!/bin/bash

gcc -mavx -O3 lab1.c -o runner
gcc lab1_only_procedure.c -o runner2

> logger.txt

for var in {0..100}
do
	(./runner $var; ./runner2 $var)  >> logger.txt
	#./runner $var >> logger.txt
done

rm runner runner2
