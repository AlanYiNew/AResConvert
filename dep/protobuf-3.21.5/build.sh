export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-c++
export LD=x86_64-w64-mingw32-ld
export AR=x86_64-w64-mingw32-ar
export AS=x86_64-w64-mingw32-as
export NM=x86_64-w64-mingw32-nm
export STRIP=x86_64-w64-mingw32-strip
export RANLIB=x86_64-w64-mingw32-ranlib
export DLLTOOL=x86_64-w64-mingw32-dlltool
export OBJDUMP=x86_64-w64-mingw32-objdump
export RESCOMP=x86_64-w64-mingw32-windres
echo $CC
echo $CXX
cmake3 . -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-c++
make -j2
