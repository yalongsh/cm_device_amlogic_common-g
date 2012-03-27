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

