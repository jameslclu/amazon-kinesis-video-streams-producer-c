# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build

# Include any dependencies generated for this target.
include CMakeFiles/kvsservice.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/kvsservice.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kvsservice.dir/flags.make

CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o: ../source/common/MethodQueue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/MethodQueue.cpp

CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/MethodQueue.cpp > CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.i

CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/MethodQueue.cpp -o CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.s

CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o: ../source/common/ServiceBase.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/ServiceBase.cpp

CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/ServiceBase.cpp > CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.i

CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/common/ServiceBase.cpp -o CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.s

CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o: ../source/KvsProducer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsProducer.cpp

CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsProducer.cpp > CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.i

CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsProducer.cpp -o CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.s

CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o: ../source/StreamSource.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/StreamSource.cpp

CMakeFiles/kvsservice.dir/source/StreamSource.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/StreamSource.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/StreamSource.cpp > CMakeFiles/kvsservice.dir/source/StreamSource.cpp.i

CMakeFiles/kvsservice.dir/source/StreamSource.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/StreamSource.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/StreamSource.cpp -o CMakeFiles/kvsservice.dir/source/StreamSource.cpp.s

CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o: ../source/KvsPlayer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsPlayer.cpp

CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsPlayer.cpp > CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.i

CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsPlayer.cpp -o CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.s

CMakeFiles/kvsservice.dir/source/KvsService.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/KvsService.cpp.o: ../source/KvsService.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/kvsservice.dir/source/KvsService.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/KvsService.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsService.cpp

CMakeFiles/kvsservice.dir/source/KvsService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/KvsService.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsService.cpp > CMakeFiles/kvsservice.dir/source/KvsService.cpp.i

CMakeFiles/kvsservice.dir/source/KvsService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/KvsService.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsService.cpp -o CMakeFiles/kvsservice.dir/source/KvsService.cpp.s

CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o: ../source/KvsServiceStub.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceStub.cpp

CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceStub.cpp > CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.i

CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceStub.cpp -o CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.s

CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o: ../source/KvsServiceProxy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceProxy.cpp

CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceProxy.cpp > CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.i

CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/KvsServiceProxy.cpp -o CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.s

CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o: ../source/ComponentProvider.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/ComponentProvider.cpp

CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/ComponentProvider.cpp > CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.i

CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/ComponentProvider.cpp -o CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.s

CMakeFiles/kvsservice.dir/source/Main.cpp.o: CMakeFiles/kvsservice.dir/flags.make
CMakeFiles/kvsservice.dir/source/Main.cpp.o: ../source/Main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/kvsservice.dir/source/Main.cpp.o"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kvsservice.dir/source/Main.cpp.o -c /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/Main.cpp

CMakeFiles/kvsservice.dir/source/Main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kvsservice.dir/source/Main.cpp.i"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/Main.cpp > CMakeFiles/kvsservice.dir/source/Main.cpp.i

CMakeFiles/kvsservice.dir/source/Main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kvsservice.dir/source/Main.cpp.s"
	/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin/aarch64-linux-gnu-g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/source/Main.cpp -o CMakeFiles/kvsservice.dir/source/Main.cpp.s

# Object files for target kvsservice
kvsservice_OBJECTS = \
"CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o" \
"CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o" \
"CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o" \
"CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o" \
"CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o" \
"CMakeFiles/kvsservice.dir/source/KvsService.cpp.o" \
"CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o" \
"CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o" \
"CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o" \
"CMakeFiles/kvsservice.dir/source/Main.cpp.o"

# External object files for target kvsservice
kvsservice_EXTERNAL_OBJECTS =

kvsservice: CMakeFiles/kvsservice.dir/source/common/MethodQueue.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/common/ServiceBase.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/KvsProducer.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/StreamSource.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/KvsPlayer.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/KvsService.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/KvsServiceStub.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/KvsServiceProxy.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/ComponentProvider.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/source/Main.cpp.o
kvsservice: CMakeFiles/kvsservice.dir/build.make
kvsservice: CMakeFiles/kvsservice.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable kvsservice"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kvsservice.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kvsservice.dir/build: kvsservice

.PHONY : CMakeFiles/kvsservice.dir/build

CMakeFiles/kvsservice.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kvsservice.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kvsservice.dir/clean

CMakeFiles/kvsservice.dir/depend:
	cd /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build /home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/kvsservice/doorbell-build/CMakeFiles/kvsservice.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kvsservice.dir/depend

