package com.standardar.service.bodydetect;

import android.content.res.AssetManager;
import android.util.Log;

import com.standardar.sensor.camera.SImageDataV1;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.DirectByteBufferNativeUtil;

import java.nio.ByteBuffer;

public class BodyDetect {
    private static final String TAG ="BodyDetectServer";

    private AssetManager mAssetManager;
    private String mPathToInternalDir;

    private long mTrackHandle;

    public BodyDetect(AssetManager assetManager, String pathToInternalDir) {
        Log.d(TAG,"BodyDetect(AssetManager assetManager, String pathToInternalDir)");
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }

    public int initBodyDetect(){
        Log.d(TAG,"initBodyDetect()");
        return 0;
    }

    public void startBodyDetect(){
        Log.d(TAG,"startBodyDetect()");
    }

    public void stopBodyDetect(){
        Log.d(TAG,"stopBodyDetect()");
    }

    public void destroyBodyDetect(){
        Log.d(TAG,"destroyBodyDetect()");
    }

    public void processFrameDirectly(SImageV1 sImageV1) {
        if (sImageV1 == null) {
            return;
        }
    }

    int[] getAllBodyId(){
        return null;
    }

    int getSkeletonPoint2dCount(int body_id){
        return 0;
    }

    public void getSkeletonPoint2d(int body_id,long out_point2d_ptr){

    }

    public void getSkeletonPoint2dConfidence(int body_id,long out_point2d_confidence_ptr){
    }
}
