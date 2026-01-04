#!/bin/sh

clang -Wall -fPIC -g -c cells.c file.c string.c -O3 -fomit-frame-pointer -g
clang -shared -Wl,-soname,libcells.so.1 -o libcells.so.1.0 cells.o file.o string.o
cp libcells.so.1.0 libcells.so

sudo cp libcells.so /usr/local/lib
sudo cp libcells.so /usr/local/lib/libcells.so.1
sudo cp cells.h /usr/local/include
sudo ldconfig

echo "installation of cells finished!"
exit 0
