package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CaptureRequest;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import com.standardar.common.CameraConstant;
import com.standardar.service.common.util.LogUtils;

import java.util.List;

public class AutoFocusCamera extends SingleStreamCamera {

    public AutoFocusCamera(Context context, int cameraType) {
        super(context, cameraType);
        for(SImageV1 sImageV1: mSImageList) {
            sImageV1.mCameraStreamType = CameraConstant.CAMERA_BACK_RGB_AUTOFOCUS;
        }
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
                                CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);

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
}
