/*
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_NDEBUG 0
#define LOG_TAG "AmlogicPlayer"
#include "utils/Log.h"
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <surfaceflinger/ISurface.h>
#include <ui/Overlay.h>

#include <cutils/properties.h>

extern int android_datasource_init(void);

#include "AmlogicPlayer.h"
#ifndef FBIOPUT_OSD_SRCCOLORKEY
#define  FBIOPUT_OSD_SRCCOLORKEY    0x46fb
#endif

#ifndef FBIOPUT_OSD_SRCKEY_ENABLE
#define  FBIOPUT_OSD_SRCKEY_ENABLE  0x46fa
#endif

#ifndef FBIOPUT_OSD_SET_GBL_ALPHA
#define  FBIOPUT_OSD_SET_GBL_ALPHA	0x4500
#endif


#ifdef HAVE_GETTID
static pid_t myTid() { return gettid(); }
#else
static pid_t myTid() { return getpid(); }
#endif

// ----------------------------------------------------------------------------

namespace android {

// ----------------------------------------------------------------------------

// TODO: Determine appropriate return codes
static status_t ERROR_NOT_OPEN = -1;
static status_t ERROR_OPEN_FAILED = -2;
static status_t ERROR_ALLOCATE_FAILED = -4;
static status_t ERROR_NOT_SUPPORTED = -8;
static status_t ERROR_NOT_READY = -16;
static status_t STATE_INIT = 0;
static status_t STATE_ERROR = 1;
static status_t STATE_OPEN = 2;

static URLProtocol android_protocol;

#define MID_800_400_FREESC	(0x10001)

AmlogicPlayer::AmlogicPlayer() :
    mPlayTime(0), mDuration(0), mState(STATE_ERROR),
    mStreamType(-1), mLoop(false), 
    mExit(false), mPaused(false), mRunning(false),
    mPlayer_id(-1),
    mWidth(0),
    mHeight(0),
    mhasVideo(0),
    mhasAudio(0),
    mIgnoreMsg(false),
    mTypeReady(false),
    mAudioTrackNum(0),    
    mInnerSubNum(0),
    mVideoTrackNum(0)
{
	Mutex::Autolock l(mMutex);
	streaminfo_valied=false;
	mOverlay=NULL;
	mISurface=NULL;
	mStrCurrentVideoCodec = NULL;
	mStrCurrentAudioCodec = NULL;
	mAudioExtInfo = NULL; 
	mSubExtInfo = NULL;
	mVideoExtInfo = NULL;
	mChangedCpuFreq=false;
  LOGV("AmlogicPlayer constructor\n");
  memset(&mAmlogicFile, 0, sizeof mAmlogicFile);
	memset(&mPlay_ctl,0,sizeof mPlay_ctl);
	memset(mTypeStr,0,sizeof(mTypeStr));
	memset(&mStreamInfo,0,sizeof(mStreamInfo));
	
}

int HistoryMgt(const char * path,int r0w1,int mTime)
{
  ///this a simple history mgt;only save the latest file,and playingtime,and must be http;
  static char static_path[1024]="";
  static Mutex HistoryMutex;
  static int lastplayingtime=-1;
  Mutex::Autolock l(HistoryMutex);
  LOGV("History mgt old[%s,%d,%d]\n",static_path,0,lastplayingtime);
  LOGV("History mgt    [%s,%d,%d]\n",path,r0w1,mTime);
  if(!r0w1)//read
  {
	if(strcmp(path,static_path)==0)
		return lastplayingtime;
		
  }else{//save
	if(strlen(path)>1024-1 || strlen(path)<10)
		return 0;
	if(memcmp(path,"http://",7)==0 || memcmp(path,"shttp://",8)==0 )//not http,we don't save it now;
	{
 		strcpy(static_path,path);
        	lastplayingtime=mTime;
		return 0;
	}
  }
  return 0;
}

status_t AmlogicPlayer::BasicInit()
{
	static int have_inited=0;
	if(!have_inited)
	{
		player_init();
		URLProtocol *prot=&android_protocol;
		prot->name="android";
	    prot->url_open=(int (*)(URLContext *, const char *, int ))vp_open;
	    prot->url_read=(int (*)(URLContext *, unsigned char *, int))vp_read;
	    prot->url_write=(int (*)(URLContext *, unsigned char *, int))vp_write;
	    prot->url_seek=(int64_t (*)(URLContext *, int64_t , int))vp_seek;
	    prot->url_close=(int (*)(URLContext *))vp_close;
	    prot->url_get_file_handle = (int (*)(URLContext *))vp_get_file_handle;
		av_register_protocol(prot);
		have_inited++;
	}
	return 0;
}



 bool AmlogicPlayer::PropIsEnable(const char* str)
{ 
	char value[PROPERTY_VALUE_MAX];
	if(property_get(str, value, NULL)>0)
	{
		if ((!strcmp(value, "1") || !strcmp(value, "true")))
		{
			LOGI("%s is enabled\n",str);
			return true;
		}
	}
	LOGI("%s is disabled\n",str);
	return false;
}


 float AmlogicPlayer::PropGetInt(const char* str)
{ 
	char value[PROPERTY_VALUE_MAX];
	float ret=0.0;
	if(property_get(str, value, NULL)>0)
	{
		if ((sscanf(value,"%f",&ret))>0)
		{
			LOGI("%s is set to %f\n",str,ret);
			return ret;
		}
	}
	LOGI("%s is not set\n",str);
	return ret;
}

status_t AmlogicPlayer::exitAllThreads()
{
	AmlogicPlayer::BasicInit();
	pid_info_t playerinfo;

	player_list_allpid(&playerinfo);
	LOGI("found %d not exit player threads,try exit it now\n",playerinfo.num);
	if(playerinfo.num>0)
	{
		int i;
		for(i=0;i<playerinfo.num;i++)
			{
			//player_exit(playerinfo.pid[i]);
			}
	}
	return NO_ERROR;
}

void AmlogicPlayer::onFirstRef()
{
	Mutex::Autolock l(mMutex);
    LOGV("onFirstRef");
	AmlogicPlayer::BasicInit();
	AmlogicPlayer::exitAllThreads();
	av_log_set_level(50);
    // create playback thread
    mState = STATE_INIT;
}

status_t AmlogicPlayer::initCheck()
{
	Mutex::Autolock l(mMutex);
	LOGV("initCheck");
    if (mState != STATE_ERROR) 
			return NO_ERROR;
    return ERROR_NOT_READY;
}

int get_sysfs_int(const char *path)
{
    int fd;
    int val = 0;
    char  bcmd[16];
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        read(fd, bcmd, sizeof(bcmd));
        val = strtol(bcmd, NULL, 10);
        close(fd);
    }
    return val;
}

int set_sys_int(const char *path,int val)
{
    	int fd;
        char  bcmd[16];
        fd=open(path, O_CREAT|O_RDWR | O_TRUNC, 0644);
        if(fd>=0)
        {
        	sprintf(bcmd,"%d",val);
        	write(fd,bcmd,strlen(bcmd));
        	close(fd);
        	return 0;
        }
	LOGV("set fs%s=%d failed\n",path,val);
        return -1;
}
#define DISABLE_VIDEO "/sys/class/video/disable_video"
void 
AmlogicPlayer::VideoViewOn(void)
{
	int ret=0;
	disable_freescale(MID_800_400_FREESC);
    	ret=player_video_overlay_en(1);
	LOGV("VideoViewOn=%d\n",ret);
	//OsdBlank("/sys/class/graphics/fb0/blank",1);
	set_sys_int(DISABLE_VIDEO,2);
}
void 
AmlogicPlayer::VideoViewClose(void)
{
	int ret=0;
    	ret=player_video_overlay_en(0);
	set_sys_int(DISABLE_VIDEO,2);
	enable_freescale(MID_800_400_FREESC);
	LOGV("VideoViewClose=%d\n",ret);
	//OsdBlank("/sys/class/graphics/fb0/blank",0);

}

void 
AmlogicPlayer::SetCpuScalingOnAudio(float mul_audio){
	const char InputFile[] = "/sys/class/audiodsp/codec_mips";
	const char OutputFile[] = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq";
	int val;
	val=get_sysfs_int(InputFile);
	if(val>0 && mul_audio >0){
		val=mul_audio*val;
		set_sys_int(OutputFile,val);
		LOGV("set_cpu_freq_scaling_based_auido %d\n",val);
	}else{
		LOGV("set_cpu_freq_scaling_based_auido failed\n");
	}
}

AmlogicPlayer::~AmlogicPlayer() {
    LOGV("AmlogicPlayer destructor\n");
	Mutex::Autolock l(mMutex);
    release();
	if(mStrCurrentAudioCodec){
		free(mStrCurrentAudioCodec);
		mStrCurrentAudioCodec = NULL;
	}	
	if(mStrCurrentVideoCodec){
		free(mStrCurrentVideoCodec);
		mStrCurrentVideoCodec = NULL;
	}
	if(mAudioExtInfo){
		free(mAudioExtInfo);
		mAudioExtInfo = NULL;
	}
	if(mSubExtInfo){
		free(mSubExtInfo);
		mSubExtInfo = NULL;
	}	
	if(mVideoExtInfo){
		free(mVideoExtInfo);
		mVideoExtInfo = NULL;
	}

	
}

status_t AmlogicPlayer::setDataSource(
        const char *uri, const KeyedVector<String8, String8> *headers) 
{
    LOGV("setDataSource");
    return setdatasource(uri, -1, 0, 0x7ffffffffffffffLL, headers); // intentionally less than LONG_MAX
}


status_t AmlogicPlayer::setDataSource(int fd, int64_t offset, int64_t length)
{	
	LOGV("setDataSource,fd=%d,offset=%lld,len=%lld,not finished\n",fd,offset,length);
	return setdatasource(NULL,fd,offset,length, NULL);
}

int AmlogicPlayer::vp_open(URLContext *h, const char *filename, int flags)
{	
	/*
	sprintf(file,"android:AmlogicPlayer=[%x:%x],AmlogicPlayer_fd=[%x:%x]",
	*/
	LOGV("vp_open=%s\n",filename);
	if(strncmp(filename,"android",strlen("android"))==0)
	{	
		unsigned int fd=0,fd1=0;
		char *str=strstr(filename,"AmlogicPlayer_fd");
		if(str==NULL)
			return -1;
		sscanf(str,"AmlogicPlayer_fd=[%x:%x]\n",(unsigned int*)&fd,(unsigned int*)&fd1);
		if(fd!=0 && ((unsigned int)fd1==~(unsigned int)fd))
		{
			AmlogicPlayer_File* af= (AmlogicPlayer_File*)fd;
			h->priv_data=(void*) fd;
			if(af!=NULL && af->fd_valid)
			{
				
				lseek(af->fd, af->mOffset, SEEK_SET);
				LOGV("android_open %s OK,h->priv_data=%p\n",filename,h->priv_data);
				return 0;
			}
			else
			{
				LOGV("android_open %s Faild\n",filename);
				return -1;
			}
		}
	}
	return -1;
}

