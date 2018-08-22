/*
* Copyright (C) 2012 Invensense, Inc.
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

#define FUNC_LOG LOGV("%s", __PRETTY_FUNCTION__)

#include <hardware/sensors.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>

#include <linux/input.h>

#include <utils/Atomic.h>
#include <utils/Log.h>

#include "sensors.h"
#include "MPLSensor.h"

/*****************************************************************************/
/* The SENSORS Module */

#ifdef ENABLE_DMP_SCREEN_AUTO_ROTATION
#define LOCAL_SENSORS (MPLSensor::NumSensors + 1)
#else
#define LOCAL_SENSORS MPLSensor::NumSensors
#endif

/* Vendor-defined Accel Load Calibration File Method 
* @param[out] Accel bias, length 3.  In HW units scaled by 2^16 in body frame
* @return '0' for a successful load, '1' otherwise
* example: int AccelLoadConfig(long* offset);
* End of Vendor-defined Accel Load Cal Method 
*/

static struct sensor_t sSensorList[LOCAL_SENSORS];
static int sensors = (sizeof(sSensorList) / sizeof(sensor_t));

static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device);

static int sensors__get_sensors_list(struct sensors_module_t* module,
                                     struct sensor_t const** list)
{
    *list = sSensorList;
    return sensors;
}

static struct hw_module_methods_t sensors_module_methods = {
        open: open_sensors
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
        common: {
                tag: HARDWARE_MODULE_TAG,
                version_major: 1,
                version_minor: 0,
                id: SENSORS_HARDWARE_MODULE_ID,
                name: "Invensense module",
                author: "Invensense Inc.",
                methods: &sensors_module_methods,
        },
        get_sensors_list: sensors__get_sensors_list,
};

struct sensors_poll_context_t {
    struct sensors_poll_device_t device; // must be first

    sensors_poll_context_t();
    ~sensors_poll_context_t();
    int activate(int handle, int enabled);
    int setDelay(int handle, int64_t ns);
    int pollEvents(sensors_event_t* data, int count);

private:
    enum {
        mpl = 0,
        compass,
        dmpOrient,
        numSensorDrivers,   // wake pipe goes here
        numFds,
    };

    struct pollfd mPollFds[numSensorDrivers];
    SensorBase *mSensor;
    CompassSensor *mCompassSensor;
};

/******************************************************************************/

