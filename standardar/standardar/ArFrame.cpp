#include "ArFrame.h"
#include "ArHitResult.h"
#include "ArMath.h"
#include "ArPlane.h"
#include "ArSession.h"
#include "StandardAR.h"

namespace standardar
{
    CFrame::CFrame()
    {


        m_DisplayGeomtryChanged = false;
        m_VideoAspect = 1.66667f;

        m_ScreenWidth = 1280;
        m_ScreenHeight= 720;
        m_ScreenAspect = 1.66667f;

        m_TrackingState = ARTRACKING_STATE_SUCCESS;
        m_EstimateState = ARILLUMINATION_ESTIMATE_STATE_VALID;

        m_pHorizontalPlane = new CPlane(SPECIAL_PLANE_ID_HORIZONTAL_PLANE);

        m_pSession = NULL;

        m_Handle = NULL;

        m_PreHitPlaneID = SPECIAL_PLANE_ID_HORIZONTAL_PLANE;
        m_HitTestMode = ARQUERY_MODE_POLYGON_PERSISTENCE;

        m_light_intensity = 0;
        m_isArServiceVersionNewerThan1_9 = true;
    }

    CFrame::~CFrame()
    {
        StandardAR_SAFE_DELETE(m_pHorizontalPlane);
        m_Handle = NULL;
    }

    void CFrame::setSession(CSession* pseesion){
        m_pSession = pseesion;
        m_isArServiceVersionNewerThan1_9 = isArServiceVersionNewerThan1_9();
    }

    void CFrame::setDisplayGeometryChanged()
    {
        m_DisplayGeomtryChanged = !m_DisplayGeomtryChanged;
    }

    bool CFrame::getDisplayGeometryChanged()
    {
        return m_DisplayGeomtryChanged;
    }

    void CFrame::setScreenRotation(int32_t rotation)
    {
        m_ScreenRotate = rotation;
    }

    void CFrame::transformDisplayUvCoords(int32_t num_elements, const float* uvs_in, float* uvs_out)
    {
        if(uvs_in==NULL || uvs_out==NULL)
            return;
            //LOGI("m_ScreenRotate:%d", m_ScreenRotate);
            switch (m_ScreenRotate) {
                case ROTATION_0:

                    (*uvs_out) = *(uvs_in + 2);
                    (*(uvs_out + 1)) = *(uvs_in + 3);
                    (*(uvs_out + 2)) = *(uvs_in + 6);
                    (*(uvs_out + 3)) = *(uvs_in + 7);
                    (*(uvs_out + 4)) = *(uvs_in);
                    (*(uvs_out + 5)) = *(uvs_in + 1);
                    (*(uvs_out + 6)) = *(uvs_in + 4);
                    (*(uvs_out + 7)) = *(uvs_in + 5);
                    break;
                case ROTATION_180:
                    (*uvs_out) = *(uvs_in);
                    (*(uvs_out + 1)) = *(uvs_in + 5);
                    (*(uvs_out + 2)) = *(uvs_in + 4);
                    (*(uvs_out + 3)) = *(uvs_in + 1);
                    (*(uvs_out + 4)) = *(uvs_in + 2);
                    (*(uvs_out + 5)) = *(uvs_in + 7);
                    (*(uvs_out + 6)) = *(uvs_in + 6);
                    (*(uvs_out + 7)) = *(uvs_in + 3);
                    break;

                case ROTATION_90:
                    (*uvs_out) = *(uvs_in);
                    (*(uvs_out + 1)) = *(uvs_in + 1);
                    (*(uvs_out + 2)) = *(uvs_in + 2);
                    (*(uvs_out + 3)) = *(uvs_in + 3);
                    (*(uvs_out + 4)) = *(uvs_in + 4);
                    (*(uvs_out + 5)) = *(uvs_in + 5);
                    (*(uvs_out + 6)) = *(uvs_in + 6);
                    (*(uvs_out + 7)) = *(uvs_in + 7);
                    break;

                case ROTATION_270:
                    (*uvs_out) = *(uvs_in + 6);
                    (*(uvs_out + 1)) = *(uvs_in + 7);
                    (*(uvs_out + 2)) = *(uvs_in + 4);
                    (*(uvs_out + 3)) = *(uvs_in + 5);
                    (*(uvs_out + 4)) = *(uvs_in + 2);
                    (*(uvs_out + 5)) = *(uvs_in + 3);
                    (*(uvs_out + 6)) = *(uvs_in + 0);
                    (*(uvs_out + 7)) = *(uvs_in + 1);
                    break;

                default:
                    break;
            }
            //LOGI("transformDisplayUvCoords:%i", num_elements);

    }

