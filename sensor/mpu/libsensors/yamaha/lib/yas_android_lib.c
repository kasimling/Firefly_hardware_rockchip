/*
 * CONFIDENTIAL
 * Copyright(c) 2013-2015 Yamaha Corporation
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#include "yas.h"
#include "yas_math.h"
#include "yas_android_lib.h"
#include "yas_mag_file.h"
#include "sysfs_util.h"

#define MAX_ATTR_PATH	(256)
#define ORIENTATION_FILTER_ENABLE	(1)
#define ORIENTATION_FILTER_LEN		(10)

#define MAG_INTENSITY_MIN		(15000) /* nT */
#define MAG_INTENSITY_MAX		(90000) /* nT */
#define HEADING_ERROR_GOOD		(10000) /* milli-degree */
#define HEADING_ERROR_BAD		(180000) /* milli-degree */
#define HEADING_ERROR_UNSUPPORT		(-1) /* milli-degree */

struct yas_android_lib {
	struct yas_mag_calib mcalib;
	struct yas_mag_filter mfilter;
	struct yas_calib_file mfile;
#if YAS_SOFTWARE_GYROSCOPE_ENABLE
	struct yas_swgyro swgyro;
#endif
#if YAS_ATTITUDE_FILTER_ENABLE
	struct yas_attitude_filter attitude_filter;
#endif
	struct yas_mag_param param;
	char classpath[MAX_ATTR_PATH];
	pthread_mutex_t mutex;
	int enable;
	int file_loaded;
};
static struct yas_android_lib algo;
static struct yas_mag_param default_mag_param = {
	{
		YAS_DEFAULT_CALIB_MODE,
		{	YAS_DEFAULT_SPREAD_1,
			YAS_DEFAULT_SPREAD_2,
			YAS_DEFAULT_SPREAD_3},
		{	YAS_DEFAULT_VARIATION_1,
			YAS_DEFAULT_VARIATION_2,
			YAS_DEFAULT_VARIATION_3},
#if YAS_MAG_CALIB_ELLIPSOID_ENABLE
		{	YAS_DEFAULT_EL_SPREAD_1,
			YAS_DEFAULT_EL_SPREAD_2,
			YAS_DEFAULT_EL_SPREAD_3},
		{	YAS_DEFAULT_EL_VARIATION_1,
			YAS_DEFAULT_EL_VARIATION_2,
			YAS_DEFAULT_EL_VARIATION_3},
#endif
#if !YAS_MAG_CALIB_MINI_ENABLE
		{YAS_DEFAULT_TRAD_VARIATION_1,
			YAS_DEFAULT_TRAD_VARIATION_2,
			YAS_DEFAULT_TRAD_VARIATION_3},
#endif
#if YAS_MAG_CALIB_WITH_GYRO_ENABLE
		{YAS_DEFAULT_CWG_THRESHOLD_0,
			YAS_DEFAULT_CWG_THRESHOLD_1,
			YAS_DEFAULT_CWG_THRESHOLD_2,
			YAS_DEFAULT_CWG_THRESHOLD_3,
			YAS_DEFAULT_CWG_THRESHOLD_4,
			YAS_DEFAULT_CWG_THRESHOLD_5,
			YAS_DEFAULT_CWG_THRESHOLD_6,
			YAS_DEFAULT_CWG_THRESHOLD_7,
			YAS_DEFAULT_CWG_THRESHOLD_8,
			YAS_DEFAULT_CWG_THRESHOLD_9,
			YAS_DEFAULT_CWG_THRESHOLD_10,
			YAS_DEFAULT_CWG_THRESHOLD_11},
#endif
	},
	YAS_DEFAULT_FILTER_ENABLE,
	{-128, -128, -128},
	{ {0, 0, 0} },
	0,
	{ {10000, 0, 0, 0, 10000, 0, 0, 0, 10000} },
};

#if ORIENTATION_FILTER_ENABLE
struct azimuth_vector {
	int azimuth;
	float x;
	float y;
};
static struct azimuth_vector orientation_log[ORIENTATION_FILTER_LEN];
static int orientation_index;
static int orientation_num;

