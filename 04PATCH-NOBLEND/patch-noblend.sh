#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- Meld

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/frameworks/base/core/java/android/view/Surface.java $ROOTDIR/frameworks/base/core/java/android/view/Surface.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/Surface.java.patch $ROOTDIR/frameworks/base/core/java/android/view/Surface.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/Surface.java < $ROOTDIR/frameworks/base/core/java/android/view/Surface.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/SurfaceHolder.java $ROOTDIR/frameworks/base/core/java/android/view/SurfaceHolder.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/SurfaceHolder.java.patch $ROOTDIR/frameworks/base/core/java/android/view/SurfaceHolder.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/SurfaceHolder.java < $ROOTDIR/frameworks/base/core/java/android/view/SurfaceHolder.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/SurfaceView.java $ROOTDIR/frameworks/base/core/java/android/view/SurfaceView.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/SurfaceView.java.patch $ROOTDIR/frameworks/base/core/java/android/view/SurfaceView.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/SurfaceView.java < $ROOTDIR/frameworks/base/core/java/android/view/SurfaceView.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/WindowManager.java.patch $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java < $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.patch

cp $ROOTDIR/frameworks/base/core/java/com/android/internal/view/BaseSurfaceHolder.java $ROOTDIR/frameworks/base/core/java/com/android/internal/view/BaseSurfaceHolder.java.original
cp $PATCHDIR/frameworks_base_core_java_com_android_internal_view/BaseSurfaceHolder.java.patch $ROOTDIR/frameworks/base/core/java/com/android/internal/view/BaseSurfaceHolder.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/com/android/internal/view/BaseSurfaceHolder.java < $ROOTDIR/frameworks/base/core/java/com/android/internal/view/BaseSurfaceHolder.java.patch

cp $ROOTDIR/frameworks/base/include/surfaceflinger/ISurfaceComposer.h $ROOTDIR/frameworks/base/include/surfaceflinger/ISurfaceComposer.h.original
cp $PATCHDIR/frameworks_base_include_surfaceflinger/ISurfaceComposer.h.patch $ROOTDIR/frameworks/base/include/surfaceflinger/ISurfaceComposer.h.patch
patch -N $ROOTDIR/frameworks/base/include/surfaceflinger/ISurfaceComposer.h < $ROOTDIR/frameworks/base/include/surfaceflinger/ISurfaceComposer.h.patch

cp $ROOTDIR/frameworks/base/libs/surfaceflinger_client/SurfaceComposerClient.cpp $ROOTDIR/frameworks/base/libs/surfaceflinger_client/SurfaceComposerClient.cpp.original
cp $PATCHDIR/frameworks_base_libs_surfaceflinger_client/SurfaceComposerClient.cpp.patch $ROOTDIR/frameworks/base/libs/surfaceflinger_client/SurfaceComposerClient.cpp.patch
patch -N $ROOTDIR/frameworks/base/libs/surfaceflinger_client/SurfaceComposerClient.cpp < $ROOTDIR/frameworks/base/libs/surfaceflinger_client/SurfaceComposerClient.cpp.patch

cp $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.original
cp $PATCHDIR/frameworks_base_services_java_com_android_server/WindowManagerService.java.patch $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch
patch -N $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java < $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch

cp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.original
cp $PATCHDIR/frameworks_base_services_surfaceflinger/LayerBase.cpp.patch $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp < $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.patch

