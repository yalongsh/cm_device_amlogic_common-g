#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- with Meld

PATCHDIR0=`pwd`
cd $PATCHDIR0/../../../..
ROOTDIR=`pwd`

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
	cp $PATCHDIR0/external_jpeg/Android.mk.patch $ROOTDIR/external/jpeg/Android.mk.patch
	patch -N $ROOTDIR/external/jpeg/Android.mk < $ROOTDIR/external/jpeg/Android.mk.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.c $ROOTDIR/external/jpeg/turbojpeg.c.original
	cp $PATCHDIR0/external_jpeg/turbojpeg.c.patch $ROOTDIR/external/jpeg/turbojpeg.c.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.c < $ROOTDIR/external/jpeg/turbojpeg.c.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.h $ROOTDIR/external/jpeg/turbojpeg.h.original
	cp $PATCHDIR0/external_jpeg/turbojpeg.h.patch $ROOTDIR/external/jpeg/turbojpeg.h.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.h < $ROOTDIR/external/jpeg/turbojpeg.h.patch

else

	rm -R jpeg
	tar -xf turbo-jpeg.tar
	cd jpeg
	echo 'update libjpeg-turbo to last version'
	git pull

	cp $ROOTDIR/external/jpeg/Android.mk $ROOTDIR/external/jpeg/Android.mk.original
	cp $PATCHDIR0/external_jpeg/Android.mk.patch $ROOTDIR/external/jpeg/Android.mk.patch
	patch -N $ROOTDIR/external/jpeg/Android.mk < $ROOTDIR/external/jpeg/Android.mk.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.c $ROOTDIR/external/jpeg/turbojpeg.c.original
	cp $PATCHDIR0/external_jpeg/turbojpeg.c.patch $ROOTDIR/external/jpeg/turbojpeg.c.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.c < $ROOTDIR/external/jpeg/turbojpeg.c.patch

	cp $ROOTDIR/external/jpeg/turbojpeg.h $ROOTDIR/external/jpeg/turbojpeg.h.original
	cp $PATCHDIR0/external_jpeg/turbojpeg.h.patch $ROOTDIR/external/jpeg/turbojpeg.h.patch
	patch -N $ROOTDIR/external/jpeg/turbojpeg.h < $ROOTDIR/external/jpeg/turbojpeg.h.patch
fi

# 1
#####
# 2

cd $ROOTDIR/external
if [ ! -f cm-busybox.tar ]
	then
	tar -cf cm-busybox.tar busybox
	rm -R busybox
fi
if [ ! -f busybox+fbset.tar ]
	then
	rm -R busybox
	echo 'clone busybox with fbset from git://github.com/psquare/android_external_busybox.git'
	git clone git://github.com/psquare/android_external_busybox.git -b gingerbread-psquare busybox
else
	rm -R busybox
	tar -xf busybox+fbset.tar busybox
	cd busybox
	echo 'update busybox+fbset to last version'
	git pull
fi

# 2
#####
# 3

cp $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.original
cp $PATCHDIR0/hardware_libhardware_legacy_wifi/Android.mk.patch $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.patch
patch -N $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk < $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.patch

cp $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.original
cp $PATCHDIR0/hardware_libhardware_legacy_wifi/wifi.c.patch $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch
patch -N $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c < $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch

echo 'end of patch :)'
