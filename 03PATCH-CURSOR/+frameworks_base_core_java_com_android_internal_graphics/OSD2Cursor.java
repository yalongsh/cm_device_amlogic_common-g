/*
 *
 */

package com.android.internal.graphics;

public final class OSD2Cursor {
    /**
     * This class is uninstantiable.
     */
    private OSD2Cursor() {
        // This space intentionally left blank.
    }

    private static native void nativeSetPosition(short x, short y);
    private static native void nativeShow();
    private static native void nativeHide();
    private static native void nativeSetRotation(short rotation);
    private static native void nativeSetDisplaySize(short w, short h);
    private static native void nativeTVOutChanged(boolean isEnabled);

    /**
     * Move the cursor to the specified position.
     *
     * @param x horizontal coordinate
     * @param y vertical coordinate
     */
    public static void setPosition(short x, short y) {
        nativeSetPosition(x, y);
    }

    /**
     * Show the cursor.  The cursor is shown by default.
     */
    public static void show() {
        nativeShow();
    }

    /**
     * Hide the cursor.
     */
    public static void hide() {
        nativeHide();
    }

    /**
     * Call when display rotated.
     *
     * @param rotation 0=0, 1=90, 2=180, 3=270 - same as Surface.ROTATION_*
     */
    public static void setRotation(short rotation) {
        nativeSetRotation(rotation);
    }

    /**
     * Call on init and when display rotated.
     *
     * @param w width
     * @param h height
     */
    public static void setDisplaySize(short w, short h) {
        nativeSetDisplaySize(w, h);
    }

    /**
     * Call when TV out is enabled or disabled and cursor needs to stay unrotated.
     *
     * @param w width
     * @param h height
     */
    public static void tvOutChanged(boolean isEnabled) {
        nativeTVOutChanged(isEnabled);
    }
}
