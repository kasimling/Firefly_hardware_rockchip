#!/bin/bash

# This is a sample of the command line make used to build
#   the libraries and binaries for the Pandaboard.
# Please customize this path to match the location of your
#   Android source tree. Other variables may also need to
#   be customized such as:
#     $CROSS, $PRODUCT, $KERNEL_ROOT

export ANDROID_BASE=/home2/tzb/develop/marshmallow/

make -C build/android \
	VERBOSE=1 \
	TARGET=android \
	ANDROID_ROOT=${ANDROID_BASE}/rk3399_6.0_64 \
	KERNEL_ROOT=${ANDROID_BASE}/rk3399_6.0_64/kernel \
	CROSS=${ANDROID_BASE}/rk3399_6.0_64/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android- \
	PRODUCT=rk3399 \
	echo_in_colors=echo \
	-f shared.mk

