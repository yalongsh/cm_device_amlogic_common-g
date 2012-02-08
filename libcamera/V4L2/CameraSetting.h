#ifndef AMLOGIC_CAMERA_CUSTOMIZE_SETTING
#define AMLOGIC_CAMERA_CUSTOMIZE_SETTING


#include <camera/CameraParameters.h>

namespace android {

//used as getinfo parameters
//#define CAMERA_EXIF_MAKE (0)
//#define CAMERA_EXIF_MODEL (1)
#define CAMERA_MIRROR_MODE (2)

//used as  GetDelay parameters
#define CAMERA_PREVIEW (0)


class CameraSetting
{
	public:
		CameraSetting(){m_pDevName = NULL;m_iDevFd = -1;m_iCamId = -1;}
		virtual ~CameraSetting(){if(m_pDevName) delete m_pDevName;}
		int		m_iDevFd;
		int		m_iCamId;
		char*	m_pDevName;
		CameraParameters m_hParameter;

		virtual status_t	InitParameters(CameraParameters& pParameters,String8 PrevFrameSize,String8 PicFrameSize);
		virtual status_t	SetParameters(CameraParameters& pParameters);
		virtual const char* GetInfo(int InfoId);

		//return the time (in us) cost for process,this is used by preview thread to decide how many times to sleep between get frames
		//most time it is the cost time of function get previewframe.
		//virtual int  		GetDelay(int Processid);
};

CameraSetting* getCameraSetting();

}



#endif
