#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- diff -bwBu

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`


#cp $ROOTDIR/frameworks/base/api/8.xml $ROOTDIR/frameworks/base/api/8.xml.original
#cp $ROOTDIR/frameworks/base/api/9.xml $ROOTDIR/frameworks/base/api/9.xml.original
#cp $ROOTDIR/frameworks/base/api/10.xml $ROOTDIR/frameworks/base/api/10.xml.original
#cp $PATCHDIR/frameworks_base_api/8.xml.patch $ROOTDIR/frameworks/base/api/8.xml.patch
#cp $PATCHDIR/frameworks_base_api/9.xml.patch $ROOTDIR/frameworks/base/api/9.xml.patch
#cp $PATCHDIR/frameworks_base_api/10.xml.patch $ROOTDIR/frameworks/base/api/10.xml.patch
#patch -N $ROOTDIR/frameworks/base/api/8.xml < $ROOTDIR/frameworks/base/api/8.xml.patch
#patch -N $ROOTDIR/frameworks/base/api/9.xml < $ROOTDIR/frameworks/base/api/9.xml.patch
#patch -N $ROOTDIR/frameworks/base/api/10.xml < $ROOTDIR/frameworks/base/api/10.xml.patch


cp $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.original
cp $ROOTDIR/frameworks/base/include/media/Metadata.h $ROOTDIR/frameworks/base/include/media/Metadata.h.original
cp $PATCHDIR/frameworks_base_include_media/MediaPlayerInterface.h.patch $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.patch
cp $PATCHDIR/frameworks_base_include_media/Metadata.h.patch $ROOTDIR/frameworks/base/include/media/Metadata.h.patch
patch -N $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h < $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.patch
patch -N $ROOTDIR/frameworks/base/include/media/Metadata.h < $ROOTDIR/frameworks/base/include/media/Metadata.h.patch


cp $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/Metadata.cpp.patch  $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp < $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.patch

cp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/AudioTrack.cpp.patch $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp < $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch

cp $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/MediaProfiles.cpp.patch $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp < $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.patch


cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.original
cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.original
cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.original
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/Android.mk.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.patch
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/MediaPlayerService.cpp.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.patch
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/MetadataRetrieverClient.cpp.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk < $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp < $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp < $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.patch


#cp $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.original
cp $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.original
#cp $PATCHDIR/frameworks_base_media_libstagefright_include/M4vH263Encoder.h.patch $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.patch
cp $PATCHDIR/frameworks_base_media_libstagefright_include/SampleTable.h.patch $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.patch
#patch -N $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h < $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h < $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.patch


cp $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.original
cp $PATCHDIR/frameworks_base_media_libstagefright_mpeg2ts/ATSParser.cpp.patch $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp < $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.patch



cp $PATCHDIR/+frameworks_base_media_libmediaplayerservice/* $ROOTDIR/frameworks/base/media/libmediaplayerservice

