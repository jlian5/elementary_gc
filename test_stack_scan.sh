gcc -g -fno-omit-frame-pointer stack_scan.c libs/libset-pthread.a libs/libvector-pthread.a libs/libcompare-pthread.a libs/libcallbacks-pthread.a -o stack_scan


echo ----------------------------BELOW IS mreplace-------------------

./mreplace ./stack_scan

echo ----------------------------BELOW IS GLIBC-------------------

# valgrind   --leak-check=full ./stack_scan
./stack_scan

rm stack_scan