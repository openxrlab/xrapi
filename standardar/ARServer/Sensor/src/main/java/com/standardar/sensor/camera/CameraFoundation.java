package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;

import android.os.SystemClock;
import android.util.Size;
import android.util.SizeF;


import com.standardar.service.common.util.LogUtils;
import com.standardar.service.common.util.PriorityUtil;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicBoolean;

public class CameraFoundation extends CameraBase {
    protected static final int STATE_PREVIEW = 0;
    protected static final int STATE_CLOSE = 1;

    protected Integer mSLAMWidth = Integer.valueOf(640), mSLAMHeight = Integer.valueOf(480), mSLAMStride = Integer.valueOf(640);
    protected Integer mRGBWidth = Integer.valueOf(1280), mRGBHeight = Integer.valueOf(960), mRGBStride = Integer.valueOf(1280), mRGBScanline = Integer.valueOf(960);
    protected Integer mDepthWidth = Integer.valueOf(240), mDepthHeight = Integer.valueOf(180), mDepthStride = Integer.valueOf(240*2);
    protected Integer mCameraType = Integer.valueOf(0), mImageBufferSize = Integer.valueOf(0);
    protected Float mFovH = Float.valueOf(0), mFovV = Float.valueOf(0);
    protected String mSupportSize;
    protected byte[] mCalibrationBuffer = null;
    protected Integer mScreenRotate = Integer.valueOf(0);
    protected Integer mAxisUpMode = Integer.valueOf(0);
    protected Long mSurfaceTextureTimestamp = Long.valueOf(0);

    protected class CameraFoundationMetaData extends CameraMetaData {
        public CameraFoundationMetaData(){
            mData.put(CAMERA_RGB_WIDTH, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mRGBWidth;
                }
            });

            mData.put(CAMERA_RGB_HEIGHT, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mRGBHeight;
                }
            });

            mData.put(CAMERA_RGB_STRIDE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mRGBStride;
                }
            });

            mData.put(CAMERA_RGB_SCANLINE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mRGBScanline;
                }
            });

            mData.put(CAMERA_SLAM_WIDTH, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mSLAMWidth;
                }
            });

            mData.put(CAMERA_SLAM_HEIGHT, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mSLAMHeight;
                }
            });

            mData.put(CAMERA_SLAM_STRIDE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mSLAMStride;
                }
            });

            mData.put(CAMERA_DEPTH_WIDTH, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mDepthWidth;
                }
            });

            mData.put(CAMERA_DEPTH_HEIGHT, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mDepthHeight;
                }
            });

            mData.put(CAMERA_DEPTH_STRIDE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mDepthStride;
                }
            });


            mData.put(CAMERA_TYPE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mCameraType;
                }
            });

            mData.put(CAMERA_IMAGE_BUFFER_SIZE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mImageBufferSize;
                }
            });

            mData.put(CAMERA_FOVH, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mFovH;
                }
            });

            mData.put(CAMERA_FOVV, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mFovV;
                }
            });

            mData.put(CAMERA_SUPPORT_SIZE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mSupportSize;
                }
            });

            mData.put(CAMERA_CALIBRATION_BUFFER, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mCalibrationBuffer;
                }
            });
            mData.put(SCREEN_ROTATE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mScreenRotate;
                }
            });

            mData.put(AXIS_UP_MODE, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mAxisUpMode;
                }
            });
			
			mData.put(CAMERA_SURFACETEXTURE_TIMESTAMP, new GetCommand() {
                @Override
                public <T> T getValue() {
                    return (T) mSurfaceTextureTimestamp;
                }
            });
        }
    }

    protected Handler mBackgroundHandler;
    protected HandlerThread mBackgroundThread;
    protected Semaphore mCameraOpenCloseLock = new Semaphore(1);

    protected CameraManager mCameraManager;
    protected CameraCaptureSession mCaptureSession;
    protected CameraDevice mCameraDevice;
    protected CaptureRequest.Builder mPreviewRequestBuilder;
    protected CaptureRequest mPreviewRequest;

    protected float mMaxFocalLength;
    protected Long mCurExposureTime = Long.valueOf(20000000);
    protected Long mMaxExposureTime = Long.valueOf(0);
    protected Long mMinExposureTime = Long.valueOf(0);
    protected Long mFixExposureTime = Long.valueOf(10000000);
    protected Long mRollingShutterSkew = Long.valueOf(0);
    protected Integer mMinSensitivity = Integer.valueOf(0);
    protected Integer mMaxSensitivity = Integer.valueOf(0);
    protected Integer mSensitivity = Integer.valueOf(0);
    protected Float mLensAperture = Float.valueOf(0);

    protected int mState = STATE_CLOSE;
    protected boolean mIsAsync;

    protected FrameAvailableListener mFrameAvailableListener;
    protected Object mLisLock = new Object();

    protected ImageReader mImageReader;
