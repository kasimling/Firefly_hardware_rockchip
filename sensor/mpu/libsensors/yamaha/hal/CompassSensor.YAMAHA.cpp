/*
 * Copyright (C) 2014 The Android Open Source Project
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

#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <linux/input.h>

#include "sensor_params.h"
#include "CompassSensor.YAMAHA.h"
#include "yas.h"

#if (YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
        || YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533)
#define COMPASS_NAME "YAS532"
#elif YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS537
#define COMPASS_NAME "YAS537"
#endif

/*****************************************************************************/

CompassSensor::CompassSensor()
                    : SensorBase(NULL, NULL)
{
    mCompassSensor = new YamahaSensor();
    mCompassSensor->enable(ID_M, 0);
}

CompassSensor::~CompassSensor()
{
    mCompassSensor->enable(ID_M, 0);
    delete mCompassSensor;
}

int CompassSensor::getFd(void) const
{
    return mCompassSensor->getFd();
}

/**
    @brief        This function will enable/disable sensor.
    @param[in]    handle    which sensor to enable/disable.
    @param[in]    en        en=1 enable; en=0 disable
    @return       if the operation is successful.
**/
int CompassSensor::enable(int32_t handle, int en)
{
    return mCompassSensor->enable(handle, en);
}

int CompassSensor::setDelay(int32_t handle, int64_t ns)
{
    return mCompassSensor->setDelay(handle, ns);
}

/**
    @brief      This function will return the state of the sensor.
    @return     1=enabled; 0=disabled
**/
int CompassSensor::getEnable(int32_t handle)
{
    return mCompassSensor->getEnable(handle);
}

/**
    @brief      This function will return the current delay for this sensor.
    @return     delay in nanoseconds.
**/
int64_t CompassSensor::getDelay(int32_t handle)
{
    return mCompassSensor->getDelay(handle);
}

/**
  @brief         Integrators need to implement this function per 3rd-party solution
  @param[out]    data      sensor data is stored in this variable. Scaled such that
  1 uT = 2^16
  @para[in]      timestamp data's timestamp
  @return        1, if 1   sample read, 0, if not, negative if error
 **/
int CompassSensor::readSample(long *data, int64_t *timestamp)
{
    return mCompassSensor->readSample(data, timestamp);
}


#if 0
/**
  @brief         Integrators need to implement this function per 3rd-party solution
  @param[out]    data      sensor data is stored in this variable. Scaled such that
  1 uT = 2^16
  @para[in]      timestamp data's timestamp
  @return        1, if 1   sample read, 0, if not, negative if error
 **/
int CompassSensor::readRawSample(float *data, int64_t *timestamp)
{
    return 0;
}
#endif

void CompassSensor::fillList(struct sensor_t *list)
{
    const char *compass = COMPASS_NAME;

    if (compass) {
        if (!strcmp(compass, "YAS532")) {
            list->maxRange = COMPASS_YAS532_RANGE;
            list->resolution = COMPASS_YAS532_RESOLUTION;
            list->power = COMPASS_YAS532_POWER;
            list->minDelay = COMPASS_YAS532_MINDELAY;
            return;
        }
        if (!strcmp(compass, "YAS537")) {
            list->maxRange = COMPASS_YAS537_RANGE;
            list->resolution = COMPASS_YAS537_RESOLUTION;
            list->power = COMPASS_YAS537_POWER;
            list->minDelay = COMPASS_YAS537_MINDELAY;
        }
    }

    ALOGE("HAL:unsupported compass id %s -- "
         "this implementation only supports YAMAHA compasses", compass);
    list->maxRange = COMPASS_YAS537_RANGE;
    list->resolution = COMPASS_YAS537_RESOLUTION;
    list->power = COMPASS_YAS537_POWER;
    list->minDelay = COMPASS_YAS537_MINDELAY;
}

void CompassSensor::getOrientationMatrix(signed char *orient)
{
    orient[0] = 1;
    orient[1] = 0;
    orient[2] = 0;
    orient[3] = 0;
    orient[4] = 1;
    orient[5] = 0;
    orient[6] = 0;
    orient[7] = 0;
    orient[8] = 1;
}

int CompassSensor::getAccuracy(void)
{
    return mCompassSensor->getAccuracy();
}


/**
 * Compass sensitivity.
 * @return A scale factor to convert device units to micro Tesla scaled by 2^16
 * such that uT  = device_units * sensitivity / 2^30. Typically
 * it works out to be the maximum uT * 2^15.
 */
long CompassSensor::getSensitivity() {
    /*
     * TODO: hard-coded for 3rd-party's sensitivity transformation
     * ??long getSensitivity() { return (1L << 30); }
     */
    return (4915 * (1L << 15));
}
