#!/bin/sh

# 显示执行的命令
set -x

rm -rf ./build
mkdir build
cd build
cmake ..
make