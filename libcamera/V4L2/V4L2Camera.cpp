//#define NDEBUG 0
#define LOG_TAG "V4L2Camera"
#define LOG_NDEBUG 0

#include <utils/Log.h>
#include "V4L2Camera.h"
#include <linux/videodev2.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <jpegenc/amljpeg_enc.h>
#include <cutils/properties.h>
#include <sys/time.h>
#include <unistd.h>

extern "C" unsigned char* getExifBuf(char** attrlist, int attrCount, int* exifLen,int thumbnailLen,char* thumbnaildata);
#include "../util.h"
namespace android {

#define V4L2_PREVIEW_BUFF_NUM (6)
#define V4L2_TAKEPIC_BUFF_NUM (1)


V4L2Camera::V4L2Camera(char* devname,int camid)
{
	m_pSetting = getCameraSetting();

	int namelen = strlen(devname)+1;
	m_pSetting->m_pDevName = new char[namelen];
	strcpy(m_pSetting->m_pDevName,devname);
	m_pSetting->m_iCamId = camid;

	m_V4L2BufNum = 0;
	pV4L2Frames = NULL;
	pV4L2FrameSize = NULL;
	m_iPicIdx = -1;
}

V4L2Camera::~V4L2Camera()
{
	delete m_pSetting;
}

#define SYSFILE_CAMERA_SET_PARA "/sys/class/vm/attr2"
#define SYSFILE_CAMERA_SET_MIRROR "/sys/class/vm/mirror"
static int writefile(char* path,char* content)
{
#ifndef AMLOGIC_USB_CAMERA_SUPPORT
    FILE* fp = fopen(path, "w+");

    LOGD("Write file %s(%p) content %s", path, fp, content);

    if (fp) {
        while( ((*content) != '\0') ) {
            if (EOF == fputc(*content,fp))
                LOGD("write char fail");
            content++;
        }

        fclose(fp);
    }
    else
        LOGD("open file fail\n");
#endif
    return 1;
}

status_t	V4L2Camera::Open()
{
    struct v4l2_control ctl;
    int temp_id=-1;
    if((m_pSetting->m_iCamId==1)&&(m_pSetting->m_iDevFd == -1)){
        LOGD("*****open %s\n", "video0+++");
        temp_id = open("/dev/video0", O_RDWR);
        if (temp_id >=0){
            LOGD("*****open %s success %d \n", "video0+++",temp_id);
            writefile(SYSFILE_CAMERA_SET_PARA,"1");
            close(temp_id);
            usleep(100);
        }
    }

    if(m_pSetting->m_iDevFd == -1){
        m_pSetting->m_iDevFd = open(m_pSetting->m_pDevName, O_RDWR);
        if (m_pSetting->m_iDevFd != -1){
            LOGD("open %s success %d \n", m_pSetting->m_pDevName,m_pSetting->m_iDevFd);

#			ifndef AMLOGIC_USB_CAMERA_SUPPORT
            const char* mirror = m_pSetting->GetInfo(CAMERA_MIRROR_MODE);
            if((mirror)&&((strcasecmp(mirror, "1")==0)||(strcasecmp(mirror, "enable")==0)||(strcasecmp(mirror, "true")==0))){
                LOGD("*****Enable camera %d L-R mirror",m_pSetting->m_iCamId);
                writefile(SYSFILE_CAMERA_SET_MIRROR,"1");
            }else{
                LOGD("*****Enable camera %d normal mode",m_pSetting->m_iCamId);
                writefile(SYSFILE_CAMERA_SET_MIRROR,"0");
            }
#			endif

            return NO_ERROR;
        }else{
            LOGD("open %s fail\n",m_pSetting->m_pDevName);
            return UNKNOWN_ERROR;
        }
    }
    return NO_ERROR;
}
status_t	V4L2Camera::Close()
{
	if(m_pSetting->m_iDevFd != -1)
	{
		close(m_pSetting->m_iDevFd);
		m_pSetting->m_iDevFd = -1;
	}
	return NO_ERROR;
}

status_t	V4L2Camera::InitParameters(CameraParameters& pParameters)
{
	String8 prevFrameSize(""),picFrameSize("");
	int ret = NO_ERROR,prevPixelFmt,picPixelFmt;
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
	prevPixelFmt = V4L2_PIX_FMT_YUYV;
	picPixelFmt = V4L2_PIX_FMT_YUYV;
#else
	prevPixelFmt = V4L2_PIX_FMT_NV21;
	picPixelFmt = V4L2_PIX_FMT_RGB24;
#endif
	ret = V4L2_GetValidFrameSize(prevPixelFmt,prevFrameSize);
	if(ret != NO_ERROR)
		return ret;
	ret = V4L2_GetValidFrameSize(picPixelFmt,picFrameSize);
	if(ret != NO_ERROR)
		return ret;
	ret = m_pSetting->InitParameters(pParameters,prevFrameSize,picFrameSize);
	return ret;
}

//write parameter to v4l2 driver,
//check parameter if valid, if un-valid first should correct it ,and return the INVALID_OPERTIONA
status_t	V4L2Camera::SetParameters(CameraParameters& pParameters)
{
	return m_pSetting->SetParameters(pParameters);
}

status_t	V4L2Camera::StartPreview()
{
	int w,h,pixelFormat;
	m_bFirstFrame = true;
	if(m_pSetting != NULL )
		m_pSetting->m_hParameter.getPreviewSize(&w,&h);
	else
		LOGE("%s,%d m_pSetting=NULL",__FILE__, __LINE__);
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
	if(NO_ERROR != Open())
		return UNKNOWN_ERROR;
	pixelFormat = V4L2_PIX_FMT_YUYV;
#else
	pixelFormat = V4L2_PIX_FMT_NV21;
#endif
	if( (NO_ERROR == V4L2_BufferInit(w,h,V4L2_PREVIEW_BUFF_NUM,pixelFormat))
		&& (V4L2_StreamOn() == NO_ERROR))
		return NO_ERROR;
	else
		return UNKNOWN_ERROR;
}
status_t	V4L2Camera::StopPreview()
{
    V4L2_StreamOff();
    V4L2_BufferUnInit();
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		Close();
#endif
		return NO_ERROR;

}

status_t	V4L2Camera::TakePicture()
{
	int w,h,pixelFormat;
	m_pSetting->m_hParameter.getPictureSize(&w,&h);
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		if(NO_ERROR != Open())
			return UNKNOWN_ERROR;
	pixelFormat = V4L2_PIX_FMT_YUYV;
#else
	pixelFormat = V4L2_PIX_FMT_RGB24;
#endif
	if(NO_ERROR != V4L2_BufferInit(w,h,V4L2_TAKEPIC_BUFF_NUM,pixelFormat)) {
		LOGD(" open devices error in w:%d,h:%d,fmt:%x\n",w,h,pixelFormat);
		return  UNKNOWN_ERROR;
	}
	V4L2_BufferEnQue(0);
	V4L2_StreamOn();
	m_iPicIdx = V4L2_BufferDeQue();
	V4L2_StreamOff();
	return NO_ERROR;
}

status_t	V4L2Camera::TakePictureEnd()
{
	int ret;
	m_iPicIdx = -1;
	ret=V4L2_BufferUnInit();
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
    ret=Close();
#endif
	return ret;
}

status_t	V4L2Camera::GetPreviewFrame(uint8_t* framebuf)
{
	int i=0;
	if(m_bFirstFrame)
	{
		for(i=0;i<V4L2_PREVIEW_BUFF_NUM;i++)
			V4L2_BufferEnQue(i);
		m_bFirstFrame = false;
		return NO_INIT;
	}
	else
	{
		int idx = V4L2_BufferDeQue();
                if (idx >= 0) {
		    memcpy((char*)framebuf,pV4L2Frames[idx],pV4L2FrameSize[idx]);
		    V4L2_BufferEnQue(idx);
		    return NO_ERROR;
                }
                else {
		    LOGE("V4L2Camera::GetPreviewFrame(%p) idx=%d\n", framebuf, idx);
		    return NO_MEMORY;
                }
	}
}

status_t	V4L2Camera::GetRawFrame(uint8_t* framebuf)
{
	if(m_iPicIdx!=-1)
	{
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		int w,h;
		m_pSetting->m_hParameter.getPictureSize(&w,&h);
		yuyv422_to_rgb16((unsigned char*)pV4L2Frames[m_iPicIdx],framebuf,pV4L2FrameSize[m_iPicIdx]);
#else
		memcpy(framebuf,pV4L2Frames[m_iPicIdx],pV4L2FrameSize[m_iPicIdx]);
#endif
	}
	else
		LOGD("GetRawFraem index -1");
	return NO_ERROR;
}


inline int CalIntLen(int content)
{
	int len = 1;
	while( (content = content/10) > 0 ) len++;
	return len;
}

int extraSmallImg(unsigned char* SrcImg,int SrcW,int SrcH,unsigned char* DstImg,int DstW,int DstH)
{
	int skipW = SrcW/DstW;
	int skipH = SrcH/DstH;

	//LOGD("skipw = %d, skipH=%d",skipW,skipH);

	unsigned char* dst = DstImg;

	unsigned char* srcrow = SrcImg;
	unsigned char* srcrowidx = srcrow;

	int i = 0,j = 0;
	for(;i<DstH;i++)
	{
		//LOGD("srcrow = %d,dst = %d",srcrow-SrcImg,dst-DstImg);
		for(j = 0;j<DstW;j++)
		{
			dst[0] = srcrowidx[0];
			dst[1] = srcrowidx[1];
			dst[2] = srcrowidx[2];
			dst+=3;
			srcrowidx+=3*skipW;
		}
	//	LOGD("srcrowidx end = %d",srcrowidx-SrcImg);

		srcrow += skipH*SrcW*3;
		srcrowidx = srcrow;
	}

	return 1;
}

int V4L2Camera::GenExif(unsigned char** pExif,int* exifLen,uint8_t* framebuf)
{
	#define MAX_EXIF_COUNT (20)
	char* exiflist[MAX_EXIF_COUNT]={0},CameraMake[20],CameraModel[20];
	int i = 0,curend = 0;

	//Make
	exiflist[i] = new char[64];
	property_get("ro.camera.exif.make",CameraMake,"Amlogic");
	sprintf(exiflist[i],"Make=%d %s",strlen(CameraMake),CameraMake);
	i++;

	//Model
	exiflist[i] = new char[64];
	property_get("ro.camera.exif.model",CameraModel,"DEFUALT");
	sprintf(exiflist[i],"Model=%d %s",strlen(CameraModel),CameraModel);
	i++;

	//Image width,height
	int width,height;
	m_pSetting->m_hParameter.getPictureSize(&width,&height);

	exiflist[i] = new char[64];
	sprintf(exiflist[i],"ImageWidth=%d %d",CalIntLen(width),width);
	i++;

	exiflist[i] = new char[64];
	sprintf(exiflist[i],"ImageLength=%d %d",CalIntLen(height),height);
	i++;

	//Image orientation
	int orientation = m_pSetting->m_hParameter.getInt(CameraParameters::KEY_ROTATION);
	//covert 0 90 180 270 to 0 1 2 3
	LOGE("get orientaion %d",orientation);
	if(orientation == 0)
		orientation = 1;
	else if(orientation == 90)
		orientation = 6;
	else if(orientation == 180)
		orientation = 3;
	else if(orientation == 270)
		orientation = 8;
	exiflist[i] = new char[64];
	sprintf(exiflist[i],"Orientation=%d %d",CalIntLen(orientation),orientation);
	i++;

	//focal length  RATIONAL
	float focallen = m_pSetting->m_hParameter.getFloat(CameraParameters::KEY_FOCAL_LENGTH);
	int focalNum = focallen*1000;
	int focalDen = 1000;
	exiflist[i] = new char[64];
	sprintf(exiflist[i],"FocalLength=%d %d/%d",CalIntLen(focalNum)+CalIntLen(focalDen)+1,focalNum,focalDen);
	i++;

	//add gps information
	//latitude info
	char* latitudestr = (char*)m_pSetting->m_hParameter.get(CameraParameters::KEY_GPS_LATITUDE);
	if(latitudestr!=NULL)
	{
		int offset = 0;
		float latitude = m_pSetting->m_hParameter.getFloat(CameraParameters::KEY_GPS_LATITUDE);
		if(latitude < 0.0)
		{
			offset = 1;
			latitude*= (float)(-1);
		}

		int latitudedegree = latitude;
		float latitudeminuts = (latitude-(float)latitudedegree)*60;
		int latitudeminuts_int = latitudeminuts;
		float latituseconds = (latitudeminuts-(float)latitudeminuts_int)*60;
		int latituseconds_int = latituseconds;
		exiflist[i] = new char[256];
		sprintf(exiflist[i],"GPSLatitude=%d %d/%d,%d/%d,%d/%d",CalIntLen(latitudedegree)+CalIntLen(latitudeminuts_int)+CalIntLen(latituseconds_int)+8,latitudedegree,1,latitudeminuts_int,1,latituseconds_int,1);
		i++;

		exiflist[i] = new char[64];
		if(offset == 1)
			sprintf(exiflist[i],"GPSLatitudeRef=1 S");
		else
			sprintf(exiflist[i],"GPSLatitudeRef=1 N ");
		i++;
	}

	//Longitude info
	char* longitudestr = (char*)m_pSetting->m_hParameter.get(CameraParameters::KEY_GPS_LONGITUDE);
	if(longitudestr!=NULL)
	{
		int offset = 0;
		float longitude = m_pSetting->m_hParameter.getFloat(CameraParameters::KEY_GPS_LONGITUDE);
		if(longitude < 0.0)
		{
			offset = 1;
			longitude*= (float)(-1);
		}

		int longitudedegree = longitude;
		float longitudeminuts = (longitude-(float)longitudedegree)*60;
		int longitudeminuts_int = longitudeminuts;
		float longitudeseconds = (longitudeminuts-(float)longitudeminuts_int)*60;
		int longitudeseconds_int = longitudeseconds;
		exiflist[i] = new char[256];
		sprintf(exiflist[i],"GPSLongitude=%d %d/%d,%d/%d,%d/%d",CalIntLen(longitudedegree)+CalIntLen(longitudeminuts_int)+CalIntLen(longitudeseconds_int)+8,longitudedegree,1,longitudeminuts_int,1,longitudeseconds_int,1);
		i++;

		exiflist[i] = new char[64];
		if(offset == 1)
			sprintf(exiflist[i],"GPSLongitudeRef=1 W");
		else
			sprintf(exiflist[i],"GPSLongitudeRef=1 E");
		i++;
	}

	//Altitude info
	char* altitudestr = (char*)m_pSetting->m_hParameter.get(CameraParameters::KEY_GPS_ALTITUDE);
	if(altitudestr!=NULL)
	{
		int offset = 0;
		float altitude = m_pSetting->m_hParameter.getFloat(CameraParameters::KEY_GPS_ALTITUDE);
		if(altitude < 0.0)
		{
			offset = 1;
			altitude*= (float)(-1);
		}

		int altitudenum = altitude*1000;
		int altitudedec= 1000;
		exiflist[i] = new char[256];
		sprintf(exiflist[i],"GPSAltitude=%d %d/%d",CalIntLen(altitudenum)+CalIntLen(altitudedec)+1,altitudenum,altitudedec);
		i++;

		exiflist[i] = new char[64];
		sprintf(exiflist[i],"GPSAltitudeRef=1 %d",offset);
		i++;
	}

	//date stamp & time stamp
	time_t times = m_pSetting->m_hParameter.getInt(CameraParameters::KEY_GPS_TIMESTAMP);
	if(times != -1)
	{
		struct tm tmstruct;
		tmstruct = *(gmtime(&times));//convert to standard time

		//date
		exiflist[i] = new char[128];
		char timestr[30];
		strftime(timestr, 20, "%Y:%m:%d", &tmstruct);
		sprintf(exiflist[i],"GPSDateStamp=%d %s",strlen(timestr),timestr);
		i++;

		//time
		exiflist[i] = new char[128];
		sprintf(exiflist[i],"GPSTimeStamp=%d %d/%d,%d/%d,%d/%d",CalIntLen(tmstruct.tm_hour)+CalIntLen(tmstruct.tm_min)+CalIntLen(tmstruct.tm_sec)+8,tmstruct.tm_hour,1,tmstruct.tm_min,1,tmstruct.tm_sec,1);
		i++;
	}

	//datetime of photo
	{
		time_t curtime = 0;
		time(&curtime);
		struct tm tmstruct;
		tmstruct = *(localtime(&times)); //convert to local time

		//date&time
		exiflist[i] = new char[64];
		char timestr[30];
    	strftime(timestr, 30, "%Y:%m:%d %H:%M:%S", &tmstruct);
		sprintf(exiflist[i],"DateTime=%d %s",strlen(timestr),timestr);
		i++;
	}

	//processing method
	char* processmethod = (char*)m_pSetting->m_hParameter.get(CameraParameters::KEY_GPS_PROCESSING_METHOD);
	if(processmethod!=NULL)
	{
		char ExifAsciiPrefix[] = { 0x41, 0x53, 0x43, 0x49, 0x49, 0x0, 0x0, 0x0 };//asicii

		exiflist[i] = new char[128];
		int len = sizeof(ExifAsciiPrefix)+strlen(processmethod);
		sprintf(exiflist[i],"GPSProcessingMethod=%d ",len);
		int curend = strlen(exiflist[i]);
		memcpy(exiflist[i]+curend,ExifAsciiPrefix,8);
		memcpy(exiflist[i]+curend+8,processmethod,strlen(processmethod));
		i++;
	}

	//print exif
	int j = 0;
	for(;j<MAX_EXIF_COUNT;j++)
	{
		if(exiflist[j]!=NULL)
			LOGE("EXIF %s",exiflist[j]);
	}

	//thumbnail
	int thumbnailsize = 0;
	char* thumbnaildata = NULL;
	int thumbnailwidth = m_pSetting->m_hParameter.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
	int thumbnailheight = m_pSetting->m_hParameter.getInt(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
	if(thumbnailwidth > 0 )
	{
	//	LOGE("creat thumbnail data");
		//create thumbnail data
		unsigned char* rgbdata = (unsigned char*)new char[thumbnailwidth*thumbnailheight*3];
		extraSmallImg(framebuf,width,height,rgbdata,thumbnailwidth,thumbnailheight);

		//code the thumbnail to jpeg
		thumbnaildata = new char[thumbnailwidth*thumbnailheight*3];
		jpeg_enc_t enc;
		enc.width = thumbnailwidth;
		enc.height = thumbnailheight;
		enc.quality = 90;
		enc.idata = (unsigned char*)rgbdata;
		enc.odata = (unsigned char*)thumbnaildata;
		enc.ibuff_size =  thumbnailwidth*thumbnailheight*3;
		enc.obuff_size =  thumbnailwidth*thumbnailheight*3;
		enc.data_in_app1 = 0;
		enc.app1_data_size = 0;
		thumbnailsize = encode_jpeg2(&enc);

		delete rgbdata;
	//	LOGD("after add thumbnail %d,%d len %d",thumbnailwidth,thumbnailheight,thumbnailsize);
	}

	*pExif = getExifBuf(exiflist,i,exifLen,thumbnailsize,thumbnaildata);

	//release exif
	for(i=0;i<MAX_EXIF_COUNT;i++)
	{
		if(exiflist[i]!=NULL)
			delete exiflist[i];
		exiflist[i] = NULL;
	}
	//release thumbnaildata
	if(thumbnaildata)
		delete thumbnaildata;

	return 1;
}

status_t	V4L2Camera::GetJpegFrame(uint8_t* framebuf,int* jpegsize)
{
	*jpegsize = 0;
	if(m_iPicIdx!=-1)
	{
		unsigned char* exifcontent = NULL;
		jpeg_enc_t enc;
		m_pSetting->m_hParameter.getPictureSize(&enc.width,&enc.height);
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		sp<MemoryHeapBase> rgbheap = new MemoryHeapBase( enc.width * 3 * enc.height);
		yuyv422_to_rgb24((unsigned char*)pV4L2Frames[m_iPicIdx],(unsigned char *)rgbheap->getBase(),enc.width,enc.height);
#endif
		enc.quality= m_pSetting->m_hParameter.getInt(CameraParameters::KEY_JPEG_QUALITY);
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		enc.idata = (unsigned char *)rgbheap->getBase();
#else
		enc.idata = (unsigned char*)pV4L2Frames[m_iPicIdx];
#endif
		enc.odata = (unsigned char*)framebuf;
		enc.ibuff_size =  pV4L2FrameSize[m_iPicIdx];
		enc.obuff_size =  pV4L2FrameSize[m_iPicIdx];
#ifdef AMLOGIC_USB_CAMERA_SUPPORT
		GenExif(&(exifcontent),&(enc.app1_data_size),(unsigned char*)rgbheap->getBase());
#else
		GenExif(&(exifcontent),&(enc.app1_data_size),(unsigned char*)pV4L2Frames[m_iPicIdx]);
#endif
		enc.data_in_app1=exifcontent+2;
		*jpegsize = encode_jpeg2(&enc);
		if(exifcontent!=0)
			free(exifcontent);
	}
	else
	{
		LOGE("GetRawFraem index -1");
		return UNKNOWN_ERROR;
	}
	return NO_ERROR;
}


//=======================================================================================
//functions for set V4L2

status_t	V4L2Camera::V4L2_GetValidFrameSize(unsigned int pixel_format,String8 &framesize)
{
	struct v4l2_frmsizeenum frmsize;
	int i=0;
	char tempwidth[5],tempheight[5];
	memset(&frmsize,0,sizeof(v4l2_frmsizeenum));
	for(i=0;;i++){
		frmsize.index = i;
		frmsize.pixel_format = pixel_format;
		if(ioctl(m_pSetting->m_iDevFd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0){
			if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE){	//only support this type
				sprintf(tempwidth,"%d",frmsize.discrete.width);
				sprintf(tempheight,"%d",frmsize.discrete.height);
				framesize.append(tempwidth);
				framesize.append("x");
				framesize.append(tempheight);
				framesize.append(",");
			}
			else
				break;
		}
		else
			break;
	}
	if(framesize.length() == 0)
		return UNKNOWN_ERROR;
	else
		return NO_ERROR;
}

status_t V4L2Camera::V4L2_BufferInit(int Buf_W,int Buf_H,int Buf_Num,int colorfmt)
{
	struct v4l2_format hformat;
	memset(&hformat,0,sizeof(v4l2_format));
	hformat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	hformat.fmt.pix.width = Buf_W;
	hformat.fmt.pix.height = Buf_H;
	hformat.fmt.pix.pixelformat = colorfmt;
	if (ioctl(m_pSetting->m_iDevFd, VIDIOC_S_FMT, &hformat) == -1)
	{
		LOGE("V4L2_BufferInit VIDIOC_S_FMT fail m_pSetting->m_iDevFd=%d width=%d height=%d pixelformat=(%c%c%c%c),errno:%d",m_pSetting->m_iDevFd,hformat.fmt.pix.width,hformat.fmt.pix.height,colorfmt&0xff, (colorfmt>>8)&0xff, (colorfmt>>16)&0xff, (colorfmt>>24)&0xff,errno);
		return UNKNOWN_ERROR;
	}

	//requeset buffers in V4L2
	v4l2_requestbuffers hbuf_req;
	memset(&hbuf_req,0,sizeof(v4l2_requestbuffers));
	hbuf_req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	hbuf_req.memory = V4L2_MEMORY_MMAP;
	hbuf_req.count = Buf_Num; //just set two frames for hal have cache buffer
	if (ioctl(m_pSetting->m_iDevFd, VIDIOC_REQBUFS, &hbuf_req) == -1)
	{
		LOGE("V4L2_BufferInit VIDIOC_REQBUFS fail");
		return UNKNOWN_ERROR;
	}
	else
	{
		if (hbuf_req.count < Buf_Num)
		{
		    LOGE("V4L2_BufferInit hbuf_req.count < Buf_Num");
			return UNKNOWN_ERROR;
		}
		else//memmap these buffer to user space
		{
			pV4L2Frames = (void**)new int[Buf_Num];
			pV4L2FrameSize = new int[Buf_Num];
			int i = 0;
			v4l2_buffer hbuf_query;
			memset(&hbuf_query,0,sizeof(v4l2_buffer));

			hbuf_query.type = hbuf_req.type;
			hbuf_query.memory = V4L2_MEMORY_MMAP;
			for(;i<Buf_Num;i++)
			{
				hbuf_query.index = i;
				if (ioctl(m_pSetting->m_iDevFd, VIDIOC_QUERYBUF, &hbuf_query) == -1)
				{
					LOGE("Memap V4L2 buffer Fail");
					return UNKNOWN_ERROR;
				}

				pV4L2FrameSize[i] = hbuf_query.length;
				LOGD("V4L2_BufferInit::Get Buffer Idx %d Len %d",i,pV4L2FrameSize[i]);
				pV4L2Frames[i] = mmap(NULL,pV4L2FrameSize[i],PROT_READ | PROT_WRITE,MAP_SHARED,m_pSetting->m_iDevFd,hbuf_query.m.offset);
				if(pV4L2Frames[i] == MAP_FAILED)
				{
					LOGE("Memap V4L2 buffer Fail");
					return UNKNOWN_ERROR;
				}
				/*
				//enqueue buffer
				if (ioctl(m_pSetting->m_iDevFd, VIDIOC_QBUF, &hbuf_query) == -1)
				{
					LOGE("GetPreviewFrame nque buffer fail");
					return UNKNOWN_ERROR;
			    }
			    */
			}
			m_V4L2BufNum = Buf_Num;
		}
	}
	return NO_ERROR;
}

status_t V4L2Camera::V4L2_BufferUnInit()
{
	if(m_V4L2BufNum > 0)
	{
		//un-memmap
		int i = 0;
		for (; i < m_V4L2BufNum; i++)
		{
			munmap(pV4L2Frames[i], pV4L2FrameSize[i]);
			pV4L2Frames[i] = NULL;
			pV4L2FrameSize[i] = 0;
			LOGD("unint buffer %d",i);
		}
		m_V4L2BufNum = 0;
		delete pV4L2Frames;
		delete pV4L2FrameSize;
		pV4L2FrameSize = NULL;
		pV4L2Frames = NULL;
	}
	return NO_ERROR;
}

status_t V4L2Camera::V4L2_BufferEnQue(int idx)
{
	v4l2_buffer hbuf_query;
	memset(&hbuf_query,0,sizeof(v4l2_buffer));
	hbuf_query.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	hbuf_query.memory = V4L2_MEMORY_MMAP;
	hbuf_query.index = idx;
    if (ioctl(m_pSetting->m_iDevFd, VIDIOC_QBUF, &hbuf_query) == -1)
	{
		LOGE("V4L2_BufferEnQue fail %d",errno);
		return UNKNOWN_ERROR;
    }

	return NO_ERROR;
}
int  V4L2Camera::V4L2_BufferDeQue()
{
	v4l2_buffer hbuf_query;
	memset(&hbuf_query,0,sizeof(v4l2_buffer));
	hbuf_query.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	hbuf_query.memory = V4L2_MEMORY_MMAP;
    if (ioctl(m_pSetting->m_iDevFd, VIDIOC_DQBUF, &hbuf_query) == -1)
	{
		LOGE("V4L2_StreamGet Deque buffer fail %d",errno);
		return -1;
    }

	assert (hbuf_query.index < m_V4L2BufNum);
	pV4L2FrameSize[hbuf_query.index] = hbuf_query.bytesused;
	return hbuf_query.index;
}

status_t	V4L2Camera::V4L2_StreamOn()
{
	//LOGD("V4L2_StreamOn");
	int stream_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(m_pSetting->m_iDevFd, VIDIOC_STREAMON, &stream_type) == -1)
		LOGE("V4L2_StreamOn Fail %d",errno);
	return NO_ERROR;
}

status_t	V4L2Camera::V4L2_StreamOff()
{
	int stream_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(m_pSetting->m_iDevFd, VIDIOC_STREAMOFF, &stream_type) == -1)
		LOGE("V4L2_StreamOff  Fail %d",errno);
	return NO_ERROR;
}

//extern CameraInterface* HAL_GetFakeCamera();
extern CameraInterface* HAL_GetCameraInterface(int Id)
{
	LOGD("HAL_GetCameraInterface return V4L2 interface");
	if(Id == 0)
		return new V4L2Camera("/dev/video0",0);
	else
		return new V4L2Camera("/dev/video0",1);
}

};
