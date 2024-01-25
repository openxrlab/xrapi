package com.standardar.service.bodydetect;

import android.content.Context;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.CommandConstant;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.LogUtils;

public class BodyDetectLibLoadAlgorithmProxy extends AlgorithmProxy {
    private Object mBodyDetectLock = new Object();
    private boolean mBodyDetectStart = false;
    private boolean mBodyDetectInit = false;

    private Context mServerContext;
    private BodyDetect mBodyDetect;


    public BodyDetectLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
        mServerContext = serverContext;
        mSDKContext = sdkContext;
        mBodyDetect = new BodyDetect(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());
    }


    @Override
    public void sendCommand(int cmdid, Object... args) {
        switch (cmdid) {
            case CommandConstant.COMMAND_COMMON_INIT_ALGORITHM: {
                initAlgorithm();
                break;
            }
            case CommandConstant.COMMAND_COMMON_RESET_ALGORITHM: {
                stopAlgorithm();
                break;
            }
            case CommandConstant.COMMAND_COMMON_START_ALGORITHM: {
                startAlgorithm();
                break;
            }
            case CommandConstant.COMMAND_COMMON_DESTROY_ALGORITHM: {
                destroyAlgorithm();
                break;
            }

            default:
                break;
        }
    }

    private void initAlgorithm() {
        LogUtils.LOGI("=========BodyDetect==initAlgorithm =====");

        synchronized (mBodyDetectLock) {

            mBodyDetect.initBodyDetect();
            mBodyDetectInit = true;
        }

    }

    private void stopAlgorithm() {
        LogUtils.LOGI("=========BodyDetect==stopAlgorithm =====");

        synchronized (mBodyDetectLock) {
            if (!mBodyDetectInit) {
                return;
            }

            mBodyDetect.stopBodyDetect();
            mBodyDetectStart = false;
        }
    }

    private void startAlgorithm() {
        LogUtils.LOGI("=========BodyDetect==startAlgorithm =====");

        synchronized (mBodyDetectLock) {
            if (mBodyDetectInit) {
                mBodyDetectStart = true;
                mBodyDetect.startBodyDetect();
            }
        }
    }

    private void destroyAlgorithm() {
        LogUtils.LOGI("=========BodyDetect==destroyAlgorithm =====");

        synchronized (mBodyDetectLock) {
            if (!mBodyDetectInit) {
                return;
            }
            mBodyDetect.destroyBodyDetect();
            mBodyDetectStart = false;
            mBodyDetectInit = false;

        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImageV1) {
        synchronized (mBodyDetectLock) {
            if (mBodyDetectStart) {
                mBodyDetect.processFrameDirectly(sImageV1);
            }
        }


    }


    public int[] getAllBodyId() {
        return mBodyDetect.getAllBodyId();
    }


    public int getSkeletonPoint2dCount(int body_id) {


       return mBodyDetect.getSkeletonPoint2dCount(body_id);
    }


    public void getSkeletonPoint2d(int body_id,long out_point2d_ptr){
        mBodyDetect.getSkeletonPoint2d(body_id,out_point2d_ptr);
    }

    public void getSkeletonPoint2dConfidence(int body_id,long out_point2d_confidence_ptr){
        mBodyDetect.getSkeletonPoint2dConfidence(body_id,out_point2d_confidence_ptr);
    }

}
