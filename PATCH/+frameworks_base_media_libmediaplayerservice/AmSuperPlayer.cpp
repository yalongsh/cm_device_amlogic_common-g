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
#define LOG_TAG "AmSuperPlayer"
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

#include "AmSuperPlayer.h"
#include "AmlogicPlayer.h"

#include "MidiFile.h"
#include "TestPlayerStub.h"
#include "StagefrightPlayer.h"


static int myTid() {
#ifdef HAVE_GETTID
    return gettid();
#else
    return getpid();
#endif
}

namespace android {
	static status_t ERROR_NOT_OPEN = -1;
	static status_t ERROR_OPEN_FAILED = -2;
	static status_t ERROR_ALLOCATE_FAILED = -4;
	static status_t ERROR_NOT_SUPPORTED = -8;
	static status_t ERROR_NOT_READY = -16;
	static status_t STATE_INIT = 0;
	static status_t STATE_ERROR = 1;
	static status_t STATE_OPEN = 2;

//#define  TRACE()	LOGV("[%s::%d]\n",__FUNCTION__,__LINE__)
#define  TRACE()	

AmSuperPlayer::AmSuperPlayer() :
    mPlayer(0),
	mState(STATE_ERROR)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	muri=NULL;
	url_valid=false;
	fd_valid=false;
	mEXIT=false;
	Prepared=false;
	subplayer_inited=false;
	oldmsg_num=0;
	mLoop = false;
	current_type=AMSUPER_PLAYER;
    LOGV("AmSuperPlayer init now\n");
	
}

AmSuperPlayer::~AmSuperPlayer() 
{
	TRACE();
	release();
}



void        AmSuperPlayer::onFirstRef()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	mState = NO_ERROR;
	
}
status_t    AmSuperPlayer::initCheck()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	return mState;
}

status_t    AmSuperPlayer::setDataSource(const char *uri, const KeyedVector<String8, String8> *headers)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if(muri!=NULL) free((void*)muri);
	muri=strdup(uri);
	if (headers) {
        mheaders = *headers;
    }
	url_valid=true;
	mState = STATE_OPEN;
	return NO_ERROR;
}
static int fdcound=0;
status_t    AmSuperPlayer::setDataSource(int fd, int64_t offset, int64_t length)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	
	TRACE();
	mfd=dup(fd);
	moffset=offset;
	mlength=length;
	fd_valid=true;
	mState = STATE_OPEN;
///#define DUMP_TO_FILE
#ifdef DUMP_TO_FILE
	int testfd=mfd;
	int dumpfd;
	int tlen;
	char buf[1028];
	int oldoffset=lseek(testfd, 0, SEEK_CUR);
	sprintf(buf,"/data/test/dump.media%d-%d.mp4",fdcound,fd);
	fdcound++;
	dumpfd=open(buf,O_CREAT | O_TRUNC | O_WRONLY,0666);
	LOGV("Dump media to file %s:%d\n",buf,dumpfd);
	lseek(testfd, 0, SEEK_SET);
	tlen=read(testfd,buf,1024);
	while(dumpfd>=0 && tlen>0){
		write(dumpfd,buf,tlen);
		tlen=read(testfd,buf,1024);
	}
	close(dumpfd);
	lseek(testfd, oldoffset, SEEK_SET);
