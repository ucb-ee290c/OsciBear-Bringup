#!/bin/bash

set -ex

WHICH_BOARD=eagle_vcu118
BASE_DIR=../beagle-sdk/buildroot-extra/board/ucb/$WHICH_BOARD/overlay/usr

cp -f build/libfesvr.so        $BASE_DIR/lib/.
cp -f build/vcu118_beagle_boot $BASE_DIR/bin/.
cp -f build/vcu118_tsi_test    $BASE_DIR/bin/.
cp -f build/vcu118_tsi_rw    $BASE_DIR/bin/.
cp -f build/vcu118_run_program    $BASE_DIR/bin/.
cp -f build/vcu118_tsi_mem_test    $BASE_DIR/bin/.

WHICH_BOARD=beagle_vcu118
BASE_DIR=../beagle-sdk/buildroot-extra/board/ucb/$WHICH_BOARD/overlay/usr

cp -f build/libfesvr.so        $BASE_DIR/lib/.
cp -f build/vcu118_beagle_boot $BASE_DIR/bin/.
cp -f build/vcu118_tsi_test    $BASE_DIR/bin/.
cp -f build/vcu118_tsi_rw    $BASE_DIR/bin/.
cp -f build/vcu118_run_program    $BASE_DIR/bin/.
cp -f build/vcu118_tsi_mem_test    $BASE_DIR/bin/.
