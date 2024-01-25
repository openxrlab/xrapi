#include <unistd.h>
#include <math.h>
#include <fstream>
#include <string>
#include "SLAMBase.h"
#include "log.h"
#include "picojson.h"
#include "ArMath.h"

char* slam_result = nullptr;
int slam_result_size = 0;
int filled_slam_result_size = 0;

using namespace standardar;

SLAMBase::SLAMBase(){
    m_TimestampOffset = 0;
    m_Mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

    string phone_brand = Utils::getPhoneBrand();
    string phone_model = Utils::getPhoneModel();
    LOGI("brand:%s model:%s", phone_brand.c_str(), phone_model.c_str());
    memset(&m_SLAM_Result, 0, sizeof(m_SLAM_Result));
    m_AxisUpMode = AR_AXIS_UP_MODE_Y;

    m_CachedBuf = nullptr;
    m_CachedSize = 0;
    m_FilledSize = 0;
}

SLAMBase::~SLAMBase()
{
    if (m_CachedBuf != nullptr)
    {
        delete[] m_CachedBuf;
        m_CachedSize = 0;
        m_FilledSize = 0;
        m_CachedBuf = nullptr;
    }
}

void SLAMBase::destroySLAM()
{
    ScopeMutex m(&m_Mutex);
    slam_result_size = 0;
    filled_slam_result_size = 0;
    if(m_CachedBuf)
        memset(m_CachedBuf, 0, m_CachedSize);
    XRSLAMDestroy();
    m_TimestampOffset = 0;
}

void SLAMBase::initDeviceMap(AAssetManager *assetManager)
{
    char* file_buf = nullptr;
    int file_buf_length = 0;
    Utils::readFromAsset(assetManager, DIVICE_MAP, &file_buf, &file_buf_length);
    char* str_buf = new char[file_buf_length + 1];
    memcpy(str_buf, file_buf, file_buf_length);
    str_buf[file_buf_length] = '\0';

    std::string jsonStr(str_buf);
    picojson::value v;
    std::string err = picojson::parse(v, jsonStr);
    if (!err.empty())
    {
        LOGI("parser support file error %s", err.c_str());
        return;
    }

    const auto& array = v.get<picojson::array>();
    m_Brand_Map.clear();
    m_Model_Map.clear();
    for (picojson::array::const_iterator it = array.begin(); it != array.end(); it++)
    {
        picojson::object obj = it->get<picojson::object>();
        picojson::array& slam_array = obj["SLAM"].get<picojson::array>();
        for (picojson::array::const_iterator slam_it = slam_array.begin(); slam_it != slam_array.end(); slam_it++)
        {
            picojson::object brand_obj = slam_it->get<picojson::object>();
            for(auto brand_obj_iter = brand_obj.begin(); brand_obj_iter != brand_obj.end(); ++brand_obj_iter)
            {
                picojson::array brand_array = brand_obj_iter->second.get<picojson::array>();
                picojson::array::const_iterator brand_array_iter = brand_array.begin();
                picojson::object name_obj = brand_array_iter->get<picojson::object>();
                std::string name_attr = name_obj["name"].get<std::string>();
                m_Brand_Map[brand_obj_iter->first] = name_attr;
                ++brand_array_iter;
                picojson::object model_obj = brand_array_iter->get<picojson::object>();
                picojson::array model_obj_array = model_obj["Model"].get<picojson::array>();
                for (picojson::array::const_iterator model_iter = model_obj_array.begin();
                        model_iter != model_obj_array.end(); ++model_iter)
                {
                    picojson::object per_model_obj = model_iter->get<picojson::object>();
                    m_Model_Map[per_model_obj["model"].get<std::string>().c_str()] = per_model_obj["name"].get<std::string>().c_str();
                }
            }
        }
    }
}

