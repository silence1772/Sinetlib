#!/bin/sh

# 显示执行的命令
set -x
# 先删除上一次编译的东西
rm -rf ./build
# 重建编译目录
mkdir build
# 进入该目录
cd build
# 下面二选一
# 1 因为CMakeLists.txt在该目录的上一级目录，因此路径为..，
#cmake ..
# 2 加上-D选项编译Debug版本
cmake -DCMAKE_BUILD_TYPE=Debug ..
# 执行cmake命令生成的Makefile文件
make
# 安装生成的库和头文件到系统目录
sudo make install