# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake3

# The command to remove a file.
RM = /usr/bin/cmake3 -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX

# Include any dependencies generated for this target.
include Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/depend.make

# Include the progress variables for this target.
include Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/progress.make

# Include the compile flags for this target's objects.
include Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/flags.make

Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/flags.make
Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/includes_CXX.rsp
Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj: Benchmarks/Benchmark.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj"
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks && /home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj -c /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks/Benchmark.cpp

Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.i"
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks && /home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks/Benchmark.cpp > CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.i

Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.s"
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks && /home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks/Benchmark.cpp -o CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.s

# Object files for target OpenXLSXBenchmark
OpenXLSXBenchmark_OBJECTS = \
"CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj"

# External object files for target OpenXLSXBenchmark
OpenXLSXBenchmark_EXTERNAL_OBJECTS =

output/windows/OpenXLSXBenchmark.exe: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/Benchmark.cpp.obj
output/windows/OpenXLSXBenchmark.exe: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/build.make
output/windows/OpenXLSXBenchmark.exe: output/windows/libbenchmark.a
output/windows/OpenXLSXBenchmark.exe: output/windows/libbenchmark_main.a
output/windows/OpenXLSXBenchmark.exe: output/windows/libOpenXLSX.a
output/windows/OpenXLSXBenchmark.exe: output/windows/libbenchmark.a
output/windows/OpenXLSXBenchmark.exe: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/linklibs.rsp
output/windows/OpenXLSXBenchmark.exe: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/objects1.rsp
output/windows/OpenXLSXBenchmark.exe: Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../output/windows/OpenXLSXBenchmark.exe"
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/OpenXLSXBenchmark.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/build: output/windows/OpenXLSXBenchmark.exe

.PHONY : Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/build

Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/clean:
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks && $(CMAKE_COMMAND) -P CMakeFiles/OpenXLSXBenchmark.dir/cmake_clean.cmake
.PHONY : Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/clean

Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/depend:
	cd /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks /home/lighthouse/work/cppworkspace/AResConvert/thirdparty/OpenXLSX/Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Benchmarks/CMakeFiles/OpenXLSXBenchmark.dir/depend
