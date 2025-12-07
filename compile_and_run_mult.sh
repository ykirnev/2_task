#! /bin/bash
clang++ -Isrc main_mult.cpp --std=c++23 -O2 -o main_mult.o
./main_mult.o 2