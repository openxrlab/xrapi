package com.standardar.algorithm;

import android.content.Context;
import com.standardar.sensor.camera.SImageV1;

import java.nio.ByteBuffer;

public class AlgorithmProxy {
    protected boolean mIsActive = false;
    protected long mResultPtr = 0;
    protected int mAlgorithmMode = -1;
    protected Context mServerContext;
    protected Context mSDKContext;
    protected int mDeviceType = 100;

    public AlgorithmProxy() {}

    public AlgorithmProxy(Context serverContext, Context sdkContext) {
        mServerContext = serverContext;
        mSDKContext = sdkContext;
    }

    public void setActive(boolean active) {
        mIsActive = active;
    }

    public boolean isActive() {
        return mIsActive;
    }

    public void sendCommand(int cmdid, Object... args) {}
    public void setValue(String key, Object value) {}
    public void setAlgorithmMode(int mode) {
        mAlgorithmMode = mode;
    }
    public void processResult(ByteBuffer buffer, int resLength) {}
    public void pushSensorData(float[] values, int tag, long timestamp) {}
    public void processFrameDirectly(SImageV1 sImageV1) {}
    public void setDeviceType(int type) {
        mDeviceType = type;
    }
    public void updateAlgorithmResult() {}
}