static int orientation_filter_update(int azimuth)
{
	int i;
	float theta = azimuth / 180000.0f * M_PI;
	float sum_x, sum_y, sum_azimuth, avg_x, avg_y, avg_azimuth, result;

	orientation_log[orientation_index].azimuth = azimuth;
	orientation_log[orientation_index].x = cosf(theta);
	orientation_log[orientation_index].y = sinf(theta);
	orientation_index++;
	if (ORIENTATION_FILTER_LEN <= orientation_index)
		orientation_index = 0;
	orientation_num++;
	if (ORIENTATION_FILTER_LEN <= orientation_num)
		orientation_num = ORIENTATION_FILTER_LEN;

	sum_x = 0;
	sum_y = 0;
	sum_azimuth = 0;
	for (i = 0; i < orientation_num; i++) {
		sum_azimuth += orientation_log[i].azimuth;
		sum_x += orientation_log[i].x;
		sum_y += orientation_log[i].y;
	}
	avg_x = sum_x / orientation_num;
	avg_y = sum_y / orientation_num;
	avg_azimuth = sum_azimuth / (float)orientation_num;
	if (fabs(avg_x) < 0.001f && fabs(avg_y) < 0.001f) {
		result = avg_azimuth;
	} else {
		avg_x = avg_x / sqrtf(avg_x * avg_x + avg_y * avg_y);
		avg_y = avg_y / sqrtf(avg_x * avg_x + avg_y * avg_y);
		result = atan2(avg_y, avg_x) / M_PI * 180.0 * 1000;
	}
	if (result < 0)
		result += 360000.0;
	if (360000.0 <= result)
		result = 0;
	return (int)result;
}

static void orientation_filter_init(void)
{
	orientation_index = 0;
	orientation_num = 0;
}
#endif

static uint32_t calc_intensity(int32_t *v)
{
	int32_t sum = 0;
	int i;
	for (i = 0; i < 3; i++)
		sum += v[i] / 100 * v[i] / 100;
	return (uint32_t)(yas_sqrt32(sum) * 100);
}

static void
apply_matrix(struct yas_vector *xyz, struct yas_matrix *m)
{
	int32_t tmp[3];
	int i;
	if (m == NULL)
		return;
	for (i = 0; i < 3; i++)
		tmp[i] = ((m->m[i*3]/10) * (xyz->v[0]/10)
				+ (m->m[i*3+1]/10) * (xyz->v[1]/10)
				+ (m->m[i*3+2]/10) * (xyz->v[2]/10)) / 100;
	for (i = 0; i < 3; i++)
		xyz->v[i] = tmp[i];
}

static int
file_load(void)
{
	char buf[sizeof("-2147483647")*3 + 1];
	int rt = 0;
	algo.mcalib.get_config(&algo.param.config);
	algo.mfile.load(&algo.param);
	sprintf(buf, "%d %d %d", algo.param.hard_offset[0],
			algo.param.hard_offset[1], algo.param.hard_offset[2]);
#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 || \
	YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533
	rt = sysfs_set_input_attr(algo.classpath, "hard_offset", buf,
			(int) strlen(buf));
#endif
	algo.mcalib.set_offset(YAS_TYPE_MAG, &algo.param.calib_offset,
			algo.param.accuracy);
#if YAS_MAG_CALIB_ELLIPSOID_ENABLE
	algo.mcalib.set_dynamic_matrix(&algo.param.dynamic_matrix);
#endif
	algo.mcalib.set_config(&algo.param.config);
	if (algo.param.filter_enable)
		algo.mfilter.init();
	return rt;
}

static int
file_save(void)
{
	char buf[sizeof("-2147483647")*3 + 1];
	int32_t tmp[3];
	int i, ret;
	if (sysfs_get_input_attr(algo.classpath, "hard_offset", buf,
				sizeof(buf)) < 0) {
		for (i = 0; i < 3; i++)
			algo.param.hard_offset[i] = -128;
	} else {
		ret = sscanf(buf, "%d %d %d", &tmp[0], &tmp[1], &tmp[2]);
		if (ret != 3) {
			for (i = 0; i < 3; i++)
				algo.param.hard_offset[i] = -128;
		} else {
			for (i = 0; i < 3; i++)
				algo.param.hard_offset[i] = (int8_t)tmp[i];
		}
	}
	return algo.mfile.save(&algo.param);
}

