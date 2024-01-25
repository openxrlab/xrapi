
#ifndef STANDARD_AR_COMMON_C_API_H_
#define STANDARD_AR_COMMON_C_API_H_

#include <stddef.h>
#include <stdint.h>

#define R17PRO  "PBDM00"
#define RGBSLAM "RGBSLAM"
#define RGBDSLAM "RGBDSLAM"

namespace standardar{

    class CPose;
    class CConfig;
    class CCameraConfig;
    class CCameraConfigList;
    class CCameraIntrinsics;

    class CSession;
    class CFrame;
    class CAnchor;
    class ITrackable;
    class CHitResult;
    class CAnchorList;
    class CTrackableList;
    class CHitResultList;

    class CImageNode;
    class CImageNodeMgr;
    class CObjectNode;
    class CReferenceObjectDatabase;
    class CHandGesture;
	class ArFaceMesh;
	class CBodyDetectNode;
}


/// ------ arcore api enum------
enum ArPreviewMode{
    AR_PREVIEW_MODE_FULLSCREEN_AUTO = 0,
    AR_PREVIEW_MODE_1920_1080 = 1,
    AR_PREVIEW_MODE_1280_720 = 2,
    AR_PREVIEW_MODE_1920_1440 = 3,
    AR_PREVIEW_MODE_1440_1080 = 4,
    AR_PREVIEW_MODE_640_480 = 5,
    AR_PREVIEW_MODE_BEST_FIT  = 6,
    AR_PREVIEW_MODE_USER_DEFINED =20,
};


enum ArCameraId {
    AR_CAMERA_ID_BACK_RGB_CAMERA0  = 0x0001,
    AR_CAMERA_ID_BACK_RGB_CAMERA1  = 0x0002,
    AR_CAMERA_ID_BACK_RGB_CAMERA2  = 0x0004,
    AR_CAMERA_ID_BACK_IR_CAMERA    = 0x0008,
    AR_CAMERA_ID_BACK_TOF_CAMCERA  = 0x0010
};

enum ArAxisUpMode{
    AR_AXIS_UP_MODE_Y = 0,
    AR_AXIS_UP_MODE_Z = 1,
};

enum ArEngineCameraAPIMode {
    AR_ENGINE_CAMERA_API_MODE_AUTO_SELECT     = 0x00,
    AR_ENGINE_CAMERA_API_MODE_JAVA_CAMERA2    = 0x01,
    AR_ENGINE_CAMERA_API_MODE_NDK_CAMERA2     = 0x02,
    AR_ENGINE_CAMERA_API_MODE_HAL_CAMERA      = 0x04,
    AR_ENGINE_CAMERA_API_MODE_SINGLE_STREAM   = 0x08,
    AR_ENGINE_CAMERA_API_MODE_MULTIPLE_STREAM = 0x10,
};

/// ------ standardar api enum------
enum ARWorldOriginMode
{
    ARWORLD_ORIGIN_MODE_PLANE = 0,   // world origin locate on feature plane
    ARWORLD_ORIGIN_MODE_EYE = 1,     // world origin locate on eye position
};


enum ARVideoResolutionMode {
    ARVIDEO_RESOLUTION_FULLSCREEN_AUTO = 0,
    ARVIDEO_RESOLUTION_1920_1080 = 1,
    ARVIDEO_RESOLUTION_1280_720 = 2,
    ARVIDEO_RESOLUTION_1920_1440 = 3,
    ARVIDEO_RESOLUTION_1440_1080 = 4,
    ARVIDEO_RESOLUTION_640_480 = 5,
    ARVIDEO_RESOLUTION_1280_960 = 6,
    ARVIDEO_RESOLUTION_BEST_FIT  = 7,
    ARVIDEO_RESOLUTION_1280_400 = 8,
    ARVIDEO_RESOLUTION_USER_DEFINED =20,
};

