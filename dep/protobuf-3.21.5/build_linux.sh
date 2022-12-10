export CC=gcc
export CXX=g++
cmake3 . -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make -j2
