# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /home/jon/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6494.38/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/jon/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6494.38/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/sidlib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sidlib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sidlib.dir/flags.make

CMakeFiles/sidlib.dir/sidlib.c.o: CMakeFiles/sidlib.dir/flags.make
CMakeFiles/sidlib.dir/sidlib.c.o: ../sidlib.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/sidlib.dir/sidlib.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/sidlib.dir/sidlib.c.o   -c /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/sidlib.c

CMakeFiles/sidlib.dir/sidlib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sidlib.dir/sidlib.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/sidlib.c > CMakeFiles/sidlib.dir/sidlib.c.i

CMakeFiles/sidlib.dir/sidlib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sidlib.dir/sidlib.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/sidlib.c -o CMakeFiles/sidlib.dir/sidlib.c.s

# Object files for target sidlib
sidlib_OBJECTS = \
"CMakeFiles/sidlib.dir/sidlib.c.o"

# External object files for target sidlib
sidlib_EXTERNAL_OBJECTS =

sidlib: CMakeFiles/sidlib.dir/sidlib.c.o
sidlib: CMakeFiles/sidlib.dir/build.make
sidlib: CMakeFiles/sidlib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable sidlib"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sidlib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sidlib.dir/build: sidlib

.PHONY : CMakeFiles/sidlib.dir/build

CMakeFiles/sidlib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sidlib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sidlib.dir/clean

CMakeFiles/sidlib.dir/depend:
	cd /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug /home/jon/Documents/prog/epfl/pps20-projet-c-la-vie/done/cmake-build-debug/CMakeFiles/sidlib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sidlib.dir/depend

