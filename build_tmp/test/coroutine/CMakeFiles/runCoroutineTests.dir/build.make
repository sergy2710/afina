# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = "/home/sergy/2 sem/c++/afina"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/sergy/2 sem/c++/afina/build_tmp"

# Include any dependencies generated for this target.
include test/coroutine/CMakeFiles/runCoroutineTests.dir/depend.make

# Include the progress variables for this target.
include test/coroutine/CMakeFiles/runCoroutineTests.dir/progress.make

# Include the compile flags for this target's objects.
include test/coroutine/CMakeFiles/runCoroutineTests.dir/flags.make

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o: test/coroutine/CMakeFiles/runCoroutineTests.dir/flags.make
test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o: ../test/coroutine/EngineTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/sergy/2 sem/c++/afina/build_tmp/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o"
	cd "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" && ccache /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o -c "/home/sergy/2 sem/c++/afina/test/coroutine/EngineTest.cpp"

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.i"
	cd "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/sergy/2 sem/c++/afina/test/coroutine/EngineTest.cpp" > CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.i

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.s"
	cd "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/sergy/2 sem/c++/afina/test/coroutine/EngineTest.cpp" -o CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.s

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.requires:

.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.requires

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.provides: test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.requires
	$(MAKE) -f test/coroutine/CMakeFiles/runCoroutineTests.dir/build.make test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.provides.build
.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.provides

test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.provides.build: test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o


# Object files for target runCoroutineTests
runCoroutineTests_OBJECTS = \
"CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o"

# External object files for target runCoroutineTests
runCoroutineTests_EXTERNAL_OBJECTS = \
"/home/sergy/2 sem/c++/afina/build_tmp/third-party/backward-cpp/CMakeFiles/backward_object.dir/backward.cpp.o"

test/coroutine/runCoroutineTests: test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o
test/coroutine/runCoroutineTests: third-party/backward-cpp/CMakeFiles/backward_object.dir/backward.cpp.o
test/coroutine/runCoroutineTests: test/coroutine/CMakeFiles/runCoroutineTests.dir/build.make
test/coroutine/runCoroutineTests: src/coroutine/libCoroutine.a
test/coroutine/runCoroutineTests: third-party/googletest-release-1.8.0/googlemock/gtest/libgtest.a
test/coroutine/runCoroutineTests: third-party/googletest-release-1.8.0/googlemock/gtest/libgtest_main.a
test/coroutine/runCoroutineTests: third-party/googletest-release-1.8.0/googlemock/gtest/libgtest.a
test/coroutine/runCoroutineTests: test/coroutine/CMakeFiles/runCoroutineTests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/sergy/2 sem/c++/afina/build_tmp/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable runCoroutineTests"
	cd "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/runCoroutineTests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/coroutine/CMakeFiles/runCoroutineTests.dir/build: test/coroutine/runCoroutineTests

.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/build

test/coroutine/CMakeFiles/runCoroutineTests.dir/requires: test/coroutine/CMakeFiles/runCoroutineTests.dir/EngineTest.cpp.o.requires

.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/requires

test/coroutine/CMakeFiles/runCoroutineTests.dir/clean:
	cd "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" && $(CMAKE_COMMAND) -P CMakeFiles/runCoroutineTests.dir/cmake_clean.cmake
.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/clean

test/coroutine/CMakeFiles/runCoroutineTests.dir/depend:
	cd "/home/sergy/2 sem/c++/afina/build_tmp" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/sergy/2 sem/c++/afina" "/home/sergy/2 sem/c++/afina/test/coroutine" "/home/sergy/2 sem/c++/afina/build_tmp" "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine" "/home/sergy/2 sem/c++/afina/build_tmp/test/coroutine/CMakeFiles/runCoroutineTests.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : test/coroutine/CMakeFiles/runCoroutineTests.dir/depend

