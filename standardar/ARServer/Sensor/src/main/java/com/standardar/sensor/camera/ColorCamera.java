package com.standardar.sensor.camera;


import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.media.Image;
import android.os.Build;
import android.util.Range;
import android.view.Surface;

import com.standardar.common.CameraConstant;

import com.standardar.service.common.util.LogUtils;

import java.util.Arrays;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class ColorCamera extends CameraFoundation {
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected Range<Integer> mFPSRange = new Range<Integer>(30, 30);
    protected int mCameraType;

    public ColorCamera(Context context, int cameraType) {
        super(context);
        mCameraType = cameraType;
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void openCamera() {
//        closeCamera();//if camera is opened, close it
        startImageProcessThread();
        startCameraBackgroundThread();
        obtainCamerService();
        openCamera2(mCameraType);
        calcFov();
        setSupportSizeStr();
    }

    public void closeCamera() {
        try {
            mCameraOpenCloseLock.acquire();
            if (mState == STATE_CLOSE) {
                mCameraOpenCloseLock.release();
                return;
            }
            mState = STATE_CLOSE;
            //closeSession();
            stopCamera();
            mCameraOpenCloseLock.release();
            stopCameraBackgroundThread();
            stopImageProcessThread();
            CameraUtils.releaseImageBuffer();
        } catch (InterruptedException e) {
            LogUtils.LOGE("lock acquire interrupt");
        }
        LogUtils.LOGI("close camera");
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected void startPreview(List<Surface> surfaceList) {
        if (surfaceList.isEmpty()) {
            LogUtils.LOGW("Empty surface list");
            return;
        }

        // set up preview session
        try {
            mCameraOpenCloseLock.acquire();
            if (mCameraDevice == null) {
                LogUtils.LOGE("camera device is null");
                return;
            }
            if (mState == STATE_PREVIEW) {
                LogUtils.LOGW("Camera is already previewing");
                return;
            }
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);

            for (Surface surface : surfaceList) {
                mPreviewRequestBuilder.addTarget(surface);
            }

            mCameraDevice.createCaptureSession(surfaceList, new CameraCaptureSession.StateCallback() {
                @Override
                public void onConfigured(CameraCaptureSession session) {
                    mCaptureSession = session;
                    try {
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                                CaptureRequest.CONTROL_AF_MODE_OFF);

                        if(Build.MODEL.contains("MI 9")){
                            mPreviewRequestBuilder.set(CaptureRequest.LENS_FOCUS_DISTANCE, 2.0f);
                        }else {
                            mPreviewRequestBuilder.set(CaptureRequest.LENS_FOCUS_DISTANCE, 0.0f);
                        }
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, mFPSRange);
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON);

                        mPreviewRequestBuilder.set(CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE,
                                CaptureRequest.LENS_OPTICAL_STABILIZATION_MODE_OFF);
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                                CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_OFF);
                        mPreviewRequestBuilder.set(CaptureRequest.STATISTICS_FACE_DETECT_MODE,
                                CaptureRequest.STATISTICS_FACE_DETECT_MODE_OFF);
//                        exposureManual();
                        mPreviewRequest = mPreviewRequestBuilder.build();
                        mCaptureSession.setRepeatingRequest(mPreviewRequest, mCaptureCallback, mBackgroundHandler);
                        mState = STATE_PREVIEW;
                    } catch (CameraAccessException e) {
                        LogUtils.LOGW("setRepeatingRequest failed");
                    }
                }

                @Override
                public void onConfigureFailed(CameraCaptureSession session) {
                    session.close();
                    LogUtils.LOGE("configure failed");
                }

                @Override
                public void onClosed(CameraCaptureSession session) {
                    try{
                        mCameraOpenCloseLock.acquire();
                        mCaptureSession = null;
                        //stopBackgroudThread();
                    } catch (InterruptedException e){
                        e.printStackTrace();
                    } finally {
                        mCameraOpenCloseLock.release();
                    }
                }
            }, mBackgroundHandler);
        } catch (CameraAccessException e) {
            LogUtils.LOGW("Create session failed can not access camera.");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void obtainCamerService() {
        if (mCameraManager == null && mContext != null) {
            mCameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
            if (mCameraManager == null) {
                LogUtils.LOGE("can not get camera service");
            }
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    @SuppressLint("MissingPermission")
    private void openCamera2(int cameraType) {
        if (mCameraManager == null && mContext != null) {
            mCameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
            if (mCameraManager == null) {
                LogUtils.LOGE("can not get camera service");
                return;
            }
        }

         try {
            for (String id : mCameraManager.getCameraIdList()) {
                CameraCharacteristics characteristics = mCameraManager.getCameraCharacteristics(id);
                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && characteristics.get(CameraCharacteristics.LENS_FACING) == cameraType) {
                    float[] focalLengths = characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS);
                    LogUtils.LOGI("available focal lengths: " + Arrays.toString(focalLengths));
                    mMaxFocalLength = 0;
                    for (float f : focalLengths) {
                        if (f > mMaxFocalLength) {
                            mMaxFocalLength = f;
                        }
                    }

                    Range<Long> exposureRange = characteristics.get(CameraCharacteristics.SENSOR_INFO_EXPOSURE_TIME_RANGE);
                    if (exposureRange != null) {
                        mMaxExposureTime = exposureRange.getUpper();
                        mMinExposureTime = exposureRange.getLower();
                    }
                    Range<Integer> sensitivityRange = characteristics.get(CameraCharacteristics.SENSOR_INFO_SENSITIVITY_RANGE);
                    if (sensitivityRange != null) {
                        mMaxSensitivity = sensitivityRange.getUpper();
                        mMinSensitivity = sensitivityRange.getLower();
                    }

                    try {
                        if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                            throw new RuntimeException("time out waiting to lock camera opening");
                        }
                        mCameraManager.openCamera(id, new CameraDevice.StateCallback() {
                            @Override
                            public void onOpened(CameraDevice camera) {
                                LogUtils.LOGI("camera open:" + camera);
                                mCameraDevice = camera;
                                mCameraOpenCloseLock.release();
                            }

                            @Override
                            public void onDisconnected(CameraDevice camera) {
                                LogUtils.LOGI("camera disconnected:" + camera);
                                mCameraOpenCloseLock.release();
                                try{
                                    mCameraOpenCloseLock.acquire();
                                    mState = STATE_CLOSE;
                                    mCaptureSession = null;
                                    camera.close();
                                    mCameraDevice = null;
                                } catch (InterruptedException e){
                                    e.printStackTrace();
                                } finally {
                                    mCameraOpenCloseLock.release();
                                }

                            }

                            @Override
                            public void onError(CameraDevice camera, int error) {
                                LogUtils.LOGI("camera error:" + error);
                                mCameraOpenCloseLock.release();
                                try{
                                    mCameraOpenCloseLock.acquire();
                                    mState = STATE_CLOSE;
                                    mCaptureSession = null;
                                    camera.close();
                                    mCameraDevice = null;
                                } catch (InterruptedException e){
                                    e.printStackTrace();
                                } finally {
                                    mCameraOpenCloseLock.release();
                                }
                            }

                            @Override
                            public void onClosed(CameraDevice camera) {
                                LogUtils.LOGI("camera closed:" + camera);
                                mCameraOpenCloseLock.release();
                                try{
                                    mCameraOpenCloseLock.acquire();
                                    mState = STATE_CLOSE;
                                    mCaptureSession = null;
                                    mCameraDevice = null;
                                } catch (InterruptedException e){
                                    e.printStackTrace();
                                } finally {
                                    mCameraOpenCloseLock.release();
                                }
                            }
                        }, mBackgroundHandler);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    break;
                }
            }
        } catch (CameraAccessException e) {
            LogUtils.LOGE("open camera failed Can not access camera");
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    protected void extract(Image image, SImageV1 sImageV1) {
        mRGBStride = CameraUtils.getYUV420Stride(image);
        mImageBufferSize = mRGBStride * mRGBHeight * 3 / 2;
        SImageDataV1 sImageDataV1 = sImageV1.mImageDataList.get(0);
        if(sImageDataV1.mDataSize < mImageBufferSize){
            sImageDataV1.allocateDirect(mImageBufferSize);
        }
        if(mCameraType ==CameraCharacteristics.LENS_FACING_FRONT){
            sImageV1.mCameraStreamType = CameraConstant.CAMERA_FRONT_RGB;
        }

        sImageDataV1.mWidth = mRGBWidth;
        sImageDataV1.mHeight = mRGBHeight;
        sImageDataV1.mImageFormat = CameraUtils.getYUV420Format(image);
        sImageDataV1.mStride = mRGBStride;
        sImageDataV1.mTimeStamp = image.getTimestamp();
        sImageDataV1.mExposureTime = mCurExposureTime == null ? 0 : mCurExposureTime;
        sImageDataV1.mRollingShutterSkew = mRollingShutterSkew == null ? 0 : mRollingShutterSkew;
        CameraUtils.getYUV420Data(image, sImageDataV1.mImageData);
        SImageDataV2 sImageDataV2 = sImageV1.mImageData2List.get(0);
        sImageDataV2.mLensAperture = mLensAperture == null ? 0 : mLensAperture;
        sImageDataV2.mSensitivity = mSensitivity == null ? 0 : mSensitivity;
    }

}
