
#ifndef COM_STANDARDAR_SERVICE_ARSERVER_SLAM_API_H
#define COM_STANDARDAR_SERVICE_ARSERVER_SLAM_API_H


#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_initNativeSLAM(JNIEnv* env, jobject obj,
                                                                     jobject assetManager,
                                                                     jstring pathToInternalPath,
                                                                     jint w, jint h, jfloat fovh, jint type);

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_destroyNativeSLAM(JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_processFrameDirectlyNative(JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getImageIntrinsicsNative(JNIEnv* env, jobject obj, jobject buffer, jint order);
JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_pushSensorDirectlyNative(JNIEnv* env, jobject obj,
                                                                            jfloatArray value, jint tag, jlong timestamp);

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_setFrame(JNIEnv* env, jobject obj, jobject image_buffer,
                                                                                    jint format, jint width, jint height, jint stride,
                                                                                    jlong exposure_time, jlong timestamp);

JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getTrackingStateNative(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_updateAlgorithmResultNative(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_setAxisUpModeNative(JNIEnv* env, jobject obj, jint mode);
JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getViewMatrixNative(JNIEnv* env, jobject obj, jlong mode, jint screen_rotate);
JNIEXPORT jfloatArray JNICALL Java_com_standardar_service_slam_algorithm_SLAM_insectPlaneWithSlamResultNative(JNIEnv* env, jobject obj,
        jfloatArray ray_origin, jfloatArray ray_direction, jintArray plane_id, jfloatArray quat_arr);
JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getNumberLandMarksNative(JNIEnv* env, jobject obj);
JNIEXPORT jlong JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getLandMarksNative(JNIEnv* env, jobject obj);
JNIEXPORT jfloat JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getDistanceFromCameraNative(JNIEnv* env, jobject obj, jfloatArray hit_pose_arr);


#ifdef __cplusplus
};
#endif
#endif //COM_STANDARDAR_SERVICE_ARSERVER_API_H
