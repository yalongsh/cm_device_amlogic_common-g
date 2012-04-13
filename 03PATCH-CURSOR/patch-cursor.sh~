#!/bin/sh

#
# 2012 Pasquale Convertini    aka psquare (psquare.dev@gmail.com)
#

# THIS IS TEMPORARY PATCH --- Meld

PATCHDIR=`pwd`
cd $PATCHDIR/../../../..
ROOTDIR=`pwd`


#cp $ROOTDIR/frameworks/base/api/current.xml $ROOTDIR/frameworks/base/api/current.xml.original
#cp $PATCHDIR/frameworks_base_api/current.xml.patch $ROOTDIR/frameworks/base/api/current.xml.patch
#patch -N $ROOTDIR/frameworks/base/api/current.xml < $ROOTDIR/frameworks/base/api/current.xml.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/IWindowManager.aidl $ROOTDIR/frameworks/base/core/java/android/view/IWindowManager.aidl.original
cp $PATCHDIR/frameworks_base_core_java_android_view/IWindowManager.aidl.patch $ROOTDIR/frameworks/base/core/java/android/view/IWindowManager.aidl.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/IWindowManager.aidl < $ROOTDIR/frameworks/base/core/java/android/view/IWindowManager.aidl.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/MotionEvent.java $ROOTDIR/frameworks/base/core/java/android/view/MotionEvent.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/MotionEvent.java.patch $ROOTDIR/frameworks/base/core/java/android/view/MotionEvent.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/MotionEvent.java < $ROOTDIR/frameworks/base/core/java/android/view/MotionEvent.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/ViewRoot.java $ROOTDIR/frameworks/base/core/java/android/view/ViewRoot.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/ViewRoot.java.patch $ROOTDIR/frameworks/base/core/java/android/view/ViewRoot.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/ViewRoot.java < $ROOTDIR/frameworks/base/core/java/android/view/ViewRoot.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/Window.java $ROOTDIR/frameworks/base/core/java/android/view/Window.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/Window.java.patch $ROOTDIR/frameworks/base/core/java/android/view/Window.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/Window.java < $ROOTDIR/frameworks/base/core/java/android/view/Window.java.patch

#cp $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.original
#cp $PATCHDIR/frameworks_base_core_java_android_view/WindowManager.java.patch $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.patch
#patch -N $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java < $ROOTDIR/frameworks/base/core/java/android/view/WindowManager.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/view/WindowManagerImpl.java $ROOTDIR/frameworks/base/core/java/android/view/WindowManagerImpl.java.original
cp $PATCHDIR/frameworks_base_core_java_android_view/WindowManagerImpl.java.patch $ROOTDIR/frameworks/base/core/java/android/view/WindowManagerImpl.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/view/WindowManagerImpl.java < $ROOTDIR/frameworks/base/core/java/android/view/WindowManagerImpl.java.patch

cp $ROOTDIR/frameworks/base/core/jni/Android.mk $ROOTDIR/frameworks/base/core/jni/Android.mk.original
cp $PATCHDIR/frameworks_base_core_jni/Android.mk.patch $ROOTDIR/frameworks/base/core/jni/Android.mk.patch
patch -N $ROOTDIR/frameworks/base/core/jni/Android.mk < $ROOTDIR/frameworks/base/core/jni/Android.mk.patch

cp $ROOTDIR/frameworks/base/core/jni/AndroidRuntime.cpp $ROOTDIR/frameworks/base/core/jni/AndroidRuntime.cpp.original
cp $PATCHDIR/frameworks_base_core_jni/AndroidRuntime.cpp.patch $ROOTDIR/frameworks/base/core/jni/AndroidRuntime.cpp.patch
patch -N $ROOTDIR/frameworks/base/core/jni/AndroidRuntime.cpp < $ROOTDIR/frameworks/base/core/jni/AndroidRuntime.cpp.patch

