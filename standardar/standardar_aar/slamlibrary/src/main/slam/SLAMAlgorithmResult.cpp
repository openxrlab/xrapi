#include <ArAnchor.h>
#include "SLAMAlgorithmResult.h"
#include "picojson.h"
using namespace standardar;
using namespace picojson;

#define XRSLAM_VIO_INITIALIZING          0x01
#define XRSLAM_BE_INITIALIZING           0x02
#define XRSLAM_VIO_TRACKING_FAIL         0x04
#define XRSLAM_BE_TRACKING_FAIL          0x08

#define XRSLAMIsVIOInitializing(value)       (value & XRSLAM_VIO_INITIALIZING)
#define XRSLAMIsBEInitializing(value)        (value & XRSLAM_BE_INITIALIZING)
#define XRSLAMIsVIOLost(value)               (value & XRSLAM_VIO_TRACKING_FAIL)
#define XRSLAMIsBELost(value)                (value & XRSLAM_BE_TRACKING_FAIL)

#define XRSLAMIsInitializing(value)          (XRSLAMIsVIOInitializing(value))
#define XRSLAMIsTrackingFaild(value)         (XRSLAMIsVIOLost(value))
#define XRSLAMIsNotStableTracking(value)     (!(XRSLAMIsVIOLost(value) && true) \
                                                        && XRSLAMIsBELost(value))
#define XRSLAMIsNormalTracking(value)        (XRSLAMIsVIOInitializing(~value) \
                                                        && XRSLAMIsBEInitializing(~value)\
                                                        && XRSLAMIsVIOLost(~value)\
                                                        && XRSLAMIsBELost(~value))


SLAMAlgorithmResult::SLAMAlgorithmResult()
{
    m_Mutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    memset(&m_LightEstimationResult, 0, sizeof(m_LightEstimationResult));
    m_SLAMJsonStr = "";
    m_SLAMInfo = "";
    m_RegionInfo = "";
    m_initProgress = 0;
    m_MapId[0] = '\0';
    m_CurrentAnchor = nullptr;
    m_mapCoupleState = 1;
}

SLAMAlgorithmResult::~SLAMAlgorithmResult()
{
    cleanResult();
}

void SLAMAlgorithmResult::setConfig(const CConfig &config)
{
    m_Config = config;
}

void SLAMAlgorithmResult::updateResult(jbyte *result_ptr, int pos, int length, int order)
{
}

void SLAMAlgorithmResult::cleanResult()
{
    ScopeMutex m(&m_Mutex);
    m_SLAMInfo = "";
    m_RegionInfo = "";
    m_initProgress = 0;
    m_SLAMInfo = "";
    m_trackingStateDetails = 0;
    m_mapQualityStatus = 0;
    m_mapCoupleState = 1;
}

void SLAMAlgorithmResult::update()
{
    ScopeMutex m(&m_Mutex);
    updateSLAMJsonStr();
	updateCloudAnchor();
}

CCameraIntrinsics SLAMAlgorithmResult::getCameraIntrinsics()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getCameraIntrinsics();
}

bool SLAMAlgorithmResult::isDepthImageAvaliable()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.isDepthImageAvaliable();
}

bool SLAMAlgorithmResult::getDepthImage(unsigned char* depth_image)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDepthImage(depth_image);
}

int SLAMAlgorithmResult::getDepthWidth()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDepthImageWidth();
}

int SLAMAlgorithmResult::getDepthHeight()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDepthImageHeight();
}

std::string SLAMAlgorithmResult::getRegionInfo()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getRegionInfo();
}

ARTrackingState SLAMAlgorithmResult::getTrackingState()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    int state = slamAlgorithmProxyWrapper.getTrackingState();
    if(state == StandardAR_SLAM_TRACKING_SUCCESS || state == StandardAR_SLAM_TRACKING_SUCCESS_NO_MAP)
        return ARTRACKING_STATE_SUCCESS;
    if(state == StandardAR_SLAM_INITIALIZING || state == StandardAR_SLAM_TRACKING_RELOCALIZING)
        return ARTRACKING_STATE_INITIALIZING;
    if(state == StandardAR_SLAM_TRACKING_FAIL)
        return ARTRACKING_STATE_LOST;
    return ARTRACKING_STATE_STOPPED;
}

