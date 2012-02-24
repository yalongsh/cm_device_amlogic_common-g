
#include <camera/CameraHardwareInterface.h>
#include "CameraSetting.h"
#include "amlogic_camera_para.h"

extern "C" {
int set_white_balance(int camera_fd,const char *swb);
int SetExposure(int camera_fd,const char *sbn);
int set_effect(int camera_fd,const char *sef);
int set_banding(int camera_fd,const char *snm);
#ifdef AMLOGIC_FLASHLIGHT_SUPPORT
int set_flash_mode(const char *sfm);
#endif
}
namespace android {
status_t	CameraSetting::InitParameters(CameraParameters& pParameters,String8 PrevFrameSize,String8 PicFrameSize)
{
	LOGE("use default InitParameters");
	//set the limited & the default parameter
//==========================must set parameter for CTS will check them
	pParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FORMATS,CameraParameters::PIXEL_FORMAT_YUV420SP);
	pParameters.setPreviewFormat(CameraParameters::PIXEL_FORMAT_YUV420SP);

	pParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_FORMATS, CameraParameters::PIXEL_FORMAT_JPEG);
	pParameters.setPictureFormat(CameraParameters::PIXEL_FORMAT_JPEG);

	pParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FRAME_RATES,"15,20");
	pParameters.setPreviewFrameRate(15);

	//must have >2 sizes and contain "0x0"
	pParameters.set(CameraParameters::KEY_SUPPORTED_JPEG_THUMBNAIL_SIZES, "180x160,0x0");
	pParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH, 180);
	pParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT, 160);

	pParameters.set(CameraParameters::KEY_SUPPORTED_FOCUS_MODES,CameraParameters::FOCUS_MODE_FIXED);
	pParameters.set(CameraParameters::KEY_FOCUS_MODE,CameraParameters::FOCUS_MODE_FIXED);

	pParameters.set(CameraParameters::KEY_SUPPORTED_ANTIBANDING,"50hz,60hz");
	pParameters.set(CameraParameters::KEY_ANTIBANDING,"50hz");

	pParameters.set(CameraParameters::KEY_FOCAL_LENGTH,"4.31");

	pParameters.set(CameraParameters::KEY_HORIZONTAL_VIEW_ANGLE,"54.8");
	pParameters.set(CameraParameters::KEY_VERTICAL_VIEW_ANGLE,"42.5");

//==========================

	pParameters.set(CameraParameters::KEY_SUPPORTED_WHITE_BALANCE,"auto,daylight,incandescent,fluorescent");
	pParameters.set(CameraParameters::KEY_WHITE_BALANCE,"auto");

	pParameters.set(CameraParameters::KEY_SUPPORTED_EFFECTS,"none,negative,sepia");
	pParameters.set(CameraParameters::KEY_EFFECT,"none");
#ifdef AMLOGIC_FLASHLIGHT_SUPPORT
	pParameters.set(CameraParameters::KEY_SUPPORTED_FLASH_MODES,"on,off,torch");
	pParameters.set(CameraParameters::KEY_FLASH_MODE,"on");
#endif
	//pParameters.set(CameraParameters::KEY_SUPPORTED_SCENE_MODES,"auto,night,snow");
	//pParameters.set(CameraParameters::KEY_SCENE_MODE,"auto");

	pParameters.set(CameraParameters::KEY_MAX_EXPOSURE_COMPENSATION,4);
	pParameters.set(CameraParameters::KEY_MIN_EXPOSURE_COMPENSATION,-4);
	pParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION_STEP,1);
	pParameters.set(CameraParameters::KEY_EXPOSURE_COMPENSATION,0);

#if 1
	pParameters.set(CameraParameters::KEY_ZOOM_SUPPORTED,CameraParameters::TRUE);
	pParameters.set(CameraParameters::KEY_SMOOTH_ZOOM_SUPPORTED,1);

	pParameters.set(CameraParameters::KEY_ZOOM_RATIOS,"100,120,140,160,180,200,220,280,300");
	pParameters.set(CameraParameters::KEY_MAX_ZOOM,8);	//think the zoom ratios as a array, the max zoom is the max index
	pParameters.set(CameraParameters::KEY_ZOOM,0);//default should be 0
