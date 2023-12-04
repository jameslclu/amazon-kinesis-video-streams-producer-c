cmake_minimum_required(VERSION 3.10)

set(modulename fxnMediaStreamProvider)

project(${modulename}
        LANGUAGES C CXX
        DESCRIPTION "Media Stream Provider"
        VERSION 0.0.1
)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations")
add_definitions(-DDOORBELL)
#set(DOORBELL 11)
#option(DOORBELL "Use openssl as crypto library" ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO_UNIX REQUIRED IMPORTED_TARGET gio-unix-2.0)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

set (PREBUILD_OSS_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/prebuild/oss/armv8-a")
set (FAKE_ROOT_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/fakeroot/usr/lib")
set (KVS_PRODUCER_LIBS_INC_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/vendor/aws/inc/kvsProducer")
set (KVS_PRODUCER_LIBS_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/vendor/aws/libs/kvsProducer")
set (ORYX_V2_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/oryx_v2")

set (oryxv2_inc
        ${ORYX_V2_DIR}/include
        ${ORYX_V2_DIR}/include/utility
        ${ORYX_V2_DIR}/include/audio
        ${ORYX_V2_DIR}/base/stream/include/record
        ${ORYX_V2_DIR}/include/stream
        /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/external/tee/mbedtls/include/mbedtls)

set (ORYX_UTIL_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/oryx_v2/base/utility/util")
set (ORYX_VIDEO_PLATFORM_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/oryx_v2/base/video/platform")
set (ORYX_OSAL_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/oryx_v2/base/utility/osal")
set (ORYX_SERIRALL_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/oryx_v2/base/hardware/serialport")
set (ORYX_CONFIG_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/oryx_v2/base/configure")
set (FAKEROOT_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/fakeroot")

set(oryxv2_libs
        -Wl,-rpath-link=${FAKEROOT_DIR}/usr/lib
        -Wl,-rpath-link=${PREBUILD_OSS_DIR}/lua/usr/lib
        ${FAKEROOT_DIR}/usr/lib/liboryx-export-client.so
        ${FAKEROOT_DIR}/usr/lib/liboryx-util.so.2
        ${FAKEROOT_DIR}/usr/lib/liboryx-osal.so.2
        ${FAKEROOT_DIR}/usr/lib/liboryx-video-address.so.2
        /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/prebuild/oss/armv8-a/json-c/usr/lib/libjson-c.so
        #/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/kvsapp/libs/libFxnConfig.a
        #/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/kvsapp/libs/libFxnDBus.a
        #/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/kvsapp/libs/libFxnUtils.a
)

set(kvsproducer_inc
        ${PREBUILD_OSS_DIR}/jsoncpp/include
        ${PREBUILD_OSS_DIR}/glib2/include
        ${PREBUILD_OSS_DIR}/glib2/include/glib-2.0
        ${PREBUILD_OSS_DIR}/glib2/include/gio-unix-2.0
)

set(player_inc
        ./../player
)


list(APPEND common_src
#        ./../common/JsonUtility.cpp
        ./../common/MethodQueue.cpp
        ./../common/ServiceBase.cpp
        ./../common/MLogger.cpp)

list(APPEND james_src
        ./../james/JamesService.cpp
        ./../james/JamesServiceStub.cpp)

list(APPEND common_inc
        ./../common
)

set(common_lib_includes
        ${PREBUILD_OSS_DIR}/jsoncpp/include
        ${PREBUILD_OSS_DIR}/glib2/include
        ${PREBUILD_OSS_DIR}/glib2/include/glib-2.0
        ${PREBUILD_OSS_DIR}/glib2/include/gio-unix-2.0
)

set(common_lib_libs
        -L${PREBUILD_OSS_DIR}/glib2/usr/lib/ -lgio-2.0
        -L${PREBUILD_OSS_DIR}/glib2/usr/lib/ -lglib-2.0
        -L${PREBUILD_OSS_DIR}/glib2/usr/lib/ -lgobject-2.0
        -L${PREBUILD_OSS_DIR}/glib2/usr/lib/ -lgmodule-2.0
        -L${PREBUILD_OSS_DIR}/zlib/usr/lib/ -lz
        -L${PREBUILD_OSS_DIR}/libffi/usr/lib/ -lffi
        -L${PREBUILD_OSS_DIR}/pcre/usr/lib/ -lpcre
        -L${PREBUILD_OSS_DIR}/libselinux/usr/lib/ -lselinux
        -L${PREBUILD_OSS_DIR}/util-linux-libs/usr/lib/ -lmount
        -L${PREBUILD_OSS_DIR}/util-linux-libs/usr/lib/ -lblkid
        -L${PREBUILD_OSS_DIR}/jsoncpp/usr/lib/ -ljsoncpp
        -L${FAKE_ROOT_DIR}/ -lcproducer
        -L${FAKE_ROOT_DIR}/ -lmbedcrypto
        -L${FAKE_ROOT_DIR}/ -lmbedtls
        -L${FAKE_ROOT_DIR}/ -lwebsockets
        -L${FAKE_ROOT_DIR}/ -lcurl
        -L${FAKE_ROOT_DIR}/ -lmbedx509
        -L${KVS_PRODUCER_LIBS_DIR}/ -lkvsCommonLws
        -L${KVS_PRODUCER_LIBS_DIR}/ -lkvsCommonCurl
)

set(player_src
    ./../player/ComponentProvider.cpp
    ./../player/KvsPlayer.cpp
    ./../player/KvsProducer.cpp
        ./../player/Settings.cpp)

add_executable(${modulename}
        ${common_src}
        ${player_src}
        ${james_src}
#        ../oryx/OryxStreaming.cpp
#        ../FmspServer.c
        #FmspMain.c
        ../main.cpp
)

target_include_directories(${modulename}
        PUBLIC
        ${GIO_INCLUDE_DIRS}
        ${GLIB_INCLUDE_DIRS}
        ${GIO_UNIX_INCLUDE_DIRS}
        ${JSONCPP_INCLUDE_DIRS}
        ${common_lib_includes}
        ${KVS_PRODUCER_LIBS_INC_DIR}
#        ${oryxv2_inc}
        ${common_inc}
        ${player_inc}
        ../inc
        /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/prebuild/oss/armv8-a/json-c/include
        /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/vendor/aws/kvs/inc
        /home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/kvsapp/producer/EventStream/inc
)

target_link_libraries(${modulename}
        PUBLIC ${GIO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GIO_UNIX_LIBRARIES}
        ${JSONCPP_LIBRARIES}
        ${common_lib_libs}
        ${oryxv2_libs}
        -lpthread -lm)