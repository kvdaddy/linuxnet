# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zhanglm/work/network

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zhanglm/work/network/build

# Include any dependencies generated for this target.
include CMakeFiles/ex9_7.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ex9_7.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ex9_7.dir/flags.make

CMakeFiles/ex9_7.dir/ex9_7.c.o: CMakeFiles/ex9_7.dir/flags.make
CMakeFiles/ex9_7.dir/ex9_7.c.o: ../ex9_7.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zhanglm/work/network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/ex9_7.dir/ex9_7.c.o"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/ex9_7.dir/ex9_7.c.o   -c /home/zhanglm/work/network/ex9_7.c

CMakeFiles/ex9_7.dir/ex9_7.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/ex9_7.dir/ex9_7.c.i"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zhanglm/work/network/ex9_7.c > CMakeFiles/ex9_7.dir/ex9_7.c.i

CMakeFiles/ex9_7.dir/ex9_7.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/ex9_7.dir/ex9_7.c.s"
	/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zhanglm/work/network/ex9_7.c -o CMakeFiles/ex9_7.dir/ex9_7.c.s

CMakeFiles/ex9_7.dir/ex9_7.c.o.requires:

.PHONY : CMakeFiles/ex9_7.dir/ex9_7.c.o.requires

CMakeFiles/ex9_7.dir/ex9_7.c.o.provides: CMakeFiles/ex9_7.dir/ex9_7.c.o.requires
	$(MAKE) -f CMakeFiles/ex9_7.dir/build.make CMakeFiles/ex9_7.dir/ex9_7.c.o.provides.build
.PHONY : CMakeFiles/ex9_7.dir/ex9_7.c.o.provides

CMakeFiles/ex9_7.dir/ex9_7.c.o.provides.build: CMakeFiles/ex9_7.dir/ex9_7.c.o


# Object files for target ex9_7
ex9_7_OBJECTS = \
"CMakeFiles/ex9_7.dir/ex9_7.c.o"

# External object files for target ex9_7
ex9_7_EXTERNAL_OBJECTS =

ex9_7: CMakeFiles/ex9_7.dir/ex9_7.c.o
ex9_7: CMakeFiles/ex9_7.dir/build.make
ex9_7: CMakeFiles/ex9_7.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zhanglm/work/network/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ex9_7"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ex9_7.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ex9_7.dir/build: ex9_7

.PHONY : CMakeFiles/ex9_7.dir/build

CMakeFiles/ex9_7.dir/requires: CMakeFiles/ex9_7.dir/ex9_7.c.o.requires

.PHONY : CMakeFiles/ex9_7.dir/requires

CMakeFiles/ex9_7.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ex9_7.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ex9_7.dir/clean

CMakeFiles/ex9_7.dir/depend:
	cd /home/zhanglm/work/network/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zhanglm/work/network /home/zhanglm/work/network /home/zhanglm/work/network/build /home/zhanglm/work/network/build /home/zhanglm/work/network/build/CMakeFiles/ex9_7.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ex9_7.dir/depend

