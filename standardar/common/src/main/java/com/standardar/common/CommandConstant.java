package com.standardar.common;

public class CommandConstant {

    private static final int COMMAND_COMMON_BASE  = 0;
    private static final int COMMAND_SIZE         = 100;
    private static final int COMMAND_SLAM_BASE    = COMMAND_COMMON_BASE  + COMMAND_SIZE; //100
    private static final int COMMAND_GESTURE_BASE = COMMAND_SLAM_BASE    + COMMAND_SIZE; //200
    private static final int COMMAND_IMAGE_TRACKINNG_BASEK = COMMAND_GESTURE_BASE + COMMAND_SIZE; //300

    private static final int COMMAND_FACE_DETECT_BASEK = COMMAND_IMAGE_TRACKINNG_BASEK + COMMAND_SIZE; //400
    private static final int COMMAND_FACE_MESH_BASEK  = COMMAND_FACE_DETECT_BASEK + COMMAND_SIZE; //500
    private static final int COMMAND_OBJECT_TRACKING_BASE = COMMAND_FACE_MESH_BASEK + COMMAND_SIZE; //600

    //command for common
    public static final int COMMAND_RESULT_OK                                   = COMMAND_COMMON_BASE;

    public static final int COMMAND_CHECK_AUTHORIZATION                  = COMMAND_COMMON_BASE + 1;
    public static final int COMMAND_SET_PACKAGE_NAME                     = COMMAND_COMMON_BASE + 2;
    public static final int COMMAND_SWITCH_ALGORITHM                     = COMMAND_COMMON_BASE + 3;
    public static final int COMMAND_SET_PACKAGE_NAME_AND_VERSION         = COMMAND_COMMON_BASE + 4;

    public static final int COMMAND_RESULT_ALGORITHM_NOT_INIT            = COMMAND_COMMON_BASE + 5;
    public static final int COMMAND_RESULT_PASS                          = COMMAND_COMMON_BASE + 6;

    public static final int COMMAND_COMMON_INIT_ALGORITHM     = COMMAND_COMMON_BASE + 7;
    public static final int COMMAND_COMMON_RESET_ALGORITHM    = COMMAND_COMMON_BASE + 8;
    public static final int COMMAND_COMMON_START_ALGORITHM    = COMMAND_COMMON_BASE + 9;
    public static final int COMMAND_COMMON_DESTROY_ALGORITHM  = COMMAND_COMMON_BASE + 10;

    public static final int COMMAND_COMMON_ENABLE_ALGORITHM   = COMMAND_COMMON_BASE + 11;
    public static final int COMMAND_COMMON_DISABLE_ALGORITHM   = COMMAND_COMMON_BASE + 12;
    public static final int COMMAND_COMMON_AXISUPMODE = COMMAND_COMMON_BASE + 13;

    //command for slam
    public static final int COMMAND_SLAM_INIT_ALGORITHM          = COMMAND_SLAM_BASE;
    public static final int COMMAND_SLAM_RESET_ALGORITHM         = COMMAND_SLAM_BASE + 1;
    public static final int COMMAND_SLAM_START_ALGORITHM         = COMMAND_SLAM_BASE + 2;
    public static final int COMMAND_SLAM_DESTROY_ALGORITHM       = COMMAND_SLAM_BASE + 3;
    public static final int COMMAND_SLAM_QUERY_SLAM_RESULT       = COMMAND_SLAM_BASE + 4;

    public static final int COMMAND_SLAM_HOST_CLOUD_ANCHOR       = COMMAND_SLAM_BASE + 5;
    public static final int COMMAND_SLAM_RESOLVE_CLOUD_ANCHOR    = COMMAND_SLAM_BASE + 6;

    public static final int COMMAND_SLAM_TOF_INIT_ALGORITHM      = COMMAND_SLAM_BASE + 7;
    public static final int COMMAND_SLAM_TOF_RESET_ALGORITHM     = COMMAND_SLAM_BASE + 8;
    public static final int COMMAND_SLAM_TOF_START_ALGORITHM     = COMMAND_SLAM_BASE + 9;
    public static final int COMMAND_SLAM_TOF_DESTROY_ALGORITHM   = COMMAND_SLAM_BASE + 10;

    public static final int COMMAND_SLAM_CHANGE_LIGHT_MODE       = COMMAND_SLAM_BASE + 11;

