/*
 * Copyright (C) 2008 The Android Open Source Project
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
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <dlfcn.h>

#include <cutils/log.h>

#include "AkmSensor.h"
#include "akm8975.h"
#define AKMD_DEFAULT_INTERVAL	200000000

/*****************************************************************************/

AkmSensor::AkmSensor()
: SensorBase(AKM_DEVICE_NAME, "compass"),
      //mPendingMask(0),
      mInputReader(32)
{
	mEnabled = 0;
	mDelay = -1;
	
    //mPendingEvents.version = sizeof(sensors_event_t);
    //mPendingEvents.sensor = ID_M;
    //mPendingEvents.type = SENSOR_TYPE_MAGNETIC_FIELD;
    //mPendingEvents.magnetic.status = SENSOR_STATUS_ACCURACY_HIGH;
    //memset(mPendingEvents.data, 0, sizeof(mPendingEvents.data));

    setEnable(ID_M, 0);
}

AkmSensor::~AkmSensor()
{
    setEnable(ID_M, 0);
}

int AkmSensor::write_sys_attribute(const char *path, const char *value, int bytes)
{
    int fd, amt;

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        LOGE("AkmSensor: write_attr failed to open %s (%s)",
                path, strerror(errno));
        return -1;
    }

    amt = write(fd, value, bytes);
    amt = ((amt == -1) ? -errno : 0);
    LOGE_IF(amt < 0, "AkmSensor: write_int failed to write %s (%s)",
                path, strerror(errno));
    close(fd);
    return amt;
}

int AkmSensor::setEnable(int32_t handle, int enabled)
{
    int id = (int)handle;
    int err = 0;
	int cmd;
	int newState;
	short flags;
	
    switch (id) {
        case ID_M:
			cmd = ECS_IOCTL_APP_SET_MFLAG;
            break;
	default:
            LOGE("AkmSensor: unknown handle (%d)", handle);
            return -EINVAL;
    }

    if (mEnabled <= 0) {
        if (enabled) {
            open_device();
			flags = 1;
			err = ioctl(dev_fd, cmd, &flags);
			if (err < 0) {
				LOGE("ECS_IOCTL_APP_SET_XXX failed (%s)", strerror(-err));
				return err;
			}
			LOGD("AkmSensor: enable");
            setDelay(handle, AKMD_DEFAULT_INTERVAL);
        }
    } else if (mEnabled == 1) {
        if (!enabled) {
			flags = 0;
			err = ioctl(dev_fd, cmd, &flags);
			if (err < 0) {
				LOGE("ECS_IOCTL_APP_SET_XXX failed (%s)", strerror(-err));
				return err;
			}
			LOGD("AkmSensor: disable");
			setDelay(handle, -1);
			close_device();
        }
    }

    if (enabled) {
        (mEnabled)++;
        if (mEnabled > 32767) mEnabled = 32767;
    } else {
        (mEnabled)--;
        if (mEnabled < 0) mEnabled = 0;
    }
    LOGD("AkmSensor: mEnabled = %d", mEnabled);

    return err;
}

int AkmSensor::setDelay(int32_t handle, int64_t ns)
{
    int id = (int)handle;
    int err = 0;
    char buffer[32];
    int cmd;

    if (ns < -1 || 2147483647 < ns) {
        LOGE("AkmSensor: invalid delay (%lld)", ns);
        return -EINVAL;
    }

    switch (id) {
        case ID_M:
			cmd = ECS_IOCTL_APP_SET_DELAY;
            break;
        default:
            LOGE("AkmSensor: unknown handle (%d)", handle);
            return -EINVAL;
    }

    if (ns != mDelay) {
        short delay = ns / 1000000;
		err = ioctl(dev_fd, ECS_IOCTL_APP_SET_DELAY, &delay);
        if (err == 0) {
            mDelay = ns;
            LOGD("AkmSensor: set delay to %f ms.", ns/1000000.0f);
        }
    }

    return err;
}

int64_t AkmSensor::getDelay()
{
    return mDelay;
}