cp $ROOTDIR/frameworks/base/include/ui/InputDispatcher.h $ROOTDIR/frameworks/base/include/ui/InputDispatcher.h.original
cp $PATCHDIR/frameworks_base_include_ui/InputDispatcher.h.patch $ROOTDIR/frameworks/base/include/ui/InputDispatcher.h.patch
patch -N $ROOTDIR/frameworks/base/include/ui/InputDispatcher.h < $ROOTDIR/frameworks/base/include/ui/InputDispatcher.h.patch

cp $ROOTDIR/frameworks/base/include/ui/InputReader.h $ROOTDIR/frameworks/base/include/ui/InputReader.h.original
cp $PATCHDIR/frameworks_base_include_ui/InputReader.h.patch $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch
patch -N $ROOTDIR/frameworks/base/include/ui/InputReader.h < $ROOTDIR/frameworks/base/include/ui/InputReader.h.patch

cp $ROOTDIR/frameworks/base/libs/ui/Android.mk $ROOTDIR/frameworks/base/libs/ui/Android.mk.original
cp $PATCHDIR/frameworks_base_libs_ui/Android.mk.patch $ROOTDIR/frameworks/base/libs/ui/Android.mk.patch
patch -N $ROOTDIR/frameworks/base/libs/ui/Android.mk < $ROOTDIR/frameworks/base/libs/ui/Android.mk.patch

cp $ROOTDIR/frameworks/base/libs/ui/InputDispatcher.cpp $ROOTDIR/frameworks/base/libs/ui/InputDispatcher.cpp.original
cp $PATCHDIR/frameworks_base_libs_ui/InputDispatcher.cpp.patch $ROOTDIR/frameworks/base/libs/ui/InputDispatcher.cpp.patch
patch -N $ROOTDIR/frameworks/base/libs/ui/InputDispatcher.cpp < $ROOTDIR/frameworks/base/libs/ui/InputDispatcher.cpp.patch

#cp $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.original
#cp $PATCHDIR/frameworks_base_libs_ui/InputManager.cpp.patch $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.patch
#patch -N $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp < $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.patch

#cp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.original
#cp $PATCHDIR/frameworks_base_libs_ui/InputReader.cpp.patch $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch
#patch -N $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp < $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch

cp $ROOTDIR/frameworks/base/libs/ui/tests/InputReader_test.cpp $ROOTDIR/frameworks/base/libs/ui/tests/InputReader_test.cpp.original
cp $PATCHDIR/frameworks_base_libs_ui_tests/InputReader_test.cpp.patch $ROOTDIR/frameworks/base/libs/ui/tests/InputReader_test.cpp.patch
patch -N $ROOTDIR/frameworks/base/libs/ui/tests/InputReader_test.cpp < $ROOTDIR/frameworks/base/libs/ui/tests/InputReader_test.cpp.patch

cp $ROOTDIR/frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarView.java $ROOTDIR/frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarView.java.original
cp $PATCHDIR/frameworks_base_packages_SystemUI_src_com_android_systemui_statusbar/StatusBarView.java.patch $ROOTDIR/frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarView.java.patch
patch -N $ROOTDIR/frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarView.java < $ROOTDIR/frameworks/base/packages/SystemUI/src/com/android/systemui/statusbar/StatusBarView.java.patch

cp $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/FingerUnlockScreen.java $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/FingerUnlockScreen.java.original
cp $PATCHDIR/frameworks_base_policy_src_com_android_internal_policy_impl/FingerUnlockScreen.java.patch $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/FingerUnlockScreen.java.patch
patch -N $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/FingerUnlockScreen.java < $ROOTDIR/frameworks/base/policy/src/com/android/internal/policy/impl/FingerUnlockScreen.java.patch

cp $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.original
cp $PATCHDIR/frameworks_base_services_java_com_android_server/InputManager.java.patch $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.patch
patch -N $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java < $ROOTDIR/frameworks/base/services/java/com/android/server/InputManager.java.patch

#cp $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.original
#cp $PATCHDIR/frameworks_base_services_java_com_android_server/WindowManagerService.java.patch $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch
#patch -N $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java < $ROOTDIR/frameworks/base/services/java/com/android/server/WindowManagerService.java.patch

#1
#####
#2

