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
CMAKE_SOURCE_DIR = /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5

# Include any dependencies generated for this target.
include CMakeFiles/protoc.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/protoc.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/protoc.dir/flags.make

CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj: CMakeFiles/protoc.dir/flags.make
CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj: CMakeFiles/protoc.dir/includes_CXX.rsp
CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj: src/google/protobuf/compiler/main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj"
	/home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj -c /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/src/google/protobuf/compiler/main.cc

CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.i"
	/home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/src/google/protobuf/compiler/main.cc > CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.i

CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.s"
	/home/lighthouse/.zeranoe/mingw-w64/x86_64/bin/x86_64-w64-mingw32-c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/src/google/protobuf/compiler/main.cc -o CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.s

# Object files for target protoc
protoc_OBJECTS = \
"CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj"

# External object files for target protoc
protoc_EXTERNAL_OBJECTS =

protoc.exe-3.21.5.0: CMakeFiles/protoc.dir/src/google/protobuf/compiler/main.cc.obj
protoc.exe-3.21.5.0: CMakeFiles/protoc.dir/build.make
protoc.exe-3.21.5.0: libprotoc.a
protoc.exe-3.21.5.0: libprotobuf.a
protoc.exe-3.21.5.0: CMakeFiles/protoc.dir/linklibs.rsp
protoc.exe-3.21.5.0: CMakeFiles/protoc.dir/objects1.rsp
protoc.exe-3.21.5.0: CMakeFiles/protoc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable protoc.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protoc.dir/link.txt --verbose=$(VERBOSE)
	$(CMAKE_COMMAND) -E cmake_symlink_executable protoc.exe-3.21.5.0 protoc.exe

protoc.exe: protoc.exe-3.21.5.0


# Rule to build all files generated by this target.
CMakeFiles/protoc.dir/build: protoc.exe

.PHONY : CMakeFiles/protoc.dir/build

CMakeFiles/protoc.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/protoc.dir/cmake_clean.cmake
.PHONY : CMakeFiles/protoc.dir/clean

CMakeFiles/protoc.dir/depend:
	cd /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5 /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5 /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5 /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5 /home/lighthouse/work/cppworkspace/AResConvert/dep/protobuf-3.21.5/CMakeFiles/protoc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/protoc.dir/depend

