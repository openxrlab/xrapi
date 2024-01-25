
#ifndef STANDARD_AR_ARCONFIG_C_API_H_
#define STANDARD_AR_ARCONFIG_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "StandardAR.h"
#include "StandardARCommon.h"
#define ARSTREAM_MODE_BACK_RGB_AUTOFOCUS 2001
#define ARSTREAM_MODE_BACK_RGB_DOUBLE_STREAM 2002
#define ARSTREAM_MODE_FRONT_RGB_DOUBLE_STREAM 2003
#define URL_MAX_LENGTH 1024


namespace standardar
{

    class CConfig
    {
    public:
        CConfig();
        CConfig(const CConfig& rightV);
        int chooseStreamMode(bool isSupportDoubleStream);
        virtual ~CConfig();

        std::string     m_PackageName;
        ArAxisUpMode    m_AxisUpMode;
        ARPixelFormat   m_VideoPixelFormat;
        ARCameraID      m_CameraID;
        ARStreamMode    m_CameraTextureTarget;

        // SLAM
        ARAlgorithmMode             m_SLAMMode;
        ARStreamMode                m_SLAM_StreamMode;
        ARTrackingRunMode           m_TrackingRunMode;
        ARWorldAlignmentMode        m_WorldAlignmentMode;
        ARWorldOriginMode           m_WorldOriginMode;
        ARDeviceType                m_SLAMDeviceType;

        // Hand Gesture
        ARAlgorithmMode         m_HandGestureMode;
        ARStreamMode            m_HandGesture_StreamMode;

        // Dense Recon
        ARAlgorithmMode         m_DenseReconMode;
        ARStreamMode            m_DenseRecon_StreamMode;

        // ARIMAGE_TRACKINNG
        ARAlgorithmMode         m_ArMarkerMode;
        ARStreamMode            m_Marker_StreamMode;

        // Plane
        ARAlgorithmMode         m_PlaneAlgorithmMode;
        ARStreamMode            m_Plane_StreamMode;
        ARPlaneDetectingMode    m_PlaneFindingMode;

        // LightEstimate
        ARAlgorithmMode         m_LightEstimateAlgorithmMode;
        ARStreamMode            m_LightEstimate_StreamMode;
        ARIlluminationEstimateMode  m_LightEstimationMode;

        // Cloud Anchor
        ARAlgorithmMode         m_CloudAnchorMode;
        ARStreamMode            m_CloudAnchor_StreamMode;

        // FaceMesh
        ARAlgorithmMode         m_FaceMeshMode;
        ARStreamMode            m_FaceMesh_StreamMode;

        // HD Map
        ARAlgorithmMode         m_MapCoupleAlgorithmMode;
        ARStreamMode            m_MapCouple_StreamMode;
        ARMapCoupleMode         m_MapCouple_Mode;
        char                    m_Url[URL_MAX_LENGTH];
        char                    m_UUID[URL_MAX_LENGTH];

        // ObjectTracking
        ARAlgorithmMode         m_ObjectTrackingMode;
        ARStreamMode            m_ObjectTracking_StreamMode;

        // ObjectScanning
        ARAlgorithmMode         m_ObjectScanningMode;


        // BodyDetect
        ARAlgorithmMode         m_BodyDetectMode;
        ARStreamMode            m_BodyDetect_StreamMode;
    };
}


#endif  // STANDARD_AR_ARCONFIG_C_API_H_