cp $ROOTDIR/frameworks/base/core/java/android/content/res/Configuration.java $ROOTDIR/frameworks/base/core/java/android/content/res/Configuration.java.original
cp $PATCHDIR/frameworks_base_core_java_android_content_res/Configuration.java.patch $ROOTDIR/frameworks/base/core/java/android/content/res/Configuration.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/content/res/Configuration.java < $ROOTDIR/frameworks/base/core/java/android/content/res/Configuration.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/database/AbstractCursor.java $ROOTDIR/frameworks/base/core/java/android/database/AbstractCursor.java.original
cp $PATCHDIR/frameworks_base_core_java_android_database/AbstractCursor.java.patch $ROOTDIR/frameworks/base/core/java/android/database/AbstractCursor.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/database/AbstractCursor.java < $ROOTDIR/frameworks/base/core/java/android/database/AbstractCursor.java.patch

cp $ROOTDIR/frameworks/base/core/java/android/text/style/DrawableMarginSpan.java $ROOTDIR/frameworks/base/core/java/android/text/style/DrawableMarginSpan.java.original
cp $PATCHDIR/frameworks_base_core_java_android_text_style/DrawableMarginSpan.java.patch $ROOTDIR/frameworks/base/core/java/android/text/style/DrawableMarginSpan.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/text/style/DrawableMarginSpan.java < $ROOTDIR/frameworks/base/core/java/android/text/style/DrawableMarginSpan.java.patch

cp $ROOTDIR/frameworks/base/include/ui/InputManager.h $ROOTDIR/frameworks/base/include/ui/InputManager.h.original
cp $PATCHDIR/frameworks_base_include_ui/InputManager.h.patch $ROOTDIR/frameworks/base/include/ui/InputManager.h.patch
patch -N $ROOTDIR/frameworks/base/include/ui/InputManager.h < $ROOTDIR/frameworks/base/include/ui/InputManager.h.patch

cp $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.original
cp $PATCHDIR/frameworks_base_libs_ui/InputManager.cpp.patch2 $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.patch2
patch -N $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp < $ROOTDIR/frameworks/base/libs/ui/InputManager.cpp.patch2

cp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.original
cp $PATCHDIR/frameworks_base_libs_ui/InputReader.cpp.patch2 $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch2
patch -N $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp < $ROOTDIR/frameworks/base/libs/ui/InputReader.cpp.patch2

#2
#####
#3

cp $ROOTDIR/frameworks/base/core/java/android/database/sqlite/SQLiteCursor.java $ROOTDIR/frameworks/base/core/java/android/database/sqlite/SQLiteCursor.java.original
cp $PATCHDIR/frameworks_base_core_java_android_database_sqlite/SQLiteCursor.java.patch $ROOTDIR/frameworks/base/core/java/android/database/sqlite/SQLiteCursor.java.patch
patch -N $ROOTDIR/frameworks/base/core/java/android/database/sqlite/SQLiteCursor.java < $ROOTDIR/frameworks/base/core/java/android/database/sqlite/SQLiteCursor.java.patch

cp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.original
cp $PATCHDIR/frameworks_base_services_jni/com_android_server_InputManager.cpp.patch $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch
patch -N $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp < $ROOTDIR/frameworks/base/services/jni/com_android_server_InputManager.cpp.patch

#3
#####
#4

cp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.original
cp $PATCHDIR/frameworks_base_media_libmedia/AudioTrack.cpp.patch $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch
patch -N $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp < $ROOTDIR/frameworks/base/media/libmedia/AudioTrack.cpp.patch

mkdir $ROOTDIR/frameworks/base/core/java/com/android/internal/graphics
cp $PATCHDIR/+frameworks_base_core_java_com_android_internal_graphics/* $ROOTDIR/frameworks/base/core/java/com/android/internal/graphics
cp $PATCHDIR/+frameworks_base_core_jni/* $ROOTDIR/frameworks/base/core/jni
cp $PATCHDIR/+frameworks_base_include_ui/* $ROOTDIR/frameworks/base/include/ui
cp $PATCHDIR/+frameworks_base_libs_ui/* $ROOTDIR/frameworks/base/libs/ui

