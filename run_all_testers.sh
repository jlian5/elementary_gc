#!/bin/bash
set -e

make clean
make -f Makefile debug

set +e

for i in {1..12};
do
    echo "running tester ${i}"
    ./mreplace testers_exe/tester-${i}-debug
    echo "return value is"
    echo $?
    echo -e "\n"
done

