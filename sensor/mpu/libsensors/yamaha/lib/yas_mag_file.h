/*
 * CONFIDENTIAL
 * Copyright(c) 2013-2014 Yamaha Corporation
 */
#ifndef __YAS_CALIB_FILE_H__
#define __YAS_CALIB_FILE_H__

#include "yas.h"

#define FILE_PATH_MAX (32)

struct yas_mag_param {
	struct yas_mag_calib_config config;
	int filter_enable;
	int8_t hard_offset[3];
	struct yas_vector calib_offset;
	uint8_t accuracy;
	struct yas_matrix dynamic_matrix;
};

struct yas_calib_file {
	int (*init)(const char *set, const char *param,
			struct yas_mag_param *p);
	int (*load)(struct yas_mag_param *p);
	int (*save)(struct yas_mag_param *p);
};

int yas_calib_file_init(struct yas_calib_file *f);

#endif
