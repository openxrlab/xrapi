package com.standardar.service.algorithm.marker;

import android.content.res.AssetManager;
import android.util.Log;

import com.standardar.common.CameraConstant;
import com.standardar.sensor.camera.SImageDataV1;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.DirectByteBufferNativeUtil;
import com.standardar.service.common.util.LogUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Marker {
    private AssetManager mAssetManager;
    private String mPathToInternalDir;

    public Marker(AssetManager assetManager, String pathToInternalDir) {
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }

    public int initMarker(int width, int height, float fovh) {
        return -1;
    }

    public void resetMarker() {

    }

    public void destroyMarker() {

    }

    public void addTargetImage(ByteBuffer buffer) {

    }

    public void processFrameDirectly(SImageV1 sImageV1) {
    }

    public void updateAlgorithmResult() {

    }

    public void getCenterPose(int marker_id, long view_matrix_ptr, int screenRotate, long pose_ptr){

    }

    public void getCorners2d(int marker_id, long corners_ptr) {

    }

    public float getExtentX(int marker_id)
    {
        return 0;
    }

    public float getExtentY(int marker_id)
    {
        return 0;
    }

    public float getExtentZ(int marker_id)
    {
        return 0;
    }

    public int[] getAllMarkerId(){
        return null;
    }

}
