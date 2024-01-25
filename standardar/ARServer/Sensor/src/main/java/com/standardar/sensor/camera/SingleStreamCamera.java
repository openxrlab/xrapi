package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.media.Image;
import android.media.ImageReader;
import android.os.Build;
import android.os.Message;

import com.standardar.common.CameraConstant;
import com.standardar.service.common.util.LogUtils;
import com.standardar.service.common.util.PriorityUtil;

import java.nio.ByteBuffer;
import java.util.Arrays;

public class SingleStreamCamera extends ColorCamera {
    private int mOESTextureId = 0;

    public SingleStreamCamera(Context context, int cameraType) {
        super(context, cameraType);
        for(int i = 0; i < listSize; i++) {
            SImageV1 sImageV1 = new SImageV1();
            SImageDataV1 sImageDataV1 = new SImageDataV1();
            SImageDataV2 sImageDataV2 = new SImageDataV2();
            sImageV1.mCameraStreamType = CameraConstant.CAMERA_BACK_RGB;
            sImageV1.mImageDataList.add(sImageDataV1);
            sImageV1.mImageData2List.add(sImageDataV2);
            mSImageList.add(sImageV1);
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    @Override
    public void startPreview() {
        LogUtils.LOGI("start preview single stream");
        mImageReader = CameraHelpler.createImageReader(mRGBWidth, mRGBHeight, ImageFormat.YUV_420_888,
                5,mOnImageAvailableListenerClient, mBackgroundHandler);
        startPreview(Arrays.asList(mImageReader.getSurface()));
    }

    @Override
    public void stopPreview() {
        super.stopPreview();
        LogUtils.LOGI("stop previve single stream");
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
            case CMD_OES_TEXID_SET:{
                if (msg == null) {
                    LogUtils.LOGE("oes texid msg buffer is null");
                    break;
                }
                mOESTextureId = msg.getInt();
                break;
            }
            case CMD_SURFACETEXTURE_UPDATE:{
                CameraUtils.updateOESTexture(mOESTextureId);
            }
            default:
                break;
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    private ImageReader.OnImageAvailableListener mOnImageAvailableListenerClient =
            new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {
                    PriorityUtil.bindCore();
                    try {
                        mCameraOpenCloseLock.acquire();
                        if (mState == STATE_CLOSE) {
                            mCameraOpenCloseLock.release();
                            return;
                        }
                    } catch (InterruptedException e) {
                            e.printStackTrace();
                    }
                    mCameraOpenCloseLock.release();

                    Image image = reader.acquireLatestImage();
                    if (null != image) {
                        if(fillIndex != processIndex) {
                            extract(image, mSImageList.get(fillIndex));
                            Message msg = Message.obtain();
                            msg.arg1 = fillIndex;
                            fillIndex = (fillIndex + 1) % listSize;
                            if(mImageProcessHandler != null)
                                mImageProcessHandler.sendMessage(msg);
                        }
                        image.close();
                    }
                }

            };

    @Override
    public void closeCamera() {
        super.closeCamera();

        CameraHelpler.releaseImageReader(mImageReader);
        mImageReader = null;
        mContext = null;
    }
}
