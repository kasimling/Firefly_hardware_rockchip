/*
 * CONFIDENTIAL
 * Copyright(c) 2014 Yamaha Corporation
 */

#ifndef __SYSFS_UTIL_H__
#define __SYSFS_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

int sysfs_get_input_classpath(const char *name, char *classpath, int len);
int sysfs_get_iio_classpath(const char *name, char *classpath, int len);
int sysfs_get_input_attr(char *classpath, const char *attr, char *buf, int len);
int sysfs_get_input_attr_by_int(char *classpath, const char *attr, int *value);
int sysfs_set_input_attr(char *classpath, const char *attr, char *value,
		int len);
int sysfs_set_input_attr_by_int(char *classpath, const char *attr, int value);

#ifdef __cplusplus
}
#endif

#endif
