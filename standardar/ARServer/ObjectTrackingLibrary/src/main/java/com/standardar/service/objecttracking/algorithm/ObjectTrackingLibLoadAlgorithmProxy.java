package com.standardar.service.objecttracking.algorithm;

import android.content.Context;
import android.content.res.AssetManager;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.CommandConstant;
import com.standardar.sensor.camera.CameraMetaData;
import com.standardar.sensor.camera.CameraSource;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.LogUtils;

import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ObjectTrackingLibLoadAlgorithmProxy extends AlgorithmProxy {
    private static final int ADD_OBJECT = 1;
    private Object mObjectTrackingLock = new Object();
    private boolean mObjectTrackingStart = false;
    private boolean mObjectTrackingInit = false;
    private ObjectTracking mObjectTracking;

    private ByteBuffer mReferenceObjectBuffer;

    protected Handler mObjectTrackingProxyHandler;
    protected HandlerThread mObjectTrackingProxyHandlerThread;
    private class ObjectTrackingProxyHandler extends Handler {
        public ObjectTrackingProxyHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case ADD_OBJECT: {
                    mObjectTracking.addObject(mReferenceObjectBuffer);
                    break;
                }
                default:
                    LogUtils.LOGW("unknown msg:" + msg.what);
                    break;
            }
        }
    }

    protected void startObjectTrackingProxyThread() {
        if (mObjectTrackingProxyHandlerThread != null) {
            return;
        }
        mObjectTrackingProxyHandlerThread = new HandlerThread("ObjectTrackingProxy");
        mObjectTrackingProxyHandlerThread.start();
        mObjectTrackingProxyHandler = new ObjectTrackingLibLoadAlgorithmProxy.ObjectTrackingProxyHandler(mObjectTrackingProxyHandlerThread.getLooper());
    }

    protected void stopObjectTrackingProxyThread() {
        if (mObjectTrackingProxyHandlerThread == null) {
            return;
        }
        mObjectTrackingProxyHandler.removeMessages(ADD_OBJECT);
        mObjectTrackingProxyHandlerThread.quitSafely();
        try {
            mObjectTrackingProxyHandlerThread.join();
            mObjectTrackingProxyHandlerThread = null;
            mObjectTrackingProxyHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public ObjectTrackingLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
        super(serverContext, sdkContext);
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
        if (mObjectTracking == null)
            mObjectTracking = new ObjectTracking(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());

        synchronized (mObjectTrackingLock) {
            CameraMetaData cameraMetaData = CameraSource.getInstance().getCameraMetaData(mAlgorithmMode);

            if (cameraMetaData == null) {
                LogUtils.LOGE("INIT OBJECT TRACKING ALGORITHM FAILED: cameraMetaData is null");
                return;
            }

            int ret = mObjectTracking.initObjectTracking(cameraMetaData.get(cameraMetaData.CAMERA_RGB_WIDTH),
                    cameraMetaData.get(cameraMetaData.CAMERA_RGB_HEIGHT));

            if (ret != 0) {
                LogUtils.LOGE("INIT ObjectTracking ALGORITHM FAILED:" + ret);
                return;
            }

            mObjectTrackingInit = true;
        }
        startObjectTrackingProxyThread();
    }

    private void stopAlgorithm() {
        synchronized (mObjectTrackingLock) {
            if (!mObjectTrackingInit) {
                return;
            }

            mObjectTrackingStart = false;
            mObjectTracking.resetObjectTracking();
        }
    }

    private void startAlgorithm() {
        synchronized (mObjectTrackingLock) {
            if (mObjectTrackingInit) {
                mObjectTrackingStart = true;
            }
        }
    }

    private void destroyAlgorithm() {
        synchronized (mObjectTrackingLock) {
            if (!mObjectTrackingInit) {
                return;
            }

            stopObjectTrackingProxyThread();

            mObjectTracking.destroyObjectTracking();
            mObjectTrackingStart = false;
            mObjectTrackingInit = false;
        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImage) {
        synchronized (mObjectTrackingLock) {
            if(mObjectTrackingStart) {
                mObjectTracking.processFrameDirectly(sImage);
            }
        }
    }

    public void setReferenceObjectDatabase(ByteBuffer buffer) {
        synchronized (mObjectTrackingLock) {
            if (!mObjectTrackingInit)
                return;
            LogUtils.LOGI("add object");
            if (mObjectTrackingProxyHandler != null) {
                mReferenceObjectBuffer = buffer;
                mReferenceObjectBuffer.order(ByteOrder.LITTLE_ENDIAN);
                mObjectTrackingProxyHandler.sendEmptyMessage(ADD_OBJECT);
            }
        }
    }

    @Override
    public void pushSensorData(float[] values, int tag, long timestamp) {
        if(!mObjectTrackingStart)
            return;

        mObjectTracking.pushSensorDirectly(values, tag, timestamp);
    }

    @Override
    public void updateAlgorithmResult() {
        if(mObjectTracking != null)
            mObjectTracking.updateAlgorithmResult();
    }

    public void getCenterPose(int object_id, long pose_ptr)
    {
        mObjectTracking.getCenterPose(object_id, pose_ptr);
    }

    public void getBoundingBox(int object_id, long vertex_ptr)
    {
        mObjectTracking.getBoundingBox(object_id, vertex_ptr);
    }

    public float getExtentX(int object_id)
    {
        return mObjectTracking.getExtentX(object_id);
    }

    public float getExtentY(int object_id)
    {
        return mObjectTracking.getExtentY(object_id);
    }

    public float getExtentZ(int object_id)
    {
        return mObjectTracking.getExtentZ(object_id);
    }

    public int[] getAllObjectId() {
        return mObjectTracking.getAllObjectId();
    }

    public String getObjectTrackingInfo() {
        if (mObjectTracking != null) {
            return mObjectTracking.getObjectTrackingInfo();
        }
        return "";
    }

    public void getViewMatrix(long view_ptr, int screenRotate) {
        mObjectTracking.getViewMatrix(view_ptr, screenRotate);
    }
}
