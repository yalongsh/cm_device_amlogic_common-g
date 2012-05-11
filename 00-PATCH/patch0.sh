#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- with Meld

PATCHDIR0=`pwd`
cd $PATCHDIR0/../../../..
ROOTDIR=`pwd`

#####
#	byteorder

cp $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.original
cp $PATCHDIR0/bionic_libc_kernel_arch-arm_asm/byteorder.h.patch $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.patch
patch -N $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h < $ROOTDIR/bionic/libc/kernel/arch-arm/asm/byteorder.h.patch

#	byteorder
#####
#	wifi

cp $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.original
cp $PATCHDIR0/hardware_libhardware_legacy_wifi/Android.mk.patch $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.patch
patch -N $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk < $ROOTDIR/hardware/libhardware_legacy/wifi/Android.mk.patch

cp $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.original
cp $PATCHDIR0/hardware_libhardware_legacy_wifi/wifi.c.patch $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch
patch -N $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c < $ROOTDIR/hardware/libhardware_legacy/wifi/wifi.c.patch

#	wifi
#####
#	fbset

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

#	fbset
#####

echo 'end of patch :)'
