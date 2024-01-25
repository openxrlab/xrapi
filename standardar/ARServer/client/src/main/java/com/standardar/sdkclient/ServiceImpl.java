package com.standardar.sdkclient;

import android.content.Context;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.FrameworkConstant;
import com.standardar.sensor.camera.SImageV1;

import java.util.LinkedHashMap;
import java.util.Map;

public class ServiceImpl {

    protected Context mContext;
    protected int mAlgorithmState = FrameworkConstant.ALGORITHM_STATE_DESTROY;
    protected Map<Integer, AlgorithmProxy> mAlgorithmProxys = new LinkedHashMap<>();
    public ServiceImpl(Context context) {
        mContext = context;
    }

    protected void initAlgorithm() {
    }

    protected void stopAlgortihm(int algorithmType) {
    }

    protected void startAlgorithm(int algorithmType) {
    }

    protected int destoryAlgorithm() {
        return 0;
    }

    protected void pushSensorData(float[] values, int tag, long timestamp) {}

    protected void processFrame(SImageV1 sImage) {}



    protected void setValue(String key, Object value) {}

    protected void enableAlgorithm(int algorithmType, long resultPtr) {
        AlgorithmProxy algorithmProxy = mAlgorithmProxys.get(algorithmType);
        if (algorithmProxy != null) {
            algorithmProxy.setActive(true);
        }
    }

    protected void disableAlgorithm(int algorithmType) {
        AlgorithmProxy algorithmProxy = mAlgorithmProxys.get(algorithmType);
        if (algorithmProxy != null) {
            algorithmProxy.setActive(false);
        }
    }

    protected void setAlgorithmStreamMode(int algorithmType, int algorithmStreamMode) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null) {
            proxy.setAlgorithmMode(algorithmStreamMode);
        }
    }

    protected void disableAllAlgorithms() {
        for (Map.Entry<Integer, AlgorithmProxy> entry : mAlgorithmProxys.entrySet()) {
            AlgorithmProxy proxy = entry.getValue();
            if (proxy != null) {
                proxy.setActive(false);
            }
        }
    }

    protected void setDeviceType(int algorithmType, int deviceType) {
        AlgorithmProxy proxy = mAlgorithmProxys.get(algorithmType);
        if (proxy != null) {
            proxy.setDeviceType(deviceType);
        }
    }
}
