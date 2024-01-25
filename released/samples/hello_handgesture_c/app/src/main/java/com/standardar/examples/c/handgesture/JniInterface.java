/* modification list:
 * 1. add onStartAlgorithm
 * 2. add onStopAlgorithm
 * 3. add onDebugCommandInt
 * 4. add readPixel
 * 5. add deleteModel
 * 6. add getSLAMInfo
 * 7. add isSLAMInitializing
 */

package com.standardar.examples.c.handgesture;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLUtils;
import android.util.Log;
import java.io.IOException;

/** JNI interface to native layer. */
public class JniInterface {
  static {
    System.loadLibrary("hello_ar_native");
  }

  private static final String TAG = "JniInterface";
  static AssetManager assetManager;

  public static native long createNativeApplication(AssetManager assetManager, Context context, String packageName);

  public static native void destroyNativeApplication(long nativeApplication);

  public static native void onDebugCommandInt(long nativeApplication, int type, int value);

  public static native void onPause(long nativeApplication);

  public static native void onResume(long nativeApplication);

  public static native void onStartAlgorithm(long nativeApplication);

  public static native void onStopAlgorithm(long nativeApplication);

  /** Allocate OpenGL resources for rendering. */
  public static native void onGlSurfaceCreated(long nativeApplication);

  /**
   * Called on the OpenGL thread before onGlSurfaceDrawFrame when the view port width, height, or
   * display rotation may have changed.
   */
  public static native void onDisplayGeometryChanged(
      long nativeApplication, int displayRotation, int width, int height);

  /** Main render loop, called on the OpenGL thread. */
  public static native void onGlSurfaceDrawFrame(long nativeApplication);

  /**Screenshot function, called on the OpenGL thread.**/
  public static native byte[] readPixel(long nativeApplication, int width, int height);

  /** OnTouch event, called on the OpenGL thread. */
  public static native void onTouched(long nativeApplication, float x, float y);

  public static native void deleteModel(long nativeApplication);

  /** Get plane count in current session. Used to disable the "searching for surfaces" snackbar. */
  public static native boolean hasDetectedPlanes(long nativeApplication);

  //Get the slam information string
  public static native String getSLAMInfo(long nativeApplication);

  //Get whether the SLAM state is initializing
  public static native boolean isSLAMInitializing(long nativeApplication);

  public static native void onDebugTouchDown(long nativeApplication, float x, float y);

  public static native void onDebugTouchMove(long nativeApplication, float x, float y);

  public static native void onDebugTouchUp(long nativeApplication, float x, float y);

  public static native void OnDebugTouchZoom(long nativeApplication, float distx, float disty);

  public static native void OnSLAM3DView(long nativeApplication, int benable);

  public static native void OnScreenSwitch(long nativeApplication, int benable);

  public static Bitmap loadImage(String imageName) {

    try {
      return BitmapFactory.decodeStream(assetManager.open(imageName));
    } catch (IOException e) {
      Log.e(TAG, "Cannot open image " + imageName);
      return null;
    }
  }

  public static void loadTexture(int target, Bitmap bitmap) {
    GLUtils.texImage2D(target, 0, bitmap, 0);
  }
}
