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

    srcWidth = 1920;
    srcHeight = 1088;
    srcFormat = HAL_PIXEL_FORMAT_YCrCb_NV12;

    dstWidth = 3840;
    dstHeight = 1080;
    dstFormat = HAL_PIXEL_FORMAT_RGBA_8888;

    src = malloc(srcWidth * srcHeight * 2);
    if (!src)
        return -ENOMEM;
    
    dst = malloc(dstWidth * dstHeight * 4);

    if (!dst) {
        free(src);
        return -ENOMEM;
    }

    RockchipRga& rkRga(RockchipRga::get());


#if 1
    char *buf = (char *)src;
    const char *yuvFilePath = "/data/inputBuffer.bin";
    FILE *file = fopen(yuvFilePath, "rb");
    if (!file) {
        fprintf(stderr, "Could not open %s\n", yuvFilePath);
        return false;
    } else
        fprintf(stderr, "open %s ok\n", yuvFilePath);

    fread(buf, 1.5 * srcWidth * srcHeight, 1, file);
    #if 0
    {
        char *pbuf = (char*)malloc(2 * mHeight * 4864);
        for (int i = 0; i < 2160 * 1.6; i++)
            memcpy(pbuf+i*4800,buf+i*6080,4800);
        const char *outFilePath = "/data/fb3840x2160-2.yuv";
        FILE *file = fopen(outFilePath, "wb+");
        if (!file) {
            fprintf(stderr, "Could not open %s\n", outFilePath);
            return false;
        }
        fwrite(pbuf, 2 * 4864 * 2160, 1, file);
        free(pbuf);
        fclose(file);
    }
    #endif
    fclose(file);
#else
    memset(buf,0x55,4*1200*1920);
#endif
    while(1) {
        
        //rkRga.RkRgaSetLogOnceFlag(1);
        //drm_rga_t rects;
        /*******************************left***********************************/
        //memset(&rects, 0, sizeof(drm_rga_t));
        //rga_set_rect(&rects.src, 0, 0, srcWidth, srcHeight, srcWidth, srcFormat);
        //rga_set_rect(&rects.dst, 0, 0, dstWidth / 2, dstHeight,
        //                                                    dstWidth, dstFormat);
        //ret = rkRga.RkRgaBlit(src, dst, &rects, 0, 0);

        /*******************************right**********************************/
        //rkRga.RkRgaSetLogOnceFlag(1);
        //rga_set_rect(&rects.src, 0, 0, srcWidth, srcHeight, srcWidth, srcFormat);
        //rga_set_rect(&rects.dst, dstWidth / 2, 0, dstWidth / 2, 
        //                                         dstHeight, dstWidth, dstFormat);
        ret = rkRga.RkRgaBlit(NULL, NULL, NULL);


        if (ret) {
            printf("rgaFillColor error : %s,hnd=%p\n",
                                          strerror(errno),(void*)(dst));
            ALOGD("rgaFillColor error : %s,hnd=%p\n",
                                          strerror(errno),(void*)(dst));
        }

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
