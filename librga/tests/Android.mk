LOCAL_PATH:= $(call my-dir)
#======================================================================
#
#rgargb565toyuv
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
	RockchipRgaCovertRgb565ToYuv.cpp

LOCAL_MODULE:= rgargb565toyuv

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)

#======================================================================
#
#rgargbatoyuv
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
	RockchipRgaCovertRgbaToYuv.cpp

LOCAL_MODULE:= rgargbatoyuv

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)

#======================================================================
#
#rgacopy
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaCopy.cpp

LOCAL_MODULE:= rgacopy

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgacopyrgba
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaCopyRgba.cpp

LOCAL_MODULE:= rgacopyrgba

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgacoveryuvtorgba
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaCovertYuvToRgba.cpp

LOCAL_MODULE:= rgacoveryuvtorgba

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgargbamirror
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaRgbaMirror.cpp

LOCAL_MODULE:= rgargbamirror

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgablit
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaBlit.cpp

LOCAL_MODULE:= rgablit

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)

#======================================================================
#
#rgauserstereo
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaStereoUserSpace.cpp

LOCAL_MODULE:= rgauserstereo

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgauserblit
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaBlitUserSpace.cpp

LOCAL_MODULE:= rgauserblit

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)


#======================================================================
#
#rgapalettetable
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
    RockchipRgaPattenBuff.cpp

LOCAL_MODULE:= rgapattenbuff

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)

#======================================================================
#
#rgablitslt
#
#======================================================================
include $(CLEAR_VARS)

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES -DEGL_EGLEXT_PROTOTYPES

LOCAL_CFLAGS += -DROCKCHIP_GPU_LIB_ENABLE

LOCAL_CFLAGS += -Wall -Werror -Wunreachable-code

LOCAL_C_INCLUDES += external/tinyalsa/include

LOCAL_C_INCLUDES += hardware/rockchip/librga
LOCAL_C_INCLUDES += hardware/rk29/librga

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libutils \
    libbinder \
    libui \
    libEGL \
    libGLESv1_CM \
    libgui \
    libhardware \
    librga

#has no "external/stlport" from Android 6.0 on
ifeq (1,$(strip $(shell expr $(PLATFORM_VERSION) \< 6.0)))
LOCAL_C_INCLUDES += \
    external/stlport/stlport

LOCAL_SHARED_LIBRARIES += \
    libstlport

LOCAL_C_INCLUDES += bionic
endif

LOCAL_SRC_FILES:= \
	RockchipFileOps.cpp \
	RockchipRgaBlitSlt.cpp

LOCAL_MODULE:= rgablitslt

ifdef TARGET_32_BIT_SURFACEFLINGER
LOCAL_32_BIT_ONLY := true
endif

include $(BUILD_EXECUTABLE)

#======================================================================
#