void SLAMAlgorithmResult::updateSLAMJsonStr()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    m_SLAMJsonStr = slamAlgorithmProxyWrapper.getSLAMInfo();
    parseJsonStr(m_SLAMJsonStr);
}

void SLAMAlgorithmResult::parseJsonStr(std::string jsonStr)
{
    picojson::value v;
    std::string err = picojson::parse(v, jsonStr);
    if (!err.empty()) {
        //LOGI("%s", err.c_str());
        m_SLAMInfo = jsonStr;
        return;
    }

    if (!v.is<picojson::object>()) {
        //LOGI("%s", "JSON is not an object");
        m_SLAMInfo = jsonStr;
        return;
    }

    m_SLAMInfo = "";

    const picojson::value::object &obj = v.get<picojson::object>();
    std::string s;

    for (picojson::value::object::const_iterator i = obj.begin(); i != obj.end(); ++i)
    {
        m_SLAMInfo += i->first + ": " + i->second.to_str() + "\n";

        if (i->first == "progress")
            m_initProgress = (int32_t)i->second.get<double>();
        if (i->first == "map_quality_status")
            m_mapQualityStatus = i->second.get<double>();
        if (i->first == "tracking_state_detail")
            m_trackingStateDetails = (int)i->second.get<double>();
        if (i->first == "cloud_locate_res")
            m_mapCoupleState = (int)i->second.get<double>();
    }

}

void SLAMAlgorithmResult::updateCloudAnchor()
{
    if (m_CurrentAnchor == nullptr)
        return;

    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    int result = slamAlgorithmProxyWrapper.getCloudResult();
    if (result != 1) {
        if (result == ARCLOUD_ANCHOR_STATE_SUCCESS) {
            if (m_CloudMode == CLOUD_HOST) {
                std::string anchorId = slamAlgorithmProxyWrapper.getAnchorId();
                ((CCloudAnchor*)m_CurrentAnchor)->setCloudAnchorId(const_cast<char *>(anchorId.c_str()));
                std::string mapId = slamAlgorithmProxyWrapper.getMapId();
                strcpy(m_MapId, mapId.c_str());
            } else if (m_CloudMode == CLOUD_RESOLVE) {
                float pose[7];
                slamAlgorithmProxyWrapper.getCloudAnchorPose(pose);

                CPose cPose;
                cPose.tx = pose[0];
                cPose.ty = pose[1];
                cPose.tz = pose[2];
                cPose.qx = pose[3];
                cPose.qy = pose[4];
                cPose.qz = pose[5];
                cPose.qw = pose[6];
                m_CurrentAnchor->m_Pose = cPose;
            }
            ((CCloudAnchor*)m_CurrentAnchor)->m_State = ARTRACKING_STATE_SUCCESS;
        }
        else
            ((CCloudAnchor*)m_CurrentAnchor)->m_State = ARTRACKING_STATE_STOPPED;

        ((CCloudAnchor*)m_CurrentAnchor)->setCloudAnchorState((ARCloudAnchorState )result);
        m_CurrentAnchor = nullptr;
    }
}

void SLAMAlgorithmResult::setMapIdAndAnchorId(std::string mapId, std::string anchorId)
{
    if(m_CurrentAnchor == nullptr) {
        return;
    }
    strcpy(m_MapId, mapId.c_str());
    ((CCloudAnchor*)m_CurrentAnchor)->setCloudAnchorId(const_cast<char *>(anchorId.c_str()));
}

void SLAMAlgorithmResult::setCloudAnchorState(ARCloudAnchorState result, ARTrackingState state)
{
    if(m_CurrentAnchor == nullptr) {
        return;
    }
    ((CCloudAnchor*)m_CurrentAnchor)->setCloudAnchorState(result);
    ((CCloudAnchor*)m_CurrentAnchor)->m_State = state;
    m_CurrentAnchor = nullptr;
}

void SLAMAlgorithmResult::setCloudAnchorPose(float *pose)
{
    if(m_CurrentAnchor == nullptr || pose == nullptr) {
        return;
    }

    CPose cPose;
    cPose.tx = pose[0];
    cPose.ty = pose[1];
    cPose.tz = pose[2];
    cPose.qx = pose[3];
    cPose.qy = pose[4];
    cPose.qz = pose[5];
    cPose.qw = pose[6];
    m_CurrentAnchor->m_Pose = cPose;
}

