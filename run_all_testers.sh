#!/bin/bash
set -e

make 
make -f Makefile

set +e

for i in {1..13};
do
    echo "running tester ${i}"
    testers_exe/tester-${i}
    echo -e "\n"
done

