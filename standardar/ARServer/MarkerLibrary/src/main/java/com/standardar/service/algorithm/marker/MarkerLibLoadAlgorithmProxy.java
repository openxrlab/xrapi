package com.standardar.service.algorithm.marker;

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

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MarkerLibLoadAlgorithmProxy extends AlgorithmProxy {
    private static final int ADD_IMAGE = 1;
    private Object mMarkerLock = new Object();
    private boolean mMarkerStart = false;
    private boolean mMarkerInit = false;

    private Marker mMarker = null;

    private ByteBuffer mMarkerResult;
    private ByteBuffer mReferenceImageBuffer;

    protected Handler mMarkerProxyHandler;
    protected HandlerThread mMarkerProxyHandlerThread;
    private class MarkerProxyHandler extends Handler {
        public MarkerProxyHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case ADD_IMAGE: {
                    mMarker.addTargetImage(mReferenceImageBuffer);
                    break;
                }
                default:
                    LogUtils.LOGW("unknown msg:" + msg.what);
                    break;
            }
        }
    }

    protected void startMarkerProxyThread() {
        if (mMarkerProxyHandlerThread != null) {
            return;
        }
        mMarkerProxyHandlerThread = new HandlerThread("MarkerProxy");
        mMarkerProxyHandlerThread.start();
        mMarkerProxyHandler = new MarkerProxyHandler(mMarkerProxyHandlerThread.getLooper());
    }

    protected void stopMarkerProxyThread() {
        if (mMarkerProxyHandlerThread == null) {
            return;
        }
        mMarkerProxyHandler.removeMessages(ADD_IMAGE);
        mMarkerProxyHandlerThread.quitSafely();
        try {
            mMarkerProxyHandlerThread.join();
            mMarkerProxyHandlerThread = null;
            mMarkerProxyHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public MarkerLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
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
        if (mMarker == null)
            mMarker = new Marker(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());

        synchronized (mMarkerLock) {
            CameraMetaData cameraMetaData = CameraSource.getInstance().getCameraMetaData(mAlgorithmMode);

            if (cameraMetaData == null) {
                LogUtils.LOGE("INIT Marker ALGORITHM FAILED: cameraMetaData is null");
                return;
            }

            int ret = mMarker.initMarker(cameraMetaData.get(cameraMetaData.CAMERA_RGB_WIDTH),
                    cameraMetaData.get(cameraMetaData.CAMERA_RGB_HEIGHT),
                    cameraMetaData.get(cameraMetaData.CAMERA_FOVH));

            if (ret != 0) {
                LogUtils.LOGE("INIT Marker ALGORITHM FAILED:" + ret);
                return;
            }

            mMarkerInit = true;
        }
    }

    private void stopAlgorithm() {
        synchronized (mMarkerLock) {
            if (!mMarkerInit) {
                return;
            }
            mMarkerStart = false;
        }
        stopMarkerProxyThread();
    }

    private void startAlgorithm() {
        synchronized (mMarkerLock) {
            if (mMarkerInit) {
                mMarkerStart = true;
            }
        }
        startMarkerProxyThread();
    }

    private void destroyAlgorithm() {
        synchronized (mMarkerLock) {
            if (!mMarkerInit) {
                return;
            }

            stopMarkerProxyThread();
            mMarker.destroyMarker();
            mMarkerStart = false;
            mMarkerInit = false;

        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImage) {
        synchronized (mMarkerLock) {
            if(mMarkerStart) {
                    mMarker.processFrameDirectly(sImage);
            }
        }
    }

    @Override
    public void updateAlgorithmResult() {
        if(mMarker != null)
            mMarker.updateAlgorithmResult();
    }

    public void setReferenceImageDatabase(ByteBuffer buffer) {
        synchronized (mMarkerLock) {
            if (!mMarkerStart)
                return;
            LogUtils.LOGI("add marker image");
            if (mMarkerProxyHandler != null) {
                mReferenceImageBuffer = buffer;
                mReferenceImageBuffer.order(ByteOrder.LITTLE_ENDIAN);
                mMarkerProxyHandler.sendEmptyMessage(ADD_IMAGE);
            }
        }
    }

    public void getCenterPose(int marker_id, long view_matrix_ptr, int screenRotate, long pose_ptr)
    {
        mMarker.getCenterPose(marker_id, view_matrix_ptr, screenRotate, pose_ptr);
    }

    public void get2DCorners(int marker_id, long corners_ptr)
    {
        mMarker.getCorners2d(marker_id, corners_ptr);
    }

    public float getExtentX(int marker_id)
    {
        return mMarker.getExtentX(marker_id);
    }

    public float getExtentY(int marker_id)
    {
        return mMarker.getExtentY(marker_id);
    }

    public float getExtentZ(int marker_id)
    {
        return mMarker.getExtentZ(marker_id);
    }

    public int[] getAllMarkerId() {
        return mMarker.getAllMarkerId();
    }
}
