#!/bin/bash
export PATH=/opt/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-
export ARCH=arm
export REAL_MULTIMACH_TARGET_SYS=cortexa8hf-vfp-neon-3.8-oe-linux-gnueabi
export TOOLCHAIN_SYS=arm-linux-gnueabihf
export TOOLCHAIN_PREFIX=$TOOLCHAIN_SYS-
export CC=${TOOLCHAIN_PREFIX}gcc
export CXX=${TOOLCHAIN_PREFIX}g++
export GDB=${TOOLCHAIN_PREFIX}gdb
export CPP="${TOOLCHAIN_PREFIX}gcc -E"
export NM=${TOOLCHAIN_PREFIX}nm
export AS=${TOOLCHAIN_PREFIX}as
export AR=${TOOLCHAIN_PREFIX}ar
export RANLIB=${TOOLCHAIN_PREFIX}ranlib
export OBJCOPY=${TOOLCHAIN_PREFIX}objcopy
export OBJDUMP=${TOOLCHAIN_PREFIX}objdump
export STRIP=${TOOLCHAIN_PREFIX}strip
export CONFIGURE_FLAGS="--target=arm-oe-linux-gnueabi --host=arm-oe-linux-gnueabi --build=i686-linux --with-libtool-sysroot=$SDK_PATH_TARGET"
export CPPFLAGS=" -march=armv7-a -marm -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8 --sysroot=$SDK_PATH_TARGET"
export CFLAGS="$CPPFLAGS"
export CXXFLAGS="$CPPFLAGS"
