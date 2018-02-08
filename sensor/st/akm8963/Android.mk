LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MULTILIB := 32

LOCAL_CFLAGS += \
	-Wno-unused-parameter

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/libAK8963 

LOCAL_SRC_FILES:= \
	AK8963Driver.c \
	DispMessage.c \
	FileIO.c \
	Measure.c \
	misc.c \
	main.c \
	Acc_mma8452.c
	# Acc_dummy.c

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/libAK8963 -lAK8963
LOCAL_MODULE := akmd
LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := false
LOCAL_SHARED_LIBRARIES := libc libm libz libutils libcutils
include $(BUILD_EXECUTABLE)

