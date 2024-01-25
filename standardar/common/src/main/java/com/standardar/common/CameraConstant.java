package com.standardar.common;

public class CameraConstant {
    //打开何种Camera的枚举值
    public static final int CAMERA_FRONT_RGB = 100;
    public static final int CAMERA_BLANK = 101;
    public static final int CAMERA_BACK_RGB = 2000;
    public static final int CAMERA_BACK_RGB_AUTOFOCUS = 2001;
    public static final int CAMERA_BACK_RGB_DOUBLE_STREAM = 2002;
    public static final int CAMERA_FRONT_RGB_DOUBLE_STREAM = 2003;
    public static final int CAMERA_BACK_RGBD = 2100;
    public static final int CAMERA_BACK_STEREO = 2200;

    public static final int CMD_IMAGEREADER_PAUSE = 1;
    public static final int CMD_IMAGEREADER_ACTIVE = 2;
    public static final int CMD_SURFACETEXTURE_UPDATE = 3;
    public static final int CMD_OES_TEXID_SET         = 4;
    public static final int CMD_GET_SURFACETEXTURE_TIMESTAMP = 5;
    public static final int CMD_SURFACETEXTURE_WAIT = 6;
}
