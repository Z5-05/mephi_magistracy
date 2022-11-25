#!/bin/bash

gcc -mavx -O3 lab1.c -o runner

> logger.txt

for var in {0..100}
do
    ./runner $var >> logger.txt
done