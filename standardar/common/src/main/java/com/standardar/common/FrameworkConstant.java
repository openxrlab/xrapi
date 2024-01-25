package com.standardar.common;

public class FrameworkConstant {

    public static final int SINGLE_ALGORITHM_FRAME_VERSION_1 = 0xff1;

    public static final int MULTI_ALGORITHM_FRAME_VERSION_1 = 0xff1;
    public static final int MULTI_ALGORITHM_FRAME_VERSION_2 = 0xff2;
    public static final int RESERVED_NUM= 0xfff;

    //SLAM_TAG专门给单算法使用
    public static final int SLAM_TAG  = 0x1A1;

    //双视频流image的tag
    public static final int PROCESS_IMAGE_TAG = 0x1;
    public static final int PREVIEW_IMAGE_TAG = 0x2;
    public static final int PROCESS_PREVIEW_IMAGE_TAG = 0x3;

    //ServiceAdapter 初始化，开始，停止，销毁algorithm时返回的状态
    public static final int ADAPTER_PASS = 1; //表示成功
    public static final int ADAPTER_RETRY = 2;  //表示需要重新最一次
    public static final int ADAPTER_ERR = 3;   //执行过程中有错误

    //ServiceAdapter.getAlgorithmState得到的算法状态
    public static final int ALGORITHM_STATE_INIT = 1;
    public static final int ALGORITHM_STATE_START = 2;
    public static final int ALGORITHM_STATE_STOP = 3;
    public static final int ALGORITHM_STATE_DESTROY = 4;

    //ServiceAdapter.setValue使用的key
    public static final String ADAPTER_KEY_PACKAGENAME       = "package_name";
    public static final String ADAPTER_KEY_VERSIONNAME       = "version_name";
    public static final String ADAPTER_KEY_PLANE_MODE        = "plane_mode";
    public static final String ADAPTER_KEY_CLOUD_MODE        = "cloudanchor_mode";
    public static final String ADAPTER_KEY_LIGHT_ESTIMATE    = "light_estimate";
    public static final String ADAPTER_KEY_WORLD_ALIGN       = "world_align";
    public static final String ADAPTER_KEY_ALGORTIHM_STATE   = "algorithm_state";
    public static final String ADAPTER_KEY_DENSE_RECON       = "dense_recon";
    public static final String ADAPTER_KEY_HAND_GESTURE_MODE = "hand_gesture_mode";
    public static final String ADAPTER_KEY_OBJECT_TRACKING       = "object_tracking";
    public static final String ADAPTER_KEY_OBJECT_SCANNING       = "object_scanning";

    public static final int RECEIVE_MAGIC_WORD = 0xabcd;
    public static final int SEND_MAGIC_WORD    = 0x3a3b;
    //arservice对应的包名以及service名字
    public static final String SINGLE_ALGORITHM_SERVICE_NAME = "com.standardar.service.standarservice";
    public static final String MULTI_ALGORITHM_SERVICE_NAME  = "com.standardar.service.xrservice";
    public static String ARSERVICE_PACKAGE_NAME = "com.standardar.xrserver";

    public static final int ARSERVICE_CURRENT_VERSION = 21;
    public static final int XRSERVER_UNCOMPAT = 19;

    public static final String ARSERVER_META_RGBSLAM = "RGBSLAM";
    public static final String ARSERVER_META_RGBDSLAM = "RGBDSLAM";
    public static final String ARSERVER_META_TRUE = "true";
    public static final String ARSERVER_META_FALSE = "false";

    public static final int DCL_LOAD_SLAM = 100;
    public static final int DCL_LOAD_GLASS_SLAM = 200;
}
