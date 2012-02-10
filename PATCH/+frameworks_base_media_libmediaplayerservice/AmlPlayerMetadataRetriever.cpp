/*
 * Copyright (C) 2009 The Android Open Source Project
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

//#define LOG_NDEBUG 0
#define LOG_TAG "AmlPlayerMetadataRetriever"
#include "AmlPlayerMetadataRetriever.h"
#include <utils/Log.h>

namespace android {

static URLProtocol android_protocol;

status_t AmlPlayerMetadataRetriever::BasicInit()
{
    static int have_inited=0;
    if(!have_inited) {

    URLProtocol *prot=&android_protocol;
    prot->name="amthumb";
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

int AmlPlayerMetadataRetriever::vp_open(URLContext *h, const char *filename, int flags)
{	
    /*
    sprintf(file,"android:AmlogicPlayer=[%x:%x],AmlogicPlayer_fd=[%x:%x]",
    */
    LOGV("vp_open=%s\n",filename);
    if(strncmp(filename,"amthumb",strlen("amthumb"))==0)
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
            }else{
                LOGV("android_open %s Faild\n",filename);
                return -1;
            }
        }
    }
	return -1;
}

int AmlPlayerMetadataRetriever::vp_read(URLContext *h, unsigned char *buf, int size)
{	
    AmlogicPlayer_File* af= (AmlogicPlayer_File*)h->priv_data;
    int ret;
    //LOGV("start%s,pos=%lld,size=%d,ret=%d\n",__FUNCTION__,(int64_t)lseek(af->fd, 0, SEEK_CUR),size,ret);
    ret=read(af->fd,buf,size);
    //LOGV("end %s,size=%d,ret=%d\n",__FUNCTION__,size,ret);
    return ret;
}

int AmlPlayerMetadataRetriever::vp_write(URLContext *h, unsigned char *buf, int size)
{	
    AmlogicPlayer_File* af= (AmlogicPlayer_File*)h->priv_data;
	
    return -1;
}

int64_t AmlPlayerMetadataRetriever::vp_seek(URLContext *h, int64_t pos, int whence)
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

int AmlPlayerMetadataRetriever::vp_close(URLContext *h)
{
    FILE* fp= (FILE*)h->priv_data;
    LOGV("%s\n",__FUNCTION__);
    return 0; /*don't close file here*/
	//return fclose(fp);
}

int AmlPlayerMetadataRetriever::vp_get_file_handle(URLContext *h)
{
    LOGV("%s\n",__FUNCTION__);
    return (intptr_t) h->priv_data;
}

AmlPlayerMetadataRetriever::AmlPlayerMetadataRetriever()
:mClient(NULL),
 mFileName(NULL),
 mAlbumArt(NULL),
 mParsedMetaData(false)
{
    LOGV("AmlPlayerMetadataRetriever()");
	
    mClient = thumbnail_res_alloc();
    if(!mClient)
        LOGV("Thumbnail register decoder failed!\n");
	
    AmlPlayerMetadataRetriever::BasicInit();
	
}

AmlPlayerMetadataRetriever::~AmlPlayerMetadataRetriever()
{
    LOGV("~AmlPlayerMetadataRetriever()");

    delete mAlbumArt;
    mAlbumArt = NULL;

    if(mFileName){
        free(mFileName);
        mFileName = NULL;
    }
	
    thumbnail_res_free(mClient);
    mClient = NULL;
    if(mAmlogicFile.fd_valid)
        close(mAmlogicFile.fd);
    mAmlogicFile.fd_valid = 0;
}

status_t AmlPlayerMetadataRetriever::setDataSource(const char * url)
{
    LOGV("setDataSource(%s)", url);
    mParsedMetaData = false;
    mMetaData.clear();
    delete mAlbumArt;
    mAlbumArt = NULL;
	
    return setdatasource(url, -1, 0, 0x7ffffffffffffffLL);
}

status_t AmlPlayerMetadataRetriever::setDataSource(int fd, int64_t offset, int64_t length)
{
    LOGV("setDataSource(%d, %lld, %lld)", fd, offset, length);
    mParsedMetaData = false;
    mMetaData.clear();

    return setdatasource(NULL, fd, offset, length);
}

