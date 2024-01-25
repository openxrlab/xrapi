#include "ArConfig.h"
#include "log.h"

namespace standardar
{

    CConfig::CConfig()
    {
        m_PackageName = "unknow";
        m_AxisUpMode = AR_AXIS_UP_MODE_Y;
        m_CameraID = ARCAMERA_ID_BACK_RGB_CAMERA0;
        m_VideoPixelFormat = ARPIXEL_FORMAT_YUV_NV21;

        m_TrackingRunMode = ARTRACKING_RUN_MODE_AUTO;

        m_WorldAlignmentMode = ARWORLD_ALIGNMENT_MODE_GRAVITY_HEADING;
        m_WorldOriginMode = ARWORLD_ORIGIN_MODE_PLANE;
        m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGB;

        m_SLAMMode = ARALGORITHM_MODE_DISABLE;
        m_SLAM_StreamMode = ARSTREAM_MODE_BACK_RGB;

        m_PlaneAlgorithmMode = ARALGORITHM_MODE_DISABLE;
        m_Plane_StreamMode = ARSTREAM_MODE_BACK_RGB;
        m_PlaneFindingMode = ARPLANE_DETECTING_MODE_HORIZONTAL_VERTICAL;

        m_LightEstimateAlgorithmMode = ARALGORITHM_MODE_DISABLE;
        m_LightEstimate_StreamMode = ARSTREAM_MODE_BACK_RGB;
        m_LightEstimationMode = ARILLUMINATION_ESTIMATE_MODE_AMBIENT_INTENSITY;

        m_HandGestureMode = ARALGORITHM_MODE_DISABLE;
        m_HandGesture_StreamMode = ARSTREAM_MODE_BACK_RGB;

        m_DenseReconMode = ARALGORITHM_MODE_DISABLE;
        m_DenseRecon_StreamMode = ARSTREAM_MODE_BACK_RGBD;

        m_CloudAnchorMode = ARALGORITHM_MODE_DISABLE;
        m_CloudAnchor_StreamMode = ARSTREAM_MODE_BACK_RGB;

        m_ArMarkerMode = ARALGORITHM_MODE_DISABLE;
        m_Marker_StreamMode = ARSTREAM_MODE_BACK_RGB;


        m_FaceMeshMode = ARALGORITHM_MODE_DISABLE;
        m_FaceMesh_StreamMode = ARSTREAM_MODE_FRONT_RGB;

        m_MapCoupleAlgorithmMode = ARALGORITHM_MODE_DISABLE;
        m_MapCouple_StreamMode = ARSTREAM_MODE_FRONT_RGB;
        m_MapCouple_Mode = ARMAP_COUPLE_HIGH;
        memset(m_Url, 0, URL_MAX_LENGTH);
        memset(m_UUID, 0, URL_MAX_LENGTH);

        m_ObjectTrackingMode = ARALGORITHM_MODE_DISABLE;
        m_ObjectTracking_StreamMode = ARSTREAM_MODE_BACK_RGB;

        m_ObjectScanningMode = ARALGORITHM_MODE_DISABLE;

        m_SLAMDeviceType = ARDEVICE_TYPE_PHONE;

        m_BodyDetectMode = ARALGORITHM_MODE_DISABLE;
        m_BodyDetect_StreamMode = ARSTREAM_MODE_BACK_RGB;
    }

