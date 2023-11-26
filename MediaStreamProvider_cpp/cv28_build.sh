#!/bin/bash
#

AARCH64_TOOLCHAIN_PATH=/usr/local/linaro-aarch64-2020.09-gcc10.2-linux5.4/bin
CROSS_COMPILE=${AARCH64_TOOLCHAIN_PATH}/aarch64-linux-gnu

export CC="${CROSS_COMPILE}-gcc -march=armv8-a -mcpu=cortex-a53+crypto"
export CXX="${CROSS_COMPILE}-g++"
export AR="${CROSS_COMPILE}-ar"
export LD="${CROSS_COMPILE}-ld"

export AS="${CROSS_COMPILE}-as"
export STRIP="${CROSS_COMPILE}-strip"
export RANLIB="${CROSS_COMPILE}-ranlib"
export OBJCOPY="${CROSS_COMPILE}-objcopy"
export OBJDUMP="${CROSS_COMPILE}-objdump"
export READELF="${CROSS_COMPILE}-readelf"
export NM="${CROSS_COMPILE}-nm"

rm -r doorbell-build
mkdir doorbell-build
cd doorbell-build
cmake ./../makefile_doorbell -DDOORBELL=1
make -j4
