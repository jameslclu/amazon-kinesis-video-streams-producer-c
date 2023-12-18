cmake_minimum_required(VERSION 3.10)

set(modulename fxnMediaStreamProvider)

project(${modulename}
        LANGUAGES C CXX
        DESCRIPTION "Media Stream Provider"
        VERSION 0.0.1
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")

#set(CV28_BUILD 0)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX REQUIRED IMPORTED_TARGET gio-unix-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
find_package(CURL REQUIRED)

set(kvsproducer_inc
    ${PREBUILD_OSS_DIR}/jsoncpp/include
    ${PREBUILD_OSS_DIR}/glib2/include
    ${PREBUILD_OSS_DIR}/glib2/include/glib-2.0
    ${PREBUILD_OSS_DIR}/glib2/include/gio-unix-2.0
)

#set(kvsproducer_libs
#        -L${PREBUILD_OSS_DIR}/glib2/usr/lib/ -lgio-2.0
#)
list(APPEND common_src
    common/JsonUtility.cpp
    common/MethodQueue.cpp
    common/ServiceBase.cpp
    common/MLogger.cpp)

list(APPEND common_inc
        common)

list(APPEND james_src
        james/JamesService.cpp
        james/JamesServiceStub.cpp)

list(APPEND james_inc
        james)

list(APPEND player_src
    player/ComponentProvider.cpp
    player/KvsPlayer.cpp
    player/Settings.cpp
    player/KvsProducer.cpp)

list(APPEND player_inc
    player)

list(APPEND oryx_src
    oryx/OryxStreaming.cpp)

list(APPEND oryx_inc
    /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/oryx_v2/include
    oryx)

list(APPEND cv28_inc
    /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/vendor/aws/kvs/inc
    ${oryx_inc}
)

list(APPEND SDK_ROOT
    ./..
)

list(APPEND sdk_src
    ${KVS_COMMON_SOURCE_FILES_BASE}
    ${KVS_COMMON_SOURCE_FILES_LWS}
    ${KVS_COMMON_SOURCE_FILES_CURL}
    ${PRODUCER_C_SOURCE_FILES}
)

list(APPEND sdk_inc
        SDK_ROOT/amazon-kinesis-video-streams-producer-c/dependency/producersdk
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/client/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/common/include
        ./../../amazon-kinesis-video-streams-producer-c/ependency/libkvspic/kvspic-src/src/duration/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/heap/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/mkvgen/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/state/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/trace/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/utils/include
        ./../../amazon-kinesis-video-streams-producer-c/dependency/libkvspic/kvspic-src/src/view/include)

add_executable(fxnMediaStreamProvider
        ${common_src}
        ${player_src}
        ${james_src}
#        ${sdk_src}
#        ${cv28_src}
#        FmspServer.c
        main.cpp
)

target_include_directories(fxnMediaStreamProvider
PUBLIC
    ${GIO_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GIO_UNIX_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${common_inc}
    ${james_inc}
    ${player_inc}
#			/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/src/include
#			/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/cmake-build-debug
    ${sdk_inc}
)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/cmake-build-debug/")

target_link_libraries(fxnMediaStreamProvider
#PRIVATE teec
    PUBLIC
    ${GIO_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GIO_UNIX_LIBRARIES}
    ${JSONCPP_LIBRARIES}
#    cproducer
    -L/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/cmake-build-debug/ -lcproducer
    -lpthread -lm)