enum ARCameraID {
    ARCAMERA_ID_BACK_RGB_CAMERA0  = 0x0001,
    ARCAMERA_ID_BACK_RGB_CAMERA1  = 0x0002,
    ARCAMERA_ID_BACK_RGB_CAMERA2  = 0x0004,
    ARCAMERA_ID_BACK_IR_CAMERA    = 0x0008,
    ARCAMERA_ID_BACK_TOF_CAMCERA  = 0x0010
};

enum AREnvironmentProbeMode{
    ARENVIRONMENT_PROBE_MODE_DISABLE = 0,
    ARENVIRONMENT_PROBE_MODE_AUTO = 1,
    ARENVIRONMENT_PROBE_MODE_MANUAL = 2
};

enum ARExtensionCommand {
    AREXTENSION_COMMAND_SHOW_FEATURE = 0,
    AREXTENSION_COMMAND_SHOW_LANDMARK = 1,
    AREXTENSION_COMMAND_SHOW_AXIS = 2,
    AREXTENSION_COMMAND_SHOW_PLANE = 3,
};

struct ARPose_
{
    float   qx, qy, qz, qw, tx, ty, tz;
};

struct ARConfig_
{
    standardar::CConfig*   m_pImpl;
};

struct ARReferenceImageDatabase_
{
    standardar::CImageNodeMgr* m_pImpl;
};

struct ARReferenceObjectDatabase_
{
	standardar::CReferenceObjectDatabase* m_pImpl;
};

struct ARCameraConfig_
{
    standardar::CCameraConfig* m_pImpl;
};

struct ARCameraConfigList_
{
    standardar::CCameraConfigList* m_pImpl;
};

struct ARCameraIntrinsics_
{
    standardar::CCameraIntrinsics* m_pImpl;
};
struct ARSession_
{
    standardar::CSession*  m_pImpl;
};

struct ARMap_
{
    standardar::CSession*  m_pImpl;
};

struct ARFrame_
{
    standardar::CFrame*    m_pImpl;
};

struct ARCamera_
{
    standardar::CFrame* m_pImpl;
};

struct ARIlluminationEstimate_
{
    standardar::CFrame* m_pImpl;
};

struct ARPointCloud_
{
    standardar::CFrame* m_pImpl;
};

struct ARDenseMesh_
{
    standardar::CFrame* m_pImpl;
};

struct ARAnchor_
{
    standardar::CAnchor* m_pImpl;
};

struct ARNode_
{
    standardar::ITrackable* m_pImpl;
};


struct ARImageNode_
{
    standardar::CImageNode* m_pImpl;
};

struct ARPlaneNode_
{
    standardar::ITrackable* m_pImpl;
};

struct ARHandGestureNode_
{
    standardar::CHandGesture* m_pImpl;
};

struct ARFaceMeshNode_
{
	standardar::ArFaceMesh* m_pImpl;
};

struct ARBodyDetectNode_
{
	standardar::CBodyDetectNode* m_pImpl;
};

struct ARObjectNode_
{
	standardar::CObjectNode* m_pImpl;
};

struct ARQueryResult_
{
    standardar::CHitResult* m_pImpl;
};

struct ARAnchorList_
{
    standardar::CAnchorList* m_pImpl;
};

struct ARNodeList_
{
    standardar::CTrackableList* m_pImpl;
};



struct ARQueryResultList_
{
    standardar::CHitResultList* m_pImpl;
};

typedef struct ARImageFeature_ ARImageFeature;
typedef struct ARObjectFeature_ ARObjectFeature;

typedef struct ARImageNode_ ARImageNode;
typedef struct ARObjectNode_ ARObjectNode;
typedef struct AREnvironmentProbeNode_ AREnvironmentProbeNode;

typedef struct ARSurfelCloud_ ARSurfelCloud;
typedef struct ARSurfaceMesh_ ARSurfaceMesh;