    public static final int COMMAND_SLAM_PREDICT_POSE            = COMMAND_SLAM_BASE + 12;
    public static final int COMMAND_SLAM_PUSH_KEY_FRAME          = COMMAND_SLAM_BASE + 13;
    public static final int COMMAND_SLAM_RUN_DENSE_RECON         = COMMAND_SLAM_BASE + 14;
    public static final int COMMAND_SLAM_SET_BOUNDINGBOX         = COMMAND_SLAM_BASE + 15;
    public static final int COMMAND_SLAM_GET_SCANNING_RESULT     = COMMAND_SLAM_BASE + 16;
    public static final int COMMAND_SLAM_GET_RESULT              = COMMAND_SLAM_BASE + 17;
    public static final int COMMAND_SLAM_SET_RESUME_FLAG         = COMMAND_SLAM_BASE + 18;
    public static final int COMMAND_SLAM_GET_PREDICTED_VIEW      = COMMAND_SLAM_BASE + 19;
    public static final int COMMAND_SLAM_SET_WINDOW              = COMMAND_SLAM_BASE + 20;
    public static final int COMMAND_SLAM_BEGIN_RENDER_EYE        = COMMAND_SLAM_BASE + 21;
    public static final int COMMAND_SLAM_END_RENDER_EYE          = COMMAND_SLAM_BASE + 22;
    public static final int COMMAND_SLAM_SUBMIT_FRAME            = COMMAND_SLAM_BASE + 23;

    //command for gesture
    public static final int COMMAND_GESTURE_INIT_ALGORITHM      = COMMAND_GESTURE_BASE;
    public static final int COMMAND_GESTURE_RESET_ALGORITHM     = COMMAND_GESTURE_BASE + 1;
    public static final int COMMAND_GESTURE_START_ALGORITHM     = COMMAND_GESTURE_BASE + 2;
    public static final int COMMAND_GESTURE_DESTROY_ALGORITHM   = COMMAND_GESTURE_BASE + 3;

    //command for marker
    public static final int COMMAND_IMAGE_TRACKINNG_INIT_ALGORITHM       = COMMAND_IMAGE_TRACKINNG_BASEK;
    public static final int COMMAND_IMAGE_TRACKINNG_DESTROY_ALGORITHM    = COMMAND_IMAGE_TRACKINNG_BASEK + 1;
    public static final int COMMAND_IMAGE_TRACKINNG_ADD_IMAGE            = COMMAND_IMAGE_TRACKINNG_BASEK + 2;
    public static final int COMMAND_IMAGE_TRACKINNG_START_ALGORITHM      = COMMAND_IMAGE_TRACKINNG_BASEK + 3;
    public static final int COMMAND_IMAGE_TRACKINNG_RESET_ALGORITHM      = COMMAND_IMAGE_TRACKINNG_BASEK + 4;

    //command for facedetect
    public static final int COMMAND_FACE_DETECT_INIT_ALGORITHM       = COMMAND_FACE_DETECT_BASEK;
    public static final int COMMAND_FACE_DETECT_RESET_ALGORITHM      = COMMAND_FACE_DETECT_BASEK + 1;
    public static final int COMMAND_FACE_DETECT_START_ALGORITHM      = COMMAND_FACE_DETECT_BASEK + 2;
    public static final int COMMAND_FACE_DETECT_DESTROY_ALGORITHM    = COMMAND_FACE_DETECT_BASEK + 3;


    //command for facemesh
    public static final int COMMAND_FACE_MESH_INIT_ALGORITHM       = COMMAND_FACE_MESH_BASEK;
    public static final int COMMAND_FACE_MESH_RESET_ALGORITHM      = COMMAND_FACE_MESH_BASEK + 1;
    public static final int COMMAND_FACE_MESH_START_ALGORITHM      = COMMAND_FACE_MESH_BASEK + 2;
    public static final int COMMAND_FACE_MESH_DESTROY_ALGORITHM    = COMMAND_FACE_MESH_BASEK + 3;

    //command for objecttracking
    public static final int COMMAND_OBJECT_TRACKINNG_INIT_ALGORITHM       = COMMAND_OBJECT_TRACKING_BASE;
    public static final int COMMAND_OBJECT_TRACKINNG_DESTROY_ALGORITHM    = COMMAND_OBJECT_TRACKING_BASE + 1;
    public static final int COMMAND_OBJECT_TRACKINNG_ADD_OBJECT           = COMMAND_OBJECT_TRACKING_BASE + 2;




}