int AmlogicPlayer::vp_read(URLContext *h, unsigned char *buf, int size)
{	
	AmlogicPlayer_File* af= (AmlogicPlayer_File*)h->priv_data;
	int ret;
	//LOGV("start%s,pos=%lld,size=%d,ret=%d\n",__FUNCTION__,(int64_t)lseek(af->fd, 0, SEEK_CUR),size,ret);
	ret=read(af->fd,buf,size);
	//LOGV("end %s,size=%d,ret=%d\n",__FUNCTION__,size,ret);
	return ret;
}

int AmlogicPlayer::vp_write(URLContext *h, unsigned char *buf, int size)
{	
	AmlogicPlayer_File* af= (AmlogicPlayer_File*)h->priv_data;
	LOGV("%s\n",__FUNCTION__);
	return -1;
}
int64_t AmlogicPlayer::vp_seek(URLContext *h, int64_t pos, int whence)
{
	AmlogicPlayer_File* af= (AmlogicPlayer_File*)h->priv_data;
	int64_t ret;
	//LOGV("%sret=%lld,pos=%lld,whence=%d,tell=%lld\n",__FUNCTION__,(int64_t)0,pos,whence,(int64_t)lseek(af->fd,0,SEEK_CUR));
	if (whence == AVSEEK_SIZE)
	{
		return af->mLength;
#if 0
		struct stat filesize;
		if(fstat(af->fd,&filesize) < 0){
				int64_t size;
				int64_t oldpos;
				oldpos=lseek(af->fd, 0, SEEK_CUR);
			 	if ((size =lseek(af->fd, -1, SEEK_END)) < 0)
		        {
		        	return size;
		        }
				size++;
				lseek(af->fd,oldpos, SEEK_SET);
				return size;
			}
		else
			return filesize.st_size;
#endif		
	}
	switch(whence){
		case SEEK_CUR:
		case SEEK_END:		
			ret=lseek(af->fd,pos, whence);
			return ret-af->mOffset;
		case SEEK_SET:
			ret=lseek(af->fd,pos+af->mOffset, whence);
			if(ret<0)
				return ret;
			else
				return ret-af->mOffset;
		default:
			return -1;
	}
	return -1;
}


