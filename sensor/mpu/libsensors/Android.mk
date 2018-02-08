# Copyright (C) 2008 The Android Open Source Project
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
# Modified 2011 by InvenSense, Inc

LOCAL_PATH := $(call my-dir)
#COMPILE_COMPASS_YAS537 := 1
COMPILE_INVENSENSE_COMPASS_CAL := 0

ifneq ($(TARGET_SIMULATOR),true)

ifeq (${TARGET_ARCH},arm64)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libmplmpu
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_arm := libmplmpu.so
LOCAL_32_BIT_ONLY := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libmplmpu
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_arm64 := libmplmpu_64.so
include $(BUILD_PREBUILT)

else

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libmplmpu.so
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

endif


ifeq (${TARGET_ARCH},arm64)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libmllite
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_arm := libmllite.so
LOCAL_32_BIT_ONLY := true
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE := libmllite
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_arm64 := libmllite_64.so
include $(BUILD_PREBUILT)

else

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := libmllite.so
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

endif



# InvenSense fragment of the HAL
include $(CLEAR_VARS)

ifeq (${TARGET_ARCH},arm64)
BIN_PATH := inv_64
else
BIN_PATH := inv_32
endif

LOCAL_MODULE := libinvensense_hal

$(info LOCAL_MODULE=$(LOCAL_MODULE))
$(info TARGET_ARCH=$(TARGET_ARCH))
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_OWNER := invensense

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\"

# ANDROID version check
MAJOR_VERSION :=$(shell echo $(PLATFORM_VERSION) | cut -f1 -d.)
MINOR_VERSION :=$(shell echo $(PLATFORM_VERSION) | cut -f2 -d.)
VERSION_JB :=$(shell test $(MAJOR_VERSION) -gt 4 -o $(MAJOR_VERSION) -eq 4 -a $(MINOR_VERSION) -gt 0 && echo true)
$(info MAJOR_VERSION=$(MAJOR_VERSION))
$(info MINOR_VERSION=$(MINOR_VERSION))
$(info VERSION_JB=$(VERSION_JB))
#ANDROID version check END

ifeq ($(VERSION_JB),true)
LOCAL_CFLAGS += -DANDROID_JELLYBEAN
endif

ifneq (,$(filter $(TARGET_BUILD_VARIANT),eng userdebug))
ifneq ($(COMPILE_INVENSENSE_COMPASS_CAL),0)
LOCAL_CFLAGS += -DINVENSENSE_COMPASS_CAL
endif
ifeq ($(COMPILE_THIRD_PARTY_ACCEL),1)
LOCAL_CFLAGS += -DTHIRD_PARTY_ACCEL
endif
ifeq ($(COMPILE_COMPASS_YAS537),1)
LOCAL_CFLAGS += -DCOMPASS_YAS537
endif
ifeq ($(COMPILE_COMPASS_AK8975),1)
LOCAL_CFLAGS += -DCOMPASS_AK8975
endif
ifeq ($(COMPILE_COMPASS_AMI306),1)
LOCAL_CFLAGS += -DCOMPASS_AMI306
endif
else # release builds, default
LOCAL_CFLAGS += -DINVENSENSE_COMPASS_CAL
endif

LOCAL_SRC_FILES += SensorBase.cpp
LOCAL_SRC_FILES += MPLSensor.cpp
LOCAL_SRC_FILES += MPLSupport.cpp
LOCAL_SRC_FILES += InputEventReader.cpp

ifneq (,$(filter $(TARGET_BUILD_VARIANT),eng userdebug))
ifeq ($(COMPILE_INVENSENSE_COMPASS_CAL),0)
LOCAL_SRC_FILES += AkmSensor.cpp
LOCAL_SRC_FILES += CompassSensor.AKM.cpp
else ifeq ($(COMPILE_COMPASS_AMI306),1)
LOCAL_SRC_FILES += CompassSensor.IIO.primary.cpp
else ifeq ($(COMPILE_COMPASS_YAS537),1)
LOCAL_SRC_FILES += CompassSensor.YAMAHA.cpp
LOCAL_SRC_FILES += YamahaSensor.cpp
else
LOCAL_SRC_FILES += CompassSensor.IIO.9150.cpp
endif
else # release builds, default
LOCAL_SRC_FILES += AkmSensor.cpp
LOCAL_SRC_FILES += CompassSensor.AKM.cpp
endif #userdebug

LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mllite
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mllite/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/driver/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/driver/include/linux
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/yamaha/inc
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/yamaha/lib

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libmllite
#LOCAL_SHARED_LIBRARIES += libyasalgo

# Additions for SysPed
LOCAL_SHARED_LIBRARIES += libmplmpu
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mpl
LOCAL_CPPFLAGS += -DLINUX=1
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

endif # !TARGET_SIMULATOR

# Build a temporary HAL that links the InvenSense .so
include $(CLEAR_VARS)

ifeq (${TARGET_ARCH},arm64)
BIN_PATH := inv_64
else
BIN_PATH := inv_32
endif

ifneq ($(filter manta grouper tilapia, $(TARGET_DEVICE)),)
LOCAL_MODULE := sensors.invensense
else
LOCAL_MODULE := sensors.${TARGET_PRODUCT}
endif
LOCAL_MODULE := sensors.rk30board

ifdef TARGET_2ND_ARCH
LOCAL_MODULE_RELATIVE_PATH := hw
else
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
endif

LOCAL_SHARED_LIBRARIES += libmplmpu
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mllite
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mllite/linux
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/mpl
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/driver/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(BIN_PATH)/core/driver/include/linux

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\"

ifeq ($(VERSION_JB),true)
LOCAL_CFLAGS += -DANDROID_JELLYBEAN
endif

ifneq (,$(filter $(TARGET_BUILD_VARIANT),eng userdebug))
ifneq ($(COMPILE_INVENSENSE_COMPASS_CAL),0)
LOCAL_CFLAGS += -DINVENSENSE_COMPASS_CAL
endif
ifeq ($(COMPILE_THIRD_PARTY_ACCEL),1)
LOCAL_CFLAGS += -DTHIRD_PARTY_ACCEL
endif
ifeq ($(COMPILE_COMPASS_YAS537),1)
LOCAL_CFLAGS += -DCOMPASS_YAS537
endif
ifeq ($(COMPILE_COMPASS_AK8975),1)
LOCAL_CFLAGS += -DCOMPASS_AK8975
endif
ifeq ($(COMPILE_COMPASS_AMI306),1)
LOCAL_CFLAGS += -DCOMPASS_AMI306
endif
else # release builds, default
LOCAL_CFLAGS += -DINVENSENSE_COMPASS_CAL
endif # userdebug

ifneq ($(filter manta grouper tilapia, $(TARGET_DEVICE)),)
LOCAL_SRC_FILES := sensors_mpl.cpp
else
LOCAL_SRC_FILES := sensors_mpl.cpp
endif

LOCAL_SHARED_LIBRARIES := libinvensense_hal
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libmllite
include $(BUILD_SHARED_LIBRARY)



