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

PATCHDIR1=`pwd`
cd $PATCHDIR1/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/external/busybox/busybox-full.links $ROOTDIR/external/busybox/busybox-full.links.original
cp $PATCHDIR1/external_busybox/busybox-full.links.patch $ROOTDIR/external/busybox/busybox-full.links.patch
patch -N $ROOTDIR/external/busybox/busybox-full.links < $ROOTDIR/external/busybox/busybox-full.links.patch

cp $ROOTDIR/external/busybox/busybox-full.sources $ROOTDIR/external/busybox/busybox-full.sources.original
cp $PATCHDIR1/external_busybox/busybox-full.sources.patch $ROOTDIR/external/busybox/busybox-full.sources.patch
patch -N $ROOTDIR/external/busybox/busybox-full.sources < $ROOTDIR/external/busybox/busybox-full.sources.patch

cp $ROOTDIR/external/busybox/busybox-minimal.links $ROOTDIR/external/busybox/busybox-minimal.links.original
cp $PATCHDIR1/external_busybox/busybox-minimal.links.patch $ROOTDIR/external/busybox/busybox-minimal.links.patch
patch -N $ROOTDIR/external/busybox/busybox-minimal.links < $ROOTDIR/external/busybox/busybox-minimal.links.patch

cp $ROOTDIR/external/busybox/busybox-minimal.sources $ROOTDIR/external/busybox/busybox-minimal.sources.original
cp $PATCHDIR1/external_busybox/busybox-minimal.sources.patch $ROOTDIR/external/busybox/busybox-minimal.sources.patch
patch -N $ROOTDIR/external/busybox/busybox-minimal.sources < $ROOTDIR/external/busybox/busybox-minimal.sources.patch

