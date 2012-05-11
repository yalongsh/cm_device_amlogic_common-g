#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- diff -bwBu

PATCHDIRN=`pwd`
cd $PATCHDIRN/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/system/core/mkbootimg/mkbootimg.c $ROOTDIR/system/core/mkbootimg/mkbootimg.c.original
cp $PATCHDIRN/system_core_mkbootimg/mkbootimg.c.patch $ROOTDIR/system/core/mkbootimg/mkbootimg.c.patch
patch -N $ROOTDIR/system/core/mkbootimg/mkbootimg.c < $ROOTDIR/system/core/mkbootimg/mkbootimg.c.patch

#1
#####
#2

#cp $ROOTDIR/frameworks/base/core/jni/android_hardware_SensorManager.cpp $ROOTDIR/frameworks/base/core/jni/android_hardware_SensorManager.cpp.original
#cp $PATCHDIRN/frameworks_base_core_jni/android_hardware_SensorManager.cpp.patch $ROOTDIR/frameworks/base/core/jni/android_hardware_SensorManager.cpp.patch
#patch -N $ROOTDIR/frameworks/base/core/jni/android_hardware_SensorManager.cpp < $ROOTDIR/frameworks/base/core/jni/android_hardware_SensorManager.cpp.patch

#cp $ROOTDIR/frameworks/base/include/ui/InputReader.h $ROOTDIR/frameworks/base/include/ui/InputReader.h.original
#cp $PATCHDIRN/frameworks_base_include_ui/InputReader.h.patch $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch
#patch -N $ROOTDIR/frameworks/base/include/ui/InputReader.h < $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch

#cp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.original3
#cp $PATCHDIRN/frameworks_base_libs_ui/InputReader.cpp.patch3 $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch3
#patch -N $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp < $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch3

#cp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.original
#cp $PATCHDIRN/frameworks_base_services_jni/com_android_server_InputManager.cpp.patch $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch
#patch -N $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp < $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch

