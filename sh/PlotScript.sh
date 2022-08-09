#!/bin/bash
cd ..

mkdir -p bin

g++ -Wall -fPIC -I./include `root-config --cflags` Integration.cpp -o ./bin/Integration  `root-config --glibs` ./include/Broad.h
g++ -Wall -fPIC -I./include `root-config --cflags` Broadening.cpp  -o ./bin/Broadening  `root-config --glibs` ./include/Integration.h

cd bin

./Integration
./Broadening

cd ..

python3 Plot.py
