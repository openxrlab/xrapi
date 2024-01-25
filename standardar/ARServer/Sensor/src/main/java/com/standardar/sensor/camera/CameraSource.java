package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.os.Build;
import android.view.Display;
import android.view.WindowManager;

import com.standardar.common.CameraConstant;
import com.standardar.service.common.util.LogUtils;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class CameraSource {
    private Context mContext;
    private static CameraSource mInstance;
    private static Object mInstanceLock = new Object();
    private int mTextureId = 0;
    private ICameraNotifyCallback mCameraNotify;
    private Map<Integer, CameraBase> mCameraList = new LinkedHashMap<>();
    private Integer mRenderCameraID;
    private SImageV1 mCurrentSImage;
    public interface ICameraNotifyCallback {
        void onCameraNotify(SImageV1 sImage);
    }

    public void setCameraNotify(ICameraNotifyCallback callback) {
        mCameraNotify = callback;
    }

    public void init(Context context) {
        mContext = context;
    }

    public static CameraSource getInstance() {
        synchronized (mInstanceLock) {
            if (mInstance != null) {
                return mInstance;
            } else {
                mInstance = new CameraSource();
                return mInstance;
            }
        }
    }

    private CameraSource() {}

    public void setContext(Context context) {
        mContext = context;
    }

    public void setTextureId(int texid) {
        mTextureId = texid;
        LogUtils.LOGI("set texture id " + mTextureId);
        ByteBuffer buffer = ByteBuffer.allocate(Integer.BYTES);
        buffer.putInt(texid);
        buffer.rewind();
        mCameraList.get(mRenderCameraID).doCommand(CameraBase.CMD_OES_TEXID_SET, buffer);
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    private void createCamera(int cameraId) {
        CameraBase camera = CameraFactory.createCamera(cameraId, mContext);
        camera.registerFrameAvailableListener(new CameraBase.FrameAvailableListener() {
            @Override
            public void onImageAvailable(SImageV1 sImage) {
                if (mCameraNotify != null) {
                    mCameraNotify.onCameraNotify(sImage);
                    mCurrentSImage = sImage;
                    mCurrentSImage.mImageDataList.get(0).mImageData.rewind();
                    CameraUtils.copyImageBuffer(mCurrentSImage.mImageDataList.get(0).mImageData, mCurrentSImage.mImageDataList.get(0).mWidth,
                            mCurrentSImage.mImageDataList.get(0).mHeight, mCurrentSImage.mImageDataList.get(0).mStride, mCurrentSImage.mImageDataList.get(0).mImageFormat,
                            mCurrentSImage.mImageDataList.get(0).mTimeStamp);
                }
            }
        });
        mCameraList.put(cameraId, camera);
    }

    public void openCamera(int[] cameraId) {
        for(int id:cameraId) {
            createCamera(id);
            mCameraList.get(id).openCamera();
        }
        LogUtils.LOGI("open camera " + Arrays.toString(cameraId));
    }

    public void setRenderCamera(int cameraId){
        mRenderCameraID = cameraId;
    }

    public void startPreview() {
        for (Map.Entry<Integer, CameraBase> entry : mCameraList.entrySet()) {
            entry.getValue().startPreview();
        }
    }

    public void stopPreview() {
        for (Map.Entry<Integer, CameraBase> entry : mCameraList.entrySet()) {
            if (entry.getValue() != null) {
                entry.getValue().stopPreview();
            }
        }
    }

    public void closeCamera() {
        for (Map.Entry<Integer, CameraBase> entry : mCameraList.entrySet()) {
            if (entry.getValue() != null) {
                entry.getValue().closeCamera();
            }
        }
        mCameraList.clear();
        mContext = null;
    }

    public int getTotalImageLength(){
        int length = 0;
        for (Map.Entry<Integer, CameraBase> entry : mCameraList.entrySet()) {
            length += entry.getValue().getCameraMetaData().get(CameraMetaData.CAMERA_IMAGE_BUFFER_SIZE);
        }
        return length;
    }

    public CameraMetaData getCameraMetaData(int cameraid) {
        if(mCameraList.get(cameraid) != null) {
            return mCameraList.get(cameraid).getCameraMetaData();
        }
        return null;
    }

    public static void saveYUVtoPicture(byte[] data,int width,int height) throws IOException {
        FileOutputStream outStream = null;
        File file = new File("/sdcard/test/");
        if(!file.exists()){
            file.mkdir();
        }

        try {
            YuvImage yuvimage = new YuvImage(data, ImageFormat.NV21, width, height, null);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            yuvimage.compressToJpeg(new Rect(0, 0,width, height), 80, baos);
            Bitmap bmp = BitmapFactory.decodeByteArray(baos.toByteArray(), 0, baos.toByteArray().length);

            outStream = new FileOutputStream(
                    String.format("/sdcard/test/%d_%s_%s.jpg",
                            System.currentTimeMillis(),String.valueOf(width),String.valueOf(height)));
            bmp.compress(Bitmap.CompressFormat.JPEG, 85, outStream);
            outStream.write(baos.toByteArray());
            outStream.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
        }
    }

    public void setScreenRotate(int screenRotate) {
        for (Map.Entry<Integer, CameraBase> entry : mCameraList.entrySet()) {
            if (entry.getValue() != null) {
                entry.getValue().setScreenRotate(screenRotate);
            }
        }
    }

    public void setPreviewSize(int width, int height) {
        mCameraList.get(mRenderCameraID).setPreviewSize(width, height);
//        arSetFov(mEnginePtr, getCameraMetaData(mRenderCameraID).get(CameraMetaData.CAMERA_FOVH), getCameraMetaData(mRenderCameraID).get(CameraMetaData.CAMERA_FOVV));
    }

    public void updateTexture() {
        CameraBase camera = mCameraList.get(mRenderCameraID);
        if (camera != null)
            camera.doCommand(CameraBase.CMD_SURFACETEXTURE_UPDATE, null);
    }

    public void updateSurfaceTextureTimestamp() {
        CameraBase camera = mCameraList.get(mRenderCameraID);
        if(camera != null)
            camera.doCommand(CameraBase.CMD_GET_SURFACETEXTURE_TIMESTAMP, null);
    }

    public void waitForNextFrame() {
        CameraBase camera = mCameraList.get(mRenderCameraID);
        if(camera != null)
            camera.doCommand(CameraBase.CMD_SURFACETEXTURE_WAIT, null);
    }

    public float getFovH() {
        return getCameraMetaData(mRenderCameraID).get(CameraMetaData.CAMERA_FOVH);
    }

    public float getFovV() {
        return getCameraMetaData(mRenderCameraID).get(CameraMetaData.CAMERA_FOVV);
    }

    public String getSupportSizeString() {
        String ret = getCameraMetaData(mRenderCameraID).get(CameraMetaData.CAMERA_SUPPORT_SIZE);
        return ret == null ? "" : ret;
    }

    public Point getDisplaySize() {
        WindowManager wm = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
        Display d = wm.getDefaultDisplay();
        Point point = new android.graphics.Point();
        d.getRealSize(point);
        Point p = new Point();
        p.x = point.x > point.y ? point.x : point.y;
        p.y = point.x > point.y ? point.y : point.x;
        return p;
    }

    public Point getPreviewSize() {
        CameraBase base = mCameraList.get(mRenderCameraID);
        if (base != null) {
            Point p = new Point();
            CameraMetaData cameraMetaData = base.getCameraMetaData();
            p.x = cameraMetaData.get(CameraMetaData.CAMERA_RGB_WIDTH);
            p.y = cameraMetaData.get(CameraMetaData.CAMERA_RGB_HEIGHT);
            return p;
        }
        return new Point();
    }

    public Point getProcessSize() {
        CameraBase base = mCameraList.get(mRenderCameraID);
        if (base != null) {
            Point p = new Point();
            CameraMetaData cameraMetaData = base.getCameraMetaData();
            p.x = cameraMetaData.get(CameraMetaData.CAMERA_SLAM_WIDTH);
            p.y = cameraMetaData.get(CameraMetaData.CAMERA_SLAM_HEIGHT);
            return p;
        }
        return new Point();
    }


    public long getCurImageTimestamp() {
        return CameraUtils.getCurImageTimestamp();
    }

    public int getCurImageWidth() {
        return CameraUtils.getCurImageWidth();
    }

    public int getCurImageHeight() {
        return CameraUtils.getCurImageHeight();
    }

    public int getCurImageStride(){
        return CameraUtils.getCurImageStride();
    }

    public int getCurImageFormat() {
        return CameraUtils.getCurImageFormat();
    }

    public boolean getCurImageData(long y_ptr, long u_ptr, long v_ptr) {
        return CameraUtils.getCurImageData(y_ptr, u_ptr, v_ptr);
    }

    public boolean isImageBufferDirty() {
        return CameraUtils.isImageBufferDirty();
    }

}

