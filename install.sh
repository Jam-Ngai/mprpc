#!/bin/bash

set -e

if [ ! -d /usr/include/mprpc ]; then
  mkdir /usr/include/mprpc
fi

cd $(pwd)/include
for header in $(ls *.h); do
  cp $header /usr/include/mprpc
done
cd ..

if [ ! -d /usr/lib/mprpc ]; then
  mkdir /usr/lib/mprpc
fi

cp $(pwd)/lib/libmprpc.so /usr/lib/mprpc

export LD_LIBRARY_PATH=/usr/lib/mprpc:$LD_LIBRARY_PATH

ldconfig

echo "Build completed!"