sensors_poll_context_t::sensors_poll_context_t() {
    VFUNC_LOG;

    mCompassSensor = new CompassSensor();
    MPLSensor *mplSensor = new MPLSensor(mCompassSensor);

   /* For Vendor-defined Accel Calibration File Load
    * Use the Following Constructor and Pass Your Load Cal File Function
    * 
	* MPLSensor *mplSensor = new MPLSensor(mCompassSensor, AccelLoadConfig);
	*/

    // setup the callback object for handing mpl callbacks
    setCallbackObject(mplSensor);

    // populate the sensor list
    sensors =
            mplSensor->populateSensorList(sSensorList, sizeof(sSensorList));

    mSensor = mplSensor;
    mPollFds[mpl].fd = mSensor->getFd();
    mPollFds[mpl].events = POLLIN;
    mPollFds[mpl].revents = 0;

    mPollFds[compass].fd = mCompassSensor->getFd();
    mPollFds[compass].events = POLLIN;
    mPollFds[compass].revents = 0;

    mPollFds[dmpOrient].fd = ((MPLSensor*) mSensor)->getDmpOrientFd();
    mPollFds[dmpOrient].events = POLLPRI;
    mPollFds[dmpOrient].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t() {
    FUNC_LOG;
    delete mSensor;
    delete mCompassSensor;
}

int sensors_poll_context_t::activate(int handle, int enabled) {
    FUNC_LOG;
    return mSensor->enable(handle, enabled);
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns)
{
    FUNC_LOG;
    return mSensor->setDelay(handle, ns);
}

//#define _DEBUG_RATE
#ifdef _DEBUG_RATE
#define NSEC_PER_SEC            1000000000

static inline int64_t timespec_to_ns(const struct timespec *ts)
{
	return ((int64_t) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static int64_t get_time_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return timespec_to_ns(&ts);
}

static int64_t tm_min=0;
static int64_t tm_max=0;
static int64_t tm_sum=0;
static int64_t tm_last_print=0;
static int64_t tm_count=0;
#endif

#define SENSOR_KEEP_ALIVE       1

#if SENSOR_KEEP_ALIVE
static int sensor_activate[32];
static int sensor_delay[32];
static int64_t sensor_prev_time[32];
#endif

/*
    0 - 0000 - no debug
    1 - 0001 - gyro data
    2 - 0010 - accl data
    4 - 0100 - mag data
    8 - 1000 - raw gyro data with uncalib and bias
 */
static int debug_lvl = 0;
#include <cutils/properties.h>
#include "sensor_params.h"
int sensors_poll_context_t::pollEvents(sensors_event_t *data, int count)
{
    VHANDLER_LOG;

    int nbEvents = 0;
    int nb, polltime = -1;
    char propbuf[PROPERTY_VALUE_MAX];
    int i=0;

    property_get("sensor.debug.level", propbuf, "0");
    debug_lvl = atoi(propbuf);

    // look for new events
    nb = poll(mPollFds, numSensorDrivers, polltime);

    if (nb > 0) {
        for (int i = 0; count && i < numSensorDrivers; i++) {
            if (mPollFds[i].revents & (POLLIN | POLLPRI)) {
                nb = 0;
                if (i == mpl) {
                    ((MPLSensor*) mSensor)->buildMpuEvent();
                    mPollFds[i].revents = 0;
                } else if (i == compass) {
                    ((MPLSensor*) mSensor)->buildCompassEvent();
                    mPollFds[i].revents = 0;
                } else if (i == dmpOrient) {
                    nb = ((MPLSensor*) mSensor)->readDmpOrientEvents(data, count);
                    mPollFds[dmpOrient].revents= 0;
                    if (isDmpScreenAutoRotationEnabled() && nb > 0) {
                        count -= nb;
                        nbEvents += nb;
                        data += nb;
                    }
                }
            }
        }
        nb = ((MPLSensor*) mSensor)->readEvents(data, count);

#if SENSOR_KEEP_ALIVE
		for (i=0; i<nb; i++) {
			if (data[i].sensor>=0 && sensor_activate[data[i].sensor]>0) {
//				LOGD("Skip sensor data, %d, %d", data[i].sensor, sensor_activate[data[i].sensor]);
				--sensor_activate[data[i].sensor];
				memset(data+i, 0, sizeof(sensors_event_t));
				data[i].sensor = -1;
			}
		}
#endif
		if (debug_lvl > 0) {
			for (i=0; i<nb; i++) {
				if ((debug_lvl&1) && data[i].sensor==SENSORS_RAW_GYROSCOPE_HANDLE) {
					float gyro_data[3] = {0,0,0};
					gyro_data[0] = data[i].uncalibrated_gyro.uncalib[0] - data[i].uncalibrated_gyro.bias[0];
					gyro_data[1] = data[i].uncalibrated_gyro.uncalib[1] - data[i].uncalibrated_gyro.bias[1];
					gyro_data[2] = data[i].uncalibrated_gyro.uncalib[2] - data[i].uncalibrated_gyro.bias[2];
					if (debug_lvl&8)
						LOGD("RAW GYRO: %+f %+f %+f - %lld, uncalib: %+f %+f %+f, bias: %+f %+f %+f", gyro_data[0], gyro_data[1], gyro_data[2], data[i].timestamp,
							data[i].uncalibrated_gyro.uncalib[0], data[i].uncalibrated_gyro.uncalib[1], data[i].uncalibrated_gyro.uncalib[2],
							data[i].uncalibrated_gyro.bias[0], data[i].uncalibrated_gyro.bias[1], data[i].uncalibrated_gyro.bias[2]);
					else
						LOGD("RAW GYRO: %+f %+f %+f - %lld", gyro_data[0], gyro_data[1], gyro_data[2], data[i].timestamp);
				}
				if ((debug_lvl&1) && data[i].sensor==SENSORS_GYROSCOPE_HANDLE) {
					LOGD("GYRO: %+f %+f %+f - %lld", data[i].gyro.v[0], data[i].gyro.v[1], data[i].gyro.v[2], data[i].timestamp);
				}
				if ((debug_lvl&2) && data[i].sensor==SENSORS_ACCELERATION_HANDLE) {
					LOGD("ACCL: %+f %+f %+f - %lld", data[i].acceleration.v[0], data[i].acceleration.v[1], data[i].acceleration.v[2], data[i].timestamp);
				}
				if ((debug_lvl&4) && (data[i].sensor==SENSORS_MAGNETIC_FIELD_HANDLE)) {
					LOGD("MAG: %+f %+f %+f - %lld", data[i].magnetic.v[0], data[i].magnetic.v[1], data[i].magnetic.v[2], data[i].timestamp);
				}
			}
		}
		
        if (nb > 0) {
			#ifdef _DEBUG_RATE
            int64_t tm_cur = get_time_ns();
            int64_t tm_delta = tm_cur - data->timestamp;
            if (tm_min==0 && tm_max==0)
                tm_min = tm_max = tm_delta;
            else if (tm_delta < tm_min)
                tm_min = tm_delta;
            else if (tm_delta > tm_max)
                tm_max = tm_delta;
            tm_sum += tm_delta;
            tm_count++;
            
            if ((tm_cur-tm_last_print) > 1000000000) {
                LOGD("poll end: [%lld] %lld,%lld,%lld\n", data->timestamp, tm_min, (tm_sum/tm_count), tm_max);
                tm_last_print = tm_cur;
                tm_min = tm_max = tm_count = tm_sum = 0;
            }
			#endif
			
            count -= nb;
            nbEvents += nb;
            data += nb;
        }
    }

    return nbEvents;
}

/******************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
    FUNC_LOG;
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    if (ctx) {
        delete ctx;
    }
    return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev,
                          int handle, int enabled)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
#if SENSOR_KEEP_ALIVE
    sensor_activate[handle] = enabled?10:0;
#endif	
    return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev,
                          int handle, int64_t ns)
{
#if SENSOR_KEEP_ALIVE
	if (sensor_delay[handle] == ns) {
//		  LOGD("keep sensor(%d) delay %d ns", handle, ns);
		return 0;
	}
	LOGD("set sensor(%d) delay %d ns", handle, ns);
	sensor_delay[handle] = ns;
#endif

    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
    int s= ctx->setDelay(handle, ns);
    return s;
}

static bool ert = false;

static int poll__poll(struct sensors_poll_device_t *dev,
                      sensors_event_t* data, int count)
{
    sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	
	if (!ert) {
		struct sched_param param = {
				.sched_priority = 90,
		};
		sched_setscheduler(0, SCHED_FIFO, &param);
		ert = true;
		  ALOGD("set %d to SCHED_FIFO,90", gettid());
	}

    return ctx->pollEvents(data, count);
}

/******************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char* id,
                        struct hw_device_t** device)
{
    FUNC_LOG;	
    int status = -EINVAL;
    sensors_poll_context_t *dev = new sensors_poll_context_t();

    memset(&dev->device, 0, sizeof(sensors_poll_device_t));

    dev->device.common.tag = HARDWARE_DEVICE_TAG;
    dev->device.common.version  = 0;
    dev->device.common.module   = const_cast<hw_module_t*>(module);
    dev->device.common.close    = poll__close;
    dev->device.activate        = poll__activate;
    dev->device.setDelay        = poll__setDelay;
    dev->device.poll            = poll__poll;

    *device = &dev->device.common;
    status = 0;
	ert = false;

#if SENSOR_KEEP_ALIVE
	memset(sensor_activate, 0, 32*sizeof(int));
	memset(sensor_delay, 0, 32*sizeof(int));
	memset(sensor_prev_time, 0, 32*sizeof(int64_t));
#endif

    return status;
}