bool SLAMAlgorithmResult::isSLAMIntializing()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    int state = slamAlgorithmProxyWrapper.getTrackingState();
    if(state==StandardAR_SLAM_INITIALIZING){
        return true;
    }else{
        return false;
    }
}

int32_t SLAMAlgorithmResult::getNumberLandMarks() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getNumberLandMarks();
}

float* SLAMAlgorithmResult::getLandMarks() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getLandMarks();
}

ARAlgorithmState SLAMAlgorithmResult::getSLAMTrackingDetails()
{
    if(XRSLAMIsInitializing(m_trackingStateDetails))
        return ARALGORITHM_STATE_INITIALIZING;
    if(XRSLAMIsTrackingFaild(m_trackingStateDetails))
        return ARALGORITHM_STATE_FAILED;
    if(XRSLAMIsNotStableTracking(m_trackingStateDetails))
        return ARALGORITHM_STATE_NOT_STABLE;
    if(XRSLAMIsNormalTracking(m_trackingStateDetails))
        return ARALGORITHM_STATE_NORMAL;
    return ARALGORITHM_STATE_INITIALIZING;
}

ARIlluminationEstimateState SLAMAlgorithmResult::getLightEstimateState()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getLightEstimateState();
}

ARIlluminationEstimateMode SLAMAlgorithmResult::getLightEstimateMode()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getLightEstimateMode();
}

float SLAMAlgorithmResult::getPixelIntensity()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPixelIntensity();
}

void SLAMAlgorithmResult::getSphericalHarmonicLighting(float* params)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    if (params != nullptr)
        slamAlgorithmProxyWrapper.getSphericalHarmonicLighting(params);
    return;
}

void SLAMAlgorithmResult::getEnvironmentTextureSize(int * width,int * height )
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    int size[2];
    slamAlgorithmProxyWrapper.getEnvironmentTextureSize(size);
    *width = size[0];
    *height = size[1];
    return;
}

void SLAMAlgorithmResult::getColorCorrection(float* correction)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    if (correction != nullptr)
        slamAlgorithmProxyWrapper.getColorCorrection(correction);
    return;
}

void SLAMAlgorithmResult::getEnvironmentalHdrMainLightDirection(float* direction)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    if (direction != nullptr)
        slamAlgorithmProxyWrapper.getEnvironmentalHdrMainLightDirection(direction);
    return;
}

void SLAMAlgorithmResult::getEnvironmentalHdrMainLightIntensity(float* intensity)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    if (intensity != nullptr)
        slamAlgorithmProxyWrapper.getEnvironmentalHdrMainLightIntensity(intensity);
    return;
}

void SLAMAlgorithmResult::getEnvironmentalHdrCubemap(float* cubemap)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getEnvironmentalHdrCubemap(cubemap);
    return;
}

int64_t SLAMAlgorithmResult::getLightTimeStamp()
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getLightTimeStamp();
}

void SLAMAlgorithmResult::setBoundingBox(const float* vertex, const float* matrix)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.setBoundingBox(vertex, matrix);
}

void SLAMAlgorithmResult::getBoundingBoxPointCloud(float** points, int* points_num)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getBoundingBoxPointCloud(points, points_num);
}

void SLAMAlgorithmResult::getScanningResult(uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size)
{
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getScanningResult(out_raw_bytes, out_raw_bytes_size);
}

void SLAMAlgorithmResult::getDenseMeshIndex(unsigned short *index_array) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getDenseMeshIndex(index_array);
}

int32_t SLAMAlgorithmResult::getDenseMeshIndexCount() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDenseMeshIndexCount();
}

void SLAMAlgorithmResult::getDenseMeshVertex(float *vertex_array) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getDenseMeshVertex(vertex_array);
}

int SLAMAlgorithmResult::getDenseMeshVertexCount() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDenseMeshVertexCount();
}

int32_t SLAMAlgorithmResult::getSLAMPlaneIndex(int32_t planeid) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getSLAMPlaneIndex(planeid);
}

void SLAMAlgorithmResult::getCameraViewMatrix(float *view, int screen_rotate) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getViewMatrix(view, screen_rotate);
}

