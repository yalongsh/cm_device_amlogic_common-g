/*
 * TVIN Modules Exported Header File
 *
 * Author: Lin Xu <lin.xu@amlogic.com>
 *         Bobby Yang <bo.yang@amlogic.com>
 *
 * Copyright (C) 2010 Amlogic Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef _AMLOGIC_CAMERA_PARA_H
#define _AMLOGIC_CAMERA_PARA_H

/*
    below macro defined applied to camera driver
*/
typedef enum camera_light_mode_e {
    ADVANCED_AWB = 0,
    SIMPLE_AWB,
    MANUAL_DAY,
    MANUAL_A,
    MANUAL_CWF,
    MANUAL_CLOUDY,
}camera_light_mode_t;

typedef enum camera_saturation_e {
    SATURATION_N4_STEP = 0,
    SATURATION_N3_STEP,
    SATURATION_N2_STEP,
    SATURATION_N1_STEP,
    SATURATION_0_STEP,
    SATURATION_P1_STEP,
    SATURATION_P2_STEP,
    SATURATION_P3_STEP,
    SATURATION_P4_STEP,
}camera_saturation_t;


typedef enum camera_brightness_e {
    BRIGHTNESS_N4_STEP = 0,
    BRIGHTNESS_N3_STEP,
    BRIGHTNESS_N2_STEP,
    BRIGHTNESS_N1_STEP,
    BRIGHTNESS_0_STEP,
    BRIGHTNESS_P1_STEP,
    BRIGHTNESS_P2_STEP,
    BRIGHTNESS_P3_STEP,
    BRIGHTNESS_P4_STEP,
}camera_brightness_t;

typedef enum camera_contrast_e {
    CONTRAST_N4_STEP = 0,
    CONTRAST_N3_STEP,
    CONTRAST_N2_STEP,
    CONTRAST_N1_STEP,
    CONTRAST_0_STEP,
    CONTRAST_P1_STEP,
    CONTRAST_P2_STEP,
    CONTRAST_P3_STEP,
    CONTRAST_P4_STEP,
}camera_contrast_t;

typedef enum camera_hue_e {
    HUE_N180_DEGREE = 0,
    HUE_N150_DEGREE,
    HUE_N120_DEGREE,
    HUE_N90_DEGREE,
    HUE_N60_DEGREE,
    HUE_N30_DEGREE,
    HUE_0_DEGREE,
    HUE_P30_DEGREE,
    HUE_P60_DEGREE,
    HUE_P90_DEGREE,
    HUE_P120_DEGREE,
    HUE_P150_DEGREE,
}camera_hue_t;

typedef enum camera_special_effect_e {
    SPECIAL_EFFECT_NORMAL = 0,
    SPECIAL_EFFECT_BW,
    SPECIAL_EFFECT_BLUISH,
    SPECIAL_EFFECT_SEPIA,
    SPECIAL_EFFECT_REDDISH,
    SPECIAL_EFFECT_GREENISH,
    SPECIAL_EFFECT_NEGATIVE,
}camera_special_effect_t;

typedef enum camera_exposure_e {
    EXPOSURE_N4_STEP = 0,
    EXPOSURE_N3_STEP,
    EXPOSURE_N2_STEP,
    EXPOSURE_N1_STEP,
    EXPOSURE_0_STEP,
    EXPOSURE_P1_STEP,
    EXPOSURE_P2_STEP,
    EXPOSURE_P3_STEP,
    EXPOSURE_P4_STEP,
}camera_exposure_t;


typedef enum camera_sharpness_e {
    SHARPNESS_1_STEP = 0,
    SHARPNESS_2_STEP,
    SHARPNESS_3_STEP,
    SHARPNESS_4_STEP,
    SHARPNESS_5_STEP,
    SHARPNESS_6_STEP,
    SHARPNESS_7_STEP,
    SHARPNESS_8_STEP,
    SHARPNESS_AUTO_STEP,
}camera_sharpness_t;

typedef enum camera_mirror_flip_e {
    MF_NORMAL = 0,
    MF_MIRROR,
    MF_FLIP,
    MF_MIRROR_FLIP,
}camera_mirror_flip_t;


typedef enum camera_wb_flip_e {
    CAM_WB_AUTO = 0,
    CAM_WB_CLOUD,
    CAM_WB_DAYLIGHT,
    CAM_WB_INCANDESCENCE,
    CAM_WB_TUNGSTEN,
    CAM_WB_FLUORESCENT,
    CAM_WB_MANUAL,
}camera_wb_flip_t;
typedef enum camera_night_mode_flip_e {
    CAM_NM_AUTO = 0,
	CAM_NM_ENABLE,
}camera_night_mode_flip_t;
typedef enum camera_effect_flip_e {
    CAM_EFFECT_ENC_NORMAL = 0,
	CAM_EFFECT_ENC_GRAYSCALE,
	CAM_EFFECT_ENC_SEPIA,
	CAM_EFFECT_ENC_SEPIAGREEN,
	CAM_EFFECT_ENC_SEPIABLUE,
	CAM_EFFECT_ENC_COLORINV,
}camera_effect_flip_t;
#endif


