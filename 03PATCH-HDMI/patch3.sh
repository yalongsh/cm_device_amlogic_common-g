#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- diff -bwBu

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`

cp $ROOTDIR/frameworks/base/core/java/android/content/Intent.java $ROOTDIR/frameworks/base/core/java/android/content/Intent.java.original
cp $PATCHDIR/frameworks_base_core_java_android_content/Intent.java.patch $ROOTDIR/frameworks/base/core/java/android/content/Intent.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/content/Intent.java < $ROOTDIR/frameworks/base/core/java/android/content/Intent.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.original
cp $PATCHDIR/frameworks_base_core_java_android_widget/MediaController.java.patch $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java < $ROOTDIR/frameworks/base/core/java/android/widget/MediaController.java.patch
cp $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.original
cp $PATCHDIR/frameworks_base_core_java_android_widget/VideoView.java.patch $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java < $ROOTDIR/frameworks/base/core/java/android/widget/VideoView.java.patch

cp $ROOTDIR/frameworks/base/include/ui/InputReader.h $ROOTDIR/frameworks/base/include/ui/InputReader.h.original
cp $PATCHDIR/frameworks_base_include_ui/InputReader.h.patch $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch
patch -N $ROOTDIR/frameworks/base/include/ui/InputReader.h < $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch

cp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.original
cp $PATCHDIR/frameworks_base_libs_ui/InputReader.cpp.patch $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch
patch -N $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp < $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch

cp $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java.original
cp $PATCHDIR/frameworks_base_policy_src_com_android_internal_policy_impl/PhoneWindowManager.java.patch $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java.patch
patch -N $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java < $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/PhoneWindowManager.java.patch

cp $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.original
cp $PATCHDIR/frameworks_base_services_java_com_android_server/InputManager.java.patch $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.patch
patch -N $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java < $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.patch
cp $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.original
cp $PATCHDIR/frameworks_base_services_java_com_android_server/WindowManagerService.java.patch $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch
patch -N $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java < $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch

cp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.original
cp $PATCHDIR/frameworks_base_services_jni/com_android_server_InputManager.cpp.patch $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp < $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch

#cp $ROOTDIR/packages/apps/Browser/src/com/android/browser/BrowserActivity.java $ROOTDIR/packages/apps/Browser/src/com/android/browser/BrowserActivity.java.original
#cp $PATCHDIR/packages_apps_Browser_src_com_android_browser/BrowserActivity.java.patch $ROOTDIR/packages/apps/Browser/src/com/android/browser/BrowserActivity.java.patch
#patch -N $ROOTDIR/packages/apps/Browser/src/com/android/browser/BrowserActivity.java < $ROOTDIR/packages/apps/Browser/src/com/android/browser/BrowserActivity.java.patch
#cp $ROOTDIR/packages/apps/Browser/src/com/android/browser/Tab.java $ROOTDIR/packages/apps/Browser/src/com/android/browser/Tab.java.original
#cp $PATCHDIR/packages_apps_Browser_src_com_android_browser/Tab.java.patch $ROOTDIR/packages/apps/Browser/src/com/android/browser/Tab.java.patch
#patch -N $ROOTDIR/packages/apps/Browser/src/com/android/browser/Tab.java < $ROOTDIR/packages/apps/Browser/src/com/android/browser/Tab.java.patch

