package com.standardar.sensor.camera;

import java.nio.ByteBuffer;

public class SImageDataV2 {

    public int mSensitivity;
    public float mLensAperture;


    public void copy(float lens, int sensitivity) {
        mLensAperture = lens;
        mSensitivity = sensitivity;

    }

    public SImageDataV2() {}

    public static int getFixBufferSize() {
        return Integer.BYTES + Float.BYTES;
    }

    @Override
    public String toString() {
        return "";
    }
}

