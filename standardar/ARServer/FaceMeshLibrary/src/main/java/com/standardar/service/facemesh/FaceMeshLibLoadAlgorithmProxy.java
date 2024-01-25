package com.standardar.service.facemesh;

import android.content.Context;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.CommandConstant;
import com.standardar.sensor.camera.CameraMetaData;
import com.standardar.sensor.camera.CameraSource;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.common.util.LogUtils;

public class FaceMeshLibLoadAlgorithmProxy extends AlgorithmProxy {
    private Object mFaceMeshLock = new Object();
    private boolean mFaceMeshStart = false;
    private boolean mFaceMeshInit = false;
    private FaceMesh mFaceMesh;
    private Context mServerContext;


    public FaceMeshLibLoadAlgorithmProxy(Context serverContext, Context sdkContext) {
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
        synchronized (mFaceMeshLock) {
            CameraMetaData cameraMetaDataProxy = CameraSource.getInstance().getCameraMetaData(mAlgorithmMode);


            if (cameraMetaDataProxy == null) {
                LogUtils.LOGE("INIT FaceMesh ALGORITHM FAILED: cameraMetaDataProxy is null");
                return;
            }

            mFaceMesh = new FaceMesh(mServerContext.getAssets(), mSDKContext.getFilesDir().getAbsolutePath());
            int ret = mFaceMesh.initFaceMesh(cameraMetaDataProxy.get(CameraMetaData.CAMERA_RGB_WIDTH),
                    cameraMetaDataProxy.get(CameraMetaData.CAMERA_RGB_HEIGHT), cameraMetaDataProxy.get(CameraMetaData.CAMERA_FOVH),
                    cameraMetaDataProxy.get(CameraMetaData.CAMERA_FOVV));
            if (ret != 0) {
                LogUtils.LOGE("INIT FaceMesh ALGORITHM FAILED:" + ret);
                return;
            }

            mFaceMeshInit = true;

        }

    }


    private void startAlgorithm() {
        synchronized (mFaceMeshLock) {
            if (mFaceMeshInit) {
                mFaceMeshStart = true;
                mFaceMesh.startFaceMesh();
            }
        }
    }


    private void stopAlgorithm() {
        synchronized (mFaceMeshLock) {
            if (!mFaceMeshInit) {
                return;
            }
            mFaceMesh.stopFaceMesh();
            mFaceMeshStart = false;
        }
    }


    private void destroyAlgorithm() {
        synchronized (mFaceMeshLock) {
            if (!mFaceMeshInit) {
                return;
            }

            mFaceMesh.destroyFaceMesh();
            mFaceMeshStart = false;
            mFaceMeshInit = false;
        }
    }

    @Override
    public void processFrameDirectly(SImageV1 sImageV1) {
        synchronized (mFaceMeshLock) {
            if (mFaceMeshStart) {
                mFaceMesh.processFrameDirectly(sImageV1);
            }
        }


    }

    public int[] getAllFaceMeshId() {
        if(mFaceMesh==null){
            return null;
        }
        return mFaceMesh.getAllFaceMeshId();
    }

    public int getVertexCount(int face_id){

        return mFaceMesh.getVertexCount(face_id);
    }

    public int getIndexCount(int face_id){

        return mFaceMesh.getIndexCount(face_id);
    }


    public int getTextureCoordinateCount(int face_id){

        return mFaceMesh.getTextureCoordinateCount(face_id);
    }

    public int getNormalCount(int face_id){

        return mFaceMesh.getNormalCount(face_id);
    }


    public long getVertices(int face_id){
        return   mFaceMesh.getVertices(face_id);
    }

    public long getIndices(int face_id){
        return mFaceMesh.getIndices(face_id);
    }


    public long getTextureCoordinates(int face_id){
        return mFaceMesh.getTextureCoordinates(face_id);
    }

    public long getNormals(int face_id){
        return mFaceMesh.getNormals(face_id);
    }

    public void getPose(int face_id,long out_pose_ptr){
        mFaceMesh.getPose(face_id,out_pose_ptr);
    }

}
