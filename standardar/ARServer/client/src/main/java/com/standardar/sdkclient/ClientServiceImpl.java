package com.standardar.sdkclient;

import android.content.Context;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.AlgorithmConstant;
import com.standardar.common.CommandConstant;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.algorithm.marker.MarkerLibLoadAlgorithmProxy;
import com.standardar.service.bodydetect.BodyDetectLibLoadAlgorithmProxy;
import com.standardar.service.facemesh.FaceMeshLibLoadAlgorithmProxy;
import com.standardar.service.handgesture.HandGestureLibLoadAlgorithmProxy;
import com.standardar.service.slam.algorithm.SLAMLibLoadAlgorithmProxy;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ClientServiceImpl {
    private ExecutorService mExecutorService;
    protected Map<Integer, AlgorithmProxy> mAlgorithmProxys = new LinkedHashMap<>();
    public ClientServiceImpl(Context serverContext, Context sdkContext) {
        mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_SLAM, new SLAMLibLoadAlgorithmProxy(serverContext, sdkContext));
        mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_IMAGE_TRACKINNG, new MarkerLibLoadAlgorithmProxy(serverContext, sdkContext));
        //mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_OBJECT_TRACKING, new ObjectTrackingLibLoadAlgorithmProxy(serverContext, sdkContext));
        mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_GESTURE, new HandGestureLibLoadAlgorithmProxy(serverContext, sdkContext));
        mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_FACE_MESH, new FaceMeshLibLoadAlgorithmProxy(serverContext, sdkContext));
        mAlgorithmProxys.put(AlgorithmConstant.ALGORITHM_BODY_DETECT, new BodyDetectLibLoadAlgorithmProxy(serverContext, sdkContext));
        mExecutorService = Executors.newFixedThreadPool(mAlgorithmProxys.size());
    }

    public void initAlgorithm() {
        sendCommand(CommandConstant.COMMAND_COMMON_INIT_ALGORITHM);
    }

    public void stopAlgortihm(int algorithmType) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null && proxy.isActive()) {
            proxy.sendCommand(CommandConstant.COMMAND_COMMON_RESET_ALGORITHM, algorithmType);
        }
    }

    public void startAlgorithm(int algorithmType) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null && proxy.isActive()) {
            proxy.sendCommand(CommandConstant.COMMAND_COMMON_START_ALGORITHM, algorithmType);
        }
    }

    public void destoryAlgorithm() {
        sendCommand(CommandConstant.COMMAND_COMMON_DESTROY_ALGORITHM);
        disableAllAlgorithms();
    }

    public void pushSensorData(float[] values, int tag, long timestamp) {
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy != null && proxy.isActive()) {
                proxy.pushSensorData(values, tag, timestamp);
            }
        }
    }

    public void processFrame(SImageV1 sImage) {
        CountDownLatch latch = new CountDownLatch(mAlgorithmProxys.size());
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            ProcessRunnable processRunnable = new ProcessRunnable(sImage, entry.getValue(), latch);
            mExecutorService.execute(processRunnable);
        }

        try {
            latch.await();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private class ProcessRunnable implements Runnable {
        private SImageV1 mSImage;
        private AlgorithmProxy mProxy;
        private CountDownLatch mLatch;
        public ProcessRunnable(SImageV1 sImage, AlgorithmProxy proxy, CountDownLatch latch) {
            mSImage = sImage;
            mProxy = proxy;
            mLatch = latch;
        }

        @Override
        public void run() {
            if (mProxy.isActive()) {
                mProxy.processFrameDirectly(mSImage);
            }
            mLatch.countDown();
        }
    }

    public void setValue(String key, Object value) {
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy != null) {
                proxy.setValue(key, value);
            }
        }
    }

    public void sendCommand(int cmdId, Object... args) {
        for(Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy.isActive()) {
                proxy.sendCommand(cmdId, args);
            }
        }
    }

    public void enableAlgorithm(int algorithmType) {
        AlgorithmProxy algorithmProxy = mAlgorithmProxys.get(algorithmType);
        if (algorithmProxy != null) {
            algorithmProxy.setActive(true);
        }
    }

    public void disableAlgorithm(int algorithmType) {
        AlgorithmProxy algorithmProxy = mAlgorithmProxys.get(algorithmType);
        if (algorithmProxy != null) {
            algorithmProxy.setActive(false);
        }
    }

    public void setAlgorithmStreamMode(int algorithmType, int algorithmStreamMode) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null) {
            proxy.setAlgorithmMode(algorithmStreamMode);
        }
    }

    public void disableAllAlgorithms() {
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy != null) {
                proxy.setActive(false);
            }
        }
    }

    public void setDeviceType(int algorithmType, int deviceType) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null) {
            proxy.setDeviceType(deviceType);
        }
    }

    public AlgorithmProxy getAlgorithmProxy(int type) {
        return mAlgorithmProxys.get(type);
    }

    public void updateAlgorithmResult() {
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy != null && proxy.isActive()) {
                proxy.updateAlgorithmResult();
            }
        }
    }


}
