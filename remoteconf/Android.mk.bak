ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES:= irremote.c config.c parsefile.c
LOCAL_MODULE := remotecfg
LOCAL_STATIC_LIBRARIES := libcutils libc
include $(BUILD_EXECUTABLE)

endif  # TARGET_SIMULATOR != true
