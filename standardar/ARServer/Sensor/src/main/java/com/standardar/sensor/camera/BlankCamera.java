package com.standardar.sensor.camera;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Message;
import android.util.Range;
import android.util.Size;
import android.util.SizeF;
import android.view.Surface;

import com.standardar.common.CameraConstant;
import com.standardar.service.common.util.LogUtils;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class BlankCamera extends CameraFoundation {

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected Range<Integer> mFPSRange = new Range<Integer>(30, 30);
    protected int mCameraType;

    public BlankCamera(Context context, int cameraType) {
        super(context);
        mCameraType = cameraType;
        for(int i = 0; i < listSize; i++) {
            SImageV1 sImageV1 = new SImageV1();
            SImageDataV1 sImageDataV1 = new SImageDataV1();
            SImageDataV2 sImageDataV2 = new SImageDataV2();
            sImageV1.mCameraStreamType = CameraConstant.CAMERA_BACK_STEREO;
            sImageV1.mImageDataList.add(sImageDataV1);
            sImageV1.mImageData2List.add(sImageDataV2);
            mSImageList.add(sImageV1);
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void openCamera() {
        return;
    }

    public void closeCamera2() {
        return;
    }

    @Override
    public void closeCamera() {
        return;
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    @Override
    public void startPreview() {
        return;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected void startPreview(List<Surface> surfaceList) {
        return;
    }

    @Override
    public void doCommand(int cmd, ByteBuffer msg) {
        return;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void obtainCamerService() {
        return;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @SuppressLint("MissingPermission")
    private void openCamera2(int cameraType) {
        return;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private List<Size> getSupportsSizes(int format) {
        return null;
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    protected void extract(Image image, SImageV1 sImageV1) {
        return;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @Override
    protected void calcFov() {
        return;
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    private ImageReader.OnImageAvailableListener mOnImageAvailableListenerClient =
            new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {
                    return;
                }
            };
}
