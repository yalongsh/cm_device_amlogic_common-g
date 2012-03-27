/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef _UI_INPUT_REMOTE_H
#define _UI_INPUT_REMOTE_H

#include <ui/InputReader.h>
#include <ui/Input.h>
#include <ui/InputDispatcher.h>
#include <utils/threads.h>
#include <utils/Timers.h>
#include <utils/RefBase.h>

namespace android {

class RemoteSocket;

/* The input remote reads input event data from the remote control client and 
 * processes it into input events that it sends to the input dispatcher. 
 */
class InputRemote : public virtual RefBase {
public:
	InputRemote(const sp<InputReaderPolicyInterface> &reader, const sp<InputDispatcherInterface>& dispatcher);
    virtual ~InputRemote();

    virtual bool loopOnce();
private:
	sp<InputDispatcherInterface>		mDispatcher;
	sp<RemoteSocket> 					mSocket;
	bool								mConnected;
	nsecs_t								mDownTime;
	int32_t 							mMetaState;
	int32_t								mCount;
	int32_t								mMotionMode;//touch or mouse
	int32_t								mDisplayWidth;
	int32_t								mDisplayHeight;
    int32_t								mDisplayOrientation;
	
	float								mPadMouseCurX;
	float								mPadMouseCurY;
	float								mPadMouseLastX;
	float								mPadMouseLastY;
	
	struct EventData {
        int32_t type;
		int32_t action;
        int32_t keycode;
        float x;
        float y;
    } mEventData;

	enum {
		EVENT_TYPE_ACK				= 0,
		EVENT_TYPE_KEY				= 1,
		EVENT_TYPE_TOUCH			= 2,
		EVENT_TYPE_TRACKBALL		= 3,
		EVENT_TYPE_SENSOR			= 4,
		EVENT_TYPE_UI_STATE			= 5,
		EVENT_TYPE_GET_PICTURE		= 6
    };

	enum {
		REMOTE_MOTION_MODE_TOUCH			= 0,
		REMOTE_MOTION_MODE_MOUSE			= 1,
    };
	
	enum {
		INPUT_DEVICE_ID_KEY				= 0x10004,
		INPUT_DEVICE_ID_TOUCH			= 0x10000,
    };

	void readEvent();
	void process(const EventData *Event);
	void sendMotion(int32_t motionType, int32_t action, float x, float y, nsecs_t downTime);
	int32_t updateMetaState(int32_t keyCode, bool down, int32_t oldMetaState);
};

class InputRemoteThread : public Thread {
public:
    InputRemoteThread(const sp<InputRemote>& remoter);
    virtual ~InputRemoteThread();

private:
	sp<InputRemote> mRemoter;

    virtual bool threadLoop();
};

class RemoteSocket : public virtual RefBase {
public:
    RemoteSocket(const char *socketNames);
    RemoteSocket(int socketFd);

    virtual ~RemoteSocket();

	bool requestConnect();
	void requestDisconnect();
	int readx(void *_buf, int count);
	int writex(const void *_buf, int count);
private:
    int                     mSocketFd;
    const char              *mSocketName;
    pthread_mutex_t         mClientsLock;
};

} // namespace android

#endif // _UI_INPUT_REMOTE_H