static void yas_normalize_attitude_under_norm(struct yas_quaternion *q)
{
	int32_t size;
	int32_t sq_size;
	int64_t sq_size64 = 0;
	int_fast8_t i;

	for (i = 3; i >= 0; --i) {
		if (q->q[i] < -11585 || q->q[i] > 11585) {
			q->q[0] = q->q[1] = q->q[2] = 0;
			q->q[3] = YAS_QUATERNION_NORM;
			return;
		}
		sq_size64 += q->q[i] * q->q[i];
	}
	if (sq_size64 > 134212225 || sq_size64 == 0) {
		q->q[0] = q->q[1] = q->q[2] = 0;
		q->q[3] = YAS_QUATERNION_NORM;
		return;
	}
	sq_size = sq_size64;
	sq_size <<= 4;
	size = yas_sqrt32(sq_size);
	if (sq_size != size * size)
		size++; /* significant process!! */

	for (i = 3; i >= 0; --i) {
		q->q[i] *= (YAS_QUATERNION_NORM << 2);
		q->q[i] /= size;
	}
}

int
Magnetic_Enable(void)
{
	int rt = 0;
	pthread_mutex_lock(&algo.mutex);
	YASLOGD(("Magnetic_Enable IN\n"));
	if (algo.enable) {
		YASLOGD(("Magnetic_Enable OUT\n"));
		pthread_mutex_unlock(&algo.mutex);
		return 0;
	}
	if (!algo.file_loaded)
		rt = file_load();
	algo.enable = 1;
	algo.file_loaded = 1;
#if ORIENTATION_FILTER_ENABLE
	orientation_filter_init();
#endif
	YASLOGD(("Magnetic_Enable OUT [%d]\n", rt));
	pthread_mutex_unlock(&algo.mutex);
	return rt;
}

int
Magnetic_Disable(void)
{
	int rt;
	pthread_mutex_lock(&algo.mutex);
	YASLOGD(("Magnetic_Disable IN\n"));
	if (!algo.enable) {
		YASLOGD(("Magnetic_Disable OUT\n"));
		pthread_mutex_unlock(&algo.mutex);
		return 0;
	}
	rt = file_save();
	algo.enable = 0;
	algo.file_loaded = 0;
	YASLOGD(("Magnetic_Disable OUT [%d]\n", rt));
	pthread_mutex_unlock(&algo.mutex);
	return rt;
}

int
Magnetic_Set_Delay(uint64_t delay)
{
	YASLOGD(("Magnetic_Set_Delay IN [%lld]\n", delay));
	(void) delay;
	YASLOGD(("Magnetic_Set_Delay OUT\n"));
	return 0;
}

int
Magnetic_Get_Euler(SENSORDATA *acccal, SENSORDATA *magcal,
		SENSORDATA *orientation)
{
	struct yas_vector acc, mag, euler;
	sfixed m[9];

	YASLOGD(("Magnetic_Get_Euler IN accel[%f %f %f] mag[%f %f %f]\n",
				acccal->vx, acccal->vy, acccal->vz, magcal->vx,
				magcal->vy, magcal->vz));
	memset(orientation, 0, sizeof(*orientation));
	acc.v[0] = (int32_t)(acccal->vx * 1000000.0f);
	acc.v[1] = (int32_t)(acccal->vy * 1000000.0f);
	acc.v[2] = (int32_t)(acccal->vz * 1000000.0f);
	mag.v[0] = (int32_t)(magcal->vx * 1000.0f);
	mag.v[1] = (int32_t)(magcal->vy * 1000.0f);
	mag.v[2] = (int32_t)(magcal->vz * 1000.0f);
	yas_get_matrix(&acc, &mag, m);
	yas_get_orientation(m, &euler);

#if ORIENTATION_FILTER_ENABLE
	orientation->vx = (float)(orientation_filter_update(euler.v[0]))
		/ 1000.0f;
#else
	orientation->vx = (float)euler.v[0] / 1000.0f;
#endif
	orientation->vy = (float)euler.v[1] / 1000.0f;
	orientation->vz = (float)euler.v[2] / 1000.0f;
	orientation->accuracy = magcal->accuracy;
	YASLOGD(("Magnetic_Get_Euler OUT orientation[%f %f %f][%d]\n",
			orientation->vx,
			orientation->vy,
			orientation->vz,
			orientation->accuracy));
	return 0;
}

