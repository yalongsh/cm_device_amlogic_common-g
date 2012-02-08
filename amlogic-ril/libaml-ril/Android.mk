# Copyright 2006 The Android Open Source Project

# XXX using libutils for simulator build only...
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    aml_ril.c \
    atchannel.c \
    misc.c \
    at_tok.c\
    aml-ril-callhandling.c\
    aml-ril-messaging.c\
    aml-ril-network.c\
    aml-ril-oem.c\
    aml-ril-pdp.c\
    aml-ril-requestdatahandler.c\
    aml-ril-services.c\
    aml-ril-sim.c\
    aml-ril-device.c\
    mulevent.c\
    vendor.c\
    liteping.c \
    ril-compatible.c

LOCAL_SHARED_LIBRARIES := \
    libcutils libutils libril

# for asprinf
LOCAL_CFLAGS := -D_GNU_SOURCE

LOCAL_C_INCLUDES := $(KERNEL_HEADERS)

ifeq ($(TARGET_DEVICE),sooner)
  LOCAL_CFLAGS += -DOMAP_CSMI_POWER_CONTROL -DUSE_TI_COMMANDS
endif

ifeq ($(TARGET_DEVICE),surf)
  LOCAL_CFLAGS += -DPOLL_CALL_STATE -DUSE_QMI
endif

ifeq ($(TARGET_DEVICE),dream)
  LOCAL_CFLAGS += -DPOLL_CALL_STATE -DUSE_QMI
endif

ifeq (foo,foo)
  #build shared library
  LOCAL_PRELINK_MODULE := false
  LOCAL_SHARED_LIBRARIES += \
      libcutils libutils
  LOCAL_LDLIBS += -lpthread
  LOCAL_CFLAGS += -DRIL_SHLIB
  LOCAL_MODULE:= libaml-ril
  LOCAL_MODULE_TAGS := optional
  include $(BUILD_SHARED_LIBRARY)
else
  #build executable
  LOCAL_SHARED_LIBRARIES += \
      libril
  LOCAL_MODULE:= libaml-ril
  include $(BUILD_EXECUTABLE)
endif
