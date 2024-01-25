#include "Utils.h"
#include "com_standardar_service_slam_algorithm_SLAM.h"
#include "SLAMBase.h"
#include "RGBSLAM.h"
#include "log.h"
#include "CommonConstant.h"
#include "ArMath.h"

using namespace standardar;

static SLAMBase* SLAM = nullptr;
static void result_fill(DirectByteBufferWrapper* bufferWrapper)
{
    int resLength = 0;
    bufferWrapper->put<int>(resLength + 4);
    bufferWrapper->put<int>(resLength);
}

JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_initNativeSLAM(
                                                                        JNIEnv* env,
                                                                        jobject obj,
                                                                        jobject assetManager,
                                                                        jstring pathToInternalPath,
                                                                        jint w, jint h, jfloat fovh, jint type)
{
    if(type == CAMERA_RGB){
        if(SLAM != nullptr)
            delete SLAM;

        SLAM = new RGBSLAM();
    }else
        return 0;

    AAssetManager* apkAssetManager = AAssetManager_fromJava(env, assetManager);
    const char *cPathToInternalDir = env->GetStringUTFChars(pathToInternalPath, 0);
    string apkInternalPath(cPathToInternalDir);
    env->ReleaseStringUTFChars(pathToInternalPath, cPathToInternalDir);
    return SLAM->initSLAM(apkAssetManager, apkInternalPath, w, h, fovh);
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_destroyNativeSLAM(JNIEnv* env,
                                                                                         jobject obj)
{
    SLAM->destroySLAM();
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_pushSensorDirectlyNative(JNIEnv* env, jobject obj,
                                                                                                   jfloatArray values, jint tag, jlong timestamp)
{
    jboolean isCopy;
    float* imu = env->GetFloatArrayElements(values, &isCopy);
    int length = env->GetArrayLength(values);
    if(SLAM)
        SLAM->pushIMUDataDirectly(imu, length, tag, timestamp);
    env->ReleaseFloatArrayElements(values, imu, 0);
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_setFrame(JNIEnv* env, jobject obj, jobject image_buffer,
                                                                                jint format, jint width, jint height, jint stride,
                                                                                jlong exposure_time, jlong timestamp)
{
    void* image_ptr = env->GetDirectBufferAddress(image_buffer);

    SLAM->setFrame((unsigned char*) image_ptr, format, width, height, stride, exposure_time, timestamp);
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_processFrameDirectlyNative(JNIEnv* env, jobject obj)
{
    SLAM->startProcessFrame();
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getImageIntrinsicsNative(JNIEnv* env, jobject obj, jobject buffer, jint order)
{
    DirectByteBufferWrapper bufferWrapper((jbyte *) env->GetDirectBufferAddress(buffer),
                                          env->GetDirectBufferCapacity(buffer),
                                          BUFFER_ORDER(order));
    SLAM->getImageIntrinsics(bufferWrapper);
}

JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getTrackingStateNative(JNIEnv* env, jobject obj)
{
    if (SLAM)
        return SLAM->getTrackingState();
    else
        return XRSLAM_STATE_TRACKING_FAIL;
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_updateAlgorithmResultNative(JNIEnv* env, jobject obj)
{
    SLAM->updateAlgorithmResult();
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_setAxisUpModeNative(JNIEnv* env, jobject obj, jint mode)
{
    SLAM->setAxisUpMode(mode);
}

JNIEXPORT void JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getViewMatrixNative(JNIEnv* env,
        jobject obj, jlong view_ptr, jint screen_rotate)
{
    SLAM->getViewMatrix(reinterpret_cast<float*>(view_ptr),screen_rotate);
}

JNIEXPORT jfloatArray JNICALL Java_com_standardar_service_slam_algorithm_SLAM_insectPlaneWithSlamResultNative(JNIEnv* env, jobject obj,
        jfloatArray ray_origin, jfloatArray ray_direction, jintArray plane_id, jfloatArray quat)
{
    vec3f ray_origin_vec = floatArrayToVec3f(env, ray_origin);
    vec3f ray_direction_vec = floatArrayToVec3f(env, ray_direction);
    vec3f quat_vec;
    int plane_id_data[1];
    env->GetIntArrayRegion(plane_id, 0, 1, plane_id_data);
    int temp_plane_id = -1;
    vec3f insect_vec = SLAM->insectPlaneWithSlamResult(ray_origin_vec, ray_direction_vec, temp_plane_id, quat_vec);
    copyVec3fToFloatArray(env, quat_vec, quat);
    jfloatArray insect = vec3fToFloatArray(env, insect_vec);
    plane_id_data[0] = temp_plane_id;
    env->SetIntArrayRegion(plane_id, 0, 1, plane_id_data);

    return insect;
}

JNIEXPORT jint JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getNumberLandMarksNative(JNIEnv* env, jobject obj)
{
    return SLAM->getNumberLandmarks();
}

JNIEXPORT jlong JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getLandMarksNative(JNIEnv* env, jobject obj)
{
    float* ptr = SLAM->getLandMarks();
    return reinterpret_cast<jlong>(ptr);
}

JNIEXPORT jfloat JNICALL Java_com_standardar_service_slam_algorithm_SLAM_getDistanceFromCameraNative(JNIEnv* env, jobject obj, jfloatArray hit_pose_arr)
{
    vec3f hit_pose = floatArrayToVec3f(env, hit_pose_arr);
    return SLAM->getDistanceFromCamera(hit_pose);
}