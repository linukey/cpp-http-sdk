#! /bin/bash

rm -rf server-tool

mkdir server-tool
mkdir server-tool/lib
mkdir server-tool/lib/include
mkdir server-tool/control

cp -r source/include server-tool/lib/
make -C source
cp source/libwebserver.a server-tool/lib
make clean -C source

cp example/main.cpp server-tool/control
cp example/Makefile server-tool
