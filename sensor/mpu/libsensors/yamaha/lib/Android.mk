#
# CONFIDENTIAL
# Copyright (C) 2013-2015 Yamaha Corporation
#
ifneq ($(BUILD_TINY_ANDROID),true)
LOCAL_PATH:= $(call my-dir)

ifeq (${TARGET_ARCH},arm64)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := libyas_mag_algo
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_SRC_FILES_arm := libyas_mag_algo_32.a
LOCAL_32_BIT_ONLY := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .a
LOCAL_MODULE := libyas_mag_algo
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_SRC_FILES_arm64 := libyas_mag_algo_64.a
include $(BUILD_PREBUILT)

else

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libyas_mag_algo.a
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

endif

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../inc
LOCAL_CFLAGS := \
    -Wall -Wextra -Wcast-align -Wcast-qual -Wdeclaration-after-statement \
    -Winit-self -Winline -Wlogical-op -Wlong-long -Wmissing-declarations \
    -Wmissing-format-attribute -Wmissing-noreturn -Wold-style-definition \
    -Woverlength-strings -Wpacked -Wpointer-arith -Wstrict-prototypes \
    -Wunsafe-loop-optimizations -Wvla -Wwrite-strings -Wstrict-aliasing=2 \
    -Wfloat-equal -Wswitch-enum \
    -O3 -fPIC
LOCAL_MODULE:= libyasalgo
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SRC_FILES:= yas_android_lib.c
LOCAL_WHOLE_STATIC_LIBRARIES := libyas_mag_algo
include $(BUILD_SHARED_LIBRARY)
endif
