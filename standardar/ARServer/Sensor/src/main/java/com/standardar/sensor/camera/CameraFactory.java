package com.standardar.sensor.camera;
import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;

import com.standardar.common.CameraConstant;
import com.standardar.service.common.util.LogUtils;

public class CameraFactory {
    public static CameraBase createCamera(int cameraid, Context context) {
        switch (cameraid) {
            case CameraConstant.CAMERA_BACK_RGB: {
                LogUtils.LOGI("create back camera:" + context.getPackageName());
                return new SingleStreamCamera(context, CameraCharacteristics.LENS_FACING_BACK);
            }
            case CameraConstant.CAMERA_BACK_RGB_AUTOFOCUS: {
                LogUtils.LOGI("create back autofocus camera:" + context.getPackageName());
                return new AutoFocusCamera(context, CameraCharacteristics.LENS_FACING_BACK);
            }
            case CameraConstant.CAMERA_FRONT_RGB: {
                LogUtils.LOGI("create front camera:" + context.getPackageName());
                SingleStreamCamera singleStreamCamera =  new SingleStreamCamera(context, CameraCharacteristics.LENS_FACING_FRONT);
                return singleStreamCamera;
            }
            case CameraConstant.CAMERA_BLANK: {
                LogUtils.LOGI("create blank camera:" + context.getPackageName());
                return new BlankCamera(context, -1);
            }
        }
        return null;
    }
}
