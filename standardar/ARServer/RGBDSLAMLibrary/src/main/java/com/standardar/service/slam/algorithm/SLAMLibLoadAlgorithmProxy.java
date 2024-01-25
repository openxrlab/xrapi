package com.standardar.service.slam.algorithm;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.util.Log;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.AlgorithmConstant;
import com.standardar.common.CameraConstant;
import com.standardar.common.CommandConstant;
import com.standardar.common.FrameworkConstant;
import com.standardar.sensor.camera.CameraSource;
import com.standardar.sensor.camera.CameraMetaData;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.LogUtils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;

public class SLAMLibLoadAlgorithmProxy extends AlgorithmProxy {
    private int mAxisUpMode = 0;

    private Object mSLAMLock = new Object();
    private boolean mSLAMStart = false;
    private boolean mSLAMInit = false;

    private SLAM mSLAM;

    // thread to run main control commands: init, start, stop, and destroy
    private Handler mCommandHandler;
    private HandlerThread mCommandHandlerThread;

    private ByteBuffer mIntrinsicsBuffer = ByteBuffer.allocateDirect(
        Integer.BYTES * 6 + Float.BYTES * 8 + Long.BYTES * 2).order(ByteOrder.LITTLE_ENDIAN);

    public SLAMLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
        super(serverContext, sdkContext);
    }

    protected void startCommandThread() {
        if (mCommandHandlerThread != null) {
            return;
        }

        mCommandHandlerThread = new HandlerThread("control commands thread");
        mCommandHandlerThread.start();
        mCommandHandler =  new Handler(mCommandHandlerThread.getLooper()) {
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what) {
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
                }
            }
        };
    }

    protected void stopCommandThread() {
        if (mCommandHandlerThread == null) {
            return;
        }
        mCommandHandler.removeMessages(CommandConstant.COMMAND_COMMON_INIT_ALGORITHM);
        mCommandHandler.removeMessages(CommandConstant.COMMAND_COMMON_RESET_ALGORITHM);
        mCommandHandler.removeMessages(CommandConstant.COMMAND_COMMON_START_ALGORITHM);
        mCommandHandler.removeMessages(CommandConstant.COMMAND_SLAM_CHANGE_LIGHT_MODE);
        mCommandHandler.removeMessages(CommandConstant.COMMAND_OBJECT_TRACKINNG_ADD_OBJECT);
        mCommandHandlerThread.quitSafely();
        try {
            mCommandHandlerThread.join();
            mCommandHandlerThread = null;
            mCommandHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void sendCommand(int cmdid, Object... args) {
        switch (cmdid) {
            case CommandConstant.COMMAND_COMMON_INIT_ALGORITHM: {
                startCommandThread();
                callHandleMain(cmdid);
                break;
            }
            case CommandConstant.COMMAND_COMMON_RESET_ALGORITHM: {
                callHandleMain(cmdid);
                break;
            }
            case CommandConstant.COMMAND_COMMON_START_ALGORITHM: {
                callHandleMain(cmdid);
                break;
            }
            case CommandConstant.COMMAND_COMMON_DESTROY_ALGORITHM: {
                callHandleMain(cmdid);
                stopCommandThread();
                break;
            }
            case CommandConstant.COMMAND_SLAM_CHANGE_LIGHT_MODE: {
                Message msg = Message.obtain();
                msg.what = cmdid;
                msg.arg1 = (int) args[0];
                if(mCommandHandler != null)
                    mCommandHandler.sendMessage(msg);
                break;
            }
            case CommandConstant.COMMAND_SLAM_PUSH_KEY_FRAME: {
                LogUtils.LOGI("push key frame in algorithm proxy");
                pushKeyFrame((byte[])args[0], (long)args[1], (int)args[2], (int)args[3],
                             (double[])args[4], (double[])args[5]);
                break;
            }
            case CommandConstant.COMMAND_SLAM_RUN_DENSE_RECON: {
                LogUtils.LOGI("run dense recon in algorithm proxy");
                break;
            }
            case CommandConstant.COMMAND_SLAM_SET_RESUME_FLAG: {
                setResumeFlag();
                break;
            }
            case CommandConstant.COMMAND_SLAM_SET_WINDOW: {
                setWindow((long)args[0], (long)args[1], (boolean)args[2]);
                break;
            }
            case CommandConstant.COMMAND_SLAM_BEGIN_RENDER_EYE: {
                beginRenderEye((int)args[0]);
                break;
            }
            case CommandConstant.COMMAND_SLAM_END_RENDER_EYE: {
                endRenderEye((int)args[0]);
                break;
            }
            case CommandConstant.COMMAND_SLAM_SUBMIT_FRAME: {
                submitFrame((int)args[0], (int)args[1]);
                break;
            }
            case CommandConstant.COMMAND_COMMON_AXISUPMODE: {
                mAxisUpMode = (int) args[0];
                break;
            }
            default:
                break;
        }
    }

    private void callHandleMain(int cmdid) {
        Message msg = Message.obtain();
        msg.what = cmdid;
        if(mCommandHandler != null)
            mCommandHandler.sendMessage(msg);
    }

    private void initAlgorithm() {
        if (mSLAM == null) {
            mSLAM = new SLAM(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());
        }
        synchronized (mSLAMLock) {
            CameraMetaData cameraMetaData = CameraSource.getInstance().getCameraMetaData(mAlgorithmMode);
            if(cameraMetaData == null) {
                LogUtils.LOGE("INIT SLAM ALGORITHM FAILED: cameraMetaDataProxy is null");
                return;
            }

            int ret = mSLAM.initSLAM(mServerContext.getAssets(),
                        mSDKContext.getFilesDir().getAbsolutePath(), mSDKContext.getPackageName(),
                        cameraMetaData.get(cameraMetaData.CAMERA_RGB_WIDTH), cameraMetaData.get(cameraMetaData.CAMERA_RGB_HEIGHT),
                        cameraMetaData.get(cameraMetaData.CAMERA_FOVH), mAlgorithmMode);
            if (ret == 0) {
                LogUtils.LOGE("INIT SLAM ALGORITHM FAILED:" + ret);
                return;
            }

            mSLAM.getImageIntrinsics(mIntrinsicsBuffer);
            mSLAM.setAxisUpMode(mAxisUpMode);
            mSLAMInit = true;
        }
    }

    private void stopAlgorithm() {
        synchronized (mSLAMLock) {
            if (!mSLAMInit) {
                return;
            }
            mSLAMStart = false;
        }
    }

    private void startAlgorithm() {
        synchronized (mSLAMLock) {
            if (mSLAMInit) {
                mSLAMStart = true;
            }
        }
    }

    private void destroyAlgorithm() {
        synchronized (mSLAMLock) {
            if (!mSLAMInit) {
                return;
            }
            mSLAM.destroySLAM();
            mSLAMStart = false;
            mSLAMInit = false;
        }
    }

    @Override
    public void setValue(String key, Object value) {
        switch (key) {
            default:
                break;
        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImage) {
        if(mSLAMStart) {
            mSLAM.processFrameDirectly(sImage);
        }
    }

    @Override
    public void pushSensorData(float[] values, int tag, long timestamp) {
        if(!mSLAMStart)
            return;
        mSLAM.pushSensorDirectly(values, tag, timestamp);
    }

    @Override
    public void updateAlgorithmResult() {
        if (!mSLAMInit || !mSLAMStart) {
            return;
        }

        synchronized (mSLAMLock) {
            if (!mSLAMStart) {
                return;
            }
            if (mSLAM != null) {
                mSLAM.updateAlgorithmResult();
            }
        }
    }

    public ByteBuffer getCameraIntrinsics() {
        if (!mSLAMStart) {
            return mIntrinsicsBuffer;
        }
        if (mSLAMInit) {
            return mIntrinsicsBuffer;
        }
        return null;
    }

    public int getTrackingState() {
        if (!mSLAMStart) {
            return -1;
        }

        if (mSLAM != null) {
            return mSLAM.getTrackingState();
        }

        return -1;
    }

    public void getViewMatrix(long view_ptr, int screenRotate) {
        if (!mSLAMStart) {
            return;
        }
        mSLAM.getViewMatrix(view_ptr, screenRotate);
    }

    public float[] insectPlaneWithSlamResult(float[] rayOrigin, float[] rayDirection, int[] planeId, float[] quat) {
        if (!mSLAMStart) {
            return new float[0];
        }
        return mSLAM.insectPlaneWithSlamResult(rayOrigin, rayDirection, planeId, quat);
    }

    public int getNumberLandMarks() {
        if (!mSLAMStart) {
            return 0;
        }
        return mSLAM.getNumberLandMarks();
    }

    public long getLandMarks() {
        if (!mSLAMStart) {
            return 0;
        }
        return mSLAM.getLandMarks();
    }

    public float getDistanceFromCamera(float[] histPose) {
        if (!mSLAMStart) {
            return 0;
        }
        return mSLAM.getDistanceFromCamera(histPose);
    }

    public byte[] predictPose(long timestamp) {
        return null;
    }

    public int pushKeyFrame(byte[] imgBuff, long timestamp, int width, int height,
                            double[] rot, double[] trans) {
        return 0;
    }

    public int runDenseRecon() {
        return 0;
    }

    public byte[] getDenseReconResult() {
        return null;
    }

    public float[] getPredictedViewMatrix() {
        return null;
    }

    public float[] getProjectionMatrixSeeThrough(float near, float far) {
        return null;
    }

    public void setWindow(long nativePtr, long appContext, boolean newPbuffer) {
    }

    public void beginRenderEye(int eyeId) {
    }

    public void endRenderEye(int eyeId) {
    }

    public void submitFrame(int leftEyeTexId, int rightEyeTexId) {
    }

    public void setResumeFlag() {
    }

    public void setBoundingBox(long vertex_ptr, long matrix_ptr) {
    }

    public void getScanningBoundingBoxPointCloud(long points_ptr, long points_num_ptr) {
    }

    public void getScanningResult(long bytes_ptr, long bytes_size_ptr){
    }

    public String getSLAMInfo() {
        return "";
    }

    public void getDenseMeshIndex(long nativeIndexArray) {
    }

    public int getDenseMeshIndexCount() {
        return 0;
    }

    public int getSLAMPlaneIndex(int planeid) {
        return 0;
    }

    public float getSLAMLightIntensity() {
        return 0;
    }

    public float[] getPlaneCenter(int planeId) {
        return null;
    }

    public void getDenseMeshVertex(long vertexArrayPtr) {
    }

    public int getDenseMeshVertexCount() {
        return 0;
    }

    public void getDenseMeshNormal(long vertexNormalPtr) {
    }
    public int getDenseMeshFormat() {
        return -1;
    }

    public float[] getCenterPose(int planeId) {
        return null;
    }

    public float[] insectSurfaceMesh(float x, float y, float[] normal, float screenAspect, int screenRotate) {
        return null;
    }

    public int getPlaneVertexCount() {
        return 0;
    }

    public void getPlaneVertexArray(long vertexArrayPtr) {
    }

    public int getPlaneIndexCount() {
        return 0;
    }

    public void getPlaneIndexArray(long indexArrayPtr) {
    }

    public int getPlaneType(int planeId) {
        return -1;
    }

    public void getPlaneNormal(long planeNormalPtr, int planeId) {
    }

    public int getPolygonSize(int planeId) {
        return 0;
    }

    public void getPolygon(long outDataPtr, int planeId) {
    }

    public float getExtent(int axis, int planeId) {
        return 0;
    }

    public int getPolygon3DSize(int planeId) {
        return 0;
    }

    public void getPolygon3D(long outDataPtr, int planeId) {
    }

    public int isPoseInPolygon(float[] pose, int planeId) {
        return 0;
    }

    public float[] getPlaneOriginPoint(int plane_id) {
        return null;
    }

    public int isPoseInExtents(float[] pose, int planeId) {
        return 0;
    }
    public int[] getAllPlaneId() {
        return null;
    }

    public boolean isDepthImageAvaliable(){
        return false;
    }

    public boolean getDepthImage(long depth_ptr) {
        return false;
    }

    public int getDepthImageWidth() {
        return 0;
    }

    public int getDepthImageHeight() {
        return 0;
    }

    public int getCloudResult() {
        return 1;
    }

    public String getMapId() {
        return "";
    }

    public String getAnchorId() {
        return "";
    }

    public float[] getCloudAnchorPose() {
        return null;
    }

    public int getLightEstimateState() {
        return 0;
    }

    public int getLightEstimateMode() {
        return 0;
    }

    public float getPixelIntensity() {
        return 0;
    }

    public float[] getSphericalHarmonicLighting() {
        return null;
    }

    public int[] getEnvironmentTextureSize() {
        return null;
    }

    public float[] getColorCorrection() {
        return null;
    }

    public float[] getEnvironmentalHdrMainLightDirection() {
        return null;
    }

    public float[] getEnvironmentalHdrMainLightIntensity() {
        return null;
    }

    public float[] getEnvironmentalHdrCubemap() {
        return null;
    }

    public long getLightTimeStamp() {
        return 0;
    }

    public void setReferenceObjectDatabase(ByteBuffer buffer) {
    }

    public void getObjectCenterPose(int object_id, long pose_ptr) {
    }

    public void getBoundingBox(int object_id, long vertex_ptr) {
    }

    public float getExtentX(int object_id) {
        return 0;
    }

    public float getExtentY(int object_id) {
        return 0;
    }

    public float getExtentZ(int object_id) {
        return 0;
    }

    public int[] getAllObjectId() {
        return null;
    }

    public String getObjectTrackingInfo() {
        return "";
    }

    public String getRegionInfo() {
        return "";
    }

}