    void CFrame::setHitTestMode(ARQueryMode mode)
    {
        m_HitTestMode = mode;
    }

    void CFrame::hitTest(float pixel_x, float pixel_y, std::vector<CHitResult*>& hit_result_list)
    {
        if (!getSession()->isAlgorithmStart())
            return;

        float normalizex = (float)pixel_x/(float)m_ScreenWidth;
        float normalizey = (float)pixel_y/(float)m_ScreenHeight;

//        LOGI("CFrame::hitTest x:%f, y:%f, width:%i, height:%i", pixel_x, pixel_y, m_ScreenWidth, m_ScreenHeight);

        //vec3f hitpoint = InsectHorizonPlane(normalizex, normalizey);
        //vec3f hitpoint = InsectMultiPlane(normalizex, normalizey);
        int32_t plane_id = -1;
        vec3f plane_normal;
        if(m_Config.m_AxisUpMode == AR_AXIS_UP_MODE_Y)
            plane_normal = vec3f(0,1,0);
        else
            plane_normal = vec3f(0,0,1);

        vec3f hitpoint;
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE) {
            if (m_Config.m_DenseReconMode == ARALGORITHM_MODE_ENABLE)
                hitpoint = InsectSurfaceMesh(normalizex, normalizey, plane_normal);
            else if(m_Config.m_PlaneAlgorithmMode == ARALGORITHM_MODE_ENABLE)
                hitpoint = InsectPlaneWithSlamResult(normalizex, normalizey, plane_id, plane_normal);
        }

        if (hitpoint.x == M_FLOAT_MAX && hitpoint.y == M_FLOAT_MAX && hitpoint.z == M_FLOAT_MAX)
        {
            LOGI("can not find a hit point");
            return;
        }

        if(!m_isArServiceVersionNewerThan1_9){
            if(m_Config.m_AxisUpMode == AR_AXIS_UP_MODE_Y)
                plane_normal = vec3f(0,1,0);
            else
                plane_normal = vec3f(0,0,1);
        }

        Quatf tempquad= Quatf(1,0,0,0);
        plane_normal.normalize();
        tempquad = fromTo(vec3f(0,1,0), plane_normal);

        //LOGI("hitpoint:%f, %f, %f", hitpoint.x, hitpoint.y, hitpoint.z);

        CHitResult* pstHitResult = new CHitResult();
        pstHitResult->m_HitPoint.tx = hitpoint.x;
        pstHitResult->m_HitPoint.ty = hitpoint.y;
        pstHitResult->m_HitPoint.tz = hitpoint.z;

        pstHitResult->m_HitPoint.qx = tempquad.v.x;
        pstHitResult->m_HitPoint.qy = tempquad.v.y;
        pstHitResult->m_HitPoint.qz = tempquad.v.z;
        pstHitResult->m_HitPoint.qw = tempquad.w;