#endif

	return NO_ERROR;
}
status_t    AmSuperPlayer::setVideoSurface(const sp<ISurface>& surface)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();

	if (mPlayer == 0) {
        mSurface = surface;

	    return OK;
	}
	
    return mPlayer->setVideoSurface(surface);
}
status_t    AmSuperPlayer::prepare()
{
	int ret;
	TRACE();
	ret=prepareAsync();
	if(ret!=NO_ERROR)
		return ret;
	while(!Prepared){
		if(mEXIT ||(mRenderTid<=0))
			break;	
		usleep(1000*10);
	}
	if(mEXIT || !Prepared || mRenderTid<0)
		return UNKNOWN_ERROR;
	return NO_ERROR;	
	////
}
status_t    AmSuperPlayer::prepareAsync()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	sp<MediaPlayerBase> p;
	TRACE();
	Prepared=false;
    createThreadEtc(startThread, this, "StartThread", ANDROID_PRIORITY_DEFAULT);
    mCondition.wait(mMutex);
    if (mRenderTid > 0) {
        LOGV("initThread(%d) started", mRenderTid);
    }
	return NO_ERROR;
}
status_t    AmSuperPlayer::start()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) {
		return UNKNOWN_ERROR;
	}
	mPlayer->setLooping(mLoop);
	return mPlayer->start();
}
status_t    AmSuperPlayer::stop()
{
	Mutex::Autolock l(mMutex);
	if (mPlayer == 0) {
		return UNKNOWN_ERROR;
	}
	return mPlayer->stop();
}
status_t    AmSuperPlayer::seekTo(int msec)
{
	int current=-1;
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) return UNKNOWN_ERROR;
        mPlayer->getCurrentPosition(&current);
        if(current>=0 && (msec<(current+1500) && msec>(current-1500)))
        {
               mNotify(mCookie, MEDIA_SEEK_COMPLETE, 0, 0);
               return NO_ERROR;
        }
	return mPlayer->seekTo(msec);
}
status_t    AmSuperPlayer::pause()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) return UNKNOWN_ERROR;
	return mPlayer->pause();
}
bool        AmSuperPlayer::isPlaying()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) return false;
	return mPlayer->isPlaying();
}
status_t    AmSuperPlayer::getCurrentPosition(int* msec)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) //return UNKNOWN_ERROR;
		return 0;
	return mPlayer->getCurrentPosition(msec);	
}
status_t    AmSuperPlayer::getDuration(int* msec)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0)// return UNKNOWN_ERROR;
		return 0;
	return mPlayer->getDuration(msec);		
}
status_t    AmSuperPlayer::release()
{
	TRACE();
	stop();
	while(mRenderTid>0){	
		Mutex::Autolock l(mMutex);
		mEXIT=true;
		mCondition.signal();
		mCondition.wait(mMutex);
	}
	if(muri!=NULL) free((void*)muri);
	
	TRACE();
	if (mPlayer == NULL) return NO_ERROR;
	mPlayer.clear();
	
	return NO_ERROR;
}
status_t    AmSuperPlayer::reset()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (fd_valid) {
		fd_valid=false;
		close(mfd);
		mfd=-1;
	}
	mheaders.clear();
	mLoop = false;
	if (mPlayer == 0) return NO_ERROR;
	return mPlayer->reset();	
}
status_t    AmSuperPlayer::setLooping(int loop)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	mLoop = (loop != 0);
	if (mPlayer == 0) return NO_ERROR;
	return mPlayer->setLooping(loop);
}
bool        AmSuperPlayer::hardwareOutput()
{
	TRACE();
	Mutex::Autolock l(mMutex);
	TRACE();
	if (mPlayer == 0) return true;
	return mPlayer->hardwareOutput();	
}
status_t    AmSuperPlayer::invoke(const Parcel& request, Parcel *reply) 
{
	TRACE();
	return INVALID_OPERATION;
}
status_t    AmSuperPlayer::getMetadata(const media::Metadata::Filter& ids,Parcel *records)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	if (mPlayer == 0) return UNKNOWN_ERROR;
	return mPlayer->getMetadata(ids,records);	

}

void AmSuperPlayer::notify(void* cookie, int msg, int ext1, int ext2)
{
	TRACE();
	 AmSuperPlayer* m=(AmSuperPlayer*)cookie;
	if(m!=0)
		m->Notify(cookie,msg,ext1,ext2);
}
void AmSuperPlayer::Notify(void* cookie, int msg, int ext1, int ext2)
{
	TRACE();
	Mutex::Autolock N(mNotifyMutex);
	LOGV("cookie=%p,msg=%x,ext1=%x,ext2=%x\n",cookie,msg,ext1,ext2);
	switch(msg){
			case MEDIA_NOP:	
				break;
			case 0x11000:/*Amlogic File type ready*/
				TRACE();
				mTypeReady=true;
				mCondition.signal();
				break;
			
				break;
			case MEDIA_BUFFERING_UPDATE:
				if (mPlayer!=NULL && mNotify!=NULL)
					mNotify(mCookie, msg, ext1, ext2);
				break;
		    case MEDIA_PREPARED:
				Prepared=true;
		    case MEDIA_PLAYBACK_COMPLETE:
		    case MEDIA_SEEK_COMPLETE:
		    case MEDIA_SET_VIDEO_SIZE:
		    case MEDIA_INFO:
			case MEDIA_ERROR:
			if(!mTypeReady){
				mTypeReady=true;
				mCondition.signal();
			}	
			if(mPlayer==NULL && oldmsg_num<9 && !subplayer_inited){
					oldmsg[oldmsg_num].msg=msg;
					oldmsg[oldmsg_num].ext1=ext1;
					oldmsg[oldmsg_num].ext2=ext2;
					oldmsg_num++;
				}
			default:
			if (mPlayer!=NULL && mNotify!=NULL) {
				mNotify(mCookie, msg, ext1, ext2);
			}
	}
	
}
void	AmSuperPlayer::setNotifyCallback(void* cookie, notify_callback_f notifyFunc)
{
	TRACE();
	Mutex::Autolock l(mMutex);
	Mutex::Autolock N(mNotifyMutex);
	
	mCookie = cookie; 
	mNotify = notifyFunc;
	if (mPlayer == 0) return;
}

