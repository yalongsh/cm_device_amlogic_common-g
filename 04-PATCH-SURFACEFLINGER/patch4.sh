#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- with Meld

PATCHDIR4=`pwd`
cd $PATCHDIR4/../../../..
ROOTDIR=`pwd`

#####
#	surfaceflinger

cp $ROOTDIR/frameworks/base/services/surfaceflinger/DisplayHardware/DisplayHardware.cpp $ROOTDIR/frameworks/base/services/surfaceflinger/DisplayHardware/DisplayHardware.cpp.original
cp $PATCHDIR4/frameworks_base_services_surfaceflinger_DisplayHardware/DisplayHardware.cpp.patch $ROOTDIR/frameworks/base/services/surfaceflinger/DisplayHardware/DisplayHardware.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/surfaceflinger/DisplayHardware/DisplayHardware.cpp < $ROOTDIR/frameworks/base/services/surfaceflinger/DisplayHardware/DisplayHardware.cpp.patch


cp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.original
cp $PATCHDIR4/frameworks_base_services_surfaceflinger/LayerBase.cpp.patch $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp < $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBase.cpp.patch

cp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBlur.cpp $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBlur.h.original
cp $PATCHDIR4/frameworks_base_services_surfaceflinger/LayerBlur.cpp.patch $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBlur.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBlur.cpp < $ROOTDIR/frameworks/base/services/surfaceflinger/LayerBlur.cpp.patch

cp $ROOTDIR/frameworks/base/services/surfaceflinger/SurfaceFlinger.cpp $ROOTDIR/frameworks/base/services/surfaceflinger/SurfaceFlinger.cpp.original
cp $PATCHDIR4/frameworks_base_services_surfaceflinger/SurfaceFlinger.cpp.patch $ROOTDIR/frameworks/base/services/surfaceflinger/SurfaceFlinger.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/surfaceflinger/SurfaceFlinger.cpp < $ROOTDIR/frameworks/base/services/surfaceflinger/SurfaceFlinger.cpp.patch

#	surfaceflinger
#####

echo 'end of patch :)'