void SLAMBase::create_buffer(char** buffer_ptr, int* size_ptr, int size_needed)
{
    int size_expand = size_needed + 1024;
    if (size_needed > *size_ptr) {
        if (*buffer_ptr) {
            delete[] *buffer_ptr;
            *buffer_ptr = new char[size_expand];
            *size_ptr = size_expand;
        } else {
            *buffer_ptr = new char[size_expand];
            *size_ptr = size_expand;
        }
    }
}

void SLAMBase::convertSLAMResultPointToYAxisUp(XRSLAMTrackingResult &slam_result) {
    float tempf = 0.0f;
    int land_mark_count = slam_result.num_landmarks;
    for (int i = 0; i < land_mark_count; ++i) {
        tempf = slam_result.landmarks[i*3+1];
        slam_result.landmarks[i*3+1] = slam_result.landmarks[i*3+2];
        slam_result.landmarks[i*3+2] = -tempf;
    }
}

void SLAMBase::getYAxisUpViewMatrix(const XRSLAMCamera&slamcamera, float *out_col_major_4x4,
                                    MatrixConvertFlag convertFlag) {
    XRSLAMCamera copycamera;
    memcpy(&copycamera, &slamcamera, sizeof(XRSLAMCamera));

    if(convertFlag==LANDSCAPERIGHT_TO_PORTRAIT)
        cameraLandscapeRightToPortrait(copycamera);
    else if(convertFlag ==LANDSCAPERIGHT_TO_LANDSCAPELEFT)
        cameraLandscapeRightToLandscapeLeft(copycamera);
    else if(convertFlag == LANDSCAPERIGHT_TO_REVERSEPORTAIT)
        cameraLandscapeRightToReversePortrait(copycamera);

    mat4f zupmatrix;
    SLAMMatrixToRenderingMatrix(copycamera, zupmatrix.data);
    mat4f rotatex90 = mat4f::createRotationAroundAxis(-90, 0, 0);
    mat4f yupmatrix = zupmatrix*rotatex90;
    memcpy(out_col_major_4x4, yupmatrix.data, sizeof(float)*16);
}

void SLAMBase::getZAxisUpViewMatrix(const XRSLAMCamera&slamcamera, float *out_col_major_4x4,
                                    MatrixConvertFlag convertFlag) {
    XRSLAMCamera copycamera;
    memcpy(&copycamera, &slamcamera, sizeof(XRSLAMCamera));

    if(convertFlag==LANDSCAPERIGHT_TO_PORTRAIT)
        cameraLandscapeRightToPortrait(copycamera);
    else if(convertFlag ==LANDSCAPERIGHT_TO_LANDSCAPELEFT)
        cameraLandscapeRightToLandscapeLeft(copycamera);
    else if(convertFlag == LANDSCAPERIGHT_TO_REVERSEPORTAIT)
        cameraLandscapeRightToReversePortrait(copycamera);

    mat4f zupmatrix;
    SLAMMatrixToRenderingMatrix(copycamera, zupmatrix.data);
    memcpy(out_col_major_4x4, zupmatrix.data, sizeof(float)*16);
}

void SLAMBase::cameraLandscapeRightToPortrait(XRSLAMCamera& slamcamera)
{
    for (int i = 0; i < 3; ++i)
    {
        std::swap(slamcamera.rotation[0][i], slamcamera.rotation[1][i]);
        slamcamera.rotation[0][i] = -slamcamera.rotation[0][i];
    }

    std::swap(slamcamera.pose.translation[0], slamcamera.pose.translation[1]);
    slamcamera.pose.translation[0] = -slamcamera.pose.translation[0];
}

void SLAMBase::cameraLandscapeRightToLandscapeLeft(XRSLAMCamera& slamcamera)
{
    for (int i = 0; i < 3; ++i)
    {
        slamcamera.rotation[0][i] = -slamcamera.rotation[0][i];
        slamcamera.rotation[1][i] = -slamcamera.rotation[1][i];
    }
    slamcamera.pose.translation[0] = -slamcamera.pose.translation[0];
    slamcamera.pose.translation[1] = -slamcamera.pose.translation[1];
}

