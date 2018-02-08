/*
 * Copyright (C) 2014-2015 Yamaha Corporation
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


#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>

#include "YamahaSensor.h"
#include "yas_android_lib.h"

#define IIO_MAX_NAME_LENGTH 30

static const char *iio_dir = "/sys/bus/iio/devices/";

static int chomp(char *buf, int len)
{
    if (buf == NULL || len < 0)
        return -1;
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[len - 1] = '\0';
        len--;
    }
    return 0;
}

static void reverse(char s[])
{
    int c, i, j;
    for (i = 0, j = (int)strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = (char)c;
    }
}

static int int2a(char *buf, int value, int bufsize)
{
    unsigned int tmp;
    int i, sign;
    if (buf == NULL)
        return -1;
    sign = value;
    if (sign < 0) {
        tmp = (unsigned int)(-value);
        bufsize--;
    } else
        tmp = (unsigned int)value;
    if (bufsize <= 0)
        return -1;
    i = 0;
    do {
        if (i >= bufsize - 1)
            return -1;
        buf[i++] = (char)(tmp % 10 + '0');
    } while ((tmp /= 10) > 0);
    if (sign < 0)
        buf[i++] = '-';
    buf[i] = '\0';
    reverse(buf);
    return 0;
}

static int sysfs_set_input_attr(char *devpath, const char *attr, char *value,
        int len)
{
    char fname[PATH_MAX];
    int fd;
    snprintf(fname, sizeof(fname), "%s/%s", devpath, attr);
    fname[sizeof(fname) - 1] = '\0';
    fd = open(fname, O_WRONLY);
    if (fd < 0)
        return -errno;
    if (write(fd, value, (size_t)len) < 0) {
        close(fd);
        return -errno;
    }
    close(fd);
    return 0;
}

static int sysfs_set_input_attr_by_int(char *devpath, const char *attr,
        int value)
{
    char buf[sizeof("-2147483647")];
    int2a(buf, value, sizeof(buf));
    return sysfs_set_input_attr(devpath, attr, buf, sizeof(buf));
}

static int sysfs_get_input_attr(char *devpath, const char *attr, char *value,
        int len)
{
    char fname[PATH_MAX];
    int fd;
    int nread;
    snprintf(fname, sizeof(fname), "%s/%s", devpath, attr);
    fname[sizeof(fname) - 1] = '\0';
    fd = open(fname, O_RDONLY);
    if (fd < 0)
        return -errno;
    nread = (int)read(fd, value, (size_t)len);
    if (nread < 0) {
        close(fd);
        return -errno;
    }
    value[nread - 1] = '\0';
    close(fd);
    chomp(value, (int)strlen(value));
    return 0;
}

static int sysfs_get_input_attr_by_int(char *devpath, const char *attr,
        int *value)
{
    char buf[sizeof("-2147483647")];
    int rt;
    if (value == NULL)
        return -EINVAL;
    rt = sysfs_get_input_attr(devpath, attr, buf, sizeof(buf));
    if (rt < 0)
        return rt;
    *value = atoi(buf);
    return 0;
}

static inline int find_type_by_name(const char *name, const char *type)
{
    const struct dirent *ent;
    int number, numstrlen;

    FILE *nameFile;
    DIR *dp;
    char thisname[IIO_MAX_NAME_LENGTH];
    char *filename;

    dp = opendir(iio_dir);
    if (dp == NULL) {
        return -ENODEV;
    }

    while (ent = readdir(dp), ent != NULL) {
        if (strcmp(ent->d_name, ".") != 0 &&
                strcmp(ent->d_name, "..") != 0 &&
                strlen(ent->d_name) > strlen(type) &&
                strncmp(ent->d_name, type, strlen(type)) == 0) {
            numstrlen = sscanf(ent->d_name + strlen(type),
                    "%d",
                    &number);
            /* verify the next character is not a colon */
            if (strncmp(ent->d_name + strlen(type) + numstrlen,
                        ":",
                        1) != 0) {
                filename = (char *)malloc(strlen(iio_dir)
                        + strlen(type)
                        + numstrlen
                        + 6);
                if (filename == NULL)
                    return -ENOMEM;
                sprintf(filename, "%s%s%d/name",
                        iio_dir,
                        type,
                        number);
                nameFile = fopen(filename, "r");
                if (!nameFile)
                    continue;
                free(filename);
                fscanf(nameFile, "%s", thisname);
                if (strcmp(name, thisname) == 0)
                    return number;
                fclose(nameFile);
            }
        }
    }
    return -ENODEV;
}

YamahaSensor::YamahaSensor()
    : SensorBase(NULL, NULL), mEnabled(0), mDelayNs(50000000),
    mCountToNotify(0), mCounter(0), mAccuracy(0)
{
    char buffer_access[PATH_MAX];
    const char *device_name = YAS_MAG_NAME;
    int rate = 20, dev_num;
    dev_num = find_type_by_name(device_name, "iio:device");
    if (dev_num < 0) {
        dev_num = 0;
    }
    snprintf(buffer_access, sizeof(buffer_access),
            "/dev/iio:device%d", dev_num);
    dev_fd = open(buffer_access, O_RDONLY | O_NONBLOCK);
    snprintf(mDevPath, sizeof(mDevPath), "%siio:device%d", iio_dir, dev_num);
    snprintf(mTriggerName, sizeof(mTriggerName), "%s-dev%d",
            device_name, dev_num);
    /* read initial rate */
    if (sysfs_get_input_attr_by_int(mDevPath, "sampling_frequency", &rate) == 0) {
        if (rate <= 0)
            rate = 20;
        mDelayNs = (int64_t)(1000000000LL / rate);
    }
    /* read initial value */
    Magnetic_Initialize();
}

