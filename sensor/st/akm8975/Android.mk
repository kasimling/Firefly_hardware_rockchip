LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MULTILIB := 32

LOCAL_CFLAGS += \
	-Wno-unused-parameter

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/libAK8975 

LOCAL_SRC_FILES:= \
	AK8975Driver.c \
	DispMessage.c \
	FileIO.c \
	Measure.c \
	misc.c \
	main.c \
	Acc_mma8452.c
	# Acc_dummy.c

LOCAL_CFLAGS += -DPLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/libAK8975 -lAK8975
LOCAL_MODULE := akmd
LOCAL_MODULE_TAGS := optional
LOCAL_FORCE_STATIC_EXECUTABLE := false
LOCAL_SHARED_LIBRARIES := libc libm libz libutils libcutils
include $(BUILD_EXECUTABLE)
