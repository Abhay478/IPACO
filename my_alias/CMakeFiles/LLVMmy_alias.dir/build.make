# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/abhay/IPACO

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abhay/IPACO

# Include any dependencies generated for this target.
include my_alias/CMakeFiles/LLVMmy_alias.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include my_alias/CMakeFiles/LLVMmy_alias.dir/compiler_depend.make

# Include the progress variables for this target.
include my_alias/CMakeFiles/LLVMmy_alias.dir/progress.make

# Include the compile flags for this target's objects.
include my_alias/CMakeFiles/LLVMmy_alias.dir/flags.make

my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o: my_alias/CMakeFiles/LLVMmy_alias.dir/flags.make
my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o: my_alias/alias_lib.cpp
my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o: my_alias/CMakeFiles/LLVMmy_alias.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/abhay/IPACO/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o"
	cd /home/abhay/IPACO/my_alias && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o -MF CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o.d -o CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o -c /home/abhay/IPACO/my_alias/alias_lib.cpp

my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.i"
	cd /home/abhay/IPACO/my_alias && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abhay/IPACO/my_alias/alias_lib.cpp > CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.i

my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.s"
	cd /home/abhay/IPACO/my_alias && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abhay/IPACO/my_alias/alias_lib.cpp -o CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.s

# Object files for target LLVMmy_alias
LLVMmy_alias_OBJECTS = \
"CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o"

# External object files for target LLVMmy_alias
LLVMmy_alias_EXTERNAL_OBJECTS =

my_alias/libLLVMmy_alias.so: my_alias/CMakeFiles/LLVMmy_alias.dir/alias_lib.cpp.o
my_alias/libLLVMmy_alias.so: my_alias/CMakeFiles/LLVMmy_alias.dir/build.make
my_alias/libLLVMmy_alias.so: my_alias/CMakeFiles/LLVMmy_alias.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/abhay/IPACO/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module libLLVMmy_alias.so"
	cd /home/abhay/IPACO/my_alias && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMmy_alias.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
my_alias/CMakeFiles/LLVMmy_alias.dir/build: my_alias/libLLVMmy_alias.so
.PHONY : my_alias/CMakeFiles/LLVMmy_alias.dir/build

my_alias/CMakeFiles/LLVMmy_alias.dir/clean:
	cd /home/abhay/IPACO/my_alias && $(CMAKE_COMMAND) -P CMakeFiles/LLVMmy_alias.dir/cmake_clean.cmake
.PHONY : my_alias/CMakeFiles/LLVMmy_alias.dir/clean

my_alias/CMakeFiles/LLVMmy_alias.dir/depend:
	cd /home/abhay/IPACO && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abhay/IPACO /home/abhay/IPACO/my_alias /home/abhay/IPACO /home/abhay/IPACO/my_alias /home/abhay/IPACO/my_alias/CMakeFiles/LLVMmy_alias.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : my_alias/CMakeFiles/LLVMmy_alias.dir/depend

