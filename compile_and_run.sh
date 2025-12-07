#! /bin/bash
clang++ -Isrc main_solo.cpp --std=c++2b -O2 -o main_solo.o
./main_solo.o jobs.csv 400 cauchy


# boltzmann, cauchy, logarithmic