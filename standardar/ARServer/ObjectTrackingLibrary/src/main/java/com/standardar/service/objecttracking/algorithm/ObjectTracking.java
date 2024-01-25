package com.standardar.service.objecttracking.algorithm;

import android.content.res.AssetManager;

import com.standardar.common.CameraConstant;
import com.standardar.sensor.camera.SImageDataV1;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.DirectByteBufferNativeUtil;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ObjectTracking {
    private AssetManager mAssetManager;
    private String mPathToInternalDir;

    public ObjectTracking(AssetManager assetManager, String pathToInternalDir) {
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }

    static {
        System.loadLibrary("objecttrackingjni");
    }

    public int initObjectTracking(int w, int h) {
        return initObjectTrackingNative(mAssetManager, mPathToInternalDir, w, h);
    }

    public void resetObjectTracking() {
        resetObjectTrackingNative();
    }

    public void destroyObjectTracking() {
        destroyObjectTrackingNative();
    }

    public void addObject(ByteBuffer buffer) {
        addObjectNative(buffer, buffer.order() == ByteOrder.BIG_ENDIAN ? 0 : 1);
    }

    public int getResultLength() {
        return getResultLengthNative();
    }

    public void processFrameDirectly(SImageV1 sImageV1) {
        if (sImageV1 == null) {
            return;
        }
        if (sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_RGB_AUTOFOCUS || sImageV1.mCameraStreamType == CameraConstant.CAMERA_BACK_RGB) {
            SImageDataV1 imageDataV1 = sImageV1.mImageDataList.get(0);
            setFrame(imageDataV1.mImageData, imageDataV1.mImageFormat, imageDataV1.mWidth,
                    imageDataV1.mHeight, imageDataV1.mStride, imageDataV1.mExposureTime, imageDataV1.mTimeStamp);

            processFrameDirectlyNative();
        }
    }

    public void getResultDirectly(ByteBuffer buffer) {
        getResultDirectlyNative(buffer, DirectByteBufferNativeUtil.order(buffer));
    }

    public void pushSensorDirectly(float[] value, int tag, long timestamp) {
        pushSensorDirectlyNative(value, tag, timestamp);
    }

    public void updateAlgorithmResult() {
        updateAlgorithmResultNative();
    }

    public void getCenterPose(int object_id, long pose_ptr){
        getCenterPoseNative(object_id, pose_ptr);
    }

    public void getBoundingBox(int object_id, long vertex_ptr){
        getBoundingBoxNative(object_id, vertex_ptr);
    }

    public float getExtentX(int object_id)
    {
        return getExtentXNative(object_id);
    }

    public float getExtentY(int object_id)
    {
        return getExtentYNative(object_id);
    }

    public float getExtentZ(int object_id)
    {
        return getExtentZNative(object_id);
    }

    public int[] getAllObjectId(){
        return getAllObjectIdNative();
    }

    public String getObjectTrackingInfo() {
        return getObjectTrackingInfoNative();
    }

    public void getViewMatrix(long view_ptr, int screenRotate) {
        getViewMatrixNative(view_ptr, screenRotate);
    }

    private native int initObjectTrackingNative(AssetManager assetManager, String pathToInternalPath, int width, int height);
    private native void resetObjectTrackingNative();
    private native void destroyObjectTrackingNative();
    private native void addObjectNative(ByteBuffer buffer, int order);
    private native int  getResultLengthNative();
    private native void setFrame(ByteBuffer buffer, int format, int width, int height, int stride, long exposureTime, long timestamp);
    private native void getResultDirectlyNative(ByteBuffer buffer, int order);
    private native void processFrameDirectlyNative();
    private native void pushSensorDirectlyNative(float[] value, int tag, long timestamp);

    private native void updateAlgorithmResultNative();
    private native void getCenterPoseNative(int object_id, long matrix_ptr);
    private native void getBoundingBoxNative(int object_id, long vertex_ptr);
    private native float getExtentXNative(int object_id);
    private native float getExtentYNative(int object_id);
    private native float getExtentZNative(int object_id);
    private native int[] getAllObjectIdNative();
    private native String getObjectTrackingInfoNative();
    private native void getViewMatrixNative(long view_ptr, int screenRotate);

}
