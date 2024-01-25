package com.standardar.common;

public class AlgorithmConstant {

    //算法使能的位，与native层CConfig.m_AlgorithmEnable对应
    public static final int ALGORITHM_SLAM             = 0x1;
    public static final int ALGORITHM_GESTURE          = 0x2;
    public static final int ALGORITHM_DENSE_RECON      = 0x3;
    public static final int ALGORITHM_IMAGE_TRACKINNG  = 0x4;
    public static final int ALGORITHM_FACE_MESH        = 0x8;
    public static final int ALGORITHM_OBJECT_TRACKING  = 0x9;
    public static final int ALGORITHM_BODY_DETECT      = 0xB;

    public static final int NOT_USING_DENSE_RECON = 0;

    public static final int NOT_USING_OBJECT_TRACKING = 0;
    public static final int USING_OBJECT_TRACKING = 1;

    public static final int NOT_USING_OBJECT_SCANNING = 0;
    public static final int USING_OBJECT_SCANNING = 1;
}
