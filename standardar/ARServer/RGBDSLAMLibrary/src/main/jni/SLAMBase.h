#ifndef ARSERVER_SLAMINTERFACE_H
#define ARSERVER_SLAMINTERFACE_H

#include <jni.h>
#include <android/asset_manager.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "commonlib.h"
#include "XRSLAM.h"
#include "XRSLAM.h"
#include "Utils.h"
#include "time_helper_debug.h"
#include "log.h"
#include "CommonConstant.h"
#include "ArMath.h"


#define CAMERA_RGB 2000

#define DEVICE_PARAM_NAME    "device_params.yaml"
#define SLAM_PARAM_NAME      "slam_params.yaml"
#define DEVICE_PARAM_PATH    "/sdcard/device_params.yaml"
#define SLAM_PARAM_PATH      "/sdcard/slam_params.yaml"

#define BRAND_DEFAULT        "default"
#define MODEL_DEFAULT        "default"

#define DIVICE_MAP           "support.txt"
extern char* slam_result;
extern int slam_result_size;
extern int filled_slam_result_size;

using namespace std;
using namespace standardar;

typedef struct XRSLAMCameraIntrinsics {
    double fx, fy, cx, cy;
    int width, height;
}XRSLAMCameraIntrinsics;

typedef struct XRSLAMCamera {
    XRSLAMPose pose;
    double rotation[3][3];    /* rotation matrix R */
    double center[3];
}XRSLAMCamera;

typedef struct XRSLAMTrackingResult{
    XRSLAMState state;
    XRSLAMCamera camera;
    int num_landmarks;
    float* landmarks;
    int num_features;
    float* features;
}XRSLAMTrackingResult;

class SLAMBase {
public:
    SLAMBase();
    virtual ~SLAMBase();
    virtual int initSLAM(AAssetManager* assetManager, string pathToInternalPath, int w, int h, float fovh) = 0;
    virtual void destroySLAM();
    virtual void setFrame(unsigned char* image_ptr, int format, int width, int height, int stride,
                        int64_t exposure_time, int64_t timestamp) = 0;
    virtual void startProcessFrame() = 0;
    virtual void pushIMUDataDirectly(float* imu, int length, int tag, int64_t timestamp);
    virtual void getImageIntrinsics(DirectByteBufferWrapper& bufferWrapper);
    virtual int getTrackingState();
    virtual void updateAlgorithmResult();
    virtual void setAxisUpMode(int mode) {m_AxisUpMode = mode;}
    virtual void getViewMatrix(float* view_ptr, int screen_rotate);
    virtual vec3f insectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t& plane_id, vec3f& quat);
    virtual int getNumberLandmarks();
    virtual float* getLandMarks();
    virtual float getDistanceFromCamera(vec3f& hit_pose);

protected:
    void getParamDir(string& brand, string& model);
    virtual bool readDeviceParams(AAssetManager* apkAssetManager, string pathToInternalPath, char** config_buffer, int* config_length);
    void initDeviceMap(AAssetManager* assetManager);

    void create_buffer(char** buffer_ptr, int* size_ptr, int size_needed);

    void convertSLAMResultPointToYAxisUp(XRSLAMTrackingResult& slam_result);
    void getYAxisUpViewMatrix(const XRSLAMCamera& slamcamera, float* out_col_major_4x4, MatrixConvertFlag convertFlag);
    void getZAxisUpViewMatrix(const XRSLAMCamera& slamcamera, float* out_col_major_4x4, MatrixConvertFlag convertFlag);
    void SLAMMatrixToRenderingMatrix(const XRSLAMCamera& slamcamera, float* out_col_major_4x4);
    void cameraLandscapeRightToPortrait(XRSLAMCamera& slamcamera);
    void cameraLandscapeRightToLandscapeLeft(XRSLAMCamera& slamcamera);
    void cameraLandscapeRightToReversePortrait(XRSLAMCamera& slamcamera);

    int32_t SerializeToSLAMTrackingResult(const XRSLAMTrackingResult& result, char* pbuffer);
    int32_t UnserializeToSLAMTrackingResult(const char* pbuffer, int32_t bufsize, XRSLAMTrackingResult* result);
    vec3f CalPlaneLineIntersectPoint(vec3f planeNormal, vec3f planePoint, vec3f lineVector, vec3f linePoint, bool& infinit);
    Quatf FromTo(const vec3f &v1, const vec3f &v2);

protected:
    int64_t m_TimestampOffset;

    map<string, string> m_Brand_Map;
    map<string, string> m_Model_Map;

    XRSLAMTrackingResult m_SLAM_Result;
    int m_AxisUpMode;

    pthread_mutex_t m_Mutex;
    char* m_CachedBuf;
    int32_t m_CachedSize;
    int32_t m_FilledSize;
    XRSLAMCameraIntrinsics m_CameraIntrinsics;
};

#endif //ARSERVER_SLAMINTERFACE_H
