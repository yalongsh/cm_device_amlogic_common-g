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

//#define OVERLAY_DEBUG 1
#define LOG_TAG "Overlay"

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <cutils/log.h>
#include <sys/ioctl.h>

#include "amvideo_utils.h"

#define LOG_FUNCTION_NAME    LOGV("%s: %s",  __FILE__, __FUNCTION__);

#ifndef LOGE
#define LOGE(fmt,args...) \
        do { printf(fmt, ##args); } \
        while (0)
#endif

#ifndef LOGI
#define LOGI(fmt,args...) \
        do { LOGE(fmt, ##args); } \
        while (0)
#endif

#define SYSCMD_BUFSIZE 40
#define DISP_DEVICE_PATH "/sys/class/video/device_resolution"
#define FB_DEVICE_PATH   "/sys/class/graphics/fb0/virtual_size"
#define ANGLE_PATH       "/sys/class/ppmgr/angle"

static int rotation = 0;
static int disp_width = 1920;
static int disp_height = 1080;

void amvideo_overlay_get_disp(int *width, int *height)
{
    LOG_FUNCTION_NAME

    char buf[SYSCMD_BUFSIZE];
    int disp_w = disp_width;
    int disp_h = disp_height;
    int dev_fd = open(FB_DEVICE_PATH, O_RDONLY);

    if (dev_fd < 0)
        goto OUT;

    read(dev_fd, buf, SYSCMD_BUFSIZE);

    if (sscanf(buf, "%d,%d", &disp_w, &disp_h) == 2) {
        LOGI("disp resolution %dx%d\n", disp_w, disp_h);
        disp_h = disp_h / 2;
    }
    else {
        close(dev_fd);
        goto OUT;
    }

    close(dev_fd);

OUT:
    *width = disp_w;
    *height = disp_h;
}

int amvideo_overlay_open(int id)
{
    LOG_FUNCTION_NAME

    if (id == AMVIDEO_OVERLAY_PLANE_VIDEO)
        return open("/sys/class/video/axis", O_RDWR);

    return -EINVAL;
}

int amvideo_overlay_init(int fd, uint32_t w, uint32_t h, uint32_t fmt)
{
    LOG_FUNCTION_NAME

    int ret = 0;

    /* to-do */

    LOGI("amvideo_overlay_init:: w=%d h=%d\n", w, h);
    return ret;
}

int amvideo_overlay_set_position(int fd, int32_t x, int32_t y, int32_t w, int32_t h)
{
    LOG_FUNCTION_NAME

    int dev_fd, dev_w, dev_h, disp_w, disp_h;
    int dst_x, dst_y, dst_w, dst_h;
    char buf[SYSCMD_BUFSIZE];
    int angle_fd = -1;

    LOGI("amvideo_overlay_set_position:: x=%d y=%d w=%d h=%d\n", x, y, w, h);

    bzero(buf, SYSCMD_BUFSIZE);

    dst_x = x;
    dst_y = y;
    dst_w = w;
    dst_h = h;

    dev_fd = open(DISP_DEVICE_PATH, O_RDONLY);
    if (dev_fd < 0)
        goto OUT;

    read(dev_fd, buf, SYSCMD_BUFSIZE);

    if (sscanf(buf, "%dx%d", &dev_w, &dev_h) == 2)
        LOGI("device resolution %dx%d\n", dev_w, dev_h);
    else {
        close(dev_fd);
        goto OUT;
    }
    close(dev_fd);
    
    amvideo_overlay_get_disp(&disp_w, &disp_h);

    if ((disp_w != dev_w) || (disp_h/2 != dev_h)) {
        dst_x = dst_x * dev_w / disp_w;
        dst_y = dst_y * dev_h / disp_h;
        dst_w = dst_w * dev_w / disp_w;
        dst_h = dst_h * dev_h / disp_h;
    }

    angle_fd = open(ANGLE_PATH, O_WRONLY);
    if (angle_fd >= 0) {
        const char *angle[4] = {"0", "1", "2", "3"};
        write(angle_fd, angle[(rotation/90) & 3], 2);

        LOGI("set ppmgr angle %s\n", angle[(rotation/90) & 3]);
    }

    if (((rotation == 90) || (rotation == 270)) && (angle_fd < 0)) {
        if (dst_h == disp_h) {
            int center = x + w/2;

            if (abs(center - disp_w/2) < 2) {
                /* a centered overlay with rotation, change to full screen */
                dst_x = 0;
                dst_y = 0;
                dst_w = dev_w;
                dst_h = dev_h;

                LOGI("centered overlay expansion");
            }
        }
    }

    if (angle_fd >= 0)
        close(angle_fd);

OUT:
    snprintf(buf, 40, "%d %d %d %d", dst_x, dst_y, dst_x+dst_w-1, dst_y+dst_h-1);
    write(fd, buf, strlen(buf));

    LOGI("amvideo_overlay_set_position (corrected):: x=%d y=%d w=%d h=%d\n", dst_x, dst_y, dst_w, dst_h);
    
    return 0;
}

int amvideo_overlay_get_position(int fd, int32_t *x, int32_t *y, int32_t *w, int32_t *h)
{
    LOG_FUNCTION_NAME

    char buf[SYSCMD_BUFSIZE];
    int x1, y1;

    read(fd, buf, SYSCMD_BUFSIZE);

    sscanf(buf, "%d %d %d %d", x, y, &x1, &y1);

    *w = x1 - *x + 1;
    *h = y1 - *y + 1;

    return 0;
}

int amvideo_overlay_set_crop(int fd, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    LOG_FUNCTION_NAME

    return 0;
}

int amvideo_overlay_get_crop(int fd, uint32_t *x, uint32_t *y, uint32_t *w, uint32_t *h)
{
    LOG_FUNCTION_NAME

    return 0;
}

int amvideo_overlay_set_rotation(int fd, int degree, int step)
{
    LOG_FUNCTION_NAME

    rotation = degree;

    return 0;
}

int amvideo_overlay_stream_on(int fd)
{
    LOG_FUNCTION_NAME

    /* to-do */
    return 0;
}

int amvideo_overlay_stream_off(int fd)
{
    LOG_FUNCTION_NAME

    /* to-do */
    return 0;
}