int AmlogicPlayer::vp_close(URLContext *h)
{
	FILE* fp= (FILE*)h->priv_data;
	LOGV("%s\n",__FUNCTION__);
	return 0; /*don't close file here*/
	//return fclose(fp);
}

int AmlogicPlayer::vp_get_file_handle(URLContext *h)
{
	LOGV("%s\n",__FUNCTION__);
    return (intptr_t) h->priv_data;
}


int AmlogicPlayer::notifyhandle(int pid,int msg,unsigned long ext1,unsigned long ext2)
{
	AmlogicPlayer *player =(AmlogicPlayer *)player_get_extern_priv(pid);
	if(player!=NULL)
		return player->NotifyHandle(pid,msg,ext1,ext2);
	else
		return -1;
}
int AmlogicPlayer::NotifyHandle(int pid,int msg,unsigned long ext1,unsigned long ext2)
{ 
	player_file_type_t *type;
	switch(msg)
		{
			case PLAYER_EVENTS_PLAYER_INFO:
				return UpdateProcess(pid,(player_info_t *)ext1);
				break;
			case PLAYER_EVENTS_STATE_CHANGED:
			case PLAYER_EVENTS_ERROR:
			case PLAYER_EVENTS_BUFFERING:
				break;
			case PLAYER_EVENTS_FILE_TYPE:
				type=(player_file_type_t *)ext1;
				mhasAudio=type->audio_tracks;
				mhasVideo=type->video_tracks;
				strncpy(mTypeStr,type->fmt_string,64);
				mTypeStr[63]='\0';
				LOGV("Type=%s,videos=%d,audios=%d\n",type->fmt_string,mhasVideo,mhasAudio);
				mTypeReady=true;
				sendEvent(0x11000);	
				break;
			default:
				break;
		}
	return 0;
}

