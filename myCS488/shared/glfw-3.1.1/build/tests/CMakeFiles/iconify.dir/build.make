# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /u2/skragib/cs488/cs488/shared/glfw-3.1.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build

# Include any dependencies generated for this target.
include tests/CMakeFiles/iconify.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/iconify.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/iconify.dir/flags.make

tests/CMakeFiles/iconify.dir/iconify.c.o: tests/CMakeFiles/iconify.dir/flags.make
tests/CMakeFiles/iconify.dir/iconify.c.o: ../tests/iconify.c
	$(CMAKE_COMMAND) -E cmake_progress_report /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object tests/CMakeFiles/iconify.dir/iconify.c.o"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/iconify.dir/iconify.c.o   -c /u2/skragib/cs488/cs488/shared/glfw-3.1.1/tests/iconify.c

tests/CMakeFiles/iconify.dir/iconify.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/iconify.dir/iconify.c.i"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /u2/skragib/cs488/cs488/shared/glfw-3.1.1/tests/iconify.c > CMakeFiles/iconify.dir/iconify.c.i

tests/CMakeFiles/iconify.dir/iconify.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/iconify.dir/iconify.c.s"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /u2/skragib/cs488/cs488/shared/glfw-3.1.1/tests/iconify.c -o CMakeFiles/iconify.dir/iconify.c.s

tests/CMakeFiles/iconify.dir/iconify.c.o.requires:
.PHONY : tests/CMakeFiles/iconify.dir/iconify.c.o.requires

tests/CMakeFiles/iconify.dir/iconify.c.o.provides: tests/CMakeFiles/iconify.dir/iconify.c.o.requires
	$(MAKE) -f tests/CMakeFiles/iconify.dir/build.make tests/CMakeFiles/iconify.dir/iconify.c.o.provides.build
.PHONY : tests/CMakeFiles/iconify.dir/iconify.c.o.provides

tests/CMakeFiles/iconify.dir/iconify.c.o.provides.build: tests/CMakeFiles/iconify.dir/iconify.c.o

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o: tests/CMakeFiles/iconify.dir/flags.make
tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o: ../deps/getopt.c
	$(CMAKE_COMMAND) -E cmake_progress_report /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/iconify.dir/__/deps/getopt.c.o   -c /u2/skragib/cs488/cs488/shared/glfw-3.1.1/deps/getopt.c

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/iconify.dir/__/deps/getopt.c.i"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /u2/skragib/cs488/cs488/shared/glfw-3.1.1/deps/getopt.c > CMakeFiles/iconify.dir/__/deps/getopt.c.i

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/iconify.dir/__/deps/getopt.c.s"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /u2/skragib/cs488/cs488/shared/glfw-3.1.1/deps/getopt.c -o CMakeFiles/iconify.dir/__/deps/getopt.c.s

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.requires:
.PHONY : tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.requires

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.provides: tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.requires
	$(MAKE) -f tests/CMakeFiles/iconify.dir/build.make tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.provides.build
.PHONY : tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.provides

tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.provides.build: tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o

# Object files for target iconify
iconify_OBJECTS = \
"CMakeFiles/iconify.dir/iconify.c.o" \
"CMakeFiles/iconify.dir/__/deps/getopt.c.o"

# External object files for target iconify
iconify_EXTERNAL_OBJECTS =

tests/iconify: tests/CMakeFiles/iconify.dir/iconify.c.o
tests/iconify: tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o
tests/iconify: tests/CMakeFiles/iconify.dir/build.make
tests/iconify: src/libglfw3.a
tests/iconify: /usr/lib/x86_64-linux-gnu/libX11.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libXrandr.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libXinerama.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libXi.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libXxf86vm.so
tests/iconify: /usr/lib/x86_64-linux-gnu/librt.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libm.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libXcursor.so
tests/iconify: /usr/lib/x86_64-linux-gnu/libGL.so
tests/iconify: tests/CMakeFiles/iconify.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable iconify"
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iconify.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/iconify.dir/build: tests/iconify
.PHONY : tests/CMakeFiles/iconify.dir/build

tests/CMakeFiles/iconify.dir/requires: tests/CMakeFiles/iconify.dir/iconify.c.o.requires
tests/CMakeFiles/iconify.dir/requires: tests/CMakeFiles/iconify.dir/__/deps/getopt.c.o.requires
.PHONY : tests/CMakeFiles/iconify.dir/requires

tests/CMakeFiles/iconify.dir/clean:
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests && $(CMAKE_COMMAND) -P CMakeFiles/iconify.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/iconify.dir/clean

tests/CMakeFiles/iconify.dir/depend:
	cd /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /u2/skragib/cs488/cs488/shared/glfw-3.1.1 /u2/skragib/cs488/cs488/shared/glfw-3.1.1/tests /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests /u2/skragib/cs488/cs488/shared/glfw-3.1.1/build/tests/CMakeFiles/iconify.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/iconify.dir/depend