void SLAMBase::cameraLandscapeRightToReversePortrait(XRSLAMCamera& slamcamera)
{
    for (int i = 0; i < 3; ++i)
    {
        std::swap(slamcamera.rotation[0][i], slamcamera.rotation[1][i]);
        slamcamera.rotation[1][i] = -slamcamera.rotation[1][i];
    }
    std::swap(slamcamera.pose.translation[0], slamcamera.pose.translation[1]);
    slamcamera.pose.translation[1] = -slamcamera.pose.translation[1];
}

int32_t SLAMBase::SerializeToSLAMTrackingResult(const XRSLAMTrackingResult& result, char* pbuffer){

    const char* ptempbuf = pbuffer;
    int32_t offset = 0;


    //state
    memcpy((void *) (ptempbuf + offset), &result.state, sizeof(XRSLAMState));
    offset += sizeof(XRSLAMState);

    if(result.state != XRSLAM_STATE_TRACKING_SUCCESS)
        return offset;

    //pose
    memcpy((void *) (ptempbuf + offset), &result.camera.pose, sizeof(XRSLAMPose));
    offset += sizeof(XRSLAMPose);

    //features
    memcpy((void *) (ptempbuf + offset), &result.num_features, sizeof(int));
    offset += sizeof(int);

    memcpy((void *) (ptempbuf + offset), result.features, sizeof(float) * 2 * result.num_features);
    offset += sizeof(float) * 2 * result.num_features;
    //landmarks
    memcpy((void *) (ptempbuf + offset), &result.num_landmarks, sizeof(int));
    offset += sizeof(int);

    memcpy((void *) (ptempbuf + offset), result.landmarks, sizeof(float) * 3 * result.num_landmarks);
    offset += sizeof(float) * 3 * result.num_landmarks;
    return offset;
}

int32_t SLAMBase::UnserializeToSLAMTrackingResult(const char *pbuffer, int32_t bufsize,
                                                  XRSLAMTrackingResult *result) {
    {
        memset(result, 0, sizeof(XRSLAMTrackingResult));

        const char* ptempbuf = pbuffer;
        int32_t offset = 0;

        // state
        memcpy(&(result->state), ptempbuf+offset, sizeof(result->state));
        offset += sizeof(result->state);

        if(result->state != XRSLAM_STATE_TRACKING_SUCCESS)
            return offset;

        //pose
        memcpy(&(result->camera.pose), (ptempbuf + offset), sizeof(XRSLAMPose));
        offset += sizeof(XRSLAMPose);

        quat<double> tmpQuat{result->camera.pose.quaternion[3], result->camera.pose.quaternion[0], result->camera.pose.quaternion[1], result->camera.pose.quaternion[2]};
        mat3d mat = tmpQuat.rotmat();
        vec3d trans = mat.transpose() * vec3d(-result->camera.pose.translation[0], -result->camera.pose.translation[1], -result->camera.pose.translation[2]);

        for(int i=0; i<3; ++i){
            result->camera.center[i] = result->camera.pose.translation[i];
            result->camera.pose.translation[i] = trans[i];
        }

        for(int i=0; i<3; ++i)
            for(int j=0; j<3; ++j)
                result->camera.rotation[i][j] = mat.at(i, j);

        //features
        memcpy(&(result->num_features), ptempbuf + offset, sizeof(int));
        offset += sizeof(int);

        if(result->num_features > 0){
            result->features = (float*)(ptempbuf + offset);
            offset += sizeof(float) * 2 * result->num_features;
        }

        //landmarks
        memcpy(&(result->num_landmarks), ptempbuf + offset, sizeof(int));
        offset += sizeof(int);

        if(result->num_landmarks > 0){
            result->landmarks = (float*)(ptempbuf + offset);
            offset += sizeof(float) * 3 * result->num_landmarks;
        }

        return offset;
    }
}

