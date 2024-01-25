package com.standardar.examples.c.object;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Paint;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

public class ObjectTrackingJniInterface {

    private static final String TAG = "ObjectTrackingJniInterface";
    static AssetManager assetManager;

    public static native long createNativeApplication(AssetManager assetManager, Context context, String packageName, String apkInternalPath);

    public static native void destroyNativeApplication(long nativeApplication);

    public static native void onPause(long nativeApplication);

    public static native void onResume(long nativeApplication, Context context, Activity activity);

    /** Allocate OpenGL resources for rendering. */
    public static native void onGlSurfaceCreated(long nativeApplication);

    public static native String getObjectTrackingInfo(long nativeApplication);

    public static native void onStopAlgorithm(long nativeApplication);

    public static native boolean isSLAMInitializing(long nativeApplication);

    /**
     * Called on the OpenGL thread before onGlSurfaceDrawFrame when the view port width, height, or
     * display rotation may have changed.
     */
    public static native void onDisplayGeometryChanged(
            long nativeApplication, int displayRotation, int width, int height);

    /** Main render loop, called on the OpenGL thread. */
    public static native void onGlSurfaceDrawFrame(
            long nativeApplication, ObjectTrackingActivity activity);

    public static Bitmap loadImage(String imageName) {

        try {
            return BitmapFactory.decodeStream(assetManager.open(imageName));
        } catch (IOException e) {
            Log.e(TAG, "Cannot open image " + imageName);
            return null;
        }
    }

    private static ByteBuffer convertBitmapToGrayscaleDirectBuffer(Bitmap image) {
        ColorMatrix colorMatrix;
        (colorMatrix = new ColorMatrix()).setSaturation(0.0F);
        ColorMatrixColorFilter filter = new ColorMatrixColorFilter(colorMatrix);
        Paint paint;
        (paint = new Paint()).setColorFilter(filter);
        Bitmap rgbdImage = Bitmap.createBitmap(image.getWidth(), image.getHeight(), Bitmap.Config.ARGB_8888);
        (new Canvas(rgbdImage)).drawBitmap(image, 0.0F, 0.0F, paint);
        ByteBuffer buffer = ByteBuffer.allocateDirect(image.getWidth() * image.getHeight());

        for (int i = 0; i < image.getHeight(); ++i) {
            for (int j = 0; j < image.getWidth(); ++j) {
                buffer.put(i * image.getWidth() + j, (byte) Color.red(rgbdImage.getPixel(j, i)));
            }
        }

        return buffer;
    }

    public static void loadTexture(int target, Bitmap bitmap) {
        GLUtils.texImage2D(target, 0, bitmap, 0);
    }

    public static void hideFitToScanImage(ObjectTrackingActivity activity) {
        activity.hideFitToScanImage();
    }
}
