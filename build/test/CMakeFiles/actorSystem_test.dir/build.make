# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /var/lib/snapd/snap/cmake/775/bin/cmake

# The command to remove a file.
RM = /var/lib/snapd/snap/cmake/775/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cezary/Studia/PW/pw2-C

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cezary/Studia/PW/pw2-C/build

# Include any dependencies generated for this target.
include test/CMakeFiles/actorSystem_test.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/actorSystem_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/actorSystem_test.dir/flags.make

test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o: test/CMakeFiles/actorSystem_test.dir/flags.make
test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o: ../test/actorSystem_test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cezary/Studia/PW/pw2-C/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o"
	cd /home/cezary/Studia/PW/pw2-C/build/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o -c /home/cezary/Studia/PW/pw2-C/test/actorSystem_test.c

test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/actorSystem_test.dir/actorSystem_test.c.i"
	cd /home/cezary/Studia/PW/pw2-C/build/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/cezary/Studia/PW/pw2-C/test/actorSystem_test.c > CMakeFiles/actorSystem_test.dir/actorSystem_test.c.i

test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/actorSystem_test.dir/actorSystem_test.c.s"
	cd /home/cezary/Studia/PW/pw2-C/build/test && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/cezary/Studia/PW/pw2-C/test/actorSystem_test.c -o CMakeFiles/actorSystem_test.dir/actorSystem_test.c.s

# Object files for target actorSystem_test
actorSystem_test_OBJECTS = \
"CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o"

# External object files for target actorSystem_test
actorSystem_test_EXTERNAL_OBJECTS =

test/actorSystem_test: test/CMakeFiles/actorSystem_test.dir/actorSystem_test.c.o
test/actorSystem_test: test/CMakeFiles/actorSystem_test.dir/build.make
test/actorSystem_test: libcacti.a
test/actorSystem_test: test/CMakeFiles/actorSystem_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cezary/Studia/PW/pw2-C/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable actorSystem_test"
	cd /home/cezary/Studia/PW/pw2-C/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/actorSystem_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/actorSystem_test.dir/build: test/actorSystem_test

.PHONY : test/CMakeFiles/actorSystem_test.dir/build

test/CMakeFiles/actorSystem_test.dir/clean:
	cd /home/cezary/Studia/PW/pw2-C/build/test && $(CMAKE_COMMAND) -P CMakeFiles/actorSystem_test.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/actorSystem_test.dir/clean

test/CMakeFiles/actorSystem_test.dir/depend:
	cd /home/cezary/Studia/PW/pw2-C/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cezary/Studia/PW/pw2-C /home/cezary/Studia/PW/pw2-C/test /home/cezary/Studia/PW/pw2-C/build /home/cezary/Studia/PW/pw2-C/build/test /home/cezary/Studia/PW/pw2-C/build/test/CMakeFiles/actorSystem_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/actorSystem_test.dir/depend
