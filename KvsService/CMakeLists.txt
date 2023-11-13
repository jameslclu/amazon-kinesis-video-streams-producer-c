cmake_minimum_required(VERSION 3.10)

set(modulename kvsservice)

project(${modulename}
        LANGUAGES C CXX
        DESCRIPTION "KVS Service"
        VERSION 0.0.1
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX REQUIRED IMPORTED_TARGET gio-unix-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

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
        ../common/source/JsonUtility.cpp
        ../common/source/popen_noshell.cpp
        ../common/source/ShellUtility.cpp
        ../common/source/MLogger.cpp
)

add_executable(${modulename}
        source/common/JsonUtility.cpp
        source/common/MethodQueue.cpp
        source/common/ServiceBase.cpp
        source/KvsProducer.cpp
        source/StreamSource.cpp
        source/KvsPlayer.cpp
        source/KvsService.cpp
        source/KvsServiceStub.cpp
        source/KvsServiceProxy.cpp
        source/ComponentProvider.cpp
        source/Main.cpp)

target_include_directories(${modulename}
        PUBLIC
        ${GIO_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${GIO_UNIX_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
        ./../dependency/libkvspic/kvspic-src/src/client/include
        ./../dependency/libkvspic/kvspic-src/src/common/include
        ./../dependency/libkvspic/kvspic-src/src/duration/include
        ./../dependency/libkvspic/kvspic-src/src/heap/include
        ./../dependency/libkvspic/kvspic-src/src/mkvgen/include
        ./../dependency/libkvspic/kvspic-src/src/state/include
        ./../dependency/libkvspic/kvspic-src/src/trace/include
        ./../dependency/libkvspic/kvspic-src/src/utils/include
        ./../dependency/libkvspic/kvspic-src/src/view/include)

target_link_libraries(${modulename}
        #PRIVATE teec
        PUBLIC ${GIO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GIO_UNIX_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        cproducer
        -lpthread -lm)
#target_link_libraries(kvsVideoOnlyRealtimeStreamingSample cproducer)