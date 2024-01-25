package com.standardar.service.facemesh;

import android.content.res.AssetManager;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.service.facedetectlibrary.FaceDetectClassloaderProcesser;

import java.nio.ByteBuffer;

public class FaceMesh {
    private AssetManager mAssetManager;
    private String mPathToInternalDir;

    public FaceMesh(AssetManager assetManager, String pathToInternalDir) {
        mAssetManager = assetManager;
        mPathToInternalDir = pathToInternalDir;
    }

    public int initFaceMesh(int width, int height, float fovH, float fovV) {
        return 0;
    }

    public void startFaceMesh() {

    }

    public void stopFaceMesh() {

    }

    public void destroyFaceMesh() {

    }

    public void processFrame(ByteBuffer buffer) {
    }

    public void processFrameDirectly(SImageV1 sImageV1) {
    }

    public int[] getAllFaceMeshId(){
        return null;
    }

    public int getVertexCount(int face_id){
        return 0;
    }

    public int getIndexCount(int face_id){
        return 0;
    }

    public int getTextureCoordinateCount(int face_id){
        return 0;
    }

    public int getNormalCount(int face_id){
        return 0;
    }

    public long getVertices(int face_id){
        return 0;
    }

    public long getIndices(int face_id){
        return 0;
    }

    public long getTextureCoordinates(int face_id){
        return 0;
    }

    public long getNormals(int face_id){
        return 0;
    }

    public void getPose(int face_id,long out_pose_ptr){
    }
}
