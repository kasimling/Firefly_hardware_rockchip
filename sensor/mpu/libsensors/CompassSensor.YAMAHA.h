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

#ifndef COMPASS_SENSOR_H
#define COMPASS_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "SensorBase.h"

#include "YamahaSensor.h"

#define COMPASS_YAS532_RANGE        (1200.0f)
#define COMPASS_YAS532_RESOLUTION   (0.15f)
#define COMPASS_YAS532_POWER        (0.16f)
#define COMPASS_YAS532_MINDELAY     (10000)
#define COMPASS_YAS537_RANGE        (2000.0f)
#define COMPASS_YAS537_RESOLUTION   (0.3f)
#define COMPASS_YAS537_POWER        (0.16f)
#define COMPASS_YAS537_MINDELAY     (10000)

class CompassSensor : public SensorBase {

    protected:

    public:
        CompassSensor();
        virtual ~CompassSensor();

        int providesCalibration() { return 1; }
        /* all 3rd pary solution have compasses on the primary bus, hence they
           have no dependency on the MPU */
        int isIntegrated() { return 0; }
        /* unnecessary for MPL solution (override 3rd-party solution) */
        virtual int readEvents(sensors_event_t *data, int count) {
            (void) data;
            (void) count;
            return 0;
        }

        /*
         * make sure either 3rd-party compass solution has following virtual
         * functions, or SensorBase.cpp could provide equal functionalities
         */
        virtual int getFd() const;
        virtual int enable(int32_t handle, int enabled);
        virtual int setDelay(int32_t handle, int64_t ns);
        virtual int getEnable(int32_t handle);
        virtual int64_t getDelay(int32_t handle);
        virtual int64_t getMinDelay() { return -1; } // stub

        /*
         * following four APIs need further implementation for MPL's reference
         * (look into .cpp for detailed information, also refer to 3rd-party's
         * readEvents() for relevant APIs)
         */
        int readSample(long *data, int64_t *timestamp);
        void fillList(struct sensor_t *list);
        void getOrientationMatrix(signed char *orient);
        int getAccuracy();
        long getSensitivity();

    private:
        YamahaSensor *mCompassSensor;
};

#endif  /* COMPASS_SENSOR_H */
