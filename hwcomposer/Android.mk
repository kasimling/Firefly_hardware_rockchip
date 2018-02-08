# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
BOARD_USES_DRM_HWCOMPOSER=true
ifeq ($(strip $(BOARD_USES_DRM_HWCOMPOSER)),true)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libdrm \
	libEGL \
	libGLESv2 \
	libhardware \
	liblog \
	libsync \
	libui \
	libutils \
	librga

LOCAL_C_INCLUDES := \
        hardware/rockchip/libgralloc \
	external/libdrm \
	external/libdrm/include/drm \
	system/core/include/utils \
	system/core/libsync \
	system/core/libsync/include \
	hardware/rockchip/librga

LOCAL_SRC_FILES := \
	autolock.cpp \
	drmresources.cpp \
	drmcomposition.cpp \
	drmcompositor.cpp \
	drmcompositorworker.cpp \
	drmconnector.cpp \
	drmcrtc.cpp \
	drmdisplaycomposition.cpp \
	drmdisplaycompositor.cpp \
	drmencoder.cpp \
	drmeventlistener.cpp \
	drmmode.cpp \
	drmplane.cpp \
	drmproperty.cpp \
	glworker.cpp \
	hwcomposer.cpp \
        platform.cpp \
        platformdrmgeneric.cpp \
        platformnv.cpp \
	separate_rects.cpp \
	virtualcompositorworker.cpp \
	vsyncworker.cpp \
	worker.cpp \
        hwc_util.cpp \
        hwc_rockchip.cpp \
        hwc_debug.cpp

ifeq ($(strip $(BOARD_DRM_HWCOMPOSER_BUFFER_IMPORTER)),nvidia-gralloc)
LOCAL_CPPFLAGS += -DUSE_NVIDIA_IMPORTER
else

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3368)
LOCAL_CPPFLAGS += -DTARGET_BOARD_PLATFORM_RK3368
USE_AFBC_LAYER = 0
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CPPFLAGS += -DRK3368_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CPPFLAGS += -DRK3368_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CPPFLAGS += -DRK3368_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CPPFLAGS += -DRK3368_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3399)
LOCAL_CPPFLAGS += -DTARGET_BOARD_PLATFORM_RK3399 -DRK_DRM_GRALLOC=1 \
               -DMALI_AFBC_GRALLOC=1
USE_AFBC_LAYER = 1

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CPPFLAGS += -DRK3399_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CPPFLAGS += -DRK3399_BOX
USE_AFBC_LAYER = 0
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CPPFLAGS += -DRK3399_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CPPFLAGS += -DRK3399_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),rk3288)
LOCAL_CPPFLAGS += -DTARGET_BOARD_PLATFORM_RK3288  -DRK_DRM_GRALLOC=1 \
               -DMALI_AFBC_GRALLOC=1
USE_AFBC_LAYER = 0
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CPPFLAGS += -DRK3288_MID
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CPPFLAGS += -DRK3288_BOX
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CPPFLAGS += -DRK3288_PHONE
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CPPFLAGS += -DRK3288_VR
endif
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),stbvr)
LOCAL_CPPFLAGS += -DRK3288_VR
endif
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),tablet)
LOCAL_CFLAGS += -DRK_MID
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),box)
LOCAL_CFLAGS += -DRK_BOX
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),phone)
LOCAL_CFLAGS += -DRK_PHONE
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),vr)
LOCAL_CFLAGS += -DRK_VIR
else
ifeq ($(strip $(TARGET_BOARD_PLATFORM_PRODUCT)),stbvr)
LOCAL_CFLAGS += -DRK_VIR
endif #stbvr
endif #vr
endif #phone
endif #box
endif #tablet

LOCAL_CPPFLAGS += -DUSE_SQUASH=0 -DRK_RGA_TEST=0 -DRK_VR=0 -DRK_STEREO=0
LOCAL_CPPFLAGS += -DUSE_DRM_GENERIC_IMPORTER \
               -DUSE_MULTI_AREAS=1 -DRK_RGA=1 \
	       -DUSE_AFBC_LAYER=$(USE_AFBC_LAYER) -DRK_SKIP_SUB=1 \
	       -DRK_VIDEO_UI_OPT=1 -DRK_VIDEO_SKIP_LINE=1 \
	       -DRK_INVALID_REFRESH=1
MAJOR_VERSION := "RK_GRAPHICS_VER=commit-id:$(shell cd $(LOCAL_PATH) && git log  -1 --oneline | awk '{print $$1}')"
LOCAL_CPPFLAGS += -DRK_GRAPHICS_VER=\"$(MAJOR_VERSION)\"
endif

LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_HARDWARE)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := $(TARGET_SHLIB_SUFFIX)
include $(BUILD_SHARED_LIBRARY)

endif
