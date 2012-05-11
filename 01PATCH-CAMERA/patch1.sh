#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- with Meld

PATCHDIR1=`pwd`
cd $PATCHDIR1/../../../..
ROOTDIR=`pwd`

#####
#	camera

cd $ROOTDIR/external
if [ ! -f cm-jpeg.tar ]
	then
	tar -cf cm-jpeg.tar jpeg
	rm -R jpeg
fi
if [ ! -f turbo-jpeg.tar ]
	then
	rm -R jpeg
	echo 'clone linaro-android branch of libjpeg-turbo'
	git clone git://git.linaro.org/people/tomgall/libjpeg-turbo/libjpeg-turbo.git -b linaro-android jpeg
	tar -cf turbo-jpeg.tar jpeg

	cp $ROOTDIR/external/jpeg/Android.mk $ROOTDIR/external/jpeg/Android.mk.original
	cp $PATCHDIR1/external_jpeg/Android.mk.patch $ROOTDIR/external/jpeg/Android.mk.patch
	patch -N $ROOTDIR/external/jpeg/Android.mk < $ROOTDIR/external/jpeg/Android.mk.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.c $ROOTDIR/external/jpeg/turbojpeg.c.original
	cp $PATCHDIR1/external_jpeg/turbojpeg.c.patch $ROOTDIR/external/jpeg/turbojpeg.c.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.c < $ROOTDIR/external/jpeg/turbojpeg.c.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.h $ROOTDIR/external/jpeg/turbojpeg.h.original
	cp $PATCHDIR1/external_jpeg/turbojpeg.h.patch $ROOTDIR/external/jpeg/turbojpeg.h.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.h < $ROOTDIR/external/jpeg/turbojpeg.h.patch

else

	rm -R jpeg
	tar -xf turbo-jpeg.tar
	cd jpeg
	echo 'update libjpeg-turbo to last version'
	git pull

	cp $ROOTDIR/external/jpeg/Android.mk $ROOTDIR/external/jpeg/Android.mk.original
	cp $PATCHDIR1/external_jpeg/Android.mk.patch $ROOTDIR/external/jpeg/Android.mk.patch
	patch -N $ROOTDIR/external/jpeg/Android.mk < $ROOTDIR/external/jpeg/Android.mk.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.c $ROOTDIR/external/jpeg/turbojpeg.c.original
	cp $PATCHDIR1/external_jpeg/turbojpeg.c.patch $ROOTDIR/external/jpeg/turbojpeg.c.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.c < $ROOTDIR/external/jpeg/turbojpeg.c.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.h $ROOTDIR/external/jpeg/turbojpeg.h.original
	cp $PATCHDIR1/external_jpeg/turbojpeg.h.patch $ROOTDIR/external/jpeg/turbojpeg.h.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.h < $ROOTDIR/external/jpeg/turbojpeg.h.patch
fi


cp $ROOTDIR/external/jhead/exif.c $ROOTDIR/external/jhead/exif.c.original
cp $PATCHDIR1/external_jhead/exif.c.patch $ROOTDIR/external/jhead/exif.c.patch
patch -N $ROOTDIR/external/jhead/exif.c < $ROOTDIR/external/jhead/exif.c.patch

cp $ROOTDIR/external/jhead/jhead.h $ROOTDIR/external/jhead/jhead.h.original
cp $PATCHDIR1/external_jhead/jhead.h.patch $ROOTDIR/external/jhead/jhead.h.patch
patch -N $ROOTDIR/external/jhead/jhead.h < $ROOTDIR/external/jhead/jhead.h.patch

# 	camera
#####
