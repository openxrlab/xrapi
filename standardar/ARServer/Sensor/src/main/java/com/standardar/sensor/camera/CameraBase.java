package com.standardar.sensor.camera;

import android.content.Context;
import android.graphics.SurfaceTexture;

import java.nio.ByteBuffer;

public abstract class CameraBase {
    public static final int CMD_IMAGEREADER_PAUSE = 1;
    public static final int CMD_IMAGEREADER_ACTIVE = 2;
    public static final int CMD_SURFACETEXTURE_UPDATE = 3;
    public static final int CMD_OES_TEXID_SET         = 4;
    public static final int CMD_GET_SURFACETEXTURE_TIMESTAMP = 5;
    public static final int CMD_SURFACETEXTURE_WAIT = 6;

    protected Context mContext;

    protected CameraMetaData mCameraMetaData;
    /**
     * Context is used to get CameraService
     * @param context
     */
    public CameraBase(Context context) {
        mContext = context;
    }

    /**
     * open Camera with Android Camera2 interface
     * subclass implements this method for RGB Camera or TOF Camera ...
     */
    public abstract void openCamera();

    /**
     * closeCamera should be invoked after openCamera in pairs
     */
    public abstract void closeCamera();

    public abstract CameraMetaData getCameraMetaData();

    /**
     * set preview size
     * @param width preview width
     * @param height preview height
     */
    public abstract void setPreviewSize(int width, int height);

    public abstract void startPreview();

    public abstract void stopPreview();
    public abstract void setScreenRotate(int screenRotate);

    public abstract void setAsync(boolean isAsync);

    /**
     * add listener to process image in pixel
     * @param lis
     */
    public abstract void registerFrameAvailableListener(FrameAvailableListener lis);

    /**
     * remove image reader listener
     * @param lis
     */
    public abstract void unregisterFrameAvailableListener(FrameAvailableListener lis);

    public interface FrameAvailableListener {
        void onImageAvailable(SImageV1 sImage);
    }

    /**
     * do some user-defined cmd
     */
    public abstract void doCommand(int cmd, ByteBuffer msg);
}
