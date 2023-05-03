#!/bin/bash
# set -x调试选项，可以帮助我们识别处运行过程中出行的问题
set -x

rm -rf `pwd`/build/*
# && 连接两个命令，只有第一个执行后，后面的才执行
# 执行完毕后删除build下的东西
cd `pwd`/build &&
    cmake .. &&
    make &&
    rm -rf *
