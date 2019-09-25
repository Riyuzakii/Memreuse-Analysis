#
# Copyright (c) 2019, Aditya Rohan
# Copyright (c) 2019, Aniket Pandey
#

CC=g++
CFLAGS= -O3 -static -pthread
CXXFLAGS= -lstdc++fs -std=c++11

all: clean setup compile

setup: prenip part2 part3 part4
	$(CC) ../target/prog1.c -o ../obj/prog1 $(CFLAGS) 
	$(CC) ../target/prog2.c -o ../obj/prog2 $(CFLAGS) 
	$(CC) ../target/prog3.c -o ../obj/prog3 $(CFLAGS) 
	$(CC) ../target/prog4.c -o ../obj/prog4 $(CFLAGS)

prenip:
	cd .. && $(MAKE) obj-intel64/addrtrace.so
	mkdir -p ../obj ../traces ../result

run:
	./run.sh $(PART) $(PROG)

.PHONY: clean cleanall

part2: ../part2.cpp
	$(CC) -o ../obj/$@ $< $(CXXFLAGS)

part3: ../part3.cpp
	$(CC) -o ../obj/$@ $< $(CXXFLAGS)

part4: ../part4.cpp
	$(CC) -o ../obj/$@ $< $(CXXFLAGS)

clean:
	rm -rf ../obj ../obj-intel64 ../part2 ../part3 ../part4

cleanall: clean
	rm -rf ../traces
