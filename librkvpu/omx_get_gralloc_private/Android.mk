LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := gralloc_priv_omx.cpp
LOCAL_SHARED_LIBRARIES := liblog libutils 
LOCAL_MODULE := libgralloc_priv_omx
LOCAL_MODULE_TAGS := optional

LOCAL_C_INCLUDES := \
	$(TOP)/hardware/rockchip/libgralloc \
	$(TOP)/hardware/libhardware/include 

ifeq ($(strip $(GRAPHIC_MEMORY_PROVIDER)),dma_buf)
	LOCAL_CFLAGS += -DUSE_DMA_BUF
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t720)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T72X=1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=0
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t760)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T76X=1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)), mali-t860)
LOCAL_CFLAGS += -DMALI_PRODUCT_ID_T86X=1
LOCAL_CFLAGS += -DMALI_AFBC_GRALLOC=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM_GPU)),G6110)
	LOCAL_CFLAGS += -DGPU_G6110
endif

ifeq ($(strip $(BOARD_USE_DRM)), true)
ifneq ($(filter rk3399 rk3288, $(strip $(TARGET_BOARD_PLATFORM))), )
        LOCAL_CFLAGS += -DUSE_DRM -DRK_DRM_GRALLOC=1 -DMALI_AFBC_GRALLOC=1
endif
endif

include $(BUILD_SHARED_LIBRARY)


