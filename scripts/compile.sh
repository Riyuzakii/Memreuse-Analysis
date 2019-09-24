#!/usr/bin/env bash
mkdir -p ../obj ../output

gcc -O3 -static -pthread ../target/prog1.c -o ../obj/prog1
gcc -O3 -static -pthread ../target/prog2.c -o ../obj/prog2
gcc -O3 -static -pthread ../target/prog3.c -o ../obj/prog3
gcc -O3 -static -pthread ../target/prog4.c -o ../obj/prog4

