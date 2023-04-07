set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_HOST_WIN32 ON)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(TARGET_OS Windows)

#set(CMAKE_FIND_ROOT_PATH /path/to/target/rootfs)
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#set(CMAKE_SYSROOT /path/to/target/sysroot)

#set(CMAKE_C_FLAGS "--sysroot=${CMAKE_SYSROOT} -march=armv7-a -mfpu=neon -mfloat-abi=hard")
#set(CMAKE_CXX_FLAGS "--sysroot=${CMAKE_SYSROOT} -march=armv7-a -mfpu=neon -mfloat-abi=hard"