    CConfig::CConfig(const CConfig& rightV)
    {
        m_PackageName = rightV.m_PackageName;
        m_AxisUpMode = rightV.m_AxisUpMode;
        m_CameraID = rightV.m_CameraID;
        m_VideoPixelFormat = rightV.m_VideoPixelFormat;

        m_TrackingRunMode = rightV.m_TrackingRunMode;
        m_WorldAlignmentMode = rightV.m_WorldAlignmentMode;
        m_WorldOriginMode = rightV.m_WorldOriginMode;
        m_CameraTextureTarget = rightV.m_CameraTextureTarget;

        m_SLAMMode = rightV.m_SLAMMode;
        m_SLAM_StreamMode = rightV.m_SLAM_StreamMode;

        m_PlaneAlgorithmMode = rightV.m_PlaneAlgorithmMode;
        m_Plane_StreamMode = rightV.m_Plane_StreamMode;
        m_PlaneFindingMode = rightV.m_PlaneFindingMode;

        m_LightEstimateAlgorithmMode = rightV.m_LightEstimateAlgorithmMode;
        m_LightEstimationMode = rightV.m_LightEstimationMode;

        m_HandGestureMode = rightV.m_HandGestureMode;
        m_HandGesture_StreamMode = rightV.m_HandGesture_StreamMode;

        m_DenseReconMode = rightV.m_DenseReconMode;
        m_DenseRecon_StreamMode = rightV.m_DenseRecon_StreamMode;

        m_ArMarkerMode = rightV.m_ArMarkerMode;
        m_Marker_StreamMode = rightV.m_Marker_StreamMode;

        m_CloudAnchorMode = rightV.m_CloudAnchorMode;
        m_CloudAnchor_StreamMode = rightV.m_CloudAnchor_StreamMode;


        m_FaceMeshMode = rightV.m_FaceMeshMode;
        m_FaceMesh_StreamMode = rightV.m_FaceMesh_StreamMode;

        m_MapCoupleAlgorithmMode = rightV.m_MapCoupleAlgorithmMode;
        m_MapCouple_StreamMode = rightV.m_MapCouple_StreamMode;
        m_MapCouple_Mode = rightV.m_MapCouple_Mode;

        memcpy(m_Url, rightV.m_Url, URL_MAX_LENGTH);
        memcpy(m_UUID, rightV.m_UUID, URL_MAX_LENGTH);

        m_ObjectTrackingMode = rightV.m_ObjectTrackingMode;
        m_ObjectTracking_StreamMode = rightV.m_ObjectTracking_StreamMode;

        m_ObjectScanningMode = rightV.m_ObjectScanningMode;

        m_SLAMDeviceType = ARDEVICE_TYPE_PHONE;

        m_BodyDetectMode = rightV.m_BodyDetectMode;
        m_BodyDetect_StreamMode = rightV.m_BodyDetect_StreamMode;
    }

    int CConfig::chooseStreamMode(bool isSupportDoubleStream)
    {
        if (m_SLAM_StreamMode == ARSTREAM_MODE_BACK_STEREO) {
            m_CameraTextureTarget = m_SLAM_StreamMode;
            return m_CameraTextureTarget;
        }

#ifdef SUPPORT_BLANK
        if (m_SLAM_StreamMode == ARSTREAM_MODE_BLANK) {
            m_CameraTextureTarget = m_SLAM_StreamMode;
            return m_CameraTextureTarget;
        }
#endif

        if (m_SLAMMode == ARALGORITHM_MODE_ENABLE && m_SLAM_StreamMode == ARSTREAM_MODE_BACK_RGB && m_ArMarkerMode == ARALGORITHM_MODE_DISABLE && isSupportDoubleStream) {
            m_SLAM_StreamMode = (ARStreamMode) ARSTREAM_MODE_BACK_RGB_DOUBLE_STREAM;
            m_CameraTextureTarget = (ARStreamMode) ARSTREAM_MODE_BACK_RGB_DOUBLE_STREAM;
        }

        if(m_SLAMMode == ARALGORITHM_MODE_ENABLE && m_SLAM_StreamMode == ARSTREAM_MODE_BACK_RGBD)
            m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGBD;

        if(m_DenseReconMode == ARALGORITHM_MODE_ENABLE && m_DenseRecon_StreamMode == ARSTREAM_MODE_BACK_RGBD) {
            m_SLAM_StreamMode = ARSTREAM_MODE_BACK_RGBD;
            m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGBD;
        }

        if(m_FaceMeshMode == ARALGORITHM_MODE_ENABLE && m_FaceMesh_StreamMode == ARSTREAM_MODE_FRONT_RGB){
            m_CameraTextureTarget = ARSTREAM_MODE_FRONT_RGB;
        }

        if(m_FaceMeshMode == ARALGORITHM_MODE_ENABLE && m_FaceMesh_StreamMode == ARSTREAM_MODE_BACK_RGB){
            m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGB;
        }

        if(m_HandGestureMode == ARALGORITHM_MODE_ENABLE && m_HandGesture_StreamMode == ARSTREAM_MODE_BACK_RGB){
            m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGB;
        }

        if(m_BodyDetectMode == ARALGORITHM_MODE_ENABLE && m_BodyDetect_StreamMode == ARSTREAM_MODE_BACK_RGB){
            m_CameraTextureTarget =  ARSTREAM_MODE_BACK_RGB;
        }

//        if(m_ObjectTrackingMode == ARALGORITHM_MODE_ENABLE && m_ObjectTracking_StreamMode == ARSTREAM_MODE_BACK_RGB)
//            m_CameraTextureTarget = ARSTREAM_MODE_BACK_RGB;

        if(m_SLAMMode == ARALGORITHM_MODE_DISABLE && m_ArMarkerMode == ARALGORITHM_MODE_ENABLE)
            m_CameraTextureTarget = (ARStreamMode)ARSTREAM_MODE_BACK_RGB_AUTOFOCUS;

        return m_CameraTextureTarget;
    }

    CConfig::~CConfig()
    {

    }
}
