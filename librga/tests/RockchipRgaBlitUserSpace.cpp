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

#define LOG_NDEBUG 0
#define LOG_TAG "RockchipRgaStereo"

#include <stdint.h>
#include <sys/types.h>
#include <math.h>
#include <fcntl.h>
#include <utils/misc.h>
#include <signal.h>
#include <time.h>

#include <cutils/properties.h>

#include <binder/IPCThreadState.h>
#include <utils/Atomic.h>
#include <utils/Errors.h>
#include <utils/Log.h>

#include <ui/PixelFormat.h>
#include <ui/Rect.h>
#include <ui/Region.h>
#include <ui/DisplayInfo.h>
#include <ui/GraphicBufferMapper.h>

#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <RockchipRga.h>

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/eglext.h>

#include <stdint.h>
#include <sys/types.h>

#include <system/window.h>

#include <utils/Thread.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

///////////////////////////////////////////////////////
//#include "../drmrga.h"
#include <hardware/hardware.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#include <sys/mman.h>
#include <linux/stddef.h>
#include "RockchipFileOps.h"
///////////////////////////////////////////////////////

using namespace android;

int main()
{
    int ret = 0;
    int srcWidth,srcHeight,srcFormat;
    int dstWidth,dstHeight,dstFormat;

    void *src = NULL;
    void *dst = NULL;

    srcWidth = 2048;
    srcHeight = 1536;
    srcFormat = HAL_PIXEL_FORMAT_RGBA_8888;

    dstWidth = 2048;
    dstHeight = 1536;
    dstFormat = HAL_PIXEL_FORMAT_RGBA_8888;

    src = malloc(srcWidth * srcHeight * 4);
    if (!src)
        return -ENOMEM;
    
    dst = malloc(dstWidth * dstHeight * 4);

    if (!dst) {
        free(src);
        return -ENOMEM;
    }

    RockchipRga& rkRga(RockchipRga::get());

    char* buf = (char *)src;

#if 1
    get_buf_from_file(buf, srcFormat, srcWidth, srcHeight, 0);
#else
    memset(buf,0x55,4*1200*1920);
#endif

    buf = (char *)dst;
    {
#if 1
        get_buf_from_file(buf, srcFormat, srcWidth, srcHeight, 1);
#else
        memset(buf,0x55,4*1200*1920);
#endif
    }

    while(1) {
        //rkRga.RkRgaSetLogOnceFlag(1);
        //drm_rga_t rects;
        //memset(&rects, 0, sizeof(drm_rga_t));
        //rga_set_rect(&rects.src, 0, 0, srcWidth, srcHeight, srcWidth, srcFormat);
        //rga_set_rect(&rects.dst, 0, 0, dstWidth, dstHeight, dstWidth, dstFormat);
	    rga_info_t rgasrc;
    	rga_info_t rgadst;
    	memset(&rgasrc, 0, sizeof(rga_info_t));
    	rgasrc.fd = -1;
    	rgasrc.mmuFlag = 1;
    	memset(&rgadst, 0, sizeof(rga_info_t));
    	rgadst.fd = -1;
    	rgadst.mmuFlag = 1;
    	rgasrc.virAddr = src;
    	rgadst.virAddr = dst;
    	rgasrc.blend = 0x880105;
    	rga_set_rect(&rgasrc.rect, 0,0,srcWidth,srcHeight,srcWidth/*stride*/,srcHeight,srcFormat);
        rga_set_rect(&rgadst.rect, 0,0,dstWidth,dstHeight,dstWidth/*stride*/,dstHeight,dstFormat);
        ret = rkRga.RkRgaBlit(&rgasrc, &rgadst, NULL);
        {
            char* dstbuf = (char *)dst;
            //for(int i =0; i < mHeight * 1.5; i++)
            //    memcpy(dstbuf + i * 2400,buf + i * 3000,2400);
            output_buf_data_to_file(dstbuf, dstFormat, dstWidth, dstHeight, 0);
        }
        printf("threadloop\n");
        usleep(500000);
	break;
    }
    return 0;
}
