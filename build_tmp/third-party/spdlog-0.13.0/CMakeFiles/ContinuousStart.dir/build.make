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

# Utility rule file for ContinuousStart.

# Include the progress variables for this target.
include third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/progress.make

third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart:
	cd "/home/sergy/2 sem/c++/afina/build_tmp/third-party/spdlog-0.13.0" && /usr/bin/ctest -D ContinuousStart

ContinuousStart: third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart
ContinuousStart: third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/build.make

.PHONY : ContinuousStart

# Rule to build all files generated by this target.
third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/build: ContinuousStart

.PHONY : third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/build

third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/clean:
	cd "/home/sergy/2 sem/c++/afina/build_tmp/third-party/spdlog-0.13.0" && $(CMAKE_COMMAND) -P CMakeFiles/ContinuousStart.dir/cmake_clean.cmake
.PHONY : third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/clean

third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/depend:
	cd "/home/sergy/2 sem/c++/afina/build_tmp" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/sergy/2 sem/c++/afina" "/home/sergy/2 sem/c++/afina/third-party/spdlog-0.13.0" "/home/sergy/2 sem/c++/afina/build_tmp" "/home/sergy/2 sem/c++/afina/build_tmp/third-party/spdlog-0.13.0" "/home/sergy/2 sem/c++/afina/build_tmp/third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : third-party/spdlog-0.13.0/CMakeFiles/ContinuousStart.dir/depend