//
//#ifdef __cplusplus
//inline ARNode *ARPlaneNodeAsARNode(ARPlaneNode *planeNode) {
//    return reinterpret_cast<ARNode *>(planeNode);
//}
//
//inline ARNode *ImageNodeAsARNode(ARImageNode *imageNode) {
//    return reinterpret_cast<ARNode *>(imageNode);
//}
//
//inline ARNode *ARObjectAsARNode(ARObjectNode *objectNode) {
//    return reinterpret_cast<ARNode *>(objectNode);
//}
//
//inline ARNode *AREnvironmentProbeNodeAsARNode(AREnvironmentProbeNode *probeNode) {
//    return reinterpret_cast<ARNode *>(probeNode);
//}
//
//inline ARPlaneNode *ARNodeAsARPlaneNode(ARNode *node) {
//    return reinterpret_cast<ARPlaneNode *>(node);
//}
//
//inline ARImageNode *ARNodeAsARImageNode(ARNode *node) {
//    return reinterpret_cast<ARImageNode *>(node);
//}
//
//inline ARObjectNode *ARNodeAsARObjectNode(ARNode *node) {
//    return reinterpret_cast<ARObjectNode *>(node);
//}
//
//inline AREnvironmentProbeNode *ARNodeAsAREnvironmentProbeNode(ARNode *node) {
//    return reinterpret_cast<AREnvironmentProbeNode *>(node);
//}
//#endif

extern "C" {

void ArPoseTostPose(const ARPose_ &arpose, standardar::CPose &stpose) ;
void stPoseToArPose(const standardar::CPose &stpose, ARPose_ &arpose) ;


bool IsARSessionAvalid(const ARSession_ *world);

bool IsARMapAvalid(const ARMap_ *map);

bool IsARConfigAvalid(const ARConfig_ *config);

bool IsARReferenceImageDatabaseAvalid(const ARReferenceImageDatabase_ *image_node_mgr);

bool IsARReferenceObjectDatabaseAvalid(const ARReferenceObjectDatabase_ *object_database);

bool IsARCameraConfigAvalid(const ARCameraConfig_ *camera_confg) ;

bool IsARCameraConfigListAvalid(const ARCameraConfigList_ *camera_config_list) ;

bool IsARCameraIntrinsicsAvalid(const ARCameraIntrinsics_ *camera_intrinsics) ;
bool IsARCameraAvalid(const ARCamera_ *camera);

bool IsARCameraIntrinsicsAvalid(const ARCameraIntrinsics_ *cameraIntrinsics);

bool IsARFrameAvalid(const ARFrame_ *frame);

bool IsARPointCloudAvalid(const ARPointCloud_ *ptcloud);

bool IsARDenseMeshAvalid(const ARDenseMesh_ *ptDenseMesh);

bool IsARIlluminationEstimateAvalid(const ARIlluminationEstimate_* illuminationestimate);

bool IsARNodeAvalid(const ARNode_ *node);

bool IsARNodeListAvalid(const ARNodeList_ * nodelist);

bool IsARImageNodeAvalid(const ARImageNode_ *image_node);

bool IsARObjectNodeAvalid(const ARObjectNode_ *object_node);

bool IsARPlaneNodeAvalid(const ARPlaneNode_ *plane);

bool IsARQueryResultAvalid(const ARQueryResult_* queryResult);

bool IsARQueryResultListAvalid(const ARQueryResultList_* queryResultList);

bool IsARAnchorAvalid(const ARAnchor_ *anchor) ;

bool IsARAnchorlistAvalid(const ARAnchorList_ *anchorlist) ;


bool IsARHandGestureAvalid(const ARHandGestureNode_* handGesture);

bool IsARFaceMeshAvalid(const ARFaceMeshNode_* arFaceMeshNode);

bool IsARBodyDetectAvalid(const ARBodyDetectNode_* arBodyDetectNode);


}

#endif  // STANDARD_AR_COMMON_C_API_H_