float SLAMAlgorithmResult::getSLAMLightIntensity() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getSLAMLightIntensity();
}

vec3f SLAMAlgorithmResult::insectPlaneWithSlamResult(standardar::vec3f ray_origin,
                                                     standardar::vec3f ray_direction,
                                                     int32_t &plane_id, standardar::vec3f &quat) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.insectPlaneWithSlamResult(ray_origin,ray_direction,plane_id,quat);
}

vec3f SLAMAlgorithmResult::getPlaneCenter(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPlaneCenter(plane_id);
}

void SLAMAlgorithmResult::getDenseMeshNormal(float *vertex_normal) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getDenseMeshNormal(vertex_normal);
}

int SLAMAlgorithmResult::getDenseMeshFormat() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDenseMeshFormat();
}

vec3f SLAMAlgorithmResult::getCenterPose(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getCenterPose(plane_id);
}

vec3f SLAMAlgorithmResult::insectSurfaceMesh(float x, float y, standardar::vec3f &normal, float screen_aspect, int screen_rotate) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.insectSurfaceMesh(x, y, normal, screen_aspect, screen_rotate);
}

float SLAMAlgorithmResult::getDistanceFromCamera(standardar::vec3f hit_pose) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getDistanceFromCamera(hit_pose);
}

int SLAMAlgorithmResult::getPlaneVertexCount() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPlaneVertexCount();
}

void SLAMAlgorithmResult::getPlaneVertexArray(float *vertex_array) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPlaneVertexArray(vertex_array);
}

int SLAMAlgorithmResult::getPlaneIndexCount() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPlaneIndexCount();
}

void SLAMAlgorithmResult::getPlaneIndexArray(unsigned short *index_array) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getPlaneIndexArray(index_array);
}

ARPlaneType SLAMAlgorithmResult::getPlaneType(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return (ARPlaneType)slamAlgorithmProxyWrapper.getPlaneType(plane_id);
}

void SLAMAlgorithmResult::getPlaneNormal(float *out_plane_normal, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getPlaneNormal(out_plane_normal, plane_id);
}

int SLAMAlgorithmResult::getPolygonSize(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPolygonSize(plane_id);
}

void SLAMAlgorithmResult::getPolygon(float *out_data, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getPolygon(out_data, plane_id);
}

float SLAMAlgorithmResult::getExtent(int axis, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getExtent(axis, plane_id);
}

int SLAMAlgorithmResult::getPolygon3DSize(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPolygon3DSize(plane_id);
}

void SLAMAlgorithmResult::getPolygon3D(float *out_data, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getPolygon3D(out_data, plane_id);
}

int SLAMAlgorithmResult::isPoseInPolygon(standardar::vec3f pose, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.isPoseInPolygon(pose, plane_id);
}

vec3f SLAMAlgorithmResult::getPlaneOriginPoint(int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getPlaneOriginPoint(plane_id);
}

int SLAMAlgorithmResult::isPoseInExtents(vec3f& p, int plane_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.isPoseInExtents(p, plane_id);
}

std::vector<int> SLAMAlgorithmResult::getAllPlaneId() {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return slamAlgorithmProxyWrapper.getAllPlaneId();
}

void SLAMAlgorithmResult::getPredictedViewMatrix(float *view_mat) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getPredictedViewMatrix(view_mat);
}

void SLAMAlgorithmResult::getProjectionMatrixSeeThrough(float near, float far, float *proj_mat) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.getProjectionMatrixSeeThrough(near, far, proj_mat);
}

void SLAMAlgorithmResult::setWindow(void *window, void *context, bool newPbuffer) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.setWindow(window, context, newPbuffer);
}

void SLAMAlgorithmResult::beginRenderEye(int eye_side) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.beginRenderEye(eye_side);
}

void SLAMAlgorithmResult::endRenderEye(int eye_side) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.endRenderEye(eye_side);
}

void SLAMAlgorithmResult::submitRenderFrame(int left_tex_id, int right_tex_id) {
    SLAMAlgorithmProxyWrapper slamAlgorithmProxyWrapper(m_AlgorithmCore, true);
    slamAlgorithmProxyWrapper.submitFrame(left_tex_id, right_tex_id);
}