int AmlogicPlayer::UpdateProcess(int pid,player_info_t *info)
{

	LOGV("update_process pid=%d, current=%d,status=[%s]\n", pid, info->current_time,player_status2str(info->status));
	if(mIgnoreMsg && info->status!=PLAYER_ERROR)	
		return 0;
	if(	info->status==PLAYER_BUFFERING 	||
		info->status==PLAYER_SEARCHING)
	{
		sendEvent(MEDIA_BUFFERING_UPDATE,mPlayTime*100/mDuration);
	}
	else if(info->status==PLAYER_INITOK)
	{	
		updateMediaInfo();
		if(info->full_time!=-1)
			mDuration=info->full_time*1000;
		sendEvent(MEDIA_SET_VIDEO_SIZE,mWidth,mHeight);
		sendEvent(MEDIA_PREPARED);
	}
	else if(info->status==PLAYER_STOPED || info->status==PLAYER_PLAYEND)
	{
		LOGV("Player status:%s, playback complete",player_status2str(info->status));
	}
	else if(info->status==PLAYER_EXIT )
	{
		LOGV("Player status:%s, playback exit",player_status2str(info->status));
		mRunning=false;
		if (!mLoop && mState != STATE_ERROR)//no errors & no loop^M
			sendEvent(MEDIA_PLAYBACK_COMPLETE);
	}
	else if(info->status==PLAYER_ERROR)
	{
		sendEvent(MEDIA_ERROR,MEDIA_ERROR_UNKNOWN,info->error_no);
		//sendEvent(MEDIA_ERROR,MEDIA_ERROR_UNKNOWN,info->error_no);
		LOGV("Player status:%s, error occur",player_status2str(info->status));
		//sendEvent(MEDIA_ERROR);
		sendEvent(MEDIA_ERROR,MEDIA_ERROR_UNKNOWN,-1);
		mState = STATE_ERROR;
	}
	else
	{

		int percent=0;
		if(mDuration>0)
			percent=(mPlayTime)*100/(mDuration);
		else
			percent=0;

		if(info->status==PLAYER_SEARCHOK)
		{ 
			///sendEvent(MEDIA_SEEK_COMPLETE);
		}
		if(info->full_time!=-1)
			mDuration=info->full_time*1000;
		if(info->current_time!=-1)
		 	mPlayTime=info->current_time*1000;		 

		
		LOGV("Playing percent =%d\n",percent);
		if(mDuration >0 && streaminfo_valied && mStreamInfo.stream_info.file_size>0)
		{
			percent+=((long long)4*1024*1024*100*info->audio_bufferlevel/mStreamInfo.stream_info.file_size);
			percent+=((long long)6*1024*1024*100*info->video_bufferlevel/mStreamInfo.stream_info.file_size);
			/*we think the lowlevel buffer size is alsways 10M */
			LOGV("Playing buffer percent =%d\n",percent);
		}
		else if(streaminfo_valied && mDuration>0&& info->bufed_time>0){
		
			percent=(info->bufed_time*100/(mDuration/1000));
			LOGV("Playing percent on percent=%d,bufed time=%dS,Duration=%dS\n",percent,info->bufed_time,mDuration/1000);
		}
		else
		{
			//percent+=info->audio_bufferlevel*4;
			//percent+=info->video_bufferlevel*6;
		}
		if(percent>100) percent=100;
		else if(percent<0) percent=0;
		sendEvent(MEDIA_BUFFERING_UPDATE,percent);		

	}

	 return 0;
}
status_t AmlogicPlayer::GetFileType(char **typestr,int *videos,int *audios)
{
	if(!mTypeReady)
		return ERROR_NOT_OPEN;
	
	LOGV("GetFileType---Type=%s,videos=%d,audios=%d\n",mTypeStr,mhasVideo,mhasAudio);
	*typestr=mTypeStr;
	
	*videos=mhasVideo;
	
	*audios=mhasAudio;
	return NO_ERROR;
}

status_t AmlogicPlayer::setdatasource(const char *path, int fd, int64_t offset, int64_t length, const KeyedVector<String8, String8> *headers)
{
	int num;
  	char * file=NULL;

	if(path==NULL)
	{
		if(fd<0 || offset<0)
			return -1;
		file=(char *)malloc(128);
		if(file==NULL)
			return NO_MEMORY;
		mAmlogicFile.oldfd=fd;
		mAmlogicFile.fd = dup(fd);
		mAmlogicFile.fd_valid=1;
		mAmlogicFile.mOffset=offset;
		mAmlogicFile.mLength=length;
		mPlay_ctl.t_pos=-1;/*don't seek to 0*/
		//mPlay_ctl.t_pos=0;/*don't seek to 0*/
		sprintf(file,"android:AmlogicPlayer=[%x:%x],AmlogicPlayer_fd=[%x:%x]",
					(unsigned int)this,(~(unsigned int)this),
					(unsigned int)&mAmlogicFile,(~(unsigned int)&mAmlogicFile));
	}
	else
	{
		int time;
		file=(char *)malloc(strlen(path)+4);
		if(file==NULL)
			return NO_MEMORY;
		if(strncmp(path,"http",strlen("http"))==0)
			{	/*http-->shttp*/
				num=sprintf(file,"s%s",path);
				file[num]='\0';
			}
		else
			{
				num=sprintf(file,"%s",path);
				file[num]='\0';
			}
		time=HistoryMgt(file,0,0);
		if(time>0)
			mPlay_ctl.t_pos=time;
		else	
			mPlay_ctl.t_pos=-1;
        	if (mPlay_ctl.headers) {
            		free(mPlay_ctl.headers);
            		mPlay_ctl.headers = NULL;
        	}
        if (headers) {
            //one huge string of the HTTP headers to add
            int len = 0;
            for (size_t i = 0; i < headers->size(); ++i) {
                len += strlen(headers->keyAt(i));
                len += strlen(": ");
                len += strlen(headers->valueAt(i));
                len += strlen("\r\n");
            }
            len += 1;
            mPlay_ctl.headers = (char *)malloc(len);
            if (mPlay_ctl.headers) {
                mPlay_ctl.headers[0] = 0;
                for (size_t i = 0; i < headers->size(); ++i) {
                    strcat(mPlay_ctl.headers, headers->keyAt(i));
                    strcat(mPlay_ctl.headers, ": ");
                    strcat(mPlay_ctl.headers, headers->valueAt(i));
                    strcat(mPlay_ctl.headers, "\r\n");
                }
                mPlay_ctl.headers[len - 1] = '\0';
            }
        }
	}
	mPlay_ctl.need_start=1;
	mAmlogicFile.datasource=file;
	mPlay_ctl.file_name=(char*)mAmlogicFile.datasource;
	LOGV("setDataSource url=%s, len=%d\n", mPlay_ctl.file_name, strlen(mPlay_ctl.file_name));
	mState = STATE_OPEN;
	return NO_ERROR;

}




