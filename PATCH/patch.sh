#!/bin/sh

#
# Copyright (C) 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# http://www.gnu.org/licenses/gpl-2.0.txt
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#

# THIS IS TEMPORARY PATCH --- diff -bwBu

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.original
cp $PATCHDIR/bionic_libc_kernel_arch-arm_asm/byteorder.h.patch $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.patch
patch -N $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h < $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.patch

cp $ROOTDIR/external/jhead/exif.c $ROOTDIR/external/jhead/exif.c.original
cp $ROOTDIR/external/jhead/jhead.h $ROOTDIR/external/jhead/jhead.h.original
cp $PATCHDIR/external_jhead/exif.c.patch $ROOTDIR/external/jhead/exif.c.patch
cp $PATCHDIR/external_jhead/jhead.h.patch $ROOTDIR/external/jhead/jhead.h.patch
patch -N $ROOTDIR/external/jhead/exif.c < $ROOTDIR/external/jhead/exif.c.patch
patch -N $ROOTDIR/external/jhead/jhead.h < $ROOTDIR/external/jhead/jhead.h.patch

cp $ROOTDIR/frameworks/base/api/8.xml $ROOTDIR/frameworks/base/api/8.xml.original
cp $ROOTDIR/frameworks/base/api/9.xml $ROOTDIR/frameworks/base/api/9.xml.original
cp $ROOTDIR/frameworks/base/api/10.xml $ROOTDIR/frameworks/base/api/10.xml.original
cp $PATCHDIR/frameworks_base_api/8.xml.patch $ROOTDIR/frameworks/base/api/8.xml.patch
cp $PATCHDIR/frameworks_base_api/9.xml.patch $ROOTDIR/frameworks/base/api/9.xml.patch
cp $PATCHDIR/frameworks_base_api/10.xml.patch $ROOTDIR/frameworks/base/api/10.xml.patch
patch -N $ROOTDIR/frameworks/base/api/8.xml < $ROOTDIR/frameworks/base/api/8.xml.patch
patch -N $ROOTDIR/frameworks/base/api/9.xml < $ROOTDIR/frameworks/base/api/9.xml.patch
patch -N $ROOTDIR/frameworks/base/api/10.xml < $ROOTDIR/frameworks/base/api/10.xml.patch

#cp $ROOTDIR/frameworks/base/api/current.xml $ROOTDIR/frameworks/base/api/current.xml.original
#cp $PATCHDIR/frameworks_base_api/current.xml.patch $ROOTDIR/frameworks/base/api/current.xml.patch
#patch -N $ROOTDIR/frameworks/base/api/current.xml < $ROOTDIR/frameworks/base/api/current.xml.patch

cp $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.original
cp $ROOTDIR/frameworks/base/include/media/Metadata.h $ROOTDIR/frameworks/base/include/media/Metadata.h.original
cp $PATCHDIR/frameworks_base_include_media/MediaPlayerInterface.h.patch $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.patch
cp $PATCHDIR/frameworks_base_include_media/Metadata.h.patch $ROOTDIR/frameworks/base/include/media/Metadata.h.patch
patch -N $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h < $ROOTDIR/frameworks/base/include/media/MediaPlayerInterface.h.patch
patch -N $ROOTDIR/frameworks/base/include/media/Metadata.h < $ROOTDIR/frameworks/base/include/media/Metadata.h.patch

cp $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/Metadata.cpp.patch  $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp < $ROOTDIR/frameworks/base/media/libmedia/Metadata.cpp.patch

cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.original
cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.original
cp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.original
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/Android.mk.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.patch
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/MediaPlayerService.cpp.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.patch
cp $PATCHDIR/frameworks_base_media_libmediaplayerservice/MetadataRetrieverClient.cpp.patch $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk < $ROOTDIR/frameworks/base/media/libmediaplayerservice/Android.mk.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp < $ROOTDIR/frameworks/base/media/libmediaplayerservice/MediaPlayerService.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp < $ROOTDIR/frameworks/base/media/libmediaplayerservice/MetadataRetrieverClient.cpp.patch

