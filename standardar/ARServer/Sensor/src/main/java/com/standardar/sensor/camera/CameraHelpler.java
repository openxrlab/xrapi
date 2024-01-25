package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.graphics.SurfaceTexture;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Handler;
import android.view.Surface;

public class CameraHelpler {
    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static ImageReader createImageReader(int width, int height, int format, int maxImages,
                                                ImageReader.OnImageAvailableListener lis,
                                                Handler handler) {
        ImageReader imageReader = ImageReader.newInstance(width, height, format, maxImages);
        imageReader.setOnImageAvailableListener(lis, handler);
        return imageReader;
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static void releaseImageReader(ImageReader imageReader) {
        if (imageReader != null) {
            imageReader.close();
        }
    }

    public static SurfaceTexture createSurfaceTexture(int texid, int w, int h,
                                                      SurfaceTexture.OnFrameAvailableListener lis) {
        SurfaceTexture st = new SurfaceTexture(texid);
        st.setOnFrameAvailableListener(lis);
        st.setDefaultBufferSize(w, h);
        return st;
    }

    public static void releaseSurfaceTexture(SurfaceTexture st) {
        if (st != null) {
            st.setOnFrameAvailableListener(null);
            st.release();
        }
    }

    public static Surface createSurface(SurfaceTexture st) {
        return new Surface(st);
    }

    public static void releaseSurface(Surface surface) {
        if (surface != null) {
            surface.release();
        }
    }
}