#endif
	pParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES,PrevFrameSize);
	if(PrevFrameSize.find("800x600") != -1)
		pParameters.setPreviewSize(800, 600);
	else
		pParameters.setPreviewSize(640, 480);
	pParameters.set(CameraParameters::KEY_SUPPORTED_PICTURE_SIZES, PicFrameSize);
	if(PrevFrameSize.find("1600x1200") != -1)
		pParameters.setPictureSize(1600, 1200);
	else
		pParameters.setPictureSize(640, 480);
	pParameters.set(CameraParameters::KEY_JPEG_QUALITY,90);
	pParameters.set(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY,90);

	pParameters.set(CameraParameters::KEY_PREVIEW_FPS_RANGE,"10500,26623");
	//pParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE,"(10500,26623),(12000,26623),(30000,30000)");
	pParameters.set(CameraParameters::KEY_SUPPORTED_PREVIEW_FPS_RANGE,"(10500,26623)");
	pParameters.set(CameraParameters::KEY_FOCUS_DISTANCES,"0.95,1.9,Infinity");
	return NO_ERROR;
}


//write parameter to v4l2 driver,
//check parameter if valid, if un-valid first should correct it ,and return the INVALID_OPERTIONA
status_t	CameraSetting::SetParameters(CameraParameters& pParameters)
{
	LOGE("use default SetParameters");
	status_t rtn = NO_ERROR;
	//check zoom value
	int zoom = pParameters.getInt(CameraParameters::KEY_ZOOM);
	int maxzoom = pParameters.getInt(CameraParameters::KEY_MAX_ZOOM);
	if((zoom > maxzoom) || (zoom < 0))
	{
		rtn = INVALID_OPERATION;
		pParameters.set(CameraParameters::KEY_ZOOM, maxzoom);
	}

	m_hParameter = pParameters;
	int min_fps,max_fps;
	const char *white_balance=NULL;
	const char *exposure=NULL;
	const char *effect=NULL;
	//const char *night_mode=NULL;
	const char *qulity=NULL;
	const char *banding=NULL;
	const char *flashmode=NULL;

	white_balance=pParameters.get(CameraParameters::KEY_WHITE_BALANCE);
	exposure=pParameters.get(CameraParameters::KEY_EXPOSURE_COMPENSATION);
	effect=pParameters.get(CameraParameters::KEY_EFFECT);
	banding=pParameters.get(CameraParameters::KEY_ANTIBANDING);
	qulity=pParameters.get(CameraParameters::KEY_JPEG_QUALITY);
	flashmode = pParameters.get(CameraParameters::KEY_FLASH_MODE);
	if(exposure)
		SetExposure(m_iDevFd,exposure);
	if(white_balance)
		set_white_balance(m_iDevFd,white_balance);
	if(effect)
		set_effect(m_iDevFd,effect);
	if(banding)
		set_banding(m_iDevFd,banding);
#ifdef AMLOGIC_FLASHLIGHT_SUPPORT
	if(flashmode)
		set_flash_mode(flashmode);
#endif
	pParameters.getPreviewFpsRange(&min_fps, &max_fps);
        if((min_fps<0)||(max_fps<0)||(max_fps<min_fps))
	{
		rtn = INVALID_OPERATION;
	}
	return rtn;
}

static char* sCameraMirrorMode[] = {
#ifdef AMLOGIC_BACK_CAMERA_SUPPORT
    "Disble",  // cam id 0
#endif
#ifdef AMLOGIC_FRONT_CAMERA_SUPPORT
    "Enable", // cam id 1
#endif
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
    "Disble",
#endif
    NULL
};


const char* CameraSetting::GetInfo(int InfoId)
{
	LOGE("use default GetInfo");
	switch(InfoId)
	{
		/*case CAMERA_EXIF_MAKE:
			return "Amlogic";
		case CAMERA_EXIF_MODEL:
			return "DEFUALT";*/
		case CAMERA_MIRROR_MODE:
	                return (const char*)sCameraMirrorMode[m_iCamId];
		default:
			return NULL;
	}
}

/*int	CameraSetting::GetDelay(int Processid)
{
	if(CAMERA_PREVIEW == Processid)
		return 1000;
	else
		return 0;
}*/

CameraSetting* getCameraSetting()
{
	LOGE("return default CameraSetting");
	return new CameraSetting();
}

static CameraInfo sCameraInfo[] = {
#ifdef AMLOGIC_BACK_CAMERA_SUPPORT
    {
        CAMERA_FACING_BACK,
//        90,  /* orientation */
        0,  /* orientation */
    },
#endif
#ifdef AMLOGIC_FRONT_CAMERA_SUPPORT
    {
        CAMERA_FACING_FRONT,
//        270,  /* orientation */
        0,  /* orientation */
    },
#endif
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
    {
        CAMERA_USB,
        0,  /* orientation */
    },
#endif
};

extern "C" int HAL_getNumberOfCameras()
{
    return sizeof(sCameraInfo) / sizeof(sCameraInfo[0]);
}

extern "C" void HAL_getCameraInfo(int cameraId, struct CameraInfo* cameraInfo)
{
    memcpy(cameraInfo, &sCameraInfo[cameraId], sizeof(CameraInfo));
}
}