cp $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.original
cp $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.original
cp $PATCHDIR/frameworks_base_media_libstagefright_include/M4vH263Encoder.h.patch $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.patch
cp $PATCHDIR/frameworks_base_media_libstagefright_include/SampleTable.h.patch $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h < $ROOTDIR/frameworks/base/media/libstagefright/include/M4vH263Encoder.h.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h < $ROOTDIR/frameworks/base/media/libstagefright/include/SampleTable.h.patch

cp $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.original
cp $PATCHDIR/frameworks_base_media_libstagefright_mpeg2ts/ATSParser.cpp.patch $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp < $ROOTDIR/frameworks/base/media/libstagefright/mpeg2ts/ATSParser.cpp.patch

cp $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.original
cp $PATCHDIR/hardware_libhardware_legacy_wifi/wifi.c.patch $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch
patch -N $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c < $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch


cp $PATCHDIR/+frameworks_base_media_libmediaplayerservice/* $ROOTDIR/frameworks/base/media/libmediaplayerservice

#1
#####
#2

cp $ROOTDIR/frameworks/base/include/camera/Camera.h $ROOTDIR/frameworks/base/include/camera/Camera.h.original
cp $PATCHDIR/frameworks_base_include_camera/Camera.h.patch $ROOTDIR/frameworks/base/include/camera/Camera.h.patch
patch -N $ROOTDIR/frameworks/base/include/camera/Camera.h < $ROOTDIR/frameworks/base/include/camera/Camera.h.patch

#?#cp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.original
#cp $PATCHDIR/frameworks_base_media_libmedia/AudioTrack.cpp.patch $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch
#patch -N $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp < $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch
cp $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/MediaProfiles.cpp.patch $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp < $ROOTDIR/frameworks/base/media/libmedia/MediaProfiles.cpp.patch

#EDIT#
cp $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/Android.mk $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/Android.mk.original
cp $PATCHDIR/frameworks_base_media_libstagefright_codecs_m4v_h263_enc/Android.mk.patch $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/Android.mk.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/Android.mk < $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/Android.mk.patch
cp $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/M4vH263Encoder.cpp $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/M4vH263Encoder.cpp.original
cp $PATCHDIR/frameworks_base_media_libstagefright_codecs_m4v_h263_enc/M4vH263Encoder.cpp.patch $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/M4vH263Encoder.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/M4vH263Encoder.cpp < $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/M4vH263Encoder.cpp.patch

cp $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/include/mp4enc_api.h $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/include/mp4enc_api.h.original
cp $PATCHDIR/frameworks_base_media_libstagefright_codecs_m4v_h263_enc_include/mp4enc_api.h.patch $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/include/mp4enc_api.h.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/include/mp4enc_api.h < $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/include/mp4enc_api.h.patch

cp $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src/mp4def.h $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src/mp4def.h.original
cp $PATCHDIR/frameworks_base_media_libstagefright_codecs_m4v_h263_enc_src/mp4def.h.patch $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src/mp4def.h.patch
patch -N $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src/mp4def.h < $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src/mp4def.h.patch

cp $ROOTDIR/frameworks/base/services/camera/libcameraservice/CameraService.cpp $ROOTDIR/frameworks/base/services/camera/libcameraservice/CameraService.cpp.original
cp $PATCHDIR/frameworks_base_services_camera_libcameraservice/CameraService.cpp.patch $ROOTDIR/frameworks/base/services/camera/libcameraservice/CameraService.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/camera/libcameraservice/CameraService.cpp < $ROOTDIR/frameworks/base/services/camera/libcameraservice/CameraService.cpp.patch


cp $PATCHDIR/+frameworks_base_media_libstagefright_codecs_m4v_h263_enc/* $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc
cp $PATCHDIR/+frameworks_base_media_libstagefright_codecs_m4v_h263_enc_src/* $ROOTDIR/frameworks/base/media/libstagefright/codecs/m4v_h263/enc/src



cd $ROOTDIR/external
if [ ! -f jpeg.tar ]
	then
	tar -cf jpeg.tar jpeg
	rm -R jpeg
fi
if [ ! -f psquarejpeg.tar ]
	then
	echo 'git clone git://github.com/psquare/jpeg.git'
	git clone git://github.com/psquare/jpeg.git
else
	tar -xf psquarejpeg.tar
	cd jpeg
	echo 'git pull - (git://github.com/psquare/jpeg.git)'
	git pull
fi