        ITrackable* pstTrackable = m_pSession->getTrackableById(plane_id);
        pstHitResult->m_pTrackable = pstTrackable ? pstTrackable : m_pHorizontalPlane;
        hit_result_list.push_back(pstHitResult);
    }

    void CFrame::hitTest(const float* ray_origin, const float* ray_direction, std::vector<CHitResult*>& hit_result_list)
    {
        if (!getSession()->isAlgorithmStart())
            return;

        //vec3f hitpoint = InsectHorizonPlane(normalizex, normalizey);
        //vec3f hitpoint = InsectMultiPlane(normalizex, normalizey);
        int32_t plane_id = -1;
        vec3f plane_normal;
        if(m_Config.m_AxisUpMode == AR_AXIS_UP_MODE_Y)
            plane_normal = vec3f(0,1,0);
        else
            plane_normal = vec3f(0,0,1);

        vec3f origin, direction;
        origin.x = ray_origin[0];
        origin.x = ray_origin[1];
        origin.x = ray_origin[2];
        direction.x = ray_origin[0];
        direction.x = ray_origin[1];
        direction.x = ray_origin[2];
        vec3f hitpoint = InsectPlaneWithSlamResult(origin, direction, plane_id, plane_normal);
        if (hitpoint.x == M_FLOAT_MAX && hitpoint.y == M_FLOAT_MAX && hitpoint.z == M_FLOAT_MAX)
        {
            LOGI("can not find a hit point");
            return;
        }
        Quatf tempquad= Quatf(1,0,0,0);
        plane_normal.normalize();
        tempquad = fromTo(vec3f(0,1,0), plane_normal);

        //LOGI("hitpoint:%f, %f, %f", hitpoint.x, hitpoint.y, hitpoint.z);

        CHitResult* pstHitResult = new CHitResult();
        pstHitResult->m_HitPoint.tx = hitpoint.x;
        pstHitResult->m_HitPoint.ty = hitpoint.y;
        pstHitResult->m_HitPoint.tz = hitpoint.z;

        pstHitResult->m_HitPoint.qx = tempquad.v.x;
        pstHitResult->m_HitPoint.qy = tempquad.v.y;
        pstHitResult->m_HitPoint.qz = tempquad.v.z;
        pstHitResult->m_HitPoint.qw = tempquad.w;

        ITrackable* pstTrackable = m_pSession->getTrackableById(plane_id);
        pstHitResult->m_pTrackable = pstTrackable ? pstTrackable : m_pHorizontalPlane;
        hit_result_list.push_back(pstHitResult);
    }

    void CFrame::updateSLAMAndDenseReconResult(SLAMAlgorithmResult* result)
    {
        SLAMAlgorithmResultWrapper wrapper(result);
        result->getCameraViewMatrix(m_ViewMatrix.data, m_ScreenRotate);
        m_light_intensity = result->getSLAMLightIntensity();
    }

    void CFrame::clearSLAMResult()
    {
//        memset(m_CachedBuffer, 0, m_CachedBufSize);
//        memset(&m_FrameResult, 0, sizeof(m_FrameResult));
//        memset(&m_FrameDenseReconResult, 0, sizeof(m_FrameDenseReconResult));
        m_PreHitPlaneID = SPECIAL_PLANE_ID_HORIZONTAL_PLANE;
    }

    void CFrame::setImageIntrinsics(CCameraIntrinsics intrinsics)
    {
        m_ImageIntrinsics = intrinsics;
    }

    ARTrackingState CFrame::getTrackingState()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getTrackingState();

        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE)
            return ARTRACKING_STATE_SUCCESS;

        return ARTRACKING_STATE_STOPPED;
    }

    ARIlluminationEstimateState CFrame::getLightEstimateState()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getLightEstimateState();
        return ARILLUMINATION_ESTIMATE_STATE_NOT_VALID;
    }

    ARIlluminationEstimateMode CFrame::getLightEstimateMode()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getLightEstimateMode();
        return ARILLUMINATION_ESTIMATE_MODE_AMBIENT_INTENSITY;
    }

    float CFrame::getPixelIntensity()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getPixelIntensity();
        return 0;
    }

    void CFrame::getSphericalHarmonicLighting(float* params)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            slamAlgorithmResult->getSphericalHarmonicLighting(params);
        return;
    }

    void CFrame::getEnvironmentTextureSize(int* width, int* height)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getEnvironmentTextureSize(width, height);
        *width = 0;
        *height = 0;
    }

    void CFrame::getColorCorrection(float* correction)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();

        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            slamAlgorithmResult->getColorCorrection(correction);
        return;
    }

    void CFrame::getEnvironmentalHdrMainLightDirection(float* direction)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            slamAlgorithmResult->getEnvironmentalHdrMainLightDirection(direction);
        return;
    }

    void CFrame::getEnvironmentalHdrMainLightIntensity(float* intensity)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getEnvironmentalHdrMainLightIntensity(intensity);
        return;
    }

    void CFrame::getEnvironmentalHdrCubemap(float* cubemap)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();

        if (slamAlgorithmResult && slamAlgorithmResult->getState()) {
            slamAlgorithmResult->getEnvironmentalHdrCubemap(cubemap);
        }
        return ;
    }

    int64_t CFrame::getLightTimeStamp()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getLightTimeStamp();
        return 0;
    }

    int32_t CFrame::getPointCloudPointCount()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getNumberLandMarks();
        return 0;
    }

    const float* CFrame::getPointCloudData()const
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getLandMarks();
        return nullptr;
    }

    float* CFrame::getPointCloudData()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
            return slamAlgorithmResult->getLandMarks();
        return nullptr;
    }

    ARVertexFormat CFrame::getDenseMeshFormat()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE)
        {
            return (ARVertexFormat)slamAlgorithmResult->getDenseMeshFormat();
        }


        return ARVERTEX_FORMAT_UNKNOWN;
    }

    int32_t CFrame::getDenseMeshVertexCount()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE)
        {
            return slamAlgorithmResult->getDenseMeshVertexCount();
        }
        return 0;
    }

    bool CFrame::getDenseMeshVertex(float* vertex_array)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE)
        {
            slamAlgorithmResult->getDenseMeshVertex(vertex_array);
            return true;
        }
        return false;
    }

    bool CFrame::getDenseMeshNormal(float* vertex_normal)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE) {
            slamAlgorithmResult->getDenseMeshNormal(vertex_normal);
            return true;
        }
        return false;
    }

    int32_t CFrame::getDenseMeshIndexCount()
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE) {
            return slamAlgorithmResult->getDenseMeshIndexCount();
        }
        return 0;
    }

    bool CFrame::getDenseMeshIndex(unsigned short* index_array)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState() && m_Config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE) {
            slamAlgorithmResult->getDenseMeshIndex(index_array);
        }
        return false;
    }

    int32_t CFrame::getSLAMPlaneIndex(int32_t planeid)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
        {
            return slamAlgorithmResult->getSLAMPlaneIndex(planeid);
        }
        return -1;
    }


    void CFrame::getCenterPose(int32_t planeid, CPose& out_pose)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult && slamAlgorithmResult->getState())
        {
            vec3f center = slamAlgorithmResult->getCenterPose(planeid);
            out_pose.tx = center.x;
            out_pose.ty = center.y;
            out_pose.tz = center.z;
        }
    }

    vec3f CFrame::InsectMultiPlane(float x, float y)
    {
        LOGI("InsectMultiPlane empty implementation");
        return vec3f(0.0f, 0.0f, 0.0f);
//        vec3f minInsectpt;
//        minInsectpt.x = M_FLOAT_MAX;
//        minInsectpt.y = M_FLOAT_MAX;
//        minInsectpt.z = M_FLOAT_MAX;
//
//        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
//        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
//
//        if (!slamAlgorithmResult && !slamAlgorithmResult->getState())
//            return minInsectpt;
//
//        SLAMAlgorithmResultWrapper slamAlgorithmResultWrapper(slamAlgorithmResult);
//        StandardARSLAMTrackingResult* slam_result = slamAlgorithmResultWrapper.getSLAMResult();
//        if (slam_result == nullptr)
//            return minInsectpt;
//
//        GLfloat zNear = 0.1f;
//        float foyrad = m_TextureIntrinsics.m_FocalLengthY * 3.1415926f / 180.0f;
//        GLfloat half_tan_fov = std::tan(foyrad/2.0f);
//        GLfloat halffrustumHeight = zNear * half_tan_fov;
//        GLfloat halffrustumWidth = halffrustumHeight * m_ScreenAspect;
//
//        float ratiox = 2.0f*x - 1.0f;
//        float ratioy = 2.0f*y - 1.0f;
//
//        vec4f tmppt;
//        tmppt.x = ratiox*halffrustumWidth;
//        tmppt.y = -ratioy*halffrustumHeight;
//        tmppt.z = -zNear;
//        tmppt.w = 1.0f;
//
//        //LOGI("camera fovy:%f, aspect:%f\n", m_FovY, m_Aspect);
//        //LOGI("frustum:%f, %f, %f\n", half_tan_fov, halffrustumWidth, halffrustumHeight);
//        //LOGI("near plane pt:%f, %f, %f\n", tmppt.x, tmppt.y, tmppt.z);
//
//        mat4f invViewMat = m_ViewMatrix.inverse();
//        vec4f tmppt2 = invViewMat*tmppt;
//
//        vec3f viewpt;
//        viewpt.x = invViewMat.data[12];
//        viewpt.y = invViewMat.data[13];
//        viewpt.z = invViewMat.data[14];
//
//        vec3f dirvec;
//        dirvec.x = tmppt2.x-viewpt.x;
//        dirvec.y = tmppt2.y-viewpt.y;
//        dirvec.z = tmppt2.z-viewpt.z;
//        dirvec.normalize();
//
//        float minDistance = 0x7fffffff;
//
//
//        int triangle_count = slam_result->dense_mesh_f_size/3;
//        float* vertexs = slam_result->dense_mesh_v;
//        int* triangles= slam_result->dense_mesh_f;
//
//        vec3f insectpt, normal;
//        for(int i=0;i<triangle_count;++i){
//            int index0, index1, index2;
//            index0 = triangles[i * 3];
//            index1 = triangles[i * 3 + 1];
//            index2 = triangles[i * 3 + 2];
//
//            vec3f vert0, vert1, vert2;
//            vert0.x = vertexs[index0 * 9];
//            vert0.y = vertexs[index0 * 9 + 1];
//            vert0.z = vertexs[index0 * 9 + 2];
//
//            vert1.x = vertexs[index1 * 9];
//            vert1.y = vertexs[index1 * 9 + 1];
//            vert1.z = vertexs[index1 * 9 + 2];
//
//            vert2.x = vertexs[index2 * 9];
//            vert2.y = vertexs[index2 * 9 + 1];
//            vert2.z = vertexs[index2 * 9 + 2];
//
//            bool binsect = false;
//            insectpt = CalTriangleIntersectPoint(vert0, vert1, vert2, dirvec, viewpt, binsect);
//            if(binsect){
//                float dis = pow((viewpt.x - insectpt.x), 2) + pow((viewpt.y - insectpt.y), 2) + pow((viewpt.z - insectpt.z), 2);
//                if(dis < minDistance){
//                    minInsectpt = insectpt;
//                    minDistance = dis;
//                }
//            }
//        }
//
//        if(minInsectpt.x == M_FLOAT_MAX && minInsectpt.y == M_FLOAT_MAX && minInsectpt == M_FLOAT_MAX)
//        {
//            vec3f centerpt;
//            minInsectpt = InsectHorizonPlane(x, y, centerpt);
//        }
//
//        return minInsectpt;
    }

    vec3f CFrame::getPlaneCenter(int32_t plane_index)
    {
        vec3f centerpt;
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();

        if (!slamAlgorithmResult && !slamAlgorithmResult->getState())
            return centerpt;
        return slamAlgorithmResult->getPlaneCenter(plane_index);
    }

    vec3f CFrame::InsectSurfaceMesh(float x, float y, vec3f& normal)
    {
        vec3f minInsectpt;
        minInsectpt.x = M_FLOAT_MAX;
        minInsectpt.y = M_FLOAT_MAX;
        minInsectpt.z = M_FLOAT_MAX;
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();

        if (!slamAlgorithmResult && !slamAlgorithmResult->getState())
            return minInsectpt;

        return slamAlgorithmResult->insectSurfaceMesh(x, y, normal, m_ScreenAspect, m_ScreenRotate);
    }

    vec3f CFrame::InsectPlaneWithSlamResult(float x, float y, int32_t& plane_id, vec3f& plane_normal)
    {
        GLfloat zNear = 0.1f;
        float foyrad = m_TextureIntrinsics.m_FocalLengthY * 3.1415926f / 180.0f;
        GLfloat half_tan_fov = std::tan(foyrad/2.0f);
        GLfloat halffrustumHeight = zNear * half_tan_fov;
        GLfloat halffrustumWidth = halffrustumHeight * m_ScreenAspect;

        float ratiox = 2.0f*x - 1.0f;
        float ratioy = 2.0f*y - 1.0f;

        vec4f tmppt;
        tmppt.x = ratiox*halffrustumWidth;
        tmppt.y = -ratioy*halffrustumHeight;
        tmppt.z = -zNear;
        tmppt.w = 1.0f;

        //LOGI("camera fovy:%f, aspect:%f\n", m_FovY, m_Aspect);
        //LOGI("frustum:%f, %f, %f\n", half_tan_fov, halffrustumWidth, halffrustumHeight);
        //LOGI("near plane pt:%f, %f, %f\n", tmppt.x, tmppt.y, tmppt.z);

        mat4f invViewMat = m_ViewMatrix.inverse();
        vec4f tmppt2 = invViewMat*tmppt;

        vec3f viewpt;
        viewpt.x = invViewMat.data[12];
        viewpt.y = invViewMat.data[13];
        viewpt.z = invViewMat.data[14];

        vec3f dirvec;
        dirvec.x = tmppt2.x-viewpt.x;
        dirvec.y = tmppt2.y-viewpt.y;
        dirvec.z = tmppt2.z-viewpt.z;
        dirvec.normalize();

        vec3f minInsectpt = InsectPlaneWithSlamResult(viewpt, dirvec, plane_id, plane_normal);

        switch (m_HitTestMode)
        {
            case ARQUERY_MODE_POLYGON_ONLY:
            {
                return minInsectpt;
            }
            case ARQUERY_MODE_POLYGON_AND_HORIZONPLANE:
            {
                if(minInsectpt.x == M_FLOAT_MAX && minInsectpt.y == M_FLOAT_MAX && minInsectpt == M_FLOAT_MAX)
                {
                    vec3f centerpt = getPlaneCenter(0);
                    minInsectpt = InsectHorizonPlane(x, y, centerpt);
                    plane_id = m_pHorizontalPlane->m_PlaneID;
                }
                break;
            }
            case ARQUERY_MODE_POLYGON_PERSISTENCE:
            {
                if(minInsectpt.x == M_FLOAT_MAX && minInsectpt.y == M_FLOAT_MAX && minInsectpt == M_FLOAT_MAX)
                {
                    if (m_PreHitPlaneID == SPECIAL_PLANE_ID_HORIZONTAL_PLANE) {
                        vec3f centerpt = getPlaneCenter(0);
                        minInsectpt = InsectHorizonPlane(x, y, centerpt);
                        plane_id = m_pHorizontalPlane->m_PlaneID;
                    } else {
                        int32_t tmpindex = getSLAMPlaneIndex(m_PreHitPlaneID);
                        if (tmpindex != -1) {
                            CPose tmpPose;
                            getCenterPose(m_PreHitPlaneID, tmpPose);

                            vec3f tmpCenter;
                            tmpCenter.x = tmpPose.tx;
                            tmpCenter.y = tmpPose.ty;
                            tmpCenter.z = tmpPose.tz;

                            minInsectpt = InsectHorizonPlane(x, y, tmpCenter);
                            plane_id = m_PreHitPlaneID;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }

//        LOGI("plane id:%i, %f, %f, %f",plane_id, minInsectpt.x, minInsectpt.y, minInsectpt.z);
        return minInsectpt;
    }

    vec3f CFrame::InsectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t& plane_id, vec3f& quat)
    {
        AlgorithmResultWrapper algorithmResultWrapper = m_pSession->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        vec3f minInsectpt;
        minInsectpt.x = M_FLOAT_MAX;
        minInsectpt.y = M_FLOAT_MAX;
        minInsectpt.z = M_FLOAT_MAX;

        if (!slamAlgorithmResult && !slamAlgorithmResult->getState())
            return minInsectpt;
        int temp_plane_id = -1;
        vec3f insect = slamAlgorithmResult->insectPlaneWithSlamResult(ray_origin, ray_direction, temp_plane_id, quat);
        if (temp_plane_id != -1) {
            m_PreHitPlaneID = temp_plane_id;
        }
        plane_id = temp_plane_id;
        return insect;
    }

    vec3f CFrame::InsectHorizonPlane(float x, float y, const vec3f& planept)
    {
        GLfloat zNear = 0.1f;
        float foyrad = m_TextureIntrinsics.m_FocalLengthY * 3.1415926f / 180.0f;
        GLfloat half_tan_fov = std::tan(foyrad/2.0f);
        GLfloat halffrustumHeight = zNear * half_tan_fov;
        GLfloat halffrustumWidth = halffrustumHeight * m_ScreenAspect;

        float ratiox = 2.0f*x - 1.0f;
        float ratioy = 2.0f*y - 1.0f;

        vec4f tmppt;
        tmppt.x = ratiox*halffrustumWidth;
        tmppt.y = -ratioy*halffrustumHeight;
        tmppt.z = -zNear;
        tmppt.w = 1.0f;

        //LOGI("camera fovy:%f, aspect:%f\n", m_FovY, m_Aspect);
        //LOGI("frustum:%f, %f, %f\n", half_tan_fov, halffrustumWidth, halffrustumHeight);
        //LOGI("near plane pt:%f, %f, %f\n", tmppt.x, tmppt.y, tmppt.z);

        mat4f invViewMat = m_ViewMatrix.inverse();
        vec4f tmppt2 = invViewMat*tmppt;

        vec3f viewpt;
        viewpt.x = invViewMat.data[12];
        viewpt.y = invViewMat.data[13];
        viewpt.z = invViewMat.data[14];

        vec3f dirvec;
        dirvec.x = tmppt2.x-viewpt.x;
        dirvec.y = tmppt2.y-viewpt.y;
        dirvec.z = tmppt2.z-viewpt.z;
        dirvec.normalize();

        vec3f planeVector;

        if(m_Config.m_AxisUpMode == AR_AXIS_UP_MODE_Y)
        {
            planeVector.x=0.0f;
            planeVector.y=1.0f;
            planeVector.z=0.0f;
        } else
        {

            planeVector.x=0.0f;
            planeVector.y=0.0f;
            planeVector.z=1.0f;
        }

        //LOGI("point on screen:%f, %f, %f\n", tmppt2.x, tmppt2.y, tmppt2.z);
        //LOGI("view point:%f, %f, %f\n", viewpt.x, viewpt.y, viewpt.z);
        //LOGI("view dir vec:%f, %f, %f\n", dirvec.x, dirvec.y, dirvec.z);
        //LOGI("plane vector:%f, %f, %f\n", planeVector.x, planeVector.y, planeVector.z);

        bool binsect = false;
        vec3f insectpt = CalPlaneLineIntersectPoint(planeVector, planept, dirvec, viewpt, binsect);
        return insectpt;
    }


/// <summary>
/// 求一条直线与平面的交点
/// </summary>
/// <param name="planeNormal">平面的法线向量，长度为3</param>
/// <param name="planePoint">平面经过的一点坐标，长度为3</param>
/// <param name="lineVector">直线的方向向量，长度为3</param>
/// <param name="linePoint">直线经过的一点坐标，长度为3</param>
/// <returns>返回交点坐标，长度为3</returns>
    vec3f CFrame::CalPlaneLineIntersectPoint(vec3f planeNormal, vec3f planePoint, vec3f lineVector, vec3f linePoint, bool& infinit)
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

    vec3f CFrame::CalTriangleIntersectPoint(vec3f vert0, vec3f vert1, vec3f vert2, vec3f lineVector, vec3f linePoint, bool& infinit)
    {
        float det, inv_det, u, v, t;
        vec3f edge1, edge2, pvec, tvec, qvec, returnResult;
        edge1.x = vert1.x - vert0.x;
        edge1.y = vert1.y - vert0.y;
        edge1.z = vert1.z - vert0.z;
        edge2.x = vert2.x - vert0.x;
        edge2.y = vert2.y - vert0.y;
        edge2.z = vert2.z - vert0.z;


        pvec.x = lineVector.y * edge2.z - lineVector.z * edge2.y;
        pvec.y = lineVector.z * edge2.x - lineVector.x * edge2.z;
        pvec.z = lineVector.x * edge2.y - lineVector.y * edge2.x;

        det = pvec.x * edge1.x + pvec.y * edge1.y + pvec.z * edge1.z;

        if(det < 0.000001 && det > -0.000001){
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        inv_det = 1.0 / det;

        tvec.x = linePoint.x - vert0.x;
        tvec.y = linePoint.y - vert0.y;
        tvec.z = linePoint.z - vert0.z;

        u = tvec.x * pvec.x + tvec.y * pvec.y + tvec.z * pvec.z;
        u *= inv_det;
        if(u < 0.0 || u > 1.0){
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        qvec.x = tvec.y * edge1.z - tvec.z * edge1.y;
        qvec.y = tvec.z * edge1.x - tvec.x * edge1.z;
        qvec.z = tvec.x * edge1.y - tvec.y * edge1.x;

        v = lineVector.x * qvec.x + lineVector.y * qvec.y + lineVector.z * qvec.z;
        v *= inv_det;
        if(v < 0.0 || u + v > 1.0){
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        t = edge2.x * qvec.x + edge2.y * qvec.y + edge2.z * qvec.z;
        t *= inv_det;

        returnResult.x = linePoint.x + lineVector.x * t;
        returnResult.y = linePoint.y + lineVector.y * t;
        returnResult.z = linePoint.z + lineVector.z * t;

        infinit = true;
        return returnResult;
    }

    vec3f CFrame::CalTriangleNormal(vec3f vert0, vec3f vert1, vec3f vert2)
    {
        vec3f edge1, edge2;
        edge1.x = vert1.x - vert0.x;
        edge1.y = vert1.y - vert0.y;
        edge1.z = vert1.z - vert0.z;
        edge2.x = vert2.x - vert0.x;
        edge2.y = vert2.y - vert0.y;
        edge2.z = vert2.z - vert0.z;

        return edge2.crossProduct(edge1);
    }

    Quatf CFrame::fromTo(const vec3f &v1, const vec3f &v2)
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
}
