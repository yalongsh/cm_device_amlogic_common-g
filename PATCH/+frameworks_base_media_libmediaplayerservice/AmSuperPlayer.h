
/*
**
** Copyright 2008, The Android Open Source Project
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

#ifndef ANDROID_AMSUPERPLAYER_H
#define ANDROID_AMSUPERPLAYER_H


#include <utils/threads.h>

#include <media/MediaPlayerInterface.h>
#include <media/AudioTrack.h>
namespace android {

typedef struct notify_msg{
	int msg,ext1,ext2;
}notify_msg_t;

class AmSuperPlayer : public MediaPlayerInterface{ 
public:
	                    AmSuperPlayer();
                        ~AmSuperPlayer();

    virtual void        onFirstRef();
    virtual status_t    initCheck();
	
    virtual status_t    setDataSource(
            const char *uri, const KeyedVector<String8, String8> *headers);

    virtual status_t    setDataSource(int fd, int64_t offset, int64_t length);
    virtual status_t    setVideoSurface(const sp<ISurface>& surface);
    virtual status_t    prepare();
    virtual status_t    prepareAsync();
    virtual status_t    start();
    virtual status_t    stop();
    virtual status_t    seekTo(int msec);
    virtual status_t    pause();
    virtual bool        isPlaying();
    virtual status_t    getCurrentPosition(int* msec);
    virtual status_t    getDuration(int* msec);
    virtual status_t    release();
    virtual status_t    reset();
    virtual status_t    setLooping(int loop);
    virtual player_type playerType() { return AMSUPER_PLAYER; }
	virtual bool        hardwareOutput();
	virtual void 		setAudioSink(const sp<AudioSink> &audioSink);
    virtual status_t    invoke(const Parcel& request, Parcel *reply);
	virtual status_t    getMetadata(const media::Metadata::Filter& ids,Parcel *records);
	virtual void        setNotifyCallback(void* cookie, notify_callback_f notifyFunc);
	static  void        notify(void* cookie, int msg, int ext1, int ext2);
	   void        		Notify(void* cookie, int msg, int ext1, int ext2);
	
private:
	player_type 		SuperGetPlayerType(char *type,int videos,int audios);
	int 				match_codecs(const char *filefmtstr,const char *fmtsetting);
	player_type			Str2PlayerType(const char *str);
	bool				PropIsEnable(const char* str);
	sp<MediaPlayerBase>	CreatePlayer();
	static  int         startThread(void*);
            int         initThread();
	
	Mutex               mMutex;
	Mutex               mNotifyMutex;
	notify_msg_t		oldmsg[10];
	int 				oldmsg_num;
	
	Condition           mCondition;
	sp<MediaPlayerBase>	mPlayer;
	player_type			current_type;
	sp<AudioSink>		mAudioSink;
	sp<ISurface>        mSurface;
	
	bool 				url_valid;
	bool 				mLoop;

	const char 			*muri;
	KeyedVector<String8, String8> mheaders;

	bool 				fd_valid;
	int 				mfd;
	int64_t 			moffset;
	int64_t 			mlength;
	void*				mCookie;
	notify_callback_f		mNotify;
	int 				steps;
	status_t            		mState;
	bool 				mTypeReady;
	bool 				Prepared;
	bool				mEXIT;
	bool 				subplayer_inited;;
	pid_t				mRenderTid;
	
	


};
	
}; // namespace android


#endif // ANDROID_AMSUPERPLAYER_H


