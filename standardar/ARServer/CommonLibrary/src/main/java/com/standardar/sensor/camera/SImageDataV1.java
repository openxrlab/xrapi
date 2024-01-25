package com.standardar.sensor.camera;

import java.nio.ByteBuffer;

public class SImageDataV1 {

    public int mImageFormat = SImageFormat.SIMAGE_FORMAT_NON;
    public int mWidth;
    public int mHeight;
    public int mStride;
    public long mExposureTime;
    public long mTimeStamp;
    public long mRollingShutterSkew;
    public int mDataSize;
    public ByteBuffer mImageData;

    public void copy(int format, int w, int h, int s, long exposureTime, long timestamp, long rss, int size, ByteBuffer buffer) {
        mImageFormat = format;
        mWidth = w;
        mHeight = h;
        mStride = s;
        mExposureTime = exposureTime;
        mTimeStamp = timestamp;
        mDataSize = size;
        mRollingShutterSkew = rss;
        mImageData = buffer;
    }

    public SImageDataV1() {}

    public void allocateDirect(int size) {
        mImageData = null;
        mDataSize = size;
        mImageData = ByteBuffer.allocateDirect(size);
    }

    public void get(byte[] dst, int offset, int length) {
        mImageData.get(dst, offset, length);
        mImageData.rewind();
    }

    public static int getFixBufferSize() {
        return Integer.BYTES * 5 + Long.BYTES * 3;
    }

    @Override
    public String toString() {
        return "" + mTimeStamp;
    }
}