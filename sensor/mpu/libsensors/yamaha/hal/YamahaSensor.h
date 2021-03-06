/*
 * Copyright (C) 2011-2014 Yamaha Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __YAMAHA_SENSOR_H__
#define __YAMAHA_SENSOR_H__

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>
#include <hardware/sensors.h>

#include "SensorBase.h"
#include "sensor_params.h"

class YamahaSensor : public SensorBase {
    public:
        YamahaSensor();
        virtual ~YamahaSensor();
        virtual int enable(int32_t handle, int enabled);
        virtual int getEnable(int32_t handle);
        virtual int setDelay(int32_t handle, int64_t ns);
        virtual int64_t getDelay(int32_t handle);
        virtual int readEvents(sensors_event_t* data, int count);
        virtual int readSample(long *data, int64_t *timestamp);
        virtual int getAccuracy();

    private:
        int updateDelay();
        int mEnabled;
        int64_t mDelayNs;
        int mCountToNotify;
        int mCounter;
        char mDevPath[PATH_MAX];
        char mTriggerName[PATH_MAX];
        int mAccuracy;
};

#endif