int
Magnetic_Get_Quaternion(SENSORDATA *acccal, SENSORDATA *magcal,
		QUATERNION *quaternion)
{
	struct yas_vector acc, mag;
	struct yas_quaternion q;
	uint32_t mag_intensity = 0;
	sfixed m[9];
	YASLOGD(("Magnetic_Get_Quaternion IN accel[%f %f %f] mag[%f %f %f]\n",
				acccal->vx, acccal->vy, acccal->vz, magcal->vx,
				magcal->vy, magcal->vz));
	memset(quaternion, 0, sizeof(*quaternion));
	acc.v[0] = (int32_t)(acccal->vx * 1000000.0f);
	acc.v[1] = (int32_t)(acccal->vy * 1000000.0f);
	acc.v[2] = (int32_t)(acccal->vz * 1000000.0f);
	mag.v[0] = (int32_t)(magcal->vx * 1000.0f);
	mag.v[1] = (int32_t)(magcal->vy * 1000.0f);
	mag.v[2] = (int32_t)(magcal->vz * 1000.0f);
	mag_intensity = calc_intensity(mag.v);
	if (MAG_INTENSITY_MIN <= mag_intensity
			&& mag_intensity <= MAG_INTENSITY_MAX)
		quaternion->heading_error = HEADING_ERROR_GOOD / 1000.0f;
	else
		quaternion->heading_error = HEADING_ERROR_BAD / 1000.0f;
	yas_get_matrix(&acc, &mag, m);
	yas_get_quaternion(m, &q);
	yas_normalize_attitude_under_norm(&q);
	quaternion->q[0] = (float)q.q[0] / 10000.0f;
	quaternion->q[1] = (float)q.q[1] / 10000.0f;
	quaternion->q[2] = (float)q.q[2] / 10000.0f;
	quaternion->q[3] = (float)q.q[3] / 10000.0f;
	YASLOGD(("Magnetic_Get_Quaternion OUT q[%f %f %f %f]\n",
				quaternion->q[0],
				quaternion->q[1],
				quaternion->q[2],
				quaternion->q[3]));
	return 0;
}

#if YAS_SOFTWARE_GYROSCOPE_ENABLE
int Magnetic_Get_SoftwareGyroscope(SENSORDATA *acccal, SENSORDATA *magcal,
		uint32_t timestamp, SENSORDATA *gyro)
{
	struct yas_data data[2];
	struct yas_swgyro_result r;
	int rt, result = 0;
	YASLOGD(("Magnetic_Get_SoftwareGyroscope IN accel[%f %f %f] "
				"mag[%f %f %f] timestamp[%u]\n",
				acccal->vx, acccal->vy, acccal->vz, magcal->vx,
				magcal->vy, magcal->vz, timestamp));
	data[0].type = YAS_TYPE_ACC;
	data[0].xyz.v[0] = (int32_t)(acccal->vx * 1000000.0f);
	data[0].xyz.v[1] = (int32_t)(acccal->vy * 1000000.0f);
	data[0].xyz.v[2] = (int32_t)(acccal->vz * 1000000.0f);
	data[0].timestamp = timestamp;
	data[0].accuracy = 0;
	data[1].type = YAS_TYPE_MAG;
	data[1].xyz.v[0] = (int32_t)(magcal->vx * 1000.0f);
	data[1].xyz.v[1] = (int32_t)(magcal->vy * 1000.0f);
	data[1].xyz.v[2] = (int32_t)(magcal->vz * 1000.0f);
	data[1].timestamp = timestamp;
	data[1].accuracy = 0;
	rt = algo.swgyro.update(data, 2);
	if (rt < 0)
		result = rt;
	rt = algo.swgyro.get_result(&r);
	if (rt < 0)
		result = rt;
	gyro->vx = (float)r.swgyro.v[0] / 57295.78f; /* mdps -> radian */
	gyro->vy = (float)r.swgyro.v[1] / 57295.78f; /* mdps -> radian */
	gyro->vz = (float)r.swgyro.v[2] / 57295.78f; /* mdps -> radian */
	gyro->accuracy = 0;
	YASLOGD(("Magnetic_Get_SoftwareGyroscope OUT gyro[%f %f %f]\n",
				gyro->vx, gyro->vy, gyro->vz));
	return result;
}
#endif

