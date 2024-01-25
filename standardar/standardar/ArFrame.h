
#ifndef STANDARD_AR_ARFRAME_C_API_H_
#define STANDARD_AR_ARFRAME_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include "SLAMAlgorithmResult.h"
#include "ArMath.h"
#include "ArCommon.h"
#include "ArConfig.h"
#include "StandardARCommon.h"
#include "StandardAR.h"
#include "ArCamera.h"
#include "SLAMCommon.h"

namespace standardar
{
    class CSession;
    class CHitResult;
    class CPlane;
    class CFrame
    {
    public:
        CFrame();
        virtual ~CFrame();

        void setSession(CSession* pseesion);
        CSession* getSession(){return m_pSession;}
        const CSession* getSession()const{return m_pSession;}

        void setConfig(const CConfig& stconfig){m_Config = stconfig;}
        CConfig& getConfig(){return m_Config;}
        const CConfig& getConfig()const{return m_Config;}

        bool getDisplayGeometryChanged();
        void setDisplayGeometryChanged();
        void setScreenRotation(int32_t rotation);
        void transformDisplayUvCoords(int32_t num_elements, const float* uvs_in, float* uvs_out);

        void setHitTestMode(ARQueryMode mode);
        void hitTest(float pixel_x, float pixel_y, std::vector<CHitResult*>& hit_result_list);
        void hitTest(const float* ray_origin, const float* ray_direction, std::vector<CHitResult*>& hit_result_list);

        vec3f getPlaneCenter(int32_t plane_index);
        vec3f InsectHorizonPlane(float x, float y, const vec3f& planept);
        vec3f InsectSurfaceMesh(float x, float y, vec3f& normal);
        vec3f InsectMultiPlane(float x, float y);
        vec3f InsectPlaneWithSlamResult(float x, float y, int32_t& plane_id, vec3f& quat);
        vec3f InsectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t& plane_id, vec3f& quat);
        ARTrackingState getTrackingState();

        ARIlluminationEstimateState getLightEstimateState();
        ARIlluminationEstimateMode getLightEstimateMode();
        float getPixelIntensity();
        void getSphericalHarmonicLighting(float* params);
        void getEnvironmentTextureSize(int * width,int * height );
        void getColorCorrection(float* correction);
        void getEnvironmentalHdrMainLightDirection(float* directuion);
        void getEnvironmentalHdrMainLightIntensity(float* intensity);
        void getEnvironmentalHdrCubemap(float* cubbemap);
        int64_t getLightTimeStamp();

        int32_t getPointCloudPointCount();
        const float* getPointCloudData()const ;
        float* getPointCloudData();

        ARVertexFormat getDenseMeshFormat();
        int32_t getDenseMeshVertexCount();
        bool getDenseMeshVertex(float* vertex_array);
        bool getDenseMeshNormal(float* vertex_normal);
        int32_t getDenseMeshIndexCount();
        bool getDenseMeshIndex(unsigned short* index_array);

        void setHorizontalFov(float fovx){m_TextureIntrinsics.m_FocalLengthX = fovx;}
        void setVerticalFov(float fovy){m_TextureIntrinsics.m_FocalLengthY = fovy;}

        void setPreviewAspect(float aspect){m_VideoAspect = aspect;}
        void setPreviewSize(int width, int height){m_TextureIntrinsics.m_ImageWidth = width; m_TextureIntrinsics.m_PrincipalPTX = width / 2;
            m_TextureIntrinsics.m_ImageHeight = height; m_TextureIntrinsics.m_PrincipalPTY = height / 2;}

        CCameraIntrinsics& getImageIntrinsics(){return m_ImageIntrinsics;}
        CCameraIntrinsics& getTextureIntrinsics(){return m_TextureIntrinsics;}

        void setScreenAspect(float aspect){m_ScreenAspect = aspect;}
        void setScreenSize(int screenwidth, int screenheight){m_ScreenWidth = screenwidth; m_ScreenHeight = screenheight;}

        void updateSLAMAndDenseReconResult(SLAMAlgorithmResult* result);

        void clearSLAMResult();

        void setTimeStamp(int64_t timeStamp){m_TimeStamp = timeStamp;}
        void setImageIntrinsics(CCameraIntrinsics intrinsics);
        int64_t getTimeStamp(){return m_TimeStamp;}

        enum Rotation:int32_t {
            ROTATION_0 = 0,
            ROTATION_90 = 1,
            ROTATION_180 = 2,
            ROTATION_270 = 3
        };

        ARFrame_* m_Handle;

    protected:

        int32_t getSLAMPlaneIndex(int32_t planeid);
        void getCenterPose(int32_t planeid, CPose& out_pose);

        vec3f CalPlaneLineIntersectPoint(vec3f planeVector,
                                                  vec3f planePoint,
                                                  vec3f lineVector,
                                                  vec3f linePoint,
                                                  bool& infinit);

        vec3f CalTriangleIntersectPoint(vec3f vert0,
                                                 vec3f vert1,
                                                 vec3f vert2,
                                                 vec3f lineVector,
                                                 vec3f linePoint,
                                                 bool& infinit);
        vec3f CalTriangleNormal(vec3f vert0, vec3f vert1, vec3f vert2);
        Quatf fromTo(const vec3f &v1, const vec3f &v2);

    protected:
        CSession*  m_pSession;
        CConfig    m_Config;

        float   m_VideoAspect;

        int32_t m_ScreenWidth;
        int32_t m_ScreenHeight;
        float   m_ScreenAspect;

        int32_t m_ScreenRotate;
        bool m_DisplayGeomtryChanged;

        mat4f  m_ViewMatrix;
        mat4f  m_ProjMatrix;

        ARQueryMode m_HitTestMode;
        int         m_PreHitPlaneID;

        CPlane*     m_pHorizontalPlane;

        ARTrackingState m_TrackingState;
        ARIlluminationEstimateState m_EstimateState;

        int64_t m_TimeStamp;

        CCameraIntrinsics m_ImageIntrinsics;
        CCameraIntrinsics m_TextureIntrinsics;
        float m_light_intensity;
        bool  m_isArServiceVersionNewerThan1_9;
    };
}

#endif  // ARFRAME_C_API_H_