//    protected SImageV1 mImageV1 = new SImageV1();
//    protected SImageDataV1 mImageDataV1 = new SImageDataV1();
//    protected SImageDataV2 mImageDataV2 = new SImageDataV2();
    protected ArrayList<SImageV1> mSImageList = new ArrayList<>();
    protected final int listSize = 3;
    protected int fillIndex = 0;
    protected int processIndex = -1;
    protected AtomicBoolean mImageReaderActive = new AtomicBoolean(false);

    protected Handler mImageProcessHandler;
    protected HandlerThread mImageProcessThread;

    public CameraFoundation(Context context) {
        super(context);
        mCameraMetaData = new CameraFoundationMetaData();
    }

    public void openCamera(){
        LogUtils.LOGI("open camera");
    }

    public void closeCamera(){
        LogUtils.LOGI("close camera");
    }

    public CameraMetaData getCameraMetaData(){
        return mCameraMetaData;
    }

    public void setPreviewSize(int width, int height){
        mRGBWidth = width;
        mRGBHeight = height;
        calcFov();
    }

    public void startPreview(){
        LogUtils.LOGI("start preview");
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public void stopPreview() {
        if (mCaptureSession != null) {
            mCaptureSession.close();
            mCaptureSession = null;
        }
    }

    public void setAsync(boolean isAsync){
        mIsAsync = isAsync;
    }

    @Override
    public void registerFrameAvailableListener(FrameAvailableListener lis) {
        synchronized (mLisLock) {
            mFrameAvailableListener = lis;
        }
    }

    @Override
    public void unregisterFrameAvailableListener(FrameAvailableListener lis) {
        synchronized (mLisLock) {
            mFrameAvailableListener = null;
        }
    }

    @Override
    public void doCommand(int cmd, ByteBuffer msg) {
        switch (cmd) {
            case CMD_IMAGEREADER_PAUSE:{
                mImageReaderActive.set(false);
                break;
            }
            case CMD_IMAGEREADER_ACTIVE:{
                mImageReaderActive.set(true);
                break;
            }
            default:
                break;
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected void calcFov() {
        if (mContext == null) {
            return;
        }
        try {
            CameraManager cameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
            mCameraOpenCloseLock.acquire();
            if (mCameraDevice == null) {
                LogUtils.LOGE("calc fov failed because camera device is null");
                return;
            }
            String id = mCameraDevice.getId();
            CameraCharacteristics characteristics =	cameraManager.getCameraCharacteristics(id);

            float[] focalLengths = characteristics.get(CameraCharacteristics.LENS_INFO_AVAILABLE_FOCAL_LENGTHS);
            LogUtils.LOGI("available focal lengths: " + Arrays.toString(focalLengths));
            mMaxFocalLength = 0;
            for (float f : focalLengths) {
                if (f > mMaxFocalLength) {
                    mMaxFocalLength = f;
                }
            }

            SizeF physicalSize = characteristics.get(CameraCharacteristics.SENSOR_INFO_PHYSICAL_SIZE);
            mFovV = (float) (2 * Math.atan(physicalSize.getHeight() / 2.0 / mMaxFocalLength) * 180.0 / Math.PI);
            mFovH = (float) (2 * Math.atan(physicalSize.getWidth() / 2.0 / mMaxFocalLength) * 180.0 / Math.PI);

            LogUtils.LOGI("phsical width:"+physicalSize.getWidth()+
                    ",height:"+ physicalSize.getHeight()+
                    ",fov horizontal:"+mFovH+
                    ",fov vertical:"+mFovV);

        } catch (CameraAccessException e) {
            LogUtils.LOGE("Calc fov failed can not access camera.");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected void stopCamera() {
        if (mCameraDevice != null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
    }

    protected void startCameraBackgroundThread() {
        if (mBackgroundThread != null) {
            return;
        }
        mBackgroundThread = new HandlerThread("camerabackgroud");
        mBackgroundThread.start();
        mBackgroundThread.setPriority(Thread.MAX_PRIORITY);
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    protected void stopCameraBackgroundThread() {
        if (mBackgroundThread == null) {
            return;
        }
        mBackgroundThread.quitSafely();
        try {
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    protected void startImageProcessThread() {
        if (mImageProcessThread != null) {
            return;
        }
        mImageProcessThread = new HandlerThread("image process", -19);
        mImageProcessThread.start();
        mImageProcessHandler = new Handler(mImageProcessThread.getLooper()){
            @Override
            public void handleMessage(Message msg) {
                PriorityUtil.bindCore();
                synchronized (mLisLock) {
                    if (mFrameAvailableListener != null) {
                        int index = msg.arg1;
                        processIndex = index;
                        mFrameAvailableListener.onImageAvailable(mSImageList.get(index));
                    }
                }
            }
        };
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
    protected void stopImageProcessThread() {
        if (mImageProcessThread == null) {
            return;
        }
        mImageProcessHandler.removeCallbacksAndMessages(null);
        mImageProcessThread.quitSafely();
        try {
            mImageProcessThread.join();
            mImageProcessThread = null;
            mImageProcessHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    protected CameraCaptureSession.CaptureCallback mCaptureCallback
            = new CameraCaptureSession.CaptureCallback() {

        private void process(CaptureResult result) {
            switch (mState) {
                case STATE_PREVIEW: {
                    mCurExposureTime = result.get(CaptureResult.SENSOR_EXPOSURE_TIME);
                    mRollingShutterSkew = result.get(CaptureResult.SENSOR_ROLLING_SHUTTER_SKEW);
                    mSensitivity = result.get(CaptureResult.SENSOR_SENSITIVITY);
                    mLensAperture = result.get(CaptureResult.LENS_APERTURE);
                    break;
                }
            }
        }
        @Override
        public void onCaptureProgressed( CameraCaptureSession session,
                                         CaptureRequest request,
                                         CaptureResult partialResult) {
            super.onCaptureProgressed(session, request, partialResult);
        }

        @Override
        public void onCaptureCompleted( CameraCaptureSession session,
                                        CaptureRequest request,
                                        TotalCaptureResult result) {
            super.onCaptureCompleted(session, request, result);
            process(result);
        }

    };

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    private void exposureManual() {
        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_OFF);
        mPreviewRequestBuilder.set(CaptureRequest.SENSOR_EXPOSURE_TIME, mFixExposureTime);
        Integer sensitivity = mMinSensitivity + (mMaxSensitivity - mMinSensitivity) * 10 / 100;
        mPreviewRequestBuilder.set(CaptureRequest.SENSOR_SENSITIVITY, sensitivity);
//        mPreviewRequestBuilder.set(CaptureRequest.SENSOR_FRAME_DURATION, new Long(0));
    }

    protected void setSupportSizeStr() {
        List<Size> sizes = null;
        try {
            mCameraOpenCloseLock.acquire();
            if (mCameraDevice != null && mCameraManager != null) {
                try {
                    String id = mCameraDevice.getId();
                    CameraCharacteristics characteristics = mCameraManager.getCameraCharacteristics(id);
                    StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);

                    Size[] size = map.getOutputSizes(ImageFormat.YUV_420_888);
                    if(size != null)
                        sizes =  Arrays.asList(size);
                } catch (CameraAccessException e) {
                    LogUtils.LOGE("get supported failed can not access camera");
                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            mCameraOpenCloseLock.release();
        }

        if (sizes == null) {
            return;
        }
        StringBuilder sb = new StringBuilder();
        for (Size s : sizes) {
            sb.append(s.getWidth()).append("x").append(s.getHeight()).append(",");
        }
        mSupportSize =  sb.deleteCharAt(sb.length() - 1).toString();
    }

    @Override
    public void setScreenRotate(int screenRotate) {
        for(SImageV1 sImageV1: mSImageList) {
            sImageV1.mScreenRotate = screenRotate;
        }
        mScreenRotate = screenRotate;
    }
}
