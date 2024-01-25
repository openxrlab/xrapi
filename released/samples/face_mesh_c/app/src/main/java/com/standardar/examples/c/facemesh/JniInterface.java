/* modification list:
 * 1. add onStartAlgorithm
 * 2. add onStopAlgorithm
 * 3. add onDebugCommandInt
 * 4. add readPixel
 * 5. add deleteModel
 * 6. add getSLAMInfo
 * 7. add isSLAMInitializing
 */

package com.standardar.examples.c.facemesh;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;


/** JNI interface to native layer. */
public class JniInterface {
  static {
    System.loadLibrary("fecemesh_demo_native");
  }

  private static final String TAG = "JniInterface";
  static AssetManager assetManager;

  public static native long createNativeApplication(AssetManager assetManager, Context context, String packageName);

  public static native void destroyNativeApplication(long nativeApplication);

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

  static public native void setTexture(long nativeApplication, Bitmap bitmap);

  public static native void switchCamera(long nativeApplication);


}
