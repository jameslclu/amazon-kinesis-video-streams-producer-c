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

# In the project, there are 3 kind of libraries:
if (DEFINED Doorbell)
set (PREBUILD_OSS_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/prebuild/oss/armv8-a")
set (FAKE_ROOT_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/out/cv28m_doorbell/fakeroot/usr/lib")
set (KVS_PRODUCER_LIBS_DIR "/home/jh/camera/AmbaCV28_3012/cv28_linux_sdk_3.0/ambarella/vendor/aws/libs/kvsProducer")

set(kvsproducer_inc
        ${PREBUILD_OSS_DIR}/jsoncpp/include
        ${PREBUILD_OSS_DIR}/glib2/include
        ${PREBUILD_OSS_DIR}/glib2/include/glib-2.0
        ${PREBUILD_OSS_DIR}/glib2/include/gio-unix-2.0
)

set (kvsproducer_dep_inc
)
set(commone_includes
		${PREBUILD_OSS_DIR}/jsoncpp/include
		${PREBUILD_OSS_DIR}/glib2/include
		${PREBUILD_OSS_DIR}/glib2/include/glib-2.0
		${PREBUILD_OSS_DIR}/glib2/include/gio-unix-2.0
)

set(common_libs
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

list(APPEND common_src
		../common/source/JsonUtility.cpp
		../common/source/popen_noshell.cpp
		../common/source/ShellUtility.cpp
		../common/source/MLogger.cpp
)

add_executable(${modulename}
		#source/common/JsonUtility.cpp
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
		${commone_includes}
		./../dependency/libkvspic/kvspic-src/src/client/include
		./../dependency/libkvspic/kvspic-src/src/common/include
		./../dependency/libkvspic/kvspic-src/src/duration/include
		./../dependency/libkvspic/kvspic-src/src/heap/include
		./../dependency/libkvspic/kvspic-src/src/mkvgen/include
		./../dependency/libkvspic/kvspic-src/src/state/include
		./../dependency/libkvspic/kvspic-src/src/trace/include
		./../dependency/libkvspic/kvspic-src/src/utils/include
		./../dependency/libkvspic/kvspic-src/src/view/include
		./../dependency/producersdk)

target_link_libraries(${modulename}
		PUBLIC ${GIO_LIBRARIES}
		${GLIB_LIBRARIES}
		${GIO_UNIX_LIBRARIES}
		${JSONCPP_LIBRARIES}
		${common_libs}
		-lpthread -lm)
else()
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
			/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/src/include
			/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/cmake-build-debug
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
			PUBLIC
			${GIO_LIBRARIES}
			${GLIB_LIBRARIES}
			${GIO_UNIX_LIBRARIES}
			${JSONCPP_LIBRARIES}
			-lcproducer
			-L/home/jh/workspaces/aws/amazon-kinesis-video-streams-producer-c/cmake-build-debug/ -lcproducer
			-lpthread -lm)

endif()