void SLAMBase::SLAMMatrixToRenderingMatrix(const XRSLAMCamera& slamcamera,
                                           float *out_col_major_4x4) {
    out_col_major_4x4[0] = slamcamera.rotation[0][0];
    out_col_major_4x4[1] = -slamcamera.rotation[1][0];
    out_col_major_4x4[2] = -slamcamera.rotation[2][0];
    out_col_major_4x4[3] = 0.0f;

    out_col_major_4x4[4] = slamcamera.rotation[0][1];
    out_col_major_4x4[5] = -slamcamera.rotation[1][1];
    out_col_major_4x4[6] = -slamcamera.rotation[2][1];
    out_col_major_4x4[7] = 0.0f;

    out_col_major_4x4[8] = slamcamera.rotation[0][2];
    out_col_major_4x4[9] = -slamcamera.rotation[1][2];
    out_col_major_4x4[10] = -slamcamera.rotation[2][2];
    out_col_major_4x4[11] = 0.0f;

    out_col_major_4x4[12] = slamcamera.pose.translation[0];
    out_col_major_4x4[13] = -slamcamera.pose.translation[1];
    out_col_major_4x4[14] = -slamcamera.pose.translation[2];
    out_col_major_4x4[15] = 1.0f;
}

void SLAMBase::getParamDir(string& brand, string& model)
{
    string phone_brand = Utils::getPhoneBrand();
    string phone_model = Utils::getPhoneModel();

    auto brand_iter = m_Brand_Map.find(phone_brand);
    if (brand_iter == m_Brand_Map.end())
    {
        brand = BRAND_DEFAULT;
    }
    else
    {
        brand = brand_iter->second;
    }

    auto model_iter = m_Model_Map.find(phone_model);
    if (model_iter == m_Model_Map.end())
    {
        model = MODEL_DEFAULT;
    }
    else
    {
        model = model_iter->second;
    }
}

bool SLAMBase::readDeviceParams(AAssetManager* apkAssetManager, string apkInternalPath,
                      char** config_buffer, int* config_length)
{
    string filename = apkInternalPath + "/" + DEVICE_PARAM_NAME;

    string brand, model;
    getParamDir(brand, model);
    string assetParamName = "multiple/param/" + brand + "/" + model + "/" + DEVICE_PARAM_NAME;
    LOGI("choose param path:%s", assetParamName.c_str());
    bool succ = Utils::readFromAsset(apkAssetManager, assetParamName, config_buffer, config_length);
    if (!succ)
    {
        LOGI("failed load param path:%s", assetParamName.c_str());
    }
    return succ;
}

void SLAMBase::pushIMUDataDirectly(float *imu, int length, int tag, int64_t timestamp)
{
    struct timespec ts;
    clock_gettime( /*CLOCK_MONOTONIC*/ CLOCK_BOOTTIME, &ts );
    int64_t curT = ts.tv_sec * 1000ULL * 1000ULL * 1000ULL + ts.tv_nsec;
    switch (tag)
    {
        case IMU_ACC_TAG:
        {
            XRSLAMAcceleration acc;
            memset(acc.data, 0, sizeof(acc.data));
            for (int i = 0; i < 3; ++i)
            {
                acc.data[i] = imu[i];
            }
            acc.timestamp = Utils::nanosecond2second(timestamp);
            XRSLAMPushSensorData(XRSLAM_SENSOR_ACCELERATION, &acc);
            break;
        }
        case IMU_GYRO_TAG:
        {
            XRSLAMGyroscope gyr;
            memset(gyr.data, 0, sizeof(gyr.data));
            for (int i = 0; i < 3; ++i) {
                gyr.data[i] = imu[i];
            }
            gyr.timestamp = Utils::nanosecond2second(timestamp);
            XRSLAMPushSensorData(XRSLAM_SENSOR_GYROSCOPE, &gyr);
            break;
        }
        case IMU_GRAVITY_TAG:
        {
            XRSLAMGravity gravity;
            memset(gravity.data, 0, sizeof(gravity.data));
            for (int i = 0; i < 3; ++i) {
                gravity.data[i] = imu[i];
            }
            gravity.timestamp = Utils::nanosecond2second(timestamp);
            XRSLAMPushSensorData(XRSLAM_SENSOR_GRAVITY, &gravity);
            break;
        }
        case IMU_RV_TAG:
        {
            XRSLAMRotationVector rv;
            memset(rv.data, 0, sizeof(rv.data));
            for (int i = 0; i < 4; ++i) {
                rv.data[i] = imu[i];
            }
            rv.timestamp = Utils::nanosecond2second(timestamp);
            XRSLAMPushSensorData(XRSLAM_SENSOR_ROTATION_VECTOR, &rv);
            break;
        }
        default:
            break;
    }
}