void AmSuperPlayer::setAudioSink(const sp<AudioSink> &audioSink) {
    MediaPlayerInterface::setAudioSink(audioSink);
	mAudioSink=audioSink;
}



bool AmSuperPlayer::PropIsEnable(const char* str)
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
player_type  AmSuperPlayer::Str2PlayerType(const char *str)
{
	if(strcmp(str,"PV_PLAYER")==0)
		return PV_PLAYER;
	else if(strcmp(str,"SONIVOX_PLAYER")==0)
		return SONIVOX_PLAYER;
	else if(strcmp(str,"STAGEFRIGHT_PLAYER")==0)
		return STAGEFRIGHT_PLAYER;
	else if(strcmp(str,"AMLOGIC_PLAYER")==0)
		return AMLOGIC_PLAYER;
	else if(strcmp(str,"AMSUPER_PLAYER")==0)
		return AMSUPER_PLAYER;
	/*default*/
	return STAGEFRIGHT_PLAYER;
}

int AmSuperPlayer::match_codecs(const char *filefmtstr,const char *fmtsetting)
{
        const char * psets=fmtsetting;
        const char *psetend;
        int psetlen=0;
        char codecstr[64]="";
		if(filefmtstr==NULL || fmtsetting==NULL)
			return 0;

        while(psets && psets[0]!='\0'){
                psetlen=0;
                psetend=strchr(psets,',');
                if(psetend!=NULL && psetend>psets && psetend-psets<64){
                        psetlen=psetend-psets;
                        memcpy(codecstr,psets,psetlen);
                        codecstr[psetlen]='\0';
                        psets=&psetend[1];//skip ";"
                }else{
                        strcpy(codecstr,psets);
                        psets=NULL;
                }
                if(strlen(codecstr)>0){
                        if(strstr(filefmtstr,codecstr)!=NULL)
                                return 1;
                }
        }
        return 0;
}

player_type AmSuperPlayer::SuperGetPlayerType(char *type,int videos,int audios)
{
	int ret;
	char value[PROPERTY_VALUE_MAX];
	bool amplayer_enabed=PropIsEnable("media.amplayer.enable");
	
	
	if(amplayer_enabed && type!=NULL)
	{
		bool audio_all,no_audiofile;
		if(audios==0 && videos==0){
			/*parser get type but have not finised get videos and audios*/
			if(match_codecs(type,"mpeg,mpegts,rtsp"))/*some can't parser stream info in header parser*/
			        return AMLOGIC_PLAYER;
		}

        if (match_codecs(type, "webm,vp8,vp6"))
            return STAGEFRIGHT_PLAYER;

		if(videos>0)
			return AMLOGIC_PLAYER;
		audio_all=PropIsEnable("media.amplayer.audio-all");
		if(audios>0 && audio_all )
			return AMLOGIC_PLAYER;
		
		ret=property_get("media.amplayer.enable-acodecs",value,NULL);
		if(ret>0 && match_codecs(type,value)){
			return AMLOGIC_PLAYER;
		}
			
	}

	if(match_codecs(type,"midi,mmf,mdi"))
		return SONIVOX_PLAYER;
	
	if(match_codecs(type,"m4a")) {
		ret=property_get("media.amsuperplayer.m4aplayer",value,NULL);
		if (ret>0) {
			LOGI("media.amsuperplayer.m4aplayer=%s\n",value);
			return Str2PlayerType(value);
		}
	}

    if (PropIsEnable("media.stagefright.enable-player")) {
        return STAGEFRIGHT_PLAYER;
    }

    return STAGEFRIGHT_PLAYER;
    

}