status_t AmlogicPlayer::prepare()
{
		LOGV("prepare\n");
		if(prepareAsync()!=NO_ERROR)
			return UNKNOWN_ERROR;
		while(player_get_state(mPlayer_id)!=PLAYER_INITOK)
			{
			if((player_get_state(mPlayer_id))==PLAYER_ERROR ||
				player_get_state(mPlayer_id)==PLAYER_STOPED ||
				player_get_state(mPlayer_id)==PLAYER_PLAYEND|| 
				player_get_state(mPlayer_id)==PLAYER_EXIT
				)
				{
				return UNKNOWN_ERROR;
				}
				usleep(1000*10);
			}
    	return NO_ERROR;
}


status_t AmlogicPlayer::prepareAsync() {
	int check_is_playlist = -1;
	float level=PropGetInt("media.amplayer.lpbufferlevel");
	LOGV("prepareAsync\n");
	mPlay_ctl.callback_fn.notify_fn=notifyhandle;
	mPlay_ctl.callback_fn.update_interval=1000;
	mPlay_ctl.audio_index=-1;
	mPlay_ctl.video_index=-1;
	mPlay_ctl.hassub = 1;  //enable subtitle
	mPlay_ctl.is_type_parser=1;
	mPlay_ctl.auto_buffing_enable=1;
	mPlay_ctl.buffing_min=(level<0.001 && level >0.0)?level/10:0.001;
	mPlay_ctl.buffing_middle=level>0?level:0.02;
	mPlay_ctl.buffing_max=level<0.8?0.8:level;
	mPlay_ctl.enable_rw_on_pause=1;
	mPlay_ctl.read_max_cnt=10000;/*retry num*/
	mPlay_ctl.nosound=PropIsEnable("media.amplayer.noaudio")?1:0;
	mPlay_ctl.novideo=PropIsEnable("media.amplayer.novideo")?1:0;
	streaminfo_valied=false;
	LOGV("buffer level setting is:%f-%f-%f\n",
		mPlay_ctl.buffing_min,
		mPlay_ctl.buffing_middle,
		mPlay_ctl.buffing_max
		);
	LOGV("prepareAsync,file_name=%s\n",mPlay_ctl.file_name);
	#if 0
	check_is_playlist = check_url_type(mPlay_ctl.file_name);
	if(check_is_playlist<0)
	{
		LOGV("Start player,check_url_type error %d!\n",check_is_playlist);
		return UNKNOWN_ERROR;
	}
	else if(check_is_playlist > 0)
	{	
		mPlayer_id=play_list_player(&mPlay_ctl,(unsigned long)this);
		if(mPlayer_id>=0)
		{
			LOGV("Start player,play_list_player ok\n");
			return NO_ERROR;
		}
	}
	else
	{
	#endif
	mPlayer_id=player_start(&mPlay_ctl,(unsigned long)this);
	if(mPlayer_id>=0)
		{
			LOGV("Start player,pid=%d\n",mPlayer_id);
			return NO_ERROR;
		}
	return UNKNOWN_ERROR;
}

status_t AmlogicPlayer::start()
{
    LOGV("start\n");
    if (mState != STATE_OPEN) {
        return ERROR_NOT_OPEN;
    }

	if (mRunning && !mPaused) {
		return NO_ERROR;
	}

	player_start_play(mPlayer_id);

	if(mPaused)
		{
		player_resume(mPlayer_id);
		}
	if(mhasVideo && !mRunning){
		initVideoSurface();
		VideoViewOn();
	}
	if(mhasAudio){
		SetCpuScalingOnAudio(2);
		mChangedCpuFreq=true;
	}
    mPaused = false;
    mRunning = true;
	//sendEvent(MEDIA_PLAYER_STARTED);
    // wake up render thread
    return NO_ERROR;
}

status_t AmlogicPlayer::stop()
{
    LOGV("stop\n");
    if (mState != STATE_OPEN) {
        return ERROR_NOT_OPEN;
    }
    mPaused = true;
    mRunning = false;
	player_stop(mPlayer_id);
	///sendEvent(MEDIA_PLAYBACK_COMPLETE);
    return NO_ERROR;
}

status_t AmlogicPlayer::seekTo(int position)
{
	if(position<0)
	{/*cancel seek*/
		return NO_ERROR;
	}
#if 0
	if(position<mPlayTime+1000 && position>=mPlayTime-1000)
	{
		sendEvent(MEDIA_SEEK_COMPLETE);
		return NO_ERROR;/**/
	}
	int time=position/1000;
	LOGV("seekTo:%d\n",position);
	player_timesearch(mPlayer_id,time);	
	if(!mRunning)/*have  not start,we tell it seek end*/
		sendEvent(MEDIA_SEEK_COMPLETE);
#else
	LOGV("seekTo:%d\n",position);
	if (!mRunning) {
		//mIgnoreMsg = true;
		//player_exit(mPlayer_id);
		//mPlayer_id = -1;
		//mPlay_ctl.t_pos = position/1000;
		//prepare();
		//start();
		//mIgnoreMsg = false;
		player_timesearch(mPlayer_id,position/1000);
		sendEvent(MEDIA_SEEK_COMPLETE);
	} else {
		if (position < (mPlayTime+1000) && position >= (mPlayTime-1000)) {
			sendEvent(MEDIA_SEEK_COMPLETE);
		}
		else{
			player_timesearch(mPlayer_id,position/1000);
			sendEvent(MEDIA_SEEK_COMPLETE);
		}
	}
#endif
    mPlayTime=position;
    return NO_ERROR;
}

status_t AmlogicPlayer::pause()
{
    LOGV("pause\n");
    if (mState != STATE_OPEN) {
        return ERROR_NOT_OPEN;
    }
	if(mhasVideo)
		player_pause(mPlayer_id);
	else
	{
		mIgnoreMsg=true;
		mPlay_ctl.t_pos=mPlayTime/1000;
		
		player_stop(mPlayer_id);
		player_exit(mPlayer_id);
		mPlayer_id=-1;
		mPlay_ctl.need_start=1;
		prepare();
		mIgnoreMsg=false;
	}
    mPaused = true;
	if(mhasAudio && mChangedCpuFreq){
		SetCpuScalingOnAudio(1);
		mChangedCpuFreq=false;
	}
    return NO_ERROR;
}

bool AmlogicPlayer::isPlaying()
{
    	///LOGV("isPlaying?----%d\n",mRender);
	if(!mPaused)
		return mRunning;
	else
		return false;
}
const char* AmlogicPlayer::getStrAudioCodec(int type){
	const char* tmp = "unkown";
	switch(type){
		case AFORMAT_MPEG:
			tmp = "MPEG";
			break;
		case AFORMAT_PCM_S16LE:
			tmp = "PCMS16LE";
			break;
		case AFORMAT_AAC:
			tmp = "AAC";
			break;
		case AFORMAT_AC3:
			tmp = "AC3";
			break;
		case AFORMAT_ALAW:
			tmp = "ALAW";
			break;
		case AFORMAT_MULAW:
			tmp = "MULAW";
			break;
		case AFORMAT_DTS:
			tmp = "DTS";
			break;
		case AFORMAT_PCM_S16BE:
			tmp = "PCMS16BE";
			break;
		case AFORMAT_FLAC:
			tmp = "FLAC";
			break;
		case AFORMAT_COOK:
			tmp = "COOK";
			break;
		case AFORMAT_PCM_U8:
			tmp = "PCMU8";
			break;
		case AFORMAT_ADPCM:
			tmp = "ADPCM";
			break;
		case AFORMAT_AMR:
			tmp = "AMR";
			break;
		case AFORMAT_RAAC:
			tmp = "RAAC";
			break;
		case AFORMAT_WMA:
			tmp = "WMA";
			break;
		case AFORMAT_WMAPRO:
			tmp = "WMAPRO";
			break;
		case AFORMAT_PCM_BLURAY:
			tmp = "BLURAY";
			break;
		case AFORMAT_ALAC:
			tmp = "ALAC";
			break;
		case AFORMAT_VORBIS:
			tmp = "VORBIS";
	    		break;
	}
	return tmp;
}

const char* AmlogicPlayer::getStrVideoCodec(int vtype){
	const char* tmp = "unkown";
	switch(vtype){
		case VFORMAT_MPEG12:
			tmp = "MPEG12";
			break;
		case VFORMAT_MPEG4:
			tmp = "MPEG4";
			break;
		case VFORMAT_H264:
			tmp = "H264";
			break;
		case VFORMAT_MJPEG:
			tmp = "MJPEG";
			break;
		case VFORMAT_REAL:
			tmp = "REAL";
			break;
		case VFORMAT_JPEG:
			tmp = "JPEG";
			break;
		case VFORMAT_VC1:
			tmp = "VC1";
			break;
		case VFORMAT_AVS:
			tmp = "AVS";
			break;
		case VFORMAT_SW:
			tmp = "SW";
			break;
		case VFORMAT_H264MVC:
			tmp = "H264MVC";
			break;
    	
	}
	return tmp;
}

 
status_t AmlogicPlayer::updateMediaInfo(void)
{
	int ret;
	int i;
	if(mPlayer_id<0)
		return OK;
	mInnerSubNum = 0;
	mAudioTrackNum = 0;
	
	
	ret=player_get_media_info(mPlayer_id,&mStreamInfo);
	if(ret!=0)
	{
		LOGV("player_get_media_info failed\n");
		return NO_INIT;
	}
	streaminfo_valied=true;
	const int buflen = 512;
	char tmp[buflen];
	int boffset = 0;
	
	if(  mStreamInfo.stream_info.total_video_num>0 && 
		 mStreamInfo.stream_info.cur_video_index>=0)
	{
		memset(tmp,0,buflen);
		snprintf(tmp,buflen,"({");
		boffset = 2;
		for (i=0; i < mStreamInfo.stream_info.total_video_num; i ++) {
		  if(mStreamInfo.video_info[i]) {
			if(mStreamInfo.video_info[i]->index == mStreamInfo.stream_info.cur_video_index) 
			{
				mWidth=mStreamInfo.video_info[i]->width;
				mHeight=mStreamInfo.video_info[i]->height;				
				LOGI("player current video info:w:%d,h:%d\n",mWidth,mHeight);
				if(mStrCurrentVideoCodec){
					free(mStrCurrentVideoCodec);
					mStrCurrentVideoCodec = NULL;
				}
				mStrCurrentVideoCodec = strdup(getStrVideoCodec(mStreamInfo.video_info[i]->format));
				LOGI("player current video info:codec:%s\n",mStrCurrentVideoCodec);
				
			} 

			snprintf(tmp+boffset,buflen,"vid:%d,vcodec:%s,bitrate:%d",mStreamInfo.video_info[i]->id,
				getStrVideoCodec(mStreamInfo.video_info[i]->format),
				mStreamInfo.video_info[i]->bit_rate>0?mStreamInfo.video_info[i]->bit_rate:mStreamInfo.stream_info.bitrate);
			boffset = strlen(tmp);
			if( i < mStreamInfo.stream_info.total_video_num){	
				
				snprintf(tmp+boffset,buflen,";");
				boffset +=1;
			}			
			
		  }
		} 
		
		snprintf(tmp+boffset,buflen,"})");
		if(mVideoExtInfo){
			free(mVideoExtInfo);
			mVideoExtInfo = NULL;
		}
		mVideoTrackNum = mStreamInfo.stream_info.total_video_num;
		if(strlen(tmp)>0){
			mVideoExtInfo =strndup(tmp,buflen);
		}
			   
	} 
	
	boffset = 0;
	if(  mStreamInfo.stream_info.total_audio_num>0)
	{
		memset(tmp,0,buflen);
		snprintf(tmp,buflen,"({");
		boffset = 2;		
		for (i=0; i < mStreamInfo.stream_info.total_audio_num; i ++) {
		  if(mStreamInfo.audio_info[i]) {
		  	if(mStreamInfo.stream_info.cur_audio_index>=0&&mStreamInfo.audio_info[i]->id == mStreamInfo.stream_info.cur_audio_index) {
				if(mStrCurrentAudioCodec){
					free(mStrCurrentAudioCodec);
					mStrCurrentAudioCodec = NULL;
				}
				mStrCurrentAudioCodec =strdup(getStrAudioCodec(mStreamInfo.audio_info[i]->aformat));
		  	}			
	  		
			snprintf(tmp+boffset,buflen,"aid:%d,acodec:%s,bitrate:%d,samplerate:%d",mStreamInfo.audio_info[i]->id,getStrAudioCodec(mStreamInfo.audio_info[i]->aformat),mStreamInfo.audio_info[i]->bit_rate,mStreamInfo.audio_info[i]->sample_rate);
			boffset = strlen(tmp);
			if( i < mStreamInfo.stream_info.total_audio_num){	
				
				snprintf(tmp+boffset,buflen,";");
				boffset +=1;
			}				
				
		  }
		}
		
		snprintf(tmp+boffset,buflen,"})");
		if(mAudioExtInfo){
			free(mAudioExtInfo);
			mAudioExtInfo = NULL;
		}
		if(strlen(tmp)>0){
			mAudioExtInfo =strndup(tmp,buflen);
		}
		
		mAudioTrackNum = mStreamInfo.stream_info.total_audio_num;
		
	} 
	
	boffset = 0;
	
	if(  mStreamInfo.stream_info.total_sub_num>0)
	{
		memset(tmp,0,buflen);
		snprintf(tmp,buflen,"({");
		boffset = 2;		
		for (i=0; i < mStreamInfo.stream_info.total_sub_num; i ++) {
		  if(mStreamInfo.sub_info[i]&& mStreamInfo.sub_info[i]->internal_external ==0) {		  	
		  	
				snprintf(tmp+boffset,buflen,"sid:%d,lang:%s",mStreamInfo.sub_info[i]->id,mStreamInfo.sub_info[i]->sub_language?mStreamInfo.sub_info[i]->sub_language:"unkown");
				boffset = strlen(tmp);
				if(i < mStreamInfo.stream_info.total_sub_num){	
					
					snprintf(tmp+boffset,buflen,";");
					boffset +=1;
				}	
				mInnerSubNum++;
		  }
		} 
		
		snprintf(tmp+boffset,buflen,"})");
		if(mSubExtInfo){
			free(mSubExtInfo);
			mSubExtInfo = NULL;
		}
		
		if(strlen(tmp)>0){
			mSubExtInfo = strndup(tmp,buflen);
		}
		LOGI("inner subtitle info:%s\n",mSubExtInfo);
	} 	
		
	return OK;
}


status_t AmlogicPlayer::getMetadata(
	const media::Metadata::Filter& ids, Parcel *records) {
	using media::Metadata;
		LOGV("getMetadata\n");
	Metadata metadata(records);

	metadata.appendBool(
	        Metadata::kPauseAvailable,true);
	metadata.appendBool(
	        Metadata::kSeekBackwardAvailable,true);

	metadata.appendBool(
	        Metadata::kSeekForwardAvailable,true);
	updateMediaInfo();

	if(mhasVideo||mhasAudio){
		if(strlen(mTypeStr)>0){
			metadata.appendCString(Metadata::kStreamType,mTypeStr);	
		}
	}
	if(mhasVideo){
		metadata.appendInt32(Metadata::kVideoWidth,mWidth);
		metadata.appendInt32(Metadata::kVideoHeight,mHeight);		
		metadata.appendCString(Metadata::kVideoCodec,mStrCurrentVideoCodec!=NULL?mStrCurrentVideoCodec:"unkown");
		metadata.appendCString(Metadata::kVideoCodecAllInfo,mVideoExtInfo!=NULL?mVideoExtInfo:"unkown");
		metadata.appendInt32(Metadata::kVideoTrackNum,mVideoTrackNum);
		LOGV("set meta video info:%s\n",mVideoExtInfo);
	}else{
		metadata.appendInt32(Metadata::kVideoTrackNum,0);	
	}

	if(mhasAudio){	
		metadata.appendInt32(Metadata::kAudioTrackNum,mAudioTrackNum);	
		metadata.appendCString(Metadata::kAudioCodec,mStrCurrentAudioCodec!=NULL?mStrCurrentAudioCodec:"unkown");
		metadata.appendCString(Metadata::kAudioCodecAllInfo,mAudioExtInfo!=NULL?mAudioExtInfo:"unkown");	
		LOGV("set meta audio info:%s\n",mAudioExtInfo);
	}else{
		metadata.appendInt32(Metadata::kAudioTrackNum,0);	
	}

	if(mInnerSubNum>0){
		metadata.appendInt32(Metadata::kInnerSubtitleNum,mInnerSubNum);	
		metadata.appendCString(Metadata::kInnerSubtitleAllInfo,mSubExtInfo!=NULL?mSubExtInfo:"unkown");	
		LOGV("set meta sub info:%s\n",mSubExtInfo);
	}else{
		metadata.appendInt32(Metadata::kInnerSubtitleNum,0);	
	}

	metadata.appendInt32(Metadata::kPlayerType,AMLOGIC_PLAYER);


	LOGV("get meta data over");
		   
    	return OK;
}
status_t AmlogicPlayer::initVideoSurface(void)
{
    if ((mISurface != NULL) && (mOverlay == NULL))  {
        sp<OverlayRef> ref = mISurface->createOverlay(
            mWidth, mHeight, HAL_PIXEL_FORMAT_YCrCb_420_SP, 0);
	

        if (ref.get() == NULL) {
            LOGE("Unable to create the overlay!");
            return UNKNOWN_ERROR;
        }

        mOverlay = ref;
        //LOGV("Overlay created %p", mOverlay);
    }
	return OK; 
}
status_t AmlogicPlayer::setVideoSurface(const sp<ISurface>& surface)
{
	LOGV("Set setVideoSurface11\n");
    Mutex::Autolock autoLock(mMutex);
    mISurface = surface;
    return OK; 
}
status_t AmlogicPlayer::getCurrentPosition(int* position)
{
	//LOGV("getCurrentPosition\n");
	Mutex::Autolock autoLock(mMutex);
	*position=mPlayTime;
    return NO_ERROR;
}

status_t AmlogicPlayer::getDuration(int* duration)
{
	Mutex::Autolock autoLock(mMutex);
    LOGV("getDuration\n");
	*duration=mDuration;
    return NO_ERROR;
}

status_t AmlogicPlayer::release()
{
	int exittime;
    LOGV("release\n");
	if(mPlayer_id>=0){
		player_stop(mPlayer_id);
		player_exit(mPlayer_id);
		if(mhasVideo)
			VideoViewClose();
	}
	mPlayer_id=-1;
	if(mDuration>1000 && mPlayTime>1000 && mPlayTime<mDuration-5000)//if 5 seconds left,I think end plaing
		exittime=mPlayTime/1000;
	else
		exittime=0;
	if(mAmlogicFile.datasource)
		HistoryMgt(mAmlogicFile.datasource,1,exittime);
	
	if(mAmlogicFile.datasource!=NULL)
		free(mAmlogicFile.datasource);
	mAmlogicFile.datasource=NULL;
	if(mAmlogicFile.fd_valid)
		close(mAmlogicFile.fd);
	mAmlogicFile.fd_valid=0;
	if (mPlay_ctl.headers) {
		free(mPlay_ctl.headers);
		mPlay_ctl.headers = NULL;
	}
	if(mhasAudio && mChangedCpuFreq){
		SetCpuScalingOnAudio(1);
		mChangedCpuFreq=false;
	}
	///sendEvent(MEDIA_PLAYBACK_COMPLETE);
    return NO_ERROR;
}

status_t AmlogicPlayer::reset()
{
	//Mutex::Autolock autoLock(mMutex);
	 LOGV("reset\n");
	///player_stop(mPlayer_id);
	mPlayTime=0;
	pause();
	mPaused = true;
    mRunning = false;
    return NO_ERROR;
}

// always call with lock held
status_t AmlogicPlayer::reset_nosync()
{
	Mutex::Autolock autoLock(mMutex);
	 LOGV("reset_nosync\n");
    // close file
    //player_stop_async(mPlayer_id);
    return NO_ERROR;
}

status_t AmlogicPlayer::setLooping(int loop)
{
	Mutex::Autolock autoLock(mMutex);
    LOGV("setLooping\n");
    mLoop = (loop != 0);
	if(mLoop)
		player_loop(mPlayer_id);
	else
		player_noloop(mPlayer_id);
    return NO_ERROR;
}

status_t  AmlogicPlayer::setVolume(float leftVolume, float rightVolume){
	Mutex::Autolock autoLock(mMutex);
   	LOGV("setVolume\n");
	audio_set_lrvolume(mPlayer_id,leftVolume,rightVolume);
       return NO_ERROR;	
}


} // end namespace android