int AkmSensor::getEnable()
{
    return mEnabled;
}

int AkmSensor::readEvents(sensors_event_t* data, int count)
{
    return 0;
}
#if 0
int AkmSensor::readEvents(sensors_event_t* data, int count)
{
    if (count < 1)
        return -EINVAL;

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0)
        return n;

    int numEventReceived = 0;
    input_event const* event;

    while (count && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            switch (event->code) {
            	case EVENT_TYPE_MAGV_X:
            	mPendingMask |= 1;
            	mPendingEvents.magnetic.x = event->value * CONVERT_M;
            	break;
        	case EVENT_TYPE_MAGV_Y:
            	mPendingMask |= 1;
            	mPendingEvents.magnetic.y = event->value * CONVERT_M;
            	break;
        	case EVENT_TYPE_MAGV_Z:
           	mPendingMask |= 1;
            	mPendingEvents.magnetic.z = event->value * CONVERT_M;
            	break;
        	case EVENT_TYPE_MAGV_STATUS:
            	mPendingMask |= 1;
            	mPendingEvents.magnetic.status = event->value;
            	break;
            }           
            mInputReader.next();
        } else if (type == EV_SYN) {
            int64_t time = timevalToNano(event->time);
            if (count && mPendingMask) {
                if (mPendingMask & 1) {
                    mPendingMask &= ~(1);
                    mPendingEvents.timestamp = time;
					//LOGD("data=%8.5f,%8.5f,%8.5f",
						//mPendingEvents.data[0],
						//mPendingEvents.data[1],
						//mPendingEvents.data[2]);
                    if (mEnabled) {
                        *data++ = mPendingEvents;
                        count--;
                        numEventReceived++;
                    }
                }
            }
            if (!mPendingMask) {
                mInputReader.next();
            }
        } else {
            LOGE("AkmSensor: unknown event (type=%d, code=%d)",
                    type, event->code);
            mInputReader.next();
        }
    }
    return numEventReceived;
}
#endif

#define COMPASS_EVENT_DEBUG (0)
void AkmSensor::processCompassEvent(int code, int value)
{
    switch (code) {
    case EVENT_TYPE_MAGV_X:
        LOGV_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_X\n");
        mCachedCompassData[0] = value * CONVERT_M * 65536;
        break;
    case EVENT_TYPE_MAGV_Y:
        LOGV_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_Y\n");
        mCachedCompassData[1] = value * CONVERT_M * 65536;
        break;
    case EVENT_TYPE_MAGV_Z:
        LOGV_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_Z\n");
        mCachedCompassData[2] = value * CONVERT_M * 65536;
        break;
    case EVENT_TYPE_MAGV_STATUS:
        LOGV_IF(COMPASS_EVENT_DEBUG, "EVENT_TYPE_MAGV_STATUS\n");
        mCachedCompassAccuracy = value;
        break;
    }
}

int AkmSensor::readSample(long *data, int64_t *timestamp)
{
    int numEventReceived = 0, done = 0;

    ssize_t n = mInputReader.fill(data_fd);
    if (n < 0) {
        return n;
    }
    
    input_event const* event;

    while (done == 0 && mInputReader.readEvent(&event)) {
        int type = event->type;
        if (type == EV_ABS) {
            // TODO: if getting compass data
            processCompassEvent(event->code, event->value);
        } else if (type == EV_SYN) {
            // TODO: if getting timestamp from 3rd-party's driver
            *timestamp = timevalToNano(event->time);
            memcpy(data, mCachedCompassData, sizeof(mCachedCompassData));
            done = 1;
       } else {
            LOGE("HAL:Compass Sensor: unknown event (type=%d, code=%d)", type, event->code);
            LOGE("AkmSensor: unknown event (type=%d, code=%d)", type, event->code);
        }
        mInputReader.next();
    }
    return done;
}

int AkmSensor::getAccuracy()
{
	  if(mCachedCompassAccuracy>=2)
	  		return 3;
	  else		
    		return mCachedCompassAccuracy;
}