static sp<MediaPlayerBase> createPlayer(player_type playerType, void* cookie,
        notify_callback_f notifyFunc)
{

    sp<MediaPlayerBase> p;
	LOGV("createPlayer");
    switch (playerType) {
        case SONIVOX_PLAYER:
            LOGV(" create MidiFile");
            p = new MidiFile();
            break;
        case STAGEFRIGHT_PLAYER:
            LOGV(" create StagefrightPlayer");
            p = new StagefrightPlayer;
            break;
        case TEST_PLAYER:
            LOGV("Create Test Player stub");
            p = new TestPlayerStub();
            break;
		case AMSUPER_PLAYER:
			 LOGV("Create AmSuperPlayer ");
			p = new AmSuperPlayer();
			break;
		case AMLOGIC_PLAYER:
			#ifdef BUILD_WITH_AMLOGIC_PLAYER
            LOGV("Create Amlogic Player");
            p = new AmlogicPlayer();
			#else
			LOGV("Have not Buildin Amlogic Player Support");
			#endif
			
            break;	
    }
    if (p != NULL) {
        if (p->initCheck() == NO_ERROR) {
            p->setNotifyCallback(cookie, notifyFunc);
        } else {
            p.clear();
        }
    }
    if (p == NULL) {
        LOGE("Failed to create player object");
    }
    return p;
}


sp<MediaPlayerBase>	AmSuperPlayer::CreatePlayer()
{
	sp<MediaPlayerBase> p;
	char *filetype;
	int mvideo,maudio;
	
	player_type newtype=AMLOGIC_PLAYER;
	mTypeReady=false;
	//p= new AmlogicPlayer();
Retry:
	{
		Mutex::Autolock N(mNotifyMutex);
		oldmsg_num=0;
		memset(oldmsg,0,sizeof(oldmsg));
	}
	p=android::createPlayer(newtype, this,notify);
	if (!p->hardwareOutput()) {
        static_cast<MediaPlayerInterface*>(p.get())->setAudioSink(mAudioSink);
    }
	TRACE();
	if(url_valid)
		p->setDataSource(muri,&mheaders);
	else if(fd_valid)
		p->setDataSource(mfd,moffset,mlength);
	else 
		return NULL;
    if(mSurface != NULL)
        p->setVideoSurface(mSurface);
	p->prepareAsync();
	if(!mTypeReady && p->playerType()==AMLOGIC_PLAYER){
		AmlogicPlayer* amplayer; 
		bool FileTypeReady=false;
		amplayer=(AmlogicPlayer *)p.get();
		while(!mTypeReady)
		{
			int ret;
			Mutex::Autolock l(mMutex);
			mCondition.wait(mMutex);
			if(mEXIT)
				break;
		}
		if(mTypeReady){
				FileTypeReady=!amplayer->GetFileType(&filetype,&mvideo,&maudio);
				TRACE();		
		}
		if(FileTypeReady){
			LOGV("SuperGetPlayerType:type=%s,videos=%d,audios=%d\n",filetype,mvideo,maudio);
			newtype=SuperGetPlayerType(filetype,mvideo,maudio);
			LOGV("GET New type =%d\n",newtype);
		}
		else{
			newtype=SuperGetPlayerType(NULL,0,0);
		}
	}
	TRACE();
	if(mEXIT){
		p->stop();
		p.clear();
		p=NULL;
	}
	else if(newtype!=p->playerType()){
		LOGV("Need to creat new player=%d\n",newtype);
		p->stop();
		p.clear();
		if(fd_valid){
			lseek(mfd,moffset,SEEK_SET);/*reset to before*/
		}
		p=NULL;
		goto Retry;
	}
	TRACE();
	LOGV("Start new player now=%d\n",newtype);
	return p;
}

int AmSuperPlayer::startThread(void*arg)
{
	AmSuperPlayer * p=(AmSuperPlayer *)arg;
	return p->initThread();
}
int AmSuperPlayer::initThread()
{	
	{
	  // 	Mutex::Autolock l(mMutex);
	    mRenderTid = myTid();
	    mCondition.signal();
	}
	mPlayer=CreatePlayer();
	if (mPlayer!=NULL && mNotify!=NULL) {
		int i;
		Mutex::Autolock N(mNotifyMutex);
		for(i=0;i<oldmsg_num;i++){
			mNotify(mCookie, oldmsg[i].msg,oldmsg[i].ext1,oldmsg[i].ext2);
		}
		oldmsg_num=0;
		subplayer_inited=true;
	}else{
		if(mNotify!=NULL)
			mNotify(mCookie, MEDIA_ERROR,MEDIA_ERROR_UNKNOWN,-1);
	}
	Mutex::Autolock l(mMutex);
	mRenderTid = -1;
	mCondition.signal();
	TRACE();

	return 0;
}
}

