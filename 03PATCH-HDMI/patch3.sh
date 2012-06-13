#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- diff -bwBu

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.original
cp $PATCHDIR/frameworks_base_core_java_android_widget/MediaController.java.patch $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java < $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.patch
cp $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.original
cp $PATCHDIR/frameworks_base_core_java_android_widget/VideoView.java.patch $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java < $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.patch

