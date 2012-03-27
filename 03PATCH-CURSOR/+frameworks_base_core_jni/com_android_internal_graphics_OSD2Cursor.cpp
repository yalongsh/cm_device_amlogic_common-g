/*
 *
 */

#define LOG_TAG "OSD2Cursor-jni"

#include "jni.h"
#include "JNIHelp.h"
#include "utils/Log.h"
#include "utils/misc.h"
#include <cutils/properties.h>
#include <android_runtime/AndroidRuntime.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdint.h>

namespace android
{

#define DEBUG 0
#define OSD2_DEV "/dev/graphics/fb1"
#define CURSOR_WIDTH  13
#define CURSOR_HEIGHT 18
#define OSD_WIDTH  18
#define OSD_HEIGHT 18
#define MAX(x, y) ((x) > (y) ? (x) : (y))
enum {
    ROTATION_0 = 0,
    ROTATION_90 = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3,
};
static int m_fb1fd = -1;
static bool m_isShown = false;
/**
 *  m_userrotation is the default screen orientation. For most LCD screens,
 *  this is landscape. A value of 270 means ROTATION_0 will be portrait.
 *  m_rotation is the rotation from the default screen orientation, usually
 *  controlled by the accelerometer.
 */
static short m_userrotation = 0;
static short m_rotation = ROTATION_0;
static short m_dispw = 0;
static short m_disph = 0;
static bool m_tvOutEnabled = false;

static void drawCursor(uint8_t *buffer, ssize_t buflen, uint32_t line_len)
{
    long int location;
    uint32_t xx, yy;
#define b 0xff000000
#define w 0xffffffff
#define t 0x00000000
// padding on left because 2x scale causes a line on the right side
int curmap[CURSOR_HEIGHT][CURSOR_WIDTH] = {
{t, b, b, t, t, t, t, t, t, t, t, t, t},
{t, b, b, b, t, t, t, t, t, t, t, t, t},
{t, b, b, b, b, t, t, t, t, t, t, t, t},
{t, b, b, w, b, b, t, t, t, t, t, t, t},
{t, b, b, w, w, b, b, t, t, t, t, t, t},
{t, b, b, w, w, w, b, b, t, t, t, t, t},
{t, b, b, w, w, w, w, b, b, t, t, t, t},
{t, b, b, w, w, w, w, w, b, b, t, t, t},
{t, b, b, w, w, w, w, w, w, b, b, t, t},
{t, b, b, w, w, w, w, w, w, w, b, b, b},
{t, b, b, w, w, w, w, w, b, b, b, b, b},
{t, b, b, w, w, b, w, w, b, b, b, b, t},
{t, b, b, w, b, b, b, w, b, b, t, t, t},
{t, b, b, b, b, b, b, w, w, b, b, t, t},
{t, b, b, b, t, t, b, b, w, w, b, b, t},
{t, b, b, t, t, t, b, b, w, w, b, b, t},
{t, t, t, t, t, t, t, b, b, w, b, b, t},
{t, t, t, t, t, t, t, t, b, b, b, t, t},
};
#undef b
#undef w
#undef t

    xx = yy = 0;
    memset(buffer, 0, buflen);
    if ((m_userrotation == 0 && m_rotation == ROTATION_0) ||
        (m_userrotation == 90 && m_rotation == ROTATION_270) ||
        (m_userrotation == 180 && m_rotation == ROTATION_180) ||
        (m_userrotation == 270 && m_rotation == ROTATION_90) ||
        m_tvOutEnabled) {
        for (yy=0; yy < CURSOR_HEIGHT; yy++) {
            location = ((yy) * line_len);
            for (xx=0; xx < CURSOR_WIDTH; xx++) {
                *(uint32_t*)(buffer + location) = curmap[yy][xx];
                location += 4; // location += (vinfo.bits_per_pixel / 8)
            }
        }
    }
    else if ((m_userrotation == 0 && m_rotation == ROTATION_90) ||
             (m_userrotation == 90 && m_rotation == ROTATION_0) ||
             (m_userrotation == 180 && m_rotation == ROTATION_270) ||
             (m_userrotation == 270 && m_rotation == ROTATION_180)) {
        for (yy=0; yy < CURSOR_WIDTH; yy++) {
            location = ((yy) * line_len);
            for (xx=0; xx < CURSOR_HEIGHT; xx++) {
                *(uint32_t*)(buffer + location) = curmap[CURSOR_HEIGHT - 1 - xx][yy];
                location += 4;
            }
        }
    }
    else if ((m_userrotation == 0 && m_rotation == ROTATION_180) ||
             (m_userrotation == 90 && m_rotation == ROTATION_90) ||
             (m_userrotation == 180 && m_rotation == ROTATION_0) ||
             (m_userrotation == 270 && m_rotation == ROTATION_270)) {
        for (yy=0; yy < CURSOR_HEIGHT; yy++) {
            location = ((yy) * line_len);
            for (xx=0; xx < CURSOR_WIDTH; xx++) {
                *(uint32_t*)(buffer + location) = curmap[CURSOR_HEIGHT  - 1 - yy][CURSOR_WIDTH  - 1 - xx];
                location += 4;
            }
        }
    }
    else if ((m_userrotation == 0 && m_rotation == ROTATION_270) ||
             (m_userrotation == 90 && m_rotation == ROTATION_180) ||
             (m_userrotation == 180 && m_rotation == ROTATION_90) ||
             (m_userrotation == 270 && m_rotation == ROTATION_0)) {
        for (yy=0; yy < CURSOR_WIDTH; yy++) {
            location = ((yy) * line_len);
            for (xx=0; xx < CURSOR_HEIGHT; xx++) {
                *(uint32_t*)(buffer + location) = curmap[xx][CURSOR_WIDTH - 1 - yy];
                location += 4;
            }
        }
    }
    if (DEBUG) LOGI("drawCursor\n");
}

static void setCursor()
{
    if (m_fb1fd < 0) {
        m_fb1fd = open(OSD2_DEV, O_RDWR);
    }
    if (m_fb1fd >= 0) {
        struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        uint8_t *buffer = NULL;
        ssize_t buflen;
        if (!ioctl(m_fb1fd, FBIOGET_VSCREENINFO, &vinfo) &&
            !ioctl(m_fb1fd, FBIOGET_FSCREENINFO, &finfo)) {

            /* width/height will be doubled if 2x scale is enabled :/ */
            if ((vinfo.xres != OSD_WIDTH && vinfo.xres != OSD_WIDTH*2) ||
                (vinfo.yres != OSD_HEIGHT && vinfo.yres != OSD_HEIGHT*2)) {
                vinfo.xres = vinfo.xres_virtual = OSD_WIDTH;
                vinfo.yres = vinfo.yres_virtual = OSD_HEIGHT;
                vinfo.xoffset = vinfo.yoffset = 0;
                vinfo.bits_per_pixel = 32;
                if (ioctl(m_fb1fd, FBIOPUT_VSCREENINFO, &vinfo))
                    LOGE("set info fail\n");
                ioctl(m_fb1fd, FBIOGET_FSCREENINFO, &finfo);
            }
            buflen = vinfo.yres_virtual * finfo.line_length;
            buffer = (uint8_t*)mmap(NULL,
                                    buflen,
                                    PROT_READ|PROT_WRITE,
                                    MAP_SHARED,
                                    m_fb1fd,
                                    0);
            if (buffer != MAP_FAILED) {
                drawCursor(buffer, buflen, finfo.line_length);
                munmap(buffer, buflen);
                if (DEBUG) LOGI("setCursor ok\n");
            }
            else
                LOGE("mmap fail\n");
        }
        else
            LOGE("get info fail\n");
    }
    else
        LOGE("open fail\n");
}

/*
 * Class:     OSD2Cursor
 * Method:    setPosition
 * Signature: (SS)V
 */
static void com_android_internal_graphics_OSD2Cursor_setPosition(JNIEnv * env, jobject clazz,
                                                                 jshort x, jshort y)
{
    if (m_isShown == false)
        return;
    if (m_fb1fd < 0)
        setCursor();
    if (m_fb1fd >= 0) {
        struct fb_cursor cinfo;
        if ((m_userrotation == 0 && m_rotation == ROTATION_0) ||
            (m_userrotation == 90 && m_rotation == ROTATION_270) ||
            (m_userrotation == 180 && m_rotation == ROTATION_180) ||
            (m_userrotation == 270 && m_rotation == ROTATION_90)) {
            cinfo.hot.x = x;
            cinfo.hot.y = y;
        }
        else if ((m_userrotation == 0 && m_rotation == ROTATION_90) ||
                 (m_userrotation == 90 && m_rotation == ROTATION_0) ||
                 (m_userrotation == 180 && m_rotation == ROTATION_270) ||
                 (m_userrotation == 270 && m_rotation == ROTATION_180)) {
            /**
             * 1. When rotated, the w/h is changed.. 1280x720 -> 720x1280
             * 2. Subtract OSD_HEIGHT to place the cursor's point (top-left
             *    in original orientation) on the desired spot
             */
            if (m_tvOutEnabled)
                cinfo.hot.x = m_disph - y;
            else
                cinfo.hot.x = m_disph - y - OSD_HEIGHT;
            cinfo.hot.y = x;
        }
        else if ((m_userrotation == 0 && m_rotation == ROTATION_180) ||
                 (m_userrotation == 90 && m_rotation == ROTATION_90) ||
                 (m_userrotation == 180 && m_rotation == ROTATION_0) ||
                 (m_userrotation == 270 && m_rotation == ROTATION_270)) {
            if (m_tvOutEnabled) {
                cinfo.hot.x = m_dispw - x;
                cinfo.hot.y = m_disph - y;
            } else {
                cinfo.hot.x = m_dispw - x - OSD_WIDTH;
                cinfo.hot.y = m_disph - y - OSD_HEIGHT;
            }
        }
        else if ((m_userrotation == 0 && m_rotation == ROTATION_270) ||
                 (m_userrotation == 90 && m_rotation == ROTATION_180) ||
                 (m_userrotation == 180 && m_rotation == ROTATION_90) ||
                 (m_userrotation == 270 && m_rotation == ROTATION_0)) {
            cinfo.hot.x = y;
            if (m_tvOutEnabled)
                cinfo.hot.y = m_dispw - x;
            else
                cinfo.hot.y = m_dispw - x - OSD_WIDTH;
        }
        else
            return;
        if (DEBUG > 1) LOGI("setPosition %d, %d -> %d, %d disp=%d, %d  userrotation=%d, rotation=%d\n",
                            x, y, (int16_t)cinfo.hot.x, (int16_t)cinfo.hot.y, m_dispw, m_disph, m_userrotation, m_rotation);
        ioctl(m_fb1fd, FBIO_CURSOR, &cinfo);
    }
}

static void setblank(bool blank)
{
    if (m_fb1fd >= 0)
        ioctl(m_fb1fd, FBIOBLANK, blank ? 1 : 0);
}

/*
 * Class:     OSD2Cursor
 * Method:    show
 * Signature: ()V
 */
static void com_android_internal_graphics_OSD2Cursor_show(JNIEnv * env, jobject clazz)
{
    char value[PROPERTY_VALUE_MAX];
    property_get("ro.sf.hwrotation", value, "0");
    m_userrotation = atoi(value);
    if (m_userrotation != 0 && m_userrotation != 90 &&
        m_userrotation != 180 && m_userrotation != 270)
        m_userrotation = 0;

    setCursor();
    setblank(false);
    m_isShown = true;

}

/*
 * Class:     OSD2Cursor
 * Method:    hide
 * Signature: ()V
 */
static void com_android_internal_graphics_OSD2Cursor_hide(JNIEnv * env, jobject clazz)
{
    setblank(true);
    m_isShown = false;
}

/*
 * Class:     OSD2Cursor
 * Method:    setRotation
 * Signature: (S)V
 */
static void com_android_internal_graphics_OSD2Cursor_setRotation(JNIEnv * env, jobject clazz,
                                                                 jshort rotation)
{
    if (rotation == ROTATION_0 || rotation == ROTATION_90 ||
        rotation == ROTATION_180 || rotation == ROTATION_270) {
        m_rotation = rotation;
        if (m_isShown)
            setCursor();
    }
    if (DEBUG) LOGI("setRotation %d\n", rotation);
}

/*
 * Class:     OSD2Cursor
 * Method:    setDisplaySize
 * Signature: (SS)V
 */
static void com_android_internal_graphics_OSD2Cursor_setDisplaySize(JNIEnv * env, jobject clazz,
                                                                    jshort w, jshort h)
{
    if (DEBUG) LOGI("setDisplaySize %d %d\n", w, h);
    m_dispw = w;
    m_disph = h;
}

/*
 * Class:     OSD2Cursor
 * Method:    tvModeChanged
 * Signature: (Z)V
 */
static void com_android_internal_graphics_OSD2Cursor_tvOutChanged(JNIEnv * env, jobject clazz,
                                                                   jboolean isEnabled)
{
    if (DEBUG) LOGI("tvOutChanged %d\n", isEnabled);
    m_tvOutEnabled = isEnabled;
    if (m_isShown)
        setCursor();
}

static JNINativeMethod method_table[] = {
    /* name, signature, funcPtr */
    { "nativeSetPosition", "(SS)V",
      (void*)com_android_internal_graphics_OSD2Cursor_setPosition },
    { "nativeShow", "()V",
      (void*)com_android_internal_graphics_OSD2Cursor_show },
    { "nativeHide", "()V",
      (void*)com_android_internal_graphics_OSD2Cursor_hide },
    { "nativeSetRotation", "(S)V",
      (void*)com_android_internal_graphics_OSD2Cursor_setRotation },
    { "nativeSetDisplaySize", "(SS)V",
      (void*)com_android_internal_graphics_OSD2Cursor_setDisplaySize },
    { "nativeTVOutChanged", "(Z)V",
      (void*)com_android_internal_graphics_OSD2Cursor_tvOutChanged },

};

int register_com_android_internal_graphics_OSD2Cursor(JNIEnv *env)
{
    return AndroidRuntime::registerNativeMethods(
        env, "com/android/internal/graphics/OSD2Cursor",
        method_table, NELEM(method_table));
}

}; // namespace android

