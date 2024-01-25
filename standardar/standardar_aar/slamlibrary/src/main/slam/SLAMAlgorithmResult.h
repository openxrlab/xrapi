#ifndef STANDARDAR_AAR_SLAMALGORITHMRESULT_H
#define STANDARDAR_AAR_SLAMALGORITHMRESULT_H

#define CLOUD_HOST      0x01
#define CLOUD_RESOLVE   0x02

#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <cstdint>
#include "ArReferenceObjectDatabase.h"
#include "AlgorithmResult.h"
#include "ArCommon.h"
#include "ArCamera.h"
#include "ArConfig.h"
#include "SLAMCommon.h"
namespace standardar {
    class SLAMAlgorithmResult : public AlgorithmResult {
    public:
        SLAMAlgorithmResult();
        virtual void updateResult(jbyte *result_ptr, int pos, int length, int order);
        virtual void update();
        virtual void cleanResult();
        virtual ~SLAMAlgorithmResult();
        void setConfig(const CConfig &config);
        CCameraIntrinsics getCameraIntrinsics();
        bool isDepthImageAvaliable();
        bool getDepthImage(unsigned char* depth_image);
        int getDepthWidth();
        int getDepthHeight();
        std::string getSLAMJsonInfo() { ScopeMutex m(&m_Mutex);return m_SLAMJsonStr;}
        std::string getSLAMInfo() {ScopeMutex m(&m_Mutex);return m_SLAMInfo;}
        std::string getRegionInfo();
        int32_t getInitProcess(){ScopeMutex m(&m_Mutex); return m_initProgress;}
        int32_t getNumberLandMarks();
        ARTrackingState getTrackingState();
        float* getLandMarks();
        double getSLAMMapQuality() {ScopeMutex m(&m_Mutex); return m_mapQualityStatus;}
        int getMapCoupleState() {ScopeMutex m(&m_Mutex); return m_mapCoupleState;}
        bool isSLAMIntializing();
        void setMapIdAndAnchorId(std::string mapId, std::string anchorId);
        void setCloudAnchorState(ARCloudAnchorState result, ARTrackingState state);
        void setCloudAnchorPose(float *pose);
        ARAlgorithmState getSLAMTrackingDetails();
        ARIlluminationEstimateState getLightEstimateState();
        ARIlluminationEstimateMode getLightEstimateMode();
        float getPixelIntensity();
        void getSphericalHarmonicLighting(float* params);
        void getEnvironmentTextureSize(int * width,int * height);
        void getColorCorrection(float* correction);
        void getEnvironmentalHdrMainLightDirection(float* direction);
        void getEnvironmentalHdrMainLightIntensity(float* intensity);
        void getEnvironmentalHdrCubemap(float* cubemap);
        int64_t getLightTimeStamp();
        void setBoundingBox(const float* vertex, const float* matrix);
		void getBoundingBoxPointCloud(float** points, int* points_num);
        void getScanningResult(uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size);

        friend class SLAMAlgorithmResultWrapper;

        void getDenseMeshIndex(unsigned short* index_array);
        int32_t getDenseMeshIndexCount();
        void getDenseMeshVertex(float* vertex_array);
        int getDenseMeshVertexCount();
        int32_t getSLAMPlaneIndex(int32_t planeid);
        void getCameraViewMatrix(float* view, int screen_rotate);
        float getSLAMLightIntensity();
        vec3f insectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t& plane_id, vec3f& quat);
        vec3f getPlaneCenter(int plane_id);
        void getDenseMeshNormal(float* vertex_normal);
        int getDenseMeshFormat();
        vec3f getCenterPose(int plane_id);
        vec3f insectSurfaceMesh(float x, float y, vec3f& normal, float screen_aspect, int screen_rotate);
        float getDistanceFromCamera(vec3f hit_pose);
        int getPlaneVertexCount();
        void getPlaneVertexArray(float* vertex_array);
        int getPlaneIndexCount();
        void getPlaneIndexArray(unsigned short* index_array);
        ARPlaneType getPlaneType(int plane_id);
        void getPlaneNormal(float* out_plane_normal, int plane_id);
        int getPolygonSize(int plane_id);
        void getPolygon(float* out_data, int plane_id);
        float getExtent(int axis, int plane_id);
        int getPolygon3DSize(int plane_id);
        void getPolygon3D(float* out_data, int plane_id);
        int isPoseInPolygon(vec3f pose, int plane_id);
        vec3f getPlaneOriginPoint(int plane_id);
        int isPoseInExtents(vec3f& p, int plane_id);
        std::vector<int> getAllPlaneId();

        void getPredictedViewMatrix(float *view_mat);
        void getProjectionMatrixSeeThrough(float near, float far, float *proj_mat);
        void setWindow(void *window, void *context, bool newPbuffer);
        void beginRenderEye(int eye_side);
        void endRenderEye(int eye_side);
        void submitRenderFrame(int left_tex_id, int right_tex_id);
    private:
        void updateSLAMJsonStr();
        void parseJsonStr(std::string jsonStr);
        void updateCloudAnchor();
    private:

        pthread_mutex_t m_Mutex;

        StandardARLightEstimationResult  m_LightEstimationResult;
        CConfig m_Config;

        std::string m_SLAMJsonStr;
        std::string m_SLAMInfo;
        std::string m_RegionInfo;
        int32_t     m_initProgress;

        uint32_t     m_trackingStateDetails;
        double      m_mapQualityStatus;
        int         m_mapCoupleState;

    public:
        char        m_MapId[128];
        CAnchor*    m_CurrentAnchor;
        ARIlluminationEstimateMode m_LightMode;
        int m_CloudMode;

    };

    class SLAMAlgorithmResultWrapper
    {
    public:
        SLAMAlgorithmResultWrapper(SLAMAlgorithmResult* result) :
                                m_slam_algorithm_result(nullptr)
        {
            if (result)
            {
                pthread_mutex_lock(&(result->m_Mutex));
                m_slam_algorithm_result = result;
            }
        }

        ~SLAMAlgorithmResultWrapper()
        {
            if (m_slam_algorithm_result)
                pthread_mutex_unlock(&(m_slam_algorithm_result->m_Mutex));
        }


        SLAMAlgorithmResult* getResult()
        {
            return m_slam_algorithm_result;
        }

    private:
        SLAMAlgorithmResult* m_slam_algorithm_result;

    };

}



#endif //STANDARDAR_AAR_SLAMALGORITHMRESULT_H