YamahaSensor::~YamahaSensor() {
    if (mEnabled)
        enable(ID_M, 0);
}

int YamahaSensor::updateDelay() {
    int64_t ns = -1;
    if (!mEnabled)
        return 0;
    ns = mDelayNs;
    if (ns == 200000000) {
        mCountToNotify = 4;
        mCounter = 0;
    } else {
        mCountToNotify = mDelayNs / ns;
        mCounter = 0;
    }
    if (ns == 200000000) {
        ns = 50000000;
    }
    if (sysfs_set_input_attr_by_int(mDevPath, "sampling_frequency",
                (int)(1000000000LL / ns)) < 0)
        return -1;
    Magnetic_Set_Delay(ns);
    return 0;
}

int YamahaSensor::getEnable(int handle)
{
    if (handle != ID_M)
        return 0;
    return mEnabled;
}

int YamahaSensor::enable(int32_t handle, int active)
{
    YLOGD(("YamahaSensor::enable IN %d %d\n", handle, active));
    YLOGD(("YamahaSensor mDevPath[%s] mTriggerName[%s]\n",
                mDevPath, mTriggerName));
    if (handle != ID_M)
        return -1;
    active = !!active;
    if (mEnabled == active)
        return 0;
    mEnabled = active;
    if (active)
        Magnetic_Enable();
    else
        Magnetic_Disable();
    updateDelay();
    if (active) {
        if (sysfs_set_input_attr_by_int(mDevPath, "buffer/length", 2) < 0)
            return -1;
        if (sysfs_set_input_attr(mDevPath, "trigger/current_trigger",
                    mTriggerName, strlen(mTriggerName)) < 0)
            return -1;
        if (sysfs_set_input_attr_by_int(mDevPath, "buffer/enable", active) < 0)
            return -1;
    } else {
        if (sysfs_set_input_attr_by_int(mDevPath, "buffer/enable", active) < 0)
            return -1;
    }
    YLOGD(("YamahaSensor::enable OUT\n"));
    return 0;
}

int64_t YamahaSensor::getDelay(int32_t handle)
{
    if (handle != ID_M)
        return 0;
    return mDelayNs;
}

int YamahaSensor::setDelay(int32_t handle, int64_t ns)
{
    YLOGD(("YamahaSensor::setDelay IN %d %lld\n", handle, ns));
    if (handle != ID_M)
        return -1;
    if (ns == 66667000)
        ns = 66666000;
    mDelayNs = ns;
    updateDelay();
    YLOGD(("YamahaSensor::setDelay OUT\n"));
    return 0;
}

int YamahaSensor::readEvents(sensors_event_t* data, int count)
{
    int32_t tmp[12], *p;
    int numEventReceived = 0, nread, i;
    SENSORDATA mag, calmag;

    YLOGD(("YamahaSensor::readEvents IN [%p] [%d]\n", data, count));
    if (count < 1 || dev_fd < 0)
        return -EINVAL;
    nread = read(dev_fd, tmp, sizeof(tmp));
    p = tmp;
    for (i = 0; i < nread / 24; i++) {
        mag.x = p[0];
        mag.y = p[1];
        mag.z = p[2];
        Magnetic_Calibrate(&mag, &calmag);
        if (mEnabled && mCountToNotify
                <= ++mCounter) {
            memset(&data[numEventReceived], 0, sizeof(sensors_event_t));
            data[numEventReceived].version = sizeof(sensors_event_t);
            data[numEventReceived].sensor = ID_M;
            data[numEventReceived].type = SENSOR_TYPE_MAGNETIC_FIELD;
            data[numEventReceived].magnetic.v[0] = calmag.vx;
            data[numEventReceived].magnetic.v[1] = calmag.vy;
            data[numEventReceived].magnetic.v[2] = calmag.vz;
            data[numEventReceived].magnetic.status = calmag.accuracy;
            data[numEventReceived].timestamp = *(int64_t *)(&p[4]);
            mCounter = 0;
            numEventReceived++;
            count--;
            if (count == 0)
                break;
        }
        p += 6;
    }
    YLOGD(("YamahaSensor::readEvents OUT[%d]\n", numEventReceived));

    return numEventReceived;
}


int YamahaSensor::readSample(long *data, int64_t *timestamp)
{
    sensors_event_t ev;
    int rt;
    int i;

    rt = readEvents(&ev, 1);
    if (rt < 0)
        return -1;
    if (rt == 0)
        return 0;
    for (i = 0; i < 3; i++)
        data[i] = (long)(ev.magnetic.v[i] * (1<<16));
    mAccuracy = ev.magnetic.status;
    *timestamp = ev.timestamp;
    return 1;
}

int YamahaSensor::getAccuracy()
{
    return mAccuracy;
}

