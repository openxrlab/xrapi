package com.standardar.service.handgesture;

import android.content.Context;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.CommandConstant;
import com.standardar.sensor.camera.CameraMetaData;
import com.standardar.sensor.camera.CameraSource;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.LogUtils;

import java.nio.ByteBuffer;

public class HandGestureLibLoadAlgorithmProxy extends AlgorithmProxy {

    private Object mHandGestureLock = new Object();
    private boolean mHandGestureStart = false;
    private boolean mHandGestureInit = false;

    private ByteBuffer mHandGestureResult;

    private int m_ScreenRotate;
    private int m_VideoHeight;
    private int m_VideoWidth;

    private Context mServerContext;

    private HandGesture mHandGesture;


    public HandGestureLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
        mServerContext = serverContext;
        mSDKContext = sdkContext;
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
        synchronized (mHandGestureLock) {

            CameraMetaData cameraMetaDataProxy = CameraSource.getInstance().getCameraMetaData(mAlgorithmMode);

            if (cameraMetaDataProxy == null) {
                LogUtils.LOGE("INIT HandGesture ALGORITHM FAILED: cameraMetaDataProxy is null");
                return;
            }

            mHandGesture = new HandGesture(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());
            m_VideoWidth = cameraMetaDataProxy.get(cameraMetaDataProxy.CAMERA_RGB_WIDTH);
            m_VideoHeight = cameraMetaDataProxy.get(cameraMetaDataProxy.CAMERA_RGB_HEIGHT);
            m_ScreenRotate = cameraMetaDataProxy.get(cameraMetaDataProxy.SCREEN_ROTATE);

            int ret = mHandGesture.initHandGesture(m_VideoWidth, m_VideoHeight, m_ScreenRotate);

            if (ret != 0) {
                LogUtils.LOGE("INIT HandGesture ALGORITHM FAILED:" + ret);
                return;
            }

            mHandGestureInit = true;

        }
    }

    private void stopAlgorithm() {
        synchronized (mHandGestureLock) {
            if (!mHandGestureInit) {
                return;
            }

            mHandGesture.stopHandGestureAlgorithm();
            mHandGestureStart = false;
        }
    }

    private void startAlgorithm() {
        synchronized (mHandGestureLock) {
            if (mHandGestureInit) {
                mHandGestureStart = true;

                mHandGesture.startHandGestureAlgorithm();
            }
        }
    }

    private void destroyAlgorithm() {
        synchronized (mHandGestureLock) {
            if (!mHandGestureInit) {
                return;
            }

            mHandGesture.destroyHandGesture();
            mHandGestureStart = false;
            mHandGestureInit = false;
        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImageV1) {
        super.processFrameDirectly(sImageV1);
        synchronized (mHandGestureLock) {
            if (mHandGestureStart) {
                mHandGesture.processFrameDirectly(sImageV1);
            }
        }
    }

    public int[] getAllHandGestureId() {
        return mHandGesture.getAllHandGestureId();
    }

    int getHandType(int hand_id){

        return mHandGesture.getHandType(hand_id);
    }

    int getHandSide(int hand_id){

        return mHandGesture.getHandSide(hand_id);
    }

    int getHandToward(int hand_id){

        return mHandGesture.getHandToward(hand_id);
    }

    float getHandTypeConfidence(int hand_id){

        return mHandGesture.getHandTypeConfidence(hand_id);
    }


    int getLandMark2DCount(int hand_id){

        return mHandGesture.getLandMark2DCount(hand_id);
    }

    void getLandMark2DArray(int hand_id ,long points2dPtr){
        mHandGesture.getLandMark2DArray(hand_id,points2dPtr);

    }
}
