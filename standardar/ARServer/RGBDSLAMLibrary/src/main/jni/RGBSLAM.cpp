#include <unistd.h>
#include <sys/resource.h>
#include <cstdlib>
#include "RGBSLAM.h"
#include "log.h"
#include "CommonConstant.h"

using namespace standardar;
RGBSLAM::RGBSLAM()
{
}

int RGBSLAM::initSLAM(AAssetManager* apkAssetManager, string pathToInternalPath, int w, int h, float fovh)
{
    initDeviceMap(apkAssetManager);

    string brand, model;
    getParamDir(brand, model);
    if (memcmp(model.c_str(), "r15_mtk", sizeof("r15_mtk")) == 0
        || memcmp(model.c_str(), "r15_qcom", sizeof("r15_qcom")) == 0
        || memcmp(model.c_str(), "x21ud", sizeof("x21ud")) == 0
        || memcmp(model.c_str(), "g8341", sizeof("g8341")) == 0) {
        m_TimestampOffset = Utils::getMonoToBoottimeOffset();
    }

    string assetDeviceParamName = "multiple/param/" + brand + "/" + model + "/" + DEVICE_PARAM_NAME;
    Utils::extractAsset(apkAssetManager, assetDeviceParamName, DEVICE_PARAM_PATH);

    string assetSLAMParamName = string("multiple/param/") + SLAM_PARAM_NAME;
    Utils::extractAsset(apkAssetManager, assetSLAMParamName, SLAM_PARAM_PATH);

    char* device_config_buf = nullptr;
    int device_config_length = 0;
    bool succ = readDeviceParams(apkAssetManager, pathToInternalPath, &device_config_buf, &device_config_length);
    if (!succ)
    {
        LOGI("read device params failed");
        return -3;
    }
    getCameraIntrinsics(device_config_buf);

    int ret = XRSLAMCreate(SLAM_PARAM_PATH, DEVICE_PARAM_PATH, nullptr, "SenseSLAMSDK", nullptr);
    if (ret != 1)
    {
        LOGW("slam create failed %d", ret);
    }

    memset(&m_SLAMImageData, 0, sizeof(m_SLAMImageData));
    return ret;
}

void RGBSLAM::getCameraIntrinsics(string config) {
    auto begin = config.find("resolution:");
    auto width_first = config.find_first_of('[', begin);
    auto width_end = config.find_first_of(',', width_first);
    auto height_end = config.find_first_of(']', width_end);
    string width = config.substr(width_first + 1, width_end - width_first - 1);
    string height = config.substr(width_end + 1, height_end - width_end - 1);

    begin = config.find("intrinsics:");
    auto split_1 = config.find_first_of('[', begin);
    auto split_2 = config.find_first_of(',', split_1);
    auto split_3 = config.find_first_of(',', split_2 + 1);
    auto split_4 = config.find_first_of(',', split_3 + 1);
    auto split_5 = config.find_first_of(']', split_4);

    string fx = config.substr(split_1 + 1, split_2 - split_1 - 1);
    string fy = config.substr(split_2 + 1, split_3 - split_2 - 1);
    string cx = config.substr(split_3 + 1, split_4 - split_3 - 1);
    string cy = config.substr(split_4 + 1, split_5 - split_4 - 1);

    m_CameraIntrinsics.width = atof(width.c_str());
    m_CameraIntrinsics.height = atof(height.c_str());
    m_CameraIntrinsics.fx = atof(fx.c_str());
    m_CameraIntrinsics.fy = atof(fy.c_str());
    m_CameraIntrinsics.cx = atof(cx.c_str());
    m_CameraIntrinsics.cy = atof(cy.c_str());
    LOGI("Camera Intrinsics:%d %d %lf %lf %lf %lf", m_CameraIntrinsics.width, m_CameraIntrinsics.height,
         m_CameraIntrinsics.fx, m_CameraIntrinsics.fy,
         m_CameraIntrinsics.cx, m_CameraIntrinsics.cy);
}

void RGBSLAM::setFrame(unsigned char* image_ptr, int format, int width, int height, int stride,
                       int64_t exposure_time, int64_t timestamp)
{

    m_SLAMImageData.data = image_ptr;
    if (m_TimestampOffset != 0) {
        m_SLAMImageData.timeStamp = Utils::nanosecond2second(timestamp + m_TimestampOffset);
    } else {
        m_SLAMImageData.timeStamp = Utils::nanosecond2second(timestamp);
    }

    m_SLAMImageData.stride = stride;
    m_SLAMImageData.width = width;
    m_SLAMImageData.height = height;

    m_SLAMImageDataExtension.exposure_time = Utils::nanosecond2second(exposure_time);
    m_SLAMImageData.ext = &m_SLAMImageDataExtension;
}

void RGBSLAM::startProcessFrame()
{
    int which = PRIO_PROCESS;
    id_t tid = gettid();
    int priority = -19;
    int ret = setpriority(which, tid, priority);
    {
        ScopeMutex m(&m_Mutex);
        XRSLAMPushSensorData(XRSLAM_SENSOR_CAMERA, &m_SLAMImageData);
        XRSLAMRunOneFrame();

        XRSLAMTrackingResult result{};
        XRSLAMGetResult(XRSLAM_RESULT_STATE, &result.state);
        int size = 0;
        if(result.state == XRSLAM_STATE_TRACKING_SUCCESS) {
            XRSLAMLandmarks landmarks{nullptr, 0};
            XRSLAMFeatures features{nullptr, 0};
            XRSLAMGetResult(XRSLAM_RESULT_CAMERA_POSE, &result.camera.pose);
            XRSLAMGetResult(XRSLAM_RESULT_LANDMARKS, &landmarks);
            XRSLAMGetResult(XRSLAM_RESULT_NORMALIZED_FEATURES, &features);

            result.num_landmarks = landmarks.num_landmarks;
            if(result.num_landmarks > 0){
                result.landmarks = new float[result.num_landmarks * 3];
                for(int i=0; i<result.num_landmarks; ++i){
                    result.landmarks[i*3] = (float)landmarks.landmarks[i].x;
                    result.landmarks[i*3 + 1] = (float)landmarks.landmarks[i].y;
                    result.landmarks[i*3 + 2] = (float)landmarks.landmarks[i].z;
                }
            }

            result.num_features = features.num_features;
            if(result.num_features > 0){
                result.features = new float[result.num_features * 2];
                for(int i=0; i<result.num_features; ++i){
                    result.features[i*2] = (float)features.features[i].x;
                    result.features[i*2 + 1] = (float)features.features[i].y;
                }
            }

            size = 4 * sizeof(int) + sizeof(XRSLAMPose) + sizeof(float) * 2 * features.num_features +
                    sizeof(float) * 3 * landmarks.num_landmarks;
        }else{
            size = sizeof(int);
        }

        create_buffer(&slam_result, &slam_result_size, size);
        filled_slam_result_size = size;

        SerializeToSLAMTrackingResult(result, slam_result);
        delete [] result.landmarks;
        delete [] result.features;
    }
}