status_t AmlPlayerMetadataRetriever::setdatasource(const char* url, int fd, int64_t offset, int64_t length)
{
    char* file;

    if(url == NULL) {
        if(fd < 0 || offset < 0)
            return -1;
		
        file = (char *)malloc(128);
        if(file == NULL)
            return NO_MEMORY;

    mAmlogicFile.fd = dup(fd);
    mAmlogicFile.fd_valid=1;
    mAmlogicFile.mOffset=offset;
    mAmlogicFile.mLength=length;
    sprintf(file,"amthumb:AmlogicPlayer=[%x:%x],AmlogicPlayer_fd=[%x:%x]",
			(unsigned int)this,(~(unsigned int)this), (unsigned int)&mAmlogicFile,(~(unsigned int)&mAmlogicFile));
    }
    else
    {
        file = (char *)malloc(strlen(url) + 1);
        if(file == NULL)
            return NO_MEMORY;

        strcpy(file, url);
    }

    mFileName = file;

    return OK;
}

VideoFrame *AmlPlayerMetadataRetriever::getFrameAtTime(int64_t timeUs, int option)
{
    LOGV("getFrameAtTime: %lld us option: %d", timeUs, option);

    int err;
    int32_t width, height;
    char *rgbbuf;

    err = thumbnail_decoder_open(mClient, mFileName);
    if(err != 0){
        LOGV("Thumbnail decode init failed!\n");
	 return NULL;
    }

    err = thumbnail_extract_video_frame(mClient, timeUs, 0);
    if(err != 0){
        LOGV("Thumbnail decode frame failed !\n");
        return NULL;
    }

    thumbnail_get_video_size(mClient, &width, &height);
    LOGV("width: %d, height: %d \n", width, height);
	

    VideoFrame *frame = new VideoFrame;
    frame->mWidth = width;
    frame->mHeight = height;
    frame->mDisplayWidth = width;
    frame->mDisplayHeight = height;
    frame->mSize = width * height * 2;
    frame->mRotationAngle = 0;
    frame->mData = new uint8_t[frame->mSize];

    thumbnail_read_frame(mClient, (char *)frame->mData);

    thumbnail_decoder_close(mClient);

    return frame;
}

const char* AmlPlayerMetadataRetriever::extractMetadata(int keyCode)
{
    if(mClient == NULL) {
        LOGV("Client is not crated !\n");
        return NULL;
    }

    if(!mParsedMetaData) {
        parseMetaData();
        mParsedMetaData = true;
    }

    ssize_t index = mMetaData.indexOfKey(keyCode);

    if(index < 0) {
        return NULL;
    }

    return strdup(mMetaData.valueAt(index).string());
}

MediaAlbumArt *AmlPlayerMetadataRetriever::extractAlbumArt() 
{
    if(mClient == NULL) {
        LOGV("Client is not crated !\n");
        return NULL;
    }

    if (!mParsedMetaData) {
        parseMetaData();

        mParsedMetaData = true;
    }

    if (mAlbumArt) {
        return new MediaAlbumArt(*mAlbumArt);
    }

    return NULL;
}

void AmlPlayerMetadataRetriever::parseMetaData()
{
    if(thumbnail_find_stream_info(mClient, mFileName)) {
        LOGV("Thumbnail find stream info failed!\n");
	 return ;
    }
	
    struct Map{
        char* from;
        int      to;
    };
    static const struct Map kMap[] = {
        { "title", METADATA_KEY_TITLE },
        { "artist", METADATA_KEY_ARTIST },
        { "album", METADATA_KEY_ALBUM },
        { "genre", METADATA_KEY_GENRE },
        { "album_artist", METADATA_KEY_ALBUMARTIST },
        { "track", METADATA_KEY_CD_TRACK_NUMBER },
        { "disc", METADATA_KEY_DISC_NUMBER },
        { "composer", METADATA_KEY_COMPOSER },
        { "date", METADATA_KEY_YEAR },
    };
    static const size_t kNumMapEntries = sizeof(kMap) / sizeof(kMap[0]);

    for(size_t i = 0; i < kNumMapEntries; ++i) {
        const char *value;
        if(thumbnail_get_key_metadata(mClient, kMap[i].from, &value)) {
            LOGV("%s: %s \n", kMap[i].from, value);
            mMetaData.add(kMap[i].to, String8(value));
        }
    }


    const void* data;
    int dataSize;
    if(thumbnail_get_key_data(mClient, "cover_pic", &data, &dataSize)) {
        mAlbumArt = new MediaAlbumArt;
        mAlbumArt->mSize = dataSize;
        mAlbumArt->mData = new uint8_t[dataSize];
        memcpy(mAlbumArt->mData, (char *)data, dataSize);
    }
	
    char tmp[32];
    int64_t durationUs;
    thumbnail_get_duration(mClient, &durationUs);
    sprintf(tmp, "%lld", (durationUs+ 500) / 1000);
    mMetaData.add(METADATA_KEY_DURATION, String8(tmp));

    thumbnail_find_stream_info_end(mClient);
}
}
