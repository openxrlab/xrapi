package com.standardar.sensor.camera;

import java.util.ArrayList;
import java.util.List;

public class SImageV1 {
    public int mCameraStreamType = -1;
    public int mScreenRotate;
    public List<SImageDataV1> mImageDataList = new ArrayList<>();
    public List<SImageDataV2> mImageData2List = new ArrayList<>();
    public void copy(int type, int screenrotate, List<SImageDataV1> list) {
        mCameraStreamType = type;
        mImageDataList = list;
        mScreenRotate = screenrotate;
    }

    public void copy(int type, int screenrotate, List<SImageDataV1> list, List<SImageDataV2> listV2) {
        mCameraStreamType = type;
        mImageDataList = list;
        mImageData2List = listV2;
        mScreenRotate = screenrotate;
    }

}
