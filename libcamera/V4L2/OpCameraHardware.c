#define LOG_NDEBUG 0
//#define NDEBUG 0
#define LOG_TAG "OpCameraHardware"
#include <utils/Log.h>
#include "amlogic_camera_para.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <cutils/properties.h>
#include <linux/videodev2.h>
#ifdef AMLOGIC_FLASHLIGHT_SUPPORT
#include <media/amlogic/flashlight.h>
#include <stdbool.h>
int get_flash_mode(void);
#endif

int set_white_balance(int camera_fd,const char *swb)
{
 	int ret = 0;
	struct v4l2_control ctl;
	if(camera_fd<0)
		return -1;

	ctl.id = V4L2_CID_DO_WHITE_BALANCE;

	if(strcasecmp(swb,"auto")==0)
		ctl.value=CAM_WB_AUTO;
	else if(strcasecmp(swb,"daylight")==0)
		ctl.value=CAM_WB_DAYLIGHT;
	else if(strcasecmp(swb,"incandescent")==0)
		ctl.value=CAM_WB_INCANDESCENCE;
	else if(strcasecmp(swb,"fluorescent")==0)
		ctl.value=CAM_WB_FLUORESCENT;

	if(ioctl(camera_fd, VIDIOC_S_CTRL, &ctl)<0)
	{
		ret = -1;
		LOGV("AMLOGIC CAMERA SetParametersToDriver fail !! ");
	}
	return ret ;
}

int SetExposure(int camera_fd,const char *sbn)
{
 	int ret = 0;
	struct v4l2_control ctl;
	if(camera_fd<0)
		return -1;

	ctl.id = V4L2_CID_EXPOSURE;

	if(strcasecmp(sbn,"4")==0)
		ctl.value=EXPOSURE_P4_STEP;
	else if(strcasecmp(sbn,"3")==0)
		ctl.value=EXPOSURE_P3_STEP;
	else if(strcasecmp(sbn,"2")==0)
		ctl.value=EXPOSURE_P2_STEP;
	else if(strcasecmp(sbn,"1")==0)
		ctl.value=EXPOSURE_P1_STEP;
	else if(strcasecmp(sbn,"0")==0)
		ctl.value=EXPOSURE_0_STEP;
	else if(strcasecmp(sbn,"-1")==0)
		ctl.value=EXPOSURE_N1_STEP;
	else if(strcasecmp(sbn,"-2")==0)
		ctl.value=EXPOSURE_N2_STEP;
	else if(strcasecmp(sbn,"-3")==0)
		ctl.value=EXPOSURE_N3_STEP;
	else if(strcasecmp(sbn,"-4")==0)
		ctl.value=EXPOSURE_N4_STEP;

	if(ioctl(camera_fd, VIDIOC_S_CTRL, &ctl)<0)
	{
		ret = -1;
		LOGV("AMLOGIC CAMERA SetParametersToDriver fail !! ");
	}

	return ret ;
}


int set_effect(int camera_fd,const char *sef)
{
 	int ret = 0;
	struct v4l2_control ctl;
	if(camera_fd<0)
		return -1;

	ctl.id = V4L2_CID_COLORFX;

	if(strcasecmp(sef,"none")==0)
		ctl.value=CAM_EFFECT_ENC_NORMAL;
	else if(strcasecmp(sef,"negative")==0)
		ctl.value=CAM_EFFECT_ENC_COLORINV;
	else if(strcasecmp(sef,"sepia")==0)
		ctl.value=CAM_EFFECT_ENC_SEPIA;

	if(ioctl(camera_fd, VIDIOC_S_CTRL, &ctl)<0)
	{
		ret = -1;
		LOGV("AMLOGIC CAMERA SetParametersToDriver fail !! ");
	}

	return ret ;
}

int set_night_mode(int camera_fd,const char *snm)
{
 	int ret = 0;
 	struct v4l2_control ctl;
	if(camera_fd<0)
		return -1;

	if(strcasecmp(snm,"auto")==0)
		ctl.value=CAM_NM_AUTO;
	else if(strcasecmp(snm,"night")==0)
		ctl.value=CAM_NM_ENABLE;

	ctl.id = V4L2_CID_DO_WHITE_BALANCE;

	if(ioctl(camera_fd, VIDIOC_S_CTRL, &ctl)<0)
	{
		ret = -1;
		LOGV("AMLOGIC CAMERA SetParametersToDriver fail !! ");
	}
	return ret ;
}

int set_banding(int camera_fd,const char *snm)
{
 	int ret = 0;
 	struct v4l2_control ctl;
	if(camera_fd<0)
		return -1;

	if(strcasecmp(snm,"50hz")==0)
		ctl.value=CAM_NM_AUTO;
	else if(strcasecmp(snm,"60hz")==0)
		ctl.value=CAM_NM_ENABLE;

	ctl.id = V4L2_CID_WHITENESS;

	if(ioctl(camera_fd, VIDIOC_S_CTRL, &ctl)<0)
	{
		ret = -1;
		LOGV("AMLOGIC CAMERA SetParametersToDriver fail !! ");
	}
	return ret ;
}

#ifdef AMLOGIC_FLASHLIGHT_SUPPORT
int set_flash(bool mode)
{
 	int flag = 0;
	FILE* fp = NULL;
	if(mode){
		flag = get_flash_mode();
		if(flag == FLASHLIGHT_OFF ||flag == FLASHLIGHT_AUTO)//handle AUTO case on camera driver
			return 0;
		else if(flag == -1)
			return -1;
	}
	fp = fopen("/sys/class/flashlight/flashlightctrl","w");
	if(fp == NULL){
		LOGE("open file fail\n");
		return -1;
	}
	fputc((int)(mode+'0'),fp);
	fclose(fp);
	return 0;
}

int set_flash_mode(const char *sfm)
{
 	int value = 0;
	FILE* fp = NULL;
	if(strcasecmp(sfm,"auto")==0)
		value=FLASHLIGHT_AUTO;
	else if(strcasecmp(sfm,"on")==0)
		value=FLASHLIGHT_ON;
	else if(strcasecmp(sfm,"off")==0)
		value=FLASHLIGHT_OFF;
	else if(strcasecmp(sfm,"off")==0)
		value=FLASHLIGHT_TORCH;
	else
		value=FLASHLIGHT_OFF;

	fp = fopen("/sys/class/flashlight/flashlightflag","w");
	if(fp == NULL){
		LOGE("open file fail\n");
		return -1;
	}
	fputc((int)(value+'0'),fp);
	fclose(fp);
	if(value == FLASHLIGHT_TORCH)//open flashlight immediately
		set_flash(true);
	else if(value == FLASHLIGHT_OFF)
		set_flash(false);
	return 0 ;
}

int get_flash_mode(void)
{
	int value = 0;
	FILE* fp = NULL;

	fp = fopen("/sys/class/flashlight/flashlightflag","r");
	if(fp == NULL){
		LOGE("open file fail\n");
		return -1;
	}
	value=fgetc(fp);
	fclose(fp);
	return value-'0';
}
#endif

