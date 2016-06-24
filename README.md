# Hotfields
$ cd Hotfields-master
$ mkdir build
$ cd build
$ cmake ..
$ make
$ (create something.c file )
$ clang -c -emit-llvm something.c -o something.bc
$ opt -load hot/libhotPass.so -printhotfields -disable-output something.bc