#if YAS_ATTITUDE_FILTER_ENABLE
int Magnetic_Get_Filtered_Quaternion(SENSORDATA *acccal, SENSORDATA *magcal,
		QUATERNION *quaternion, SENSORDATA *gravity,
		SENSORDATA *linear_acceleration)
{
	struct yas_data data[2];
	struct yas_attitude_filter_result r;
	int rt, result = 0;
	YASLOGD(("Magnetic_Get_Filtered_Quaternion IN accel[%f %f %f] "
				"mag[%f %f %f]\n",
				acccal->vx, acccal->vy, acccal->vz,
				magcal->vx, magcal->vy, magcal->vz));
	data[0].type = YAS_TYPE_ACC;
	data[0].xyz.v[0] = (int32_t)(acccal->vx * 1000000.0f);
	data[0].xyz.v[1] = (int32_t)(acccal->vy * 1000000.0f);
	data[0].xyz.v[2] = (int32_t)(acccal->vz * 1000000.0f);
	data[0].timestamp = 0;
	data[0].accuracy = 0;
	data[1].type = YAS_TYPE_MAG;
	data[1].xyz.v[0] = (int32_t)(magcal->vx * 1000.0f);
	data[1].xyz.v[1] = (int32_t)(magcal->vy * 1000.0f);
	data[1].xyz.v[2] = (int32_t)(magcal->vz * 1000.0f);
	data[1].timestamp = 0;
	data[1].accuracy = 0;
	rt = algo.attitude_filter.update(data, 2);
	if (rt < 0)
		result = rt;
	rt = algo.attitude_filter.get_result(&r);
	if (rt < 0)
		result = rt;
	quaternion->q[0] = (float)r.quaternion.q[0] / 10000.0f;
	quaternion->q[1] = (float)r.quaternion.q[1] / 10000.0f;
	quaternion->q[2] = (float)r.quaternion.q[2] / 10000.0f;
	quaternion->q[3] = (float)r.quaternion.q[3] / 10000.0f;
	gravity->vx = (float)r.gravity.v[0] / 1000000.0f;
	gravity->vy = (float)r.gravity.v[1] / 1000000.0f;
	gravity->vz = (float)r.gravity.v[2] / 1000000.0f;
	gravity->accuracy = 0;
	linear_acceleration->vx
		= (float)r.linear_acceleration.v[0] / 1000000.0f;
	linear_acceleration->vy
		= (float)r.linear_acceleration.v[1] / 1000000.0f;
	linear_acceleration->vz
		= (float)r.linear_acceleration.v[2] / 1000000.0f;
	linear_acceleration->accuracy = 0;
	YASLOGD(("Magnetic_Get_Filtered_Quaternion OUT quaternion[%f %f %f %f] "
				" gravity[%f %f %f]"
				" linear_acceleration[%f %f %f]\n",
				quaternion->q[0],
				quaternion->q[1],
				quaternion->q[2],
				quaternion->q[3],
				gravity->vx, gravity->vy, gravity->vz,
				linear_acceleration->vx,
				linear_acceleration->vy,
				linear_acceleration->vz));
	return result;
}
#endif

