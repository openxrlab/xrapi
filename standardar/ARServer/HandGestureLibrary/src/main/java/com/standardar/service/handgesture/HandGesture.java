package com.standardar.service.handgesture;

import android.content.res.AssetManager;
import com.standardar.sensor.camera.SImageV1;

public class HandGesture {
    private static final String TAG = "HandGesture";

    private AssetManager mAssetManager;
    private String mPathToInternalDir;

    public HandGesture(AssetManager assetManager, String pathToInternalDir) {
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }


    public int initHandGesture(int width, int height, float fovh) {
        return 0;
    }

    public void startHandGestureAlgorithm() {
    }

    public void stopHandGestureAlgorithm() {
    }


    public void destroyHandGesture() {
    }

    public void processFrameDirectly(SImageV1 sImageV1) {
    }

    public int[] getAllHandGestureId() {
        return null;
    }

    public int getHandSide(int hand_id){
        return -1;
    }

    public int getHandType(int hand_id){
        return -1;
    }

    public int getHandToward(int hand_id){
        return -1;
    }

    public float getHandTypeConfidence(int hand_id){
        return -1;
    }

    public int getLandMark2DCount(int hand_id){
        return 0;
    }

    void getLandMark2DArray(int hand_id ,long points2dPtr){
    }

}
