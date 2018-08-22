#! /bin/bash

rm -rf own_business 2>/dev/null
mkdir -p own_business/lib/include

cp -r source/include own_business/lib/
make -C source
cp source/libwebserver.a own_business/lib
make clean -C source

cp example/* own_business -r