void SLAMBase::getImageIntrinsics(DirectByteBufferWrapper& bufferWrapper)
{
    bufferWrapper.put<int>(SEND_MAGIC_WORD);
    bufferWrapper.put<int>(sizeof(int) * 4 + sizeof(float) * 4);
    bufferWrapper.put<int>((int)m_CameraIntrinsics.width);
    bufferWrapper.put<int>((int)m_CameraIntrinsics.height);
    bufferWrapper.put<float>((float)m_CameraIntrinsics.fx);
    bufferWrapper.put<float>((float)m_CameraIntrinsics.fy);
    bufferWrapper.put<float>((float)m_CameraIntrinsics.cx);
    bufferWrapper.put<float>((float)m_CameraIntrinsics.cy);
}

int SLAMBase::getTrackingState() {
    ScopeMutex m(&m_Mutex);
    return m_SLAM_Result.state;
}

void SLAMBase::updateAlgorithmResult() {
    ScopeMutex m(&m_Mutex);
    if (m_CachedBuf == nullptr || m_CachedSize < slam_result_size) {
        delete[] m_CachedBuf;
        m_CachedBuf = new char[slam_result_size];
        m_CachedSize = slam_result_size;
    }

    if (slam_result_size == 0 || slam_result == nullptr)
        return;

    memcpy(m_CachedBuf, slam_result, slam_result_size);
    UnserializeToSLAMTrackingResult(m_CachedBuf, filled_slam_result_size, &m_SLAM_Result);
    if (m_AxisUpMode == AR_AXIS_UP_MODE_Y) {
        convertSLAMResultPointToYAxisUp(m_SLAM_Result);
    }
}

void SLAMBase::getViewMatrix(float *view_ptr, int screen_rotate) {
    ScopeMutex m(&m_Mutex);
    switch (screen_rotate)
    {
        case ROTATION_0:
        {
            if(m_AxisUpMode == AR_AXIS_UP_MODE_Y)
                getYAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_PORTRAIT);
            else
                getZAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_PORTRAIT);

            break;
        }
        case ROTATION_180:
        {
            if(m_AxisUpMode == AR_AXIS_UP_MODE_Y)
                getYAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_REVERSEPORTAIT);
            else
                getZAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_REVERSEPORTAIT);

            break;
        }
        case ROTATION_90:
        {
            if(m_AxisUpMode == AR_AXIS_UP_MODE_Y)
                getYAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, NO_CONVERT);
            else
                getZAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, NO_CONVERT);

            break;
        }
        case ROTATION_270:
        {
            if(m_AxisUpMode == AR_AXIS_UP_MODE_Y)
                getYAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_LANDSCAPELEFT);
            else
                getZAxisUpViewMatrix(m_SLAM_Result.camera, view_ptr, LANDSCAPERIGHT_TO_LANDSCAPELEFT);
            break;
        }
    }
}

