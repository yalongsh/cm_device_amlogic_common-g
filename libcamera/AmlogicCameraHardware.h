
#ifndef AMLOGIC_HARDWARE_CAMERA_HARDWARE_H
#define AMLOGIC_HARDWARE_CAMERA_HARDWARE_H

#include <utils/threads.h>
#include <camera/CameraHardwareInterface.h>
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>


#ifndef AMLOGIC_USB_CAMERA_SUPPORT
#define AMLOGIC_CAMERA_OVERLAY_SUPPORT 1
#endif

namespace android {

class CameraInterface
{
public:
	CameraInterface(){};
	virtual ~CameraInterface(){};
	virtual status_t	Open() = 0;
	virtual status_t	Close() = 0;
	virtual status_t	StartPreview() = 0;
	virtual status_t	StopPreview() = 0;
	virtual status_t	StartRecord() = 0;
	virtual status_t	StopRecord() = 0;
	virtual status_t	TakePicture() = 0;
	virtual status_t	TakePictureEnd() = 0;
	virtual status_t	StartFocus(){return 1;};
	virtual status_t	StopFocus(){return 1;};
	virtual status_t	InitParameters(CameraParameters& pParameters) = 0;
	virtual status_t	SetParameters(CameraParameters& pParameters) = 0;
	virtual status_t	GetPreviewFrame(uint8_t* framebuf) = 0;
	virtual status_t	GetRawFrame(uint8_t* framebuf) = 0;
	virtual status_t	GetJpegFrame(uint8_t* framebuf,int* jpegsize) = 0;

	virtual int			GetCamId() {return 0;};
};


class AmlogicCameraHardware : public CameraHardwareInterface {
public:
    virtual sp<IMemoryHeap> getPreviewHeap() const;
    virtual sp<IMemoryHeap> getRawHeap() const;
	
    virtual void        setCallbacks(notify_callback notify_cb,
                                     data_callback data_cb,
                                     data_callback_timestamp data_cb_timestamp,
                                     void* user);

    virtual void        enableMsgType(int32_t msgType);
    virtual void        disableMsgType(int32_t msgType);
    virtual bool        msgTypeEnabled(int32_t msgType);

    virtual status_t    startPreview();
    virtual void        stopPreview();
    virtual bool        previewEnabled();

    virtual status_t    startRecording();
    virtual void        stopRecording();
    virtual bool        recordingEnabled();
    virtual void        releaseRecordingFrame(const sp<IMemory>& mem);

    virtual status_t    autoFocus();
    virtual status_t    cancelAutoFocus();
    virtual status_t    takePicture();
    virtual status_t    cancelPicture();
    virtual status_t    dump(int fd, const Vector<String16>& args) const;
    virtual status_t    setParameters(const CameraParameters& params);
    virtual CameraParameters  getParameters() const;
    virtual status_t    sendCommand(int32_t command, int32_t arg1,
                                    int32_t arg2);
    virtual void release();

    static sp<CameraHardwareInterface> createInstance(int CamId);

#ifdef AMLOGIC_CAMERA_OVERLAY_SUPPORT
	virtual bool		useOverlay() {return true;}
    virtual status_t     setOverlay(const sp<Overlay> &overlay);
#else
	virtual bool		useOverlay() {return false;}

#endif

private:
                        AmlogicCameraHardware(int camid = 0);
    virtual             ~AmlogicCameraHardware();

    static const int kBufferCount = 6;

#ifndef AMLOGIC_CAMERA_OVERLAY_SUPPORT
	sp<MemoryHeapBase>  mRecordHeap;
    sp<MemoryBase>      mRecordBuffers[kBufferCount];
#else
    volatile int32_t    mRecordBufCount;
#endif

    class PreviewThread : public Thread {
        AmlogicCameraHardware* mHardware;
    public:
        PreviewThread(AmlogicCameraHardware* hw) :
#ifdef SINGLE_PROCESS
            // In single process mode this thread needs to be a java thread,
            // since we won't be calling through the binder.
            Thread(true),
#else
            Thread(false),
#endif
              mHardware(hw) { }
        virtual void onFirstRef() {
            run("CameraPreviewThread", PRIORITY_URGENT_DISPLAY);
        }
        virtual bool threadLoop() {
            mHardware->previewThread();
            // loop until we need to quit
            return true;
        }
    };

    void initDefaultParameters();
    void initHeapLocked();

    int previewThread();

    static int beginAutoFocusThread(void *cookie);
    int autoFocusThread();

    static int beginPictureThread(void *cookie);
    int pictureThread();

    mutable Mutex       mLock;
	mutable Mutex       mStateLock;

	int					mState;

    CameraParameters    mParameters;

    sp<MemoryHeapBase>  mPreviewHeap;
    sp<MemoryHeapBase>  mHeap;
    sp<MemoryHeapBase>  mRawHeap;
    sp<MemoryBase>      mBuffers[kBufferCount];
    sp<MemoryBase>      mPreviewBuffers[kBufferCount];
    //FakeCamera          *mFakeCamera;
    bool                mPreviewRunning;
    int                 mPreviewFrameSize;
    int                  mHeapSize;	

    // protected by mLock
    sp<PreviewThread>   mPreviewThread;

	//if reord is enable!
	bool				mRecordEnable;

    notify_callback    mNotifyCb;
    data_callback      mDataCb;
    data_callback_timestamp mDataCbTimestamp;
    void               *mCallbackCookie;

    int32_t             mMsgEnabled;

    // only used from PreviewThread
    int                 mCurrentPreviewFrame;

	CameraInterface* 	mCamera;


	enum
	{
		PROCESS_FOCUS = 0x1,
		PROCESS_TAKEPIC = 0x2,
	};
};

}; // namespace android



#endif