int
Magnetic_Calibrate(SENSORDATA *raw, SENSORDATA *cal)
{
	struct yas_data mag;
	int rt, i;
	pthread_mutex_lock(&algo.mutex);
	YASLOGD(("Magnetic_Calibrate IN [%d %d %d]\n", raw->x, raw->y, raw->z));
	memset(cal, 0, sizeof(*cal));
	mag.type = YAS_TYPE_MAG;
	mag.timestamp = 0;
	mag.xyz.v[0] = raw->x;
	mag.xyz.v[1] = raw->y;
	mag.xyz.v[2] = raw->z;
	rt = algo.mcalib.update(&mag, 1);
	if (0 < rt) {
		algo.mcalib.get_offset(YAS_TYPE_MAG, &algo.param.calib_offset,
				&algo.param.accuracy);
#if YAS_MAG_CALIB_ELLIPSOID_ENABLE
		algo.mcalib.get_dynamic_matrix(&algo.param.dynamic_matrix);
#endif
#if YAS_MSM_PLATFORM
		file_save();
#endif
	}
	for (i = 0; i < 3; i++)
		mag.xyz.v[i] -= algo.param.calib_offset.v[i];
#if YAS_MAG_CALIB_ELLIPSOID_ENABLE
	if (algo.param.config.mode == YAS_MAG_CALIB_MODE_ELLIPSOID
		|| algo.param.config.mode
		== YAS_MAG_CALIB_MODE_ELLIPSOID_WITH_GYRO)
		apply_matrix(&mag.xyz, &algo.param.dynamic_matrix);
#endif
	if (algo.param.filter_enable)
		algo.mfilter.update(&mag.xyz, &mag.xyz);
	cal->vx = (float)mag.xyz.v[0] / 1000.0f;
	cal->vy = (float)mag.xyz.v[1] / 1000.0f;
	cal->vz = (float)mag.xyz.v[2] / 1000.0f;
	cal->ox = (float)algo.param.calib_offset.v[0] / 1000.0f;
	cal->oy = (float)algo.param.calib_offset.v[1] / 1000.0f;
	cal->oz = (float)algo.param.calib_offset.v[2] / 1000.0f;
	cal->accuracy = algo.param.accuracy;
	YASLOGD(("Magnetic_Calibrate OUT [%d] [%f %f %f] [%f %f %f]\n",
				cal->accuracy, cal->vx, cal->vy, cal->vz,
				cal->ox, cal->oy, cal->oz));
	pthread_mutex_unlock(&algo.mutex);
	return 0;
}

int
Magnetic_Initialize(void)
{
	int rt, result = 0;
	YASLOGD(("Magnetic_Initialize IN\n"));
	pthread_mutex_init(&algo.mutex, NULL);
	yas_mag_calib_init(&algo.mcalib);
	algo.mcalib.init();
	yas_mag_filter_init(&algo.mfilter);
	yas_calib_file_init(&algo.mfile);
#if YAS_SOFTWARE_GYROSCOPE_ENABLE
	yas_swgyro_init(&algo.swgyro);
	rt = algo.swgyro.init();
	if (rt < 0)
		result = rt;
#endif
#if YAS_ATTITUDE_FILTER_ENABLE
	yas_attitude_filter_init(&algo.attitude_filter);
	rt = algo.attitude_filter.init();
	if (rt < 0)
		result = rt;
#endif
	rt = algo.mfile.init(YAS_ALGO_CONFIG_FILE_PATH,
			YAS_ALGO_PARAM_FILE_PATH,
			&default_mag_param);
	if (rt < 0)
		result = rt;
#if YAS_MSM_PLATFORM
	strcpy(algo.classpath, "/sys/class/sensors/compass/");
#else
#if YAS_TYPE_MAG == YAS_TYPE_AM_MAG
	rt = sysfs_get_iio_classpath(YAS_ACC_MAG_NAME, algo.classpath,
			sizeof(algo.classpath)-1);
#else
	rt = sysfs_get_iio_classpath(YAS_MAG_NAME, algo.classpath,
			sizeof(algo.classpath)-1);
#endif
	if (rt < 0)
		result = rt;
#endif
	rt = file_load();
	if (rt < 0)
		result = rt;
	algo.enable = 0;
	algo.file_loaded = 1;
#if ORIENTATION_FILTER_ENABLE
	orientation_filter_init();
#endif
	YASLOGD(("Magnetic_Initialize OUT[%d]\n", result));
	return result;
}