vec3f SLAMBase::CalPlaneLineIntersectPoint(vec3f planeNormal, vec3f planePoint, vec3f lineVector, vec3f linePoint, bool& infinit)
{
    vec3f returnResult;
    float vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t,vpt;

    vp1 = planeNormal.x;
    vp2 = planeNormal.y;
    vp3 = planeNormal.z;

    n1 = planePoint.x;
    n2 = planePoint.y;
    n3 = planePoint.z;

    v1 = lineVector.x;
    v2 = lineVector.y;
    v3 = lineVector.z;

    m1 = linePoint.x;
    m2 = linePoint.y;
    m3 = linePoint.z;

    vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;

    infinit = true;
    //首先判断直线是否与平面平行
    if (vpt == 0)
    {
        infinit = false;
        returnResult.x = M_FLOAT_MAX;
        returnResult.y = M_FLOAT_MAX;
        returnResult.z = M_FLOAT_MAX;
    }
    else
    {
        t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
        returnResult.x = m1 + v1 * t;
        returnResult.y = m2 + v2 * t;
        returnResult.z = m3 + v3 * t;
    }

    return returnResult;
}

Quatf SLAMBase::FromTo(const vec3f &v1, const vec3f &v2)
{
    Quatf q;
    float dot = v1.dotProduct(v2);

    if ( dot >= 1 )
    {
        q = Quatf(1,0,0,0);
    }
    else if ( dot < -0.999999 )
    {
        vec3f axis = vec3f(1,0,0).crossProduct(v2);

        if (axis.length() == 0)
            axis = vec3f(0,1,0).crossProduct(v2);
        axis.normalize();
        q = q.fromAxisRot(axis, 180);
    }
    else
    {
        float s = sqrt( (1+dot)*2 );
        float invs = 1 / s;

        vec3f c = v1.crossProduct(v2);

        q.v.x = c.x * invs;
        q.v.y = c.y * invs;
        q.v.z = c.z * invs;
        q.w = s * 0.5f;
    }
    q.normalize();
    return q;
}

vec3f SLAMBase::insectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t &plane_id,
                                         vec3f &quat) {
    ScopeMutex m(&m_Mutex);
    vec3f minInsectpt;
    minInsectpt.x = M_FLOAT_MAX;
    minInsectpt.y = M_FLOAT_MAX;
    minInsectpt.z = M_FLOAT_MAX;

    vec3f planeCenter{0, 0, 0};
    vec3f planeVector;

    if(m_AxisUpMode == AR_AXIS_UP_MODE_Y) {
        planeVector.x=0.0f;
        planeVector.y=1.0f;
        planeVector.z=0.0f;
    } else {

        planeVector.x=0.0f;
        planeVector.y=0.0f;
        planeVector.z=1.0f;
    }

    float minDistance = M_FLOAT_MAX;
    for (int i = 0; i < m_SLAM_Result.num_landmarks; ++i) {
        vec3f cur{m_SLAM_Result.landmarks[i*3],
                  m_SLAM_Result.landmarks[i*3+1],
                  m_SLAM_Result.landmarks[i*3+2]};

        float length = (cur - ray_origin).length();
        if(length < minDistance) {
            minInsectpt = cur;
            minDistance = length;
        }
    }

    quat = planeVector;
    plane_id = 0;
    return minInsectpt;
}

int SLAMBase::getNumberLandmarks() {
    ScopeMutex m(&m_Mutex);
    return m_SLAM_Result.num_landmarks;
}

float* SLAMBase::getLandMarks() {
    ScopeMutex m(&m_Mutex);
    return m_SLAM_Result.landmarks;
}

float SLAMBase::getDistanceFromCamera(vec3f &hit_pose) {
    ScopeMutex m(&m_Mutex);
    XRSLAMCamera & slamCamera = m_SLAM_Result.camera;
    vec3f center;
    center.x = slamCamera.center[0];
    center.y = slamCamera.center[1];
    center.z = slamCamera.center[2];
    float distance = sqrt(pow(center.x - hit_pose.x,2)
                          + pow(center.y - hit_pose.y, 2)
                          + pow(center.z - hit_pose.z, 2));
    return distance;
}