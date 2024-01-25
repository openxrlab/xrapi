package com.standardar.service.slam.algorithm;

import android.content.res.AssetManager;
import com.standardar.common.CameraConstant;
import com.standardar.sensor.camera.SImageDataV1;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.DirectByteBufferNativeUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class SLAM {
    protected AssetManager mAssetManager;
    protected String mPathToInternalDir;

    private static final int COPY_BUFFER_SIZE = 4 * 1024 * 1024;

    protected ByteBuffer mCopyBuffer;


    public SLAM(AssetManager assetManager, String pathToInternalDir) {
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }

    static {
        System.loadLibrary("rgbd_slamjni");
    }

    public int initSLAM(AssetManager assetManager, String pathToInternalPath, String packageName,
                        int w, int h, float fovh, int type) {
        mCopyBuffer = ByteBuffer.allocateDirect(COPY_BUFFER_SIZE);
        mCopyBuffer.order(ByteOrder.nativeOrder());
        return initNativeSLAM(assetManager, pathToInternalPath, w, h, fovh, type);
    }

    public void destroySLAM() {
        destroyNativeSLAM();
    }

    public void processFrameDirectly(SImageV1 sImageV1) {
        if (sImageV1 == null) {
            return;
        }
        if (sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_RGB ||
            sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_STEREO || sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_RGB_DOUBLE_STREAM) {
            SImageDataV1 imageDataV1 = sImageV1.mImageDataList.get(0);
            setFrame(imageDataV1.mImageData, imageDataV1.mImageFormat, imageDataV1.mWidth,
                    imageDataV1.mHeight, imageDataV1.mStride, imageDataV1.mExposureTime, imageDataV1.mTimeStamp);

        } else if (sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_RGBD) {
            if (sImageV1.mImageDataList.size() == 1) {
                return;
            }
            for (SImageDataV1 imageDataV1 : sImageV1.mImageDataList) {
                setFrame(imageDataV1.mImageData, imageDataV1.mImageFormat, imageDataV1.mWidth,
                        imageDataV1.mHeight, imageDataV1.mStride, imageDataV1.mExposureTime, imageDataV1.mTimeStamp);
            }
        }
        processFrameDirectlyNative();
    }

    public void pushSensorDirectly(float[] value, int tag, long timestamp) {
        pushSensorDirectlyNative(value, tag, timestamp);
    }

    public void getImageIntrinsics(ByteBuffer buffer) {
        getImageIntrinsicsNative(buffer, DirectByteBufferNativeUtil.order(buffer));
    }

    public int getTrackingState() {
        return getTrackingStateNative();
    }


    public void updateAlgorithmResult() {
        updateAlgorithmResultNative();
    }

    public void setAxisUpMode(int mode) {
        setAxisUpModeNative(mode);
    }

    public void getViewMatrix(long view_ptr, int screenRotate) {
        getViewMatrixNative(view_ptr, screenRotate);
    }

    public float[] insectPlaneWithSlamResult(float[] rayOrigin, float[] rayDirection, int[] planeId, float[] quat) {
        return insectPlaneWithSlamResultNative(rayOrigin, rayDirection, planeId, quat);
    }

    public int getNumberLandMarks() {
        return getNumberLandMarksNative();
    }

    public long getLandMarks() {
        return getLandMarksNative();
    }

    public float getDistanceFromCamera(float[] hitPose) {
        return getDistanceFromCameraNative(hitPose);
    }

    private native int initNativeSLAM(AssetManager assetManager, String pathToInternalPath,
                                    int w, int h, float fovh, int type);
    private native void destroyNativeSLAM();
    private native void pushSensorDirectlyNative(float[] value, int tag, long timestamp);
    private native void setFrame(ByteBuffer buffer, int format, int width, int height, int stride, long exposureTime, long timestamp);
    private native void processFrameDirectlyNative();
    private native void getImageIntrinsicsNative(ByteBuffer buffer, int order);
    private native int getTrackingStateNative();
    private native void updateAlgorithmResultNative();
    private native void setAxisUpModeNative(int mode);
    private native void getViewMatrixNative(long view_ptr, int screenRotate);
    private native float[] insectPlaneWithSlamResultNative(float[] rayOrigin, float[] rayDirection, int[] planeId, float[] quat);
    private native int getNumberLandMarksNative();
    private native long getLandMarksNative();
    private native float getDistanceFromCameraNative(float[] hitPose);
}
