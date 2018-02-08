/*
 * Copyright (C) 2016 Rockchip Electronics Co.Ltd
 * Authors:
 *	Zhiqin Wei <wzq@rock-chips.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _rk_drm_drm_rga_api_
#define _rk_drm_drm_rga_api_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "drmrga.h"
#include <android/native_window.h>

#include <utils/Log.h>
#include <cutils/atomic.h>

#include "DrmmodeRgaContext.h"
#include "../GraphicBuffer.h"

static int dumpRgaRects(drm_rga_t tmpRects);
static int isRectValid(rga_rect_t rect);
static int getDrmFomatFromAndroidFormat(int format);
static int getRgaRect(buffer_handle_t hnd, rga_rect_t *rect);
static int getRgaRects(buffer_handle_t src, buffer_handle_t dst, drm_rga_t* tmpRects);
static int getPixelWidthByDrmFormat(int format);
static int getPixelWidthByAndroidFormat(int format);
static int computeRgaStrideByDrmFormat(int stride, int format);
static int computeRgaStrideByAndroidFormat(int stride, int format);
#endif
