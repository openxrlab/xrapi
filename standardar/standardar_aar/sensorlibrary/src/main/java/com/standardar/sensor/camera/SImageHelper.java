package com.standardar.sensor.camera;

import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.util.List;

import dalvik.system.DexClassLoader;

public class SImageHelper {
    private Object mImage;

    private Field mImageDataV1List;
    private Field mImageFormat;
    private Field mWidth;
    private Field mHeight;
    private Field mStride;
    private Field mTimeStamp;
    private Field mExposureTime;
    private Field mRollingShutterSkew;
    private Field mDataSize;
    private Field mImageData;

    public SImageHelper(DexClassLoader dexClassLoader) {
        try {
            Class<?> sImageV1Clz = dexClassLoader.loadClass("com.standardar.sensor.camera.SImageV1");
            Class<?> sImageDataV1Clz = dexClassLoader.loadClass("com.standardar.sensor.camera.SImageDataV1");
            mImageDataV1List = sImageV1Clz.getField("mImageDataList");
            mImageFormat = sImageDataV1Clz.getField("mImageFormat");
            mWidth = sImageDataV1Clz.getField("mWidth");
            mHeight = sImageDataV1Clz.getField("mHeight");
            mStride = sImageDataV1Clz.getField("mStride");
            mTimeStamp = sImageDataV1Clz.getField("mTimeStamp");
            mExposureTime = sImageDataV1Clz.getField("mExposureTime");
            mRollingShutterSkew = sImageDataV1Clz.getField("mRollingShutterSkew");
            mDataSize = sImageDataV1Clz.getField("mDataSize");
            mImageData = sImageDataV1Clz.getField("mImageData");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public void setImageObj(Object image) {
        mImage = image;
    }

    public ByteBuffer getImageBuffer(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return null;
            }
            return (ByteBuffer) mImageData.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return null;
    }

    public int getWidth(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return 0;
            }
            return (int) mWidth.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return 0;
    }

    public int getHeight(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return 0;
            }
            return (int) mHeight.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return 0;
    }

    public int getStride(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return 0;
            }
            return (int) mStride.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return 0;
    }

    public int getFormat(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return 0;
            }
            return (int) mImageFormat.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return 0;
    }

    public long getTimestamp(int index) {
        try {
            List<Object> imageV1s = (List<Object>)mImageDataV1List.get(mImage);
            if (imageV1s.size() <= index) {
                return 0;
            }
            return (long) mTimeStamp.get(imageV1s.get(index));
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
        return 0;
    }


    public Object getSImage() {
        return mImage;
    }


}
