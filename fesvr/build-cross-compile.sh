#!/bin/bash

set -e

# build fesvr for riscv target

# make sure path points to correct tools
#RISCV_TCHN=/tools/B/abejgonza/beagle-work/beagle-sdk/work/bootstrap/host
RISCV_TCHN=$RISCV
export PATH=$RISCV_TCHN/bin:$PATH
export LD_LIBRARY_PATH=$RISCV_TCHN/lib:$LD_LIBRARY_PATH

# prefix used to determine the cross compile
RISCV_PREFIX=riscv64-ucb-linux-gnu

# build directory
BUILD_DIR_NAME=build
BUILD_DIR=$PWD/$BUILD_DIR_NAME
OUTPUT_DIR=$BUILD_DIR

mkdir -p $BUILD_DIR
cd $BUILD_DIR
../configure --host=$RISCV_PREFIX --target=$RISCV_PREFIX --prefix=$OUTPUT_DIR
make
make install
