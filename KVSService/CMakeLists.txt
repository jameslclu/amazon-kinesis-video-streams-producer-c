cmake_minimum_required(VERSION 3.10)

set(modulename kvsservice)

project(${modulename}
        LANGUAGES C CXX
        DESCRIPTION "KVS Service"
        VERSION 0.0.1
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX REQUIRED IMPORTED_TARGET gio-unix-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

add_executable(${modulename}
        source/KVSProducer.cpp
        source/Main.cpp)

target_include_directories(${modulename}
        PUBLIC
        ${GIO_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${GIO_UNIX_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS})

target_link_libraries(${modulename}
        #PRIVATE teec
        PUBLIC ${GIO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GIO_UNIX_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        -lpthread -lm)

