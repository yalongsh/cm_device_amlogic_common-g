#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

########################

include $(CLEAR_VARS)
LOCAL_MODULE := init-pppd.sh
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

########################

include $(CLEAR_VARS)
LOCAL_MODULE := ip-up
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/ppp
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

########################

#	include $(CLEAR_VARS)
#	LOCAL_MODULE := chat
#	LOCAL_MODULE_TAGS := user
#	LOCAL_MODULE_CLASS := EXECUTABLES
#	LOCAL_MODULE_PATH := $(TARGET_OUT_EXECUTABLES)
#	LOCAL_SRC_FILES := $(LOCAL_MODULE)
#	include $(BUILD_PREBUILT)

########################

mcli_copy_from :=       \
    mcli-gsm            \
    mcli-cdma
    
mcli_copy_to := $(addprefix $(TARGET_OUT_ETC)/ppp/peers/,$(mcli_copy_from))
mcli_copy_from := $(addprefix $(LOCAL_PATH)/,$(mcli_copy_from))
$(mcli_copy_to) : $(TARGET_OUT_ETC)/ppp/peers/% : $(LOCAL_PATH)/% | $(ACP)
	   $(transform-prebuilt-to-target)

chat_copy_from :=             \
    chat-mcli-cdma            \
    chat-mcli-gsm
    
chat_copy_to := $(addprefix $(TARGET_OUT_ETC)/ppp/chat/,$(chat_copy_from))
chat_copy_from := $(addprefix $(LOCAL_PATH)/,$(chat_copy_from))
$(chat_copy_to) : $(TARGET_OUT_ETC)/ppp/chat/% : $(LOCAL_PATH)/% | $(ACP)
	   $(transform-prebuilt-to-target)

ALL_PREBUILT += $(chat_copy_to) $(mcli_copy_to)


PRODUCT_COPY_FILES := \
		$(filter-out %:system/etc/apns-conf.xml,$(PRODUCT_COPY_FILES)) \
		$(LOCAL_PATH)/apns-conf.xml:system/etc/apns-conf.xml \
		$(LOCAL_PATH)/voicemail-conf.xml:system/etc/voicemail-conf.xml
