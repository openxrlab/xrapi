package com.standardar.service.facedetectlibrary;
import android.content.res.AssetManager;
import java.nio.ByteBuffer;
public class FaceDetectClassloaderProcesser  {
    AssetManager mAssetManager;
    public FaceDetectClassloaderProcesser(AssetManager assetManager , String pathToInternalDir) {
        mAssetManager = assetManager;
    }
    public int getResultLength() {
        return 0;
    }
    public long getResultCallbackHandler() {
        return 0;
    }

    public void initFaceDetect(String modePath) { }

    public FaceInfo processFrameBuffer(ByteBuffer buffer) {
        return null;
    }

    public FaceInfo processImgBuffer(ByteBuffer buffer,int width,int height,int stride,int cameraStreamType,int screenRotate) {
        return null;
    }
    public void destroyAlgorithm() { }

}
