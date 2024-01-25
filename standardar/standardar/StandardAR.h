

#ifndef STANDARD_AR_C_API_H_
#define STANDARD_AR_C_API_H_

#include <stddef.h>
#include <stdint.h>


typedef struct ARPose_ ARPose;
typedef struct ARConfig_ ARConfig;
typedef struct ARSession_ ARSession;
typedef struct ARMap_ ARWorldMap;

typedef struct ARReferenceImageDatabase_ ARReferenceImageDatabase;
typedef struct ARReferenceObjectDatabase_ ARReferenceObjectDatabase;

typedef struct ARCameraConfig_ ARCameraConfig;
typedef struct ARCameraConfigList_ ARCameraConfigList;

typedef struct ARAnchor_ ARAnchor;
typedef struct ARAnchorList_ ARAnchorList;
typedef struct ARQueryResult_ ARQueryResult;
typedef struct ARQueryResultList_ ARQueryResultList;

typedef struct ARFrame_ ARFrame;
typedef struct ARCamera_ ARCamera;
typedef struct ARCameraIntrinsics_ ARCameraIntrinsics;
typedef struct ARIlluminationEstimate_ ARIlluminationEstimate;
typedef struct ARPointCloud_ ARPointCloud;
typedef struct ARDenseMesh_ ARDenseMesh;

typedef struct ARNode_ ARNode;
typedef struct ARNodeList_ ARNodeList;
typedef struct ARPlaneNode_ ARPlaneNode;
typedef struct ARImageNode_ ARImageNode;
typedef struct ARHandGestureNode_ ARHandGestureNode;
typedef struct ARFaceMeshNode_    ARFaceMeshNode;
typedef struct ARObjectNode_ ARObjectNode;
typedef struct ARBodyDetectNode_   ARBodyDetectNode;


#ifdef __cplusplus
inline ARNode *ARPlaneNodeAsARNode(ARPlaneNode *planeNode) {
    return reinterpret_cast<ARNode *>(planeNode);
}

inline ARPlaneNode *ARNodeAsARPlaneNode(ARNode *node) {
    return reinterpret_cast<ARPlaneNode *>(node);
}

inline ARNode *ARImageNodeAsARNode(ARImageNode *imageNode) {
    return reinterpret_cast<ARNode *>(imageNode);
}

inline ARImageNode *ARNodeAsARImageNode(ARNode *node) {
    return reinterpret_cast<ARImageNode *>(node);
}

inline ARNode *ARHandGestureNodeAsARNode(ARHandGestureNode *handGestureNode) {
    return reinterpret_cast<ARNode *>(handGestureNode);
}

inline ARHandGestureNode *ARNodeAsARHandGestureNode(ARNode *arNode) {
    return reinterpret_cast<ARHandGestureNode *>(arNode);
}



inline ARFaceMeshNode *ARNodeAsARFaceMeshNode(ARNode *node) {
    return reinterpret_cast<ARFaceMeshNode *>(node);
}

inline ARNode *ARObjectNodeAsARNode(ARObjectNode *objectNode) {
    return reinterpret_cast<ARNode *>(objectNode);
}

inline ARObjectNode *ARNodeAsARObjectNode(ARNode *arNode) {
    return reinterpret_cast<ARObjectNode *>(arNode);
}

inline ARBodyDetectNode *ARNodeAsARBodyDetecthNode(ARNode *node) {
    return reinterpret_cast<ARBodyDetectNode *>(node);
}


#endif

enum ARResult {
    ARRESULT_SUCCESS = 0,
    ARRESULT_ERROR_INVALID_ARGUMENT = -1,
    ARRESULT_ERROR_FATAL = -2,
    ARRESULT_ERROR_WORLD_PAUSED = -3,
    ARRESULT_ERROR_WORLD_NOT_PAUSED = -4,
    ARRESULT_ERROR_NOT_TRACKING = -5,
    ARRESULT_ERROR_TEXTURE_NOT_SET = -6,
    ARRESULT_ERROR_UNSUPPORTED_VIDEOFORMAT = -7,
    ARRESULT_ERROR_CAMERA_NOT_AVAILABLE = -8,
    ARRESULT_ERROR_CAMERA_PERMISSION_NOT_GRANTED = -9,
    ARRESULT_ERROR_UNSUPPORTED_CONFIG = -10,
    ARRESULT_ERROR_RESOURCE_EXHAUSTED = -11,
    ARRESULT_ERROR_CLOUD_ANCHORS_NOT_CONFIGURED = -12,
    ARRESULT_ERROR_ANCHOR_NOT_SUPPORTED_FOR_HOSTING = -13,
    ARRESULT_ERROR_DEVICE_UNSUPPORT = -14,
    ARRESULT_ERROR_ALGORITHM_UNSUPPORT = -15,
    ARRESULT_ERROR_UNSUPPORTED_APPLICATION = -16,
};


enum ARAvailability {
    ARAVAILABILITY_SUPPORTED = 1,
    ARAVAILABILITY_UNKNOWN_ERROR = -1,
    ARAVAILABILITY_UNSUPPORTED_DEVICE_NOT_CAPABLE = -100,
    ARAVAILABILITY_SUPPORTED_NOT_INSTALLED = -201,
    ARAVAILABILITY_SUPPORTED_APK_TOO_OLD = -202
};

enum ARInstallState {
    ARINSTALLSTATE_INSTALLED = 0,
    ARINSTALLSTATE_INSTALL_REQUESETED = 1
};

enum ARTrackingState {
    ARTRACKING_STATE_SUCCESS = 0,
    ARTRACKING_STATE_LOST = 1,
    ARTRACKING_STATE_STOPPED = 2,
    ARTRACKING_STATE_INITIALIZING = 10
};

enum ARTrackingRunMode {
    ARTRACKING_RUN_MODE_AUTO = 0,      // start tracking when resume
    ARTRACKING_RUN_MODE_MANUAL = 1,    // start and stop tracking by call arSessionStartUp and arSessionStop
};

enum ARWorldAlignmentMode{
    ARWORLD_ALIGNMENT_MODE_GRAVITY = 0,         // Y axis (0, -gravity, 0)
    ARWORLD_ALIGNMENT_MODE_GRAVITY_HEADING = 1, // Y axis (0, -gravity, 0), Z axis (0, 0, -north)
};

enum ARNodeType {
    ARNODE_TYPE_UNKNOWN = -1,
    ARNODE_TYPE_PLANE = 1,
    ARNODE_TYPE_IMAGE = 2,
    ARNODE_TYPE_HAND_GESTURE = 3,
    ARNODE_TYPE_OBJECT = 4,
    ARNODE_TYPE_ENVPROBE = 5,
    ARNODE_TYPE_FACE_MESH = 6,
    ARNODE_TYPE_BODY_DETECT = 7
};

enum ARParameterEnum{
    ARPARAMETER_VIDEO_VERTICAL_FOV = 50,           //getFloatValue, get vertical fov

    ARPARAMETER_ALGORITHM_SLAM_INFO = 100,         //getStringValue, get SLAM result info
    ARPRARMETER_ALGORITHM_SLAM_MAP_QUALITY = 101,  //getFloatValue, get SLAM Map quality
    ARPARAMETER_ALGORITHM_SLAM_DETAIL_STATE = 102, //getIntValue, get SLAM detail state, compare with ARAlgorithmState
    ARPARAMETER_ALGORITHM_OBJECT_TRACKING = 103,
    ARPARAMETER_ALGORITHM_MAP_COUPLE_STATE = 104,   /* getIntValue
                                                    * @return value:
                                                    *      0 ---- LOCATE_SUCCESS 定位成功
                                                    *      1 ---- LOCATE_FAIL  定位失败
                                                    *      2 ---- INVILAD_INPUT 非法输入
                                                    *      3 ---- ILLEGAL_QUERY_IMG 查询图像失败
                                                    *      4 ---- QUERY_FEATURE_FAIL 查询特征失败
                                                    *      5 ---- IMAGE_MATCH_FAIL 图片匹配失败
                                                    *      6 ---- CLUSTER_FILTER_FAIL 聚类失败
                                                    *      7 ---- REGISTER_FAIL  注册失败
                                                    *      8 ---- GRAVITY_CHECK_FAIL  重力检测失败
                                                    *      9 ---- LOCAL_DESP_MATCH_FAIL ORB点云匹配失败
                                                    *      10---- CONSIST_CHECK_FAIL  一致性检测失败
                                                    *      11 ---- LONG_TIME_WAIT_FAIL  网络延时过大
                                                    *      12 ---- OHTERS_FAIL 其余失败
                                                    * */
    ARPARAMETER_ALGORITHM_MAP_COUPLE_PARAM_JSON = 105,      //setStringValue
    ARPARAMETER_ALGORITHM_MAP_COUPLE_RESULT_JSON = 106,     //getStringValue
    ARPARAMETER_CAMERA_TEXTURE_TARGET = 3000,               //setIntValue, set render target to determin front or back camera. value is ARStreamMode

    /*
     * used in arSessionGetFloatValue(), array size must be 14 or 28
     * when float array size is 14, intrinsics of master camera is retrieved
     * when float array size is 28, intrinsics of both master and slave camera is retrieved
     * order of intrinsics for a camera:
     * width, height, fx, fy, cx, cy, k1, k2, k3, k4, k5, k6, k7, k8
     * */
    ARPARAMETER_CAMERA_INTRINSICS = 4000,
};

enum ARVertexFormat {
    ARVERTEX_FORMAT_UNKNOWN = -1,
    ARVERTEX_FORMAT_3D_POSITION = 0,
    ARVERTEX_FORMAT_3D_POSITION_NORMAL = 1,
    ARVERTEX_FORMAT_3D_POSITION_NORMAL_COLOR = 2
};

enum ARPixelFormat {
    ARPIXEL_FORMAT_UNKNOWN     = -1,

    ARPIXEL_FORMAT_LUMINANCE8   = 0,
    ARPIXEL_FORMAT_RG16         = 1,
    ARPIXEL_FORMAT_RGB24        = 2,
    ARPIXEL_FORMAT_RGBA32       = 3,
    ARPIXEL_FORMAT_DEPTH16      = 4,

    ARPIXEL_FORMAT_YUV_NV21     = 1000,
    ARPIXEL_FORMAT_YUV_NV12     = 1001,
    ARPIXEL_FORMAT_YUV_I420     = 1002,
    ARPIXEL_FORMAT_YUV_YV12     = 1003
};


enum ARStreamMode{
    ARSTREAM_MODE_FRONT_RGB                     = 100,
    ARSTREAM_MODE_BACK_RGB                      = 2000,
    ARSTREAM_MODE_BACK_RGBD                     = 2100,
    ARSTREAM_MODE_BACK_STEREO	                = 2200
};

enum ARDeviceType {
    ARDEVICE_TYPE_PHONE                     = 100,
    ARDEVICE_TYPE_GLASS                     = 200
};

enum ARAlgorithmType{
    ARALGORITHM_TYPE_UNKNOWN              = -1,
    ARALGORITHM_TYPE_SLAM                   = 0x0001,
    ARALGORITHM_TYPE_HAND_GESTURE           = 0x0002,
    ARALGORITHM_TYPE_DENSE_RECON            = 0x0003,
    ARALGORITHM_TYPE_IMAGE_TRACKING         = 0x0004,
    ARALGORITHM_TYPE_PLANE_DETECTION        = 0x0005,
    ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION     = 0x0006,
    ARALGORITHM_TYPE_CLOUD_ANCHOR           = 0x0007,
    ARALGORITHM_TYPE_FACE_MESH              = 0x0008,
    ARALGORITHM_TYPE_OBJECT_TRACKING        = 0x0009,
    ARALGORITHM_TYPE_OBJECT_SCANNING        = 0x000A,
    ARALGORITHM_TYPE_BODY_DETECT            = 0x000B,
    ARALGORITHM_TYPE_MAP_COUPLE             = 0x000C,
    ARALGORITHM_TYPE_ALL                    = 0xFFFF
};

enum ARAlgorithmMode{
    ARALGORITHM_MODE_DISABLE           = 0,
    ARALGORITHM_MODE_ENABLE            = 1,
};

enum ARAlgorithmState {
    ARALGORITHM_STATE_INITIALIZING = 1,
    ARALGORITHM_STATE_FAILED = 2,
    ARALGORITHM_STATE_NOT_STABLE = 3,
    ARALGORITHM_STATE_NORMAL = 4
};


enum ARIlluminationEstimateMode {
    ARILLUMINATION_ESTIMATE_MODE_AMBIENT_INTENSITY = 1,
    ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR = 2
};

enum ARIlluminationEstimateState {
    ARILLUMINATION_ESTIMATE_STATE_NOT_VALID = 0,
    ARILLUMINATION_ESTIMATE_STATE_VALID = 1
};

enum ARMapCoupleMode {
    ARMAP_COUPLE_LOW = 0,
    ARMAP_COUPLE_MEDIUM = 1,
    ARMAP_COUPLE_HIGH = 2
};

enum ARPlaneType{
    ARPLANE_TYPE_UNKNOWN = -1,
    ARPLANE_TYPE_UPWARD_FACING = 0,
    ARPLANE_TYPE_DOWNWARD_FACING = 1,
    ARPLANE_TYPE_VERTICAL = 2
};

enum ARPlaneDetectingMode {
    ARPLANE_DETECTING_MODE_HORIZONTAL = 1,
    ARPLANE_DETECTING_MODE_VERTICAL = 2,
    ARPLANE_DETECTING_MODE_HORIZONTAL_VERTICAL = 3
};

enum ARCloudAnchorState {
    ARCLOUD_ANCHOR_STATE_NONE = 0,
    ARCLOUD_ANCHOR_STATE_TASK_IN_PROGRESS = 1,
    ARCLOUD_ANCHOR_STATE_SUCCESS = 2,
    ARCLOUD_ANCHOR_STATE_ERROR_INTERNAL = -1,
    ARCLOUD_ANCHOR_STATE_ERROR_NOT_AUTHORIZED = -2,
    ARCLOUD_ANCHOR_STATE_ERROR_SERVICE_UNAVAILABLE = -3,
    ARCLOUD_ANCHOR_STATE_ERROR_RESOURCE_EXHAUSTED = -4,
    ARCLOUD_ANCHOR_STATE_ERROR_HOSTING_DATASET_PROCESSING_FAILED = -5,
    ARCLOUD_ANCHOR_STATE_ERROR_CLOUD_ID_NOT_FOUND = -6,
    ARCLOUD_ANCHOR_STATE_ERROR_RESOLVING_LOCALIZATION_NO_MATCH = -7,
    ARCLOUD_ANCHOR_STATE_ERROR_RESOLVING_SDK_VERSION_TOO_OLD = -8,
    ARCLOUD_ANCHOR_STATE_ERROR_RESOLVING_SDK_VERSION_TOO_NEW = -9,
    ARCLOUD_ANCHOR_STATE_ERROR_UNKNOWN = -11
};

enum ARHandGestureType{
    ARHAND_GESTURE_TYPE_UNKNOWN = -1,
    ARHAND_GESTURE_TYPE_OK = 0,
    ARHAND_GESTURE_TYPE_SCISSORS = 1,
    ARHAND_GESTURE_TYPE_THUMBS_UP = 2,
    ARHAND_GESTURE_TYPE_PAPER = 3,
    ARHAND_GESTURE_TYPE_GUN = 4,
    ARHAND_GESTURE_TYPE_ROCK = 5,
    ARHAND_GESTURE_TYPE_FINGER_HEART = 6,
    ARHAND_GESTURE_TYPE_FINGERTIP = 7,
    ARHAND_GESTURE_TYPE_WELL_PLAYED = 8,
    ARHAND_GESTURE_TYPE_THREE_FINGERS = 9,
    ARHAND_GESTURE_TYPE_FOUR_FINGERS = 10,
    ARHAND_GESTURE_TYPE_I_LOVE_YOU = 11,
    ARHAND_GESTURE_TYPE_INDEX_FINGER_AND_LITTLE_FINGER = 12,
    ARHAND_GESTURE_TYPE_LITTLE_FINGER = 13,
};

enum ARHandType{
    ARHAND_SIDE_HAND_UNKNOWN = -1,
    ARHAND_SIDE_RIGHT_HAND = 0 ,
    ARHAND_SIDE_LEFT_HAND  = 1,
};

enum ARHandTowards{
    ARHAND_TOWARDS_UNKNOWN = -1,
    ARHAND_TOWARDS_PALM = 0,
    ARHAND_TOWARDS_THE_BACK_OF_HAND = 1,
    ARHAND_TOWARDS_SIDE_HAND = 2,
};

enum ARQueryMode {
    ARQUERY_MODE_POLYGON_ONLY = 0,             // the plane which is constructed from point cloud
    ARQUERY_MODE_POLYGON_AND_HORIZONPLANE = 1, // ARQUERY_MODE_POLYGON_ONLY and X-Z plane
    ARQUERY_MODE_POLYGON_PERSISTENCE = 2       // the infinity extended plane which is constructed with polygon
};

//// timewarp / prediction enum
enum AREyeSide
{
    AREYE_LEFT = 0,
    AREYE_RIGHT = 1,
};

#ifdef __cplusplus
extern "C" {
#endif

/// ------ Java Environment ------

/// Call this method in JNI_OnLoad.
///@param[in] vm Java VM pointer
///@param[in] reverved reverved information. can be null
int32_t arJavaEnvOnLoad(void *vm, void *reserved);

/// if you CAN NOT rewrite JNI_OnLoad. Call this function and pass classloader class object
///and findclass methodid. For example.In UNREAL. you should call this function when you
///initialized your system
///@param[in] vm Java VM pointer
///@param[in] classloader classloader object
///@param[in] findclassid findClass method id
void arJavaEnvSetClassLoader(void* vm, void* classloader, void* findclassid);

/// ------ ARCapability ------

/// Determines if StandardAR is supported on this device.
///
/// if the device don't support StandarAR, it will return #ARAVAILABILITY_UNSUPPORTED_DEVICE_NOT_CAPABLE.
/// And it will check the availability of the arserver, return one of #AR_AVAILABILITY_SUPPORTED_INSTALLED,
/// #AR_AVAILABILITY_SUPPORTED_APK_TOO_OLD, or
/// #AR_AVAILABILITY_SUPPORTED_NOT_INSTALLED.
///
///@param env JavaENV
///@param application_context a context of android application, can be an activity context
///@param[out] output ARAvailability to out_availability
void arCapabilityCheckAvailability(void *env, void *activity_context, ARAvailability *out_availability);

/// check which algorithm mode is supported.
///
///@param env JavaENV
///@param application_context a context of android application, can be an activity context
///@param type algorithm type
///@param mode algorithm mode of algorithm type
///return ARRESULT_SUCCESS if the algorithm mode is supported
ARResult arCapabilityCheckAlgorithm(void *env, void *activity_context, ARAlgorithmType type, ARStreamMode mode);


/// ------ ARPose api ------

/// the order of values is :qx, qy, qz, qw, tx, ty, tz.

/// create pose
///
///@param pose_raw
///@param out_pose
void arPoseCreate(const float *pose_raw, ARPose **out_pose);

/// destroy pose
///
///@param pose
void arPoseDestroy(ARPose *pose);

/// get raw data from pose
///
///@param pose
///@param out_pose_raw
void arPoseGetPoseRaw(const ARPose *pose, float *out_pose_raw);

/// get 4x4 matrix from pose
///
///@param pose
///@param out_matrix_col_major_4x4
void arPoseGetMatrix(const ARPose *pose, float *out_matrix_col_major_4x4);


/// ------ ARConfig api ------

/// Create a new configuration object.
///@param[inout] store newly created ARConfig
void arConfigCreate(ARConfig **out_config);

/// Destory a configuration object.
///@param[in] ARConfig to be destroyed
void arConfigDestroy(ARConfig *config);

/// Get the current illumination estimate mode of the configuration.
///@param[in] target ARConfig
///@param[inout] output mode to illumination_estimate_mode
void arConfigGetIlluminationEstimateMode(const ARConfig *config, ARIlluminationEstimateMode *illumination_estimate_mode);

/// Set the current illumination estimate mode of the configuration.
///@param[in] target ARConfig
///@param[in] illumination estimate mode
void arConfigSetIlluminationEstimateMode(ARConfig *config, ARIlluminationEstimateMode illumination_estimate_mode);

/// Get the current map couple mode of the configuration.
///@param[in] target ARConfig
///@param[in] illumination estimate mode
void arConfigGetMapCoupleMode(ARConfig *config, ARMapCoupleMode *map_couple_mode);

/// Set the current map couple mode of the configuration.
///@param[in] target ARConfig
///@param[in] illumination estimate mode
void arConfigSetMapCoupleMode(ARConfig *config, ARMapCoupleMode map_couple_mode);

/// Get the current plane detecting mode of the configuration.
///@param[in] target ARConfig
///@param[inout] output mode to plane_finding_mode
void arConfigGetPlaneDetectingMode(const ARConfig *config, ARPlaneDetectingMode *plane_finding_mode);

/// Set the current plane detecting of the configuration.
///@param[in] target ARConfig
///@param[in] plane detecting mode
void arConfigSetPlaneDetectingMode(ARConfig *config, ARPlaneDetectingMode plane_detecting_mode);

/// Get the current tracking run mode of the configuration.
///@param[in] target ARConfig
///@param[inout] output mode to tracking_run_mode
void arConfigGetTrackingRunMode(const ARConfig* config, ARTrackingRunMode * tracking_run_mode);

/// Set the current tracking run mode of the configuration.
///@param[in] target ARConfig
///@param[in] tracking run mode
void arConfigSetTrackingRunMode(ARConfig* config, ARTrackingRunMode tracking_run_mode);

/// Get the current world coordinate system alignment mode of the configuration.
///@param[in] target ARConfig
///@param[inout] output mode to world_alignment_mode
void arConfigGetWorldAlignmentMode(const ARConfig* config, ARWorldAlignmentMode * world_alignment_mode);

/// Set the current world coordinate system alignment mode of the configuration.
///@param[in] target ARConfig
///@param[in] world coordinate system alignment mode
void arConfigSetWorldAlignmentMode(ARConfig* config, ARWorldAlignmentMode world_alignment_mode);

/// Gets the algorithm mode from the ::ArConfig.
///@param[in] target ARConfig
///@param[in] algorithm type
///@param[inout] algorithm mode
void arConfigGetAlgorithmMode(const ARConfig *config, ARAlgorithmType algorithm_type, ARAlgorithmMode *out_algorithm_mode);

/// Sets the algorithm mode that should be used. See ::ArAlgorithmMode
/// for available options.
/// Gets the algorithm mode from the ::ArConfig.
///@param[in] target ARConfig
///@param[in] algorithm type
///@param[in] algorithm mode
void arConfigSetAlgorithmMode(ARConfig *config, ARAlgorithmType algorithm_type, ARAlgorithmMode algorithm_mode);

/// Sets the image database in the session configuration.
/// This function makes a copy of the image database.
///@param[in] target ARSession
///@param[in] target ARConfig
///@param[in] reference image database
void arConfigSetReferenceImageDatabase(const ARSession *session, ARConfig *config, const ARReferenceImageDatabase *image_database);

/// Returns the image database from the session configuration.
/// This function returns a copy of the internally stored image database.
///@param[in] target ARSession
///@param[in] target ARConfig
///@param[inout] reference image database
void arConfigGetReferenceImageDatabase(const ARSession *session, const ARConfig *config, ARReferenceImageDatabase *out_image_database);

/// Sets the object database in the session configuration.
/// This function makes a copy of the object database.
///@param[in] target ARSession
///@param[in] target ARConfig
///@param[in] reference object database
void arConfigSetReferenceObjectDatabase(const ARSession *session, ARConfig *config, const ARReferenceObjectDatabase *object_database);

/// Returns the object database from the session configuration.
/// This function returns a copy of the internally stored object database.
///@param[in] target ARSession
///@param[in] target ARConfig
///@param[inout] reference object database
void arConfigGetReferenceObjectDatabase(const ARSession *session, const ARConfig *config, ARReferenceObjectDatabase *out_object_database);

/// Set the current stream mode of the configuration.
///@param[in] target ARConfig
///@param[in] algorithm type
///@param[in] stream mode
void arConfigSetAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type, ARStreamMode stream_mode);

/// Get the current stream mode of the configuration.
///@param[in] target ARConfig
///@param[inout] output mode to out_stream_mode
void arConfigGetAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type, ARStreamMode *out_stream_mode);

/// Set the server address in map couple mode.
///@param[in] target ARConfig
///@param[in] server url
void arConfigSetMapServerURL(ARConfig *config, const char *url);

/// Set the Beacon UUID in map couole mode.
///@param[in] target ARConfig
///@param[in] beacon uuid
void arConfigSetBeaconUUID(ARConfig *config, const char *uuid);

/// Set device type
///@param[in] target ARConfig
///@param[in] algorithm type
///@param[in] device type
void arConfigSetDeviceType(ARConfig *config, ARDeviceType device_type);

/// Set device type
///@param[in] target ARConfig
///@param[in] algorithm type
///@param[inout] device type
void arConfigGetDeviceType(ARConfig *config, ARDeviceType *out_device_type);

/// ------ ARSession api ------

/// Create a new ARSession object.
///@param[int] java environment
///@param[int] activity context
///@param[inout] store newly created ARSession
ARResult arSessionCreate(void *env, void *activity_context, ARSession** out_session);

/// Destory a ARSession object.
///@param[in] arSession to be destroyed
void arSessionDestroy(ARSession *session);

/// Get the current version of SDK.
///@param[in] target ARSession
///@param[inout] buffer to fill SDK version, which must have already been allocated
///@param[in] version buffer size, at least 32 bytes
void arSessionGetVersion(const ARSession* session, char* version, int32_t versionSize);

/// Set AppKey and AppSecret for ARCloud serivce.
///@param[in] target ARSession
///@param[in] AppKey
///@param[in] AppSecret
void arSessionSetKeyAndSecret(const ARSession* session, const char* app_key, const char* app_secret);

/// Set Camera Texture ID.
///@param[in] target ARSession
///@param[in] texture id
void arSessionSetCameraTextureName(ARSession *session, uint32_t texture_id);

/// Set view properties .
///@param[in] target ARSession
///@param[in] display rotation mode
///@param[in] view width, in pixel
///@param[in] view height, in pixel
void arSessionSetDisplayGeometry(ARSession *session, int32_t rotation, int32_t width, int32_t height);

/// Set ARSession's configuration .
///@param[in] target ARSession
///@param[in] display rotation mode
ARResult arSessionConfigure(ARSession *session, const ARConfig *config);

/// Start tracking and mapping process,
///@param[in] target ARSession
/// @returns #ARRESULT_SUCCESS or any of:
/// - #ARRESULT_ERROR_FATAL
ARResult arSessionStartAlgorithm(ARSession * session, ARAlgorithmType type, int32_t option);

/// Stop tracking and mapping process
///@param[in] target ARSession
/// @returns #ARRESULT_SUCCESS or any of:
/// - #ARRESULT_ERROR_FATAL
ARResult arSessionStopAlgorithm(ARSession * session, ARAlgorithmType type, int32_t option);

/// Resume ARSession
///@param[in] target ARSession
/// @returns #ARRESULT_SUCCESS or any of:
/// - #ARRESULT_ERROR_FATAL
ARResult arSessionResume(ARSession *session);

/// Pause ARSession
///@param[in] target ARSession
/// @returns #ARRESULT_SUCCESS or any of:
/// - #ARRESULT_ERROR_FATAL
ARResult arSessionPause(ARSession *session);

/// Update ARSession
///@param[in] target ARSession
/// @returns #ARRESULT_SUCCESS or any of:
/// - #ARRESULT_ERROR_FATAL
ARResult arSessionUpdate(ARSession *session, ARFrame *out_frame);

/// return the current ARWorldMap of ARSession.
///@param[in] target ARSession
///@param[inout] current ARWorldMap of ARSession
ARResult arSessionAcquireWorldMap(const ARSession *session, ARWorldMap **out_map);

/// return the current tracking state of ARSession.
///@param[in] target ARSession
///@param[inout] current tracking state of ARSession
void arSessionGetTrackingState(const ARSession *session, ARTrackingState *out_tracking_state);

/// host the anchor to the cloud.
///@param[in] target ARSession
///@param[in] target ARAnchor
///@param[inout] cloud anchor object
ARResult arSessionHostAnchor(const ARSession *session, const ARAnchor *anchor, ARAnchor **out_cloud_anchor);

/// resolve the anchor to the cloud.
///@param[in] target ARSession
///@param[in] anchor id of target ARAnchor
///@param[inout] cloud anchor object
ARResult arSessionResolveAnchor(const ARSession *session, const char *cloud_anchor_id, ARAnchor **out_cloud_anchor);

/// set or get int/float/string value of ARParameterEnum
///@param[in] target ARSession
///@param[in] parameter enum type
///@param[in] values or @param[inout] values
///@param[in] value's size
void arSessionSetIntValue(const ARSession* session, ARParameterEnum type, int32_t* value, int size);
void arSessionSetFloatValue(const ARSession* session, ARParameterEnum type, float* value, int size);
void arSessionSetStringValue(const ARSession* session, ARParameterEnum type, char* value, int size);
void arSessionGetIntValue(const ARSession* session, ARParameterEnum type, int32_t* value, int size);
void arSessionGetFloatValue(const ARSession* session, ARParameterEnum type, float* value, int size);
void arSessionGetStringValue(const ARSession* session, ARParameterEnum type, char* value, int* length, int size);

/// Gets the current config. More specifically, fills the given ARConfig object
/// with the copy of the configuration most recently set by
/// arSessionConfigure(). Note: if the ARSession was not explicitly configured, a
/// default configuration is returned (same as arConfigCreate()).
void arSessionGetConfig(ARSession* session, ARConfig *out_config);

/// Enumerates the list of supported camera configs on the device.
/// Can be called at any time.  The supported camera configs will be filled in
/// the provided list after clearing it.
///
/// The list will always return 3 camera configs. The GPU texture resolutions
/// are the same in all three configs. Currently, most devices provide GPU
/// texture resolution of 1920 x 1080 but this may vary with device
/// capabilities. The CPU image resolutions returned are VGA, a middle
/// resolution, and a large resolution matching the GPU texture. The middle
/// resolution will often be 1280 x 720, but may vary with device capabilities.
///
///
/// @param[in] target ARSession
/// @param[inout] The list to fill. This list must have already
/// been allocated with arCameraConfigListCreate().  The list is cleared
/// to remove any existing elements.  Once it is no longer needed, the list
/// must be destroyed using arCameraConfigListDestroy to release allocated
/// memory.
void arSessionGetSupportedCameraConfigs(const ARSession* session, ARCameraConfigList *list);

/// Gets the ARCameraConfig that the ARSession is currently using.  If the
/// camera config was not explicitly set then it returns the default
/// camera config.  Use arCameraConfigDestroy to release memory associated with
/// the returned camera config once it is no longer needed.
///
/// @param[in] target session
/// @param[inout] out_camera_config The camera config object to fill. This
/// object must have already been allocated with arCameraConfigCreate().
/// Use arCameraConfigDestroy to release memory associated with
/// out_camera_config once it is no longer needed.
void arSessionGetCameraConfig(const ARSession* session, ARCameraConfig *out_camera_config);

/// Sets the boundingBox of the object needed to be scanned.
///@param[in] target ARSession
///@param[in] vertexes of boundingBox
///@param[in] matrix of boundingBox
void arSessionSetBoundingBox(const ARSession *session, const float* vertex, const float* matrix);

///Get scanning result of the object inside the boundingBox.
///@param[in] target ARSession
///@param[inout] raw bytes of result
///@param[inout] size of raw bytes
void arSessionGetBoundingBoxScanningResult(const ARSession *session, uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size);

///Get pointcloud inside the boundingBox.
///@param[in] target ARSession
///@param[inout] points
///@param[inout] num of points
void arSessionGetBoundingBoxPointCloud(const ARSession *session, float** points, int* points_num);

///Begin to render one side in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t eye id, 0 left 1 right
ARResult arSessionRenderBeginEye(ARSession* session, AREyeSide eye);

///End to render one side in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t eye id, 0 left 1 right
ARResult arSessionRenderEndEye(ARSession* session, AREyeSide eye);

///Submit rendered frame in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t texture id of left eye
///@param[in] uint32_t texture id of right eye
ARResult arSessionRenderSubmitFrame(ARSession* session, const int32_t leftEyeTextureId = -1, const int32_t rightEyeTextureId = -1);

///Set nativewindow
///@param[in] target ARSession
///@param[in] void* nativewindow window pointer
ARResult arSessionRenderSetWindow(ARSession *session, void *window);

///Begin to render one side in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t eye id, 0 left 1 right
void arSessionRenderBeginEyeUnity(ARSession* session, AREyeSide eye);

///End to render one side in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t eye id, 0 left 1 right
void arSessionRenderEndEyeUnity(ARSession* session, AREyeSide eye);

///Submit rendered frame in the stereo display of AR glasses
///@param[in] target ARSession
///@param[in] uint32_t texture id of left eye
///@param[in] uint32_t texture id of right eye
void arSessionRenderSubmitFrameUnity(ARSession* session, const int32_t leftEyeTextureId, const int32_t rightEyeTextureId);

/// ------ ARWorldMap api ------

/// Defines a tracked location in the physical session.
ARResult arWorldMapAcquireNewAnchor(ARSession *session, const ARWorldMap *ar_map, const ARPose *pose, ARAnchor **out_anchor);

/// Returns all known anchors, including those not currently tracked.
// Anchors  entering the ARTRACKING_STATE_STOPPED state will not be included.
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[inout] out_anchor_list The list to fill.  This list must have already
///     been allocated with ARAnchorListCreate().  If previously used, the list
///     will first be cleared.
void arWorldMapGetAllAnchors(const ARSession *session, const ARWorldMap *ar_map, ARAnchorList *out_anchor_list);

/// Returns the list of all known node "nodes".  This includes
/// ::ARPlane objects if plane detection is enabled, as well as ::ARHandGesture
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[in]    filter_type        The type(s) of nodes to return.  See ARNodeType for legal values.
/// @param[inout] out_node_list The list to fill.  This list must have
///     already been allocated with ARNodeListCreate().  If previously
///     used, the list will first be cleared.
void arWorldMapGetAllNodes(const ARSession *session, const ARWorldMap *ar_map, ARNodeType filter_type, ARNodeList *out_node_list);

/// return the recognised planes total vertex count
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[inout] out_vertex_count the value to fill
void arWorldMapGetAllPlanesVertexCount(const ARSession* session, const ARWorldMap *ar_map, int32_t* out_vertex_count);

/// return the recognised planes total vertex buffer, and the buffer size is (plane vertex count)*3
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[inout] out_vertex_array the vertex buffer to fill, size is equal VertexCount*3
void arWorldMapGetAllPlanesIndexCount(const ARSession* session, const ARWorldMap *ar_map, int32_t* out_index_count);

/// return the recognised planes total index count
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[inout] out_index_count the value to fill
void arWorldMapGetAllPlanesVertexArray(const ARSession* session, const ARWorldMap *ar_map, float* out_vertex_array);

/// return the recognised planes total index buffer, and the buffer size is (plane index count)
///
/// @param[in]    session    target ARSession
/// @param[in]    ar_map    target ARWorldMap
/// @param[inout] out_index_array the index buffer to fill
void arWorldMapGetAllPlanesIndexArray(const ARSession* session, const ARWorldMap *ar_map, unsigned short* out_index_array);

/// Destory a ARWorldMap object.
void arWorldMapRelease(ARWorldMap* ar_map);


/// ------ ARFrame api ------

///Create current ARFrame.
///
///@param[in] session ARSession
///@param[out] out_frame output ARFrame pointer
void arFrameCreate(const ARSession *session, ARFrame **out_frame);

///Destroy given ARFrame.
///
///@param[in] frame the ARFrame to be destroyed
void arFrameDestroy(ARFrame *frame);

///Check if display rotation or viewport changed.
///this application should call arCameraGetProjectionMatrix and arFrameTransformDisplayUvCoords
///if the out_geometry_changed is non-zero
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] out_geometry_changed non-zero for display geometry changed
void arFrameGetDisplayGeometryChanged(const ARSession *session, const ARFrame *frame, int32_t *out_geometry_changed);

///Transform the give texture coordinates to show background correctly
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] num_elements number of uv to be transformed
///@param[in] uvs_in uv coordinates to be transformed
///@param[out] uvs_out the transformed uv coordinates
void arFrameTransformDisplayUvCoords(const ARSession *session, const ARFrame *frame, int32_t num_elements, const float *uvs_in, float *uvs_out);

///Set query mode
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] mode set the ARQueryMode
void arFrameSetPointQueryMode(const ARSession* session, const ARFrame* frame, ARQueryMode mode);

///Query the hit points with the planes
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] pixel_x screen x coordinate
///@param[in] pixel_y screen y coordinate
///@param[out] query_result_list thre query list which contain a list of ARQueryResult.
///You can extract the hit pose from ARQueryResult
void arFramePointQuery(const ARSession *session, const ARFrame *frame, float pixel_x, float pixel_y, ARQueryResultList *query_result_list);

///Query the ray with the planes
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] ray_origin_3 origin(x, y, z) of the ray
///@param[in] ray_direction_3 direction(x, y, z) of the ray
///@param[out] query_result_list thre query list which contain a list of ARQueryResult.
///You can extract the hit pose from ARQueryResult
void arFrameRayQuery(const ARSession *session, const ARFrame *frame, const float *ray_origin_3, const float *ray_direction_3, ARQueryResultList *hit_result_list);

///Get light estimate from current frame
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_light_estimate light estimate will store in this param.
///out_light_estimate must create by arIlluminationEstimateCreate
void arFrameGetIlluminationEstimate(const ARSession *session, const ARFrame *frame, ARIlluminationEstimate *out_light_estimate);

///Acquire 3d point cloud from this frame.
///arPointCloudRelease must be called after call this function
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_point_cloud the output ARPointCloud pointer to an ARPointCloud*
///receive the address of point cloud.
///@return #ARRESULT_SUCCESS for success receive cloud point
///        #ARRESULT_ERROR_INVALID_ARGUMENT for intput param is invalid
ARResult arFrameAcquirePointCloud(const ARSession *session, const ARFrame *frame, ARPointCloud **out_point_cloud);

///Acquire dense mesh from this frame.
///arDenseMeshRelease must be called after call this function
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_dense_mesh the output ARDenseMesh pointer to an ARDenseMesh*
///receive the address of dense mesh.
///@return #ARRESULT_SUCCESS for success receive cloud point
///        #ARRESULT_ERROR_INVALID_ARGUMENT for intput param is invalid
ARResult arFrameAcquireDenseMesh(const ARSession *session, const ARFrame *frame, ARDenseMesh **out_dense_mesh);

///Acquire current frame's camera.
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_camera the output ARCamera pointer to an ARCamera*.
///a match call to arCameraRelease must be made when the camera is not used
void arFrameAcquireCamera(const ARSession *session, const ARFrame *frame, ARCamera **out_camera);

///Get the updated anchor list when ar algorithm is updated.
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] ARAnchorList the output ARAnchorList which the anchors are updated.
void arFrameGetUpdatedAnchors(const ARSession *session, const ARFrame *frame, ARAnchorList *out_anchor_list);

///Get the updated node list when ar algorithm is updated.
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[in] filter_type get the filter_type node
///@param[out] out_node_list the output ARNodeList which node is updated.
void arFrameGetUpdatedNodes(const ARSession *session, const ARFrame *frame, ARNodeType filter_type, ARNodeList *out_node_list);

///See if the ImageData is update
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] data_updated the image data is updated.
void arFrameIsImageDataUpdated(const ARSession* session, const ARFrame* frame, int* data_updated);

///Get the frame pixel format
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] video_format the format of pixel of frame.
void arFrameGetImageFormat(const ARSession* session, const ARFrame* frame, ARPixelFormat* video_image_format);

///Get image resolution
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] outwidth the image's width
///@param[out] outheight the image's height
///@return #ARRESULT_SUCCESS for successfully get image resolution
///        #ARRESULT_ERROR_INVALID_ARGUMENT for input param is invalid
ARResult arFrameGetImageResolution(const ARSession* session, const ARFrame* frame, int32_t* outwidth, int32_t* outheight);

///Get image data with y and uv data
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_gray_data the y channel pixel of image
///@param[out] out_uv_data the uv channel pixel of image
///@return #ARRESULT_SUCCESS for successfully get image resolution
///        #ARRESULT_ERROR_INVALID_ARGUMENT for input param is invalid
///
/// ARPIXEL_FORMAT_YUV_NV21
/// \param out_data1 output the y channel pixel of image
/// \param out_data2 output the uv channel pixel of image
/// \param out_data3 no output
///
ARResult arFrameGetImageData(const ARSession* session, const ARFrame* frame, unsigned char* out_data1, unsigned char* out_data2, unsigned char* out_data3);

/// check if Depth is avaliable
///
///@param[in] session ARSession
///@param[in] frame ARFrame
/// @return #ARRESULT_SUCCESS indicating DepthImage is avaliable, or #ARRESULT_ERROR_FATAL otherwise
ARResult arFrameIsDepthImageAvaliable(const ARSession* session, const ARFrame* frame);

/// Get the Resolution of DepthImage
///
///@param[in] session ARSession
///@param[in] frame ARFrame
/// @param[out] out_width            The width of depth image
/// @param[out] out_height           The height of depth image
/// @return #ARRESULT_SUCCESS indicating get the resolution pf DepthImage is success, or  #ARRESULT_ERROR_FATAL otherwise
ARResult arFrameGetDepthImageResolution(const ARSession* session, const ARFrame* frame, int* out_width, int* out_height);

/// get DepthImage data
/// you need allocate space firstly
///
///@param[in] session ARSession
///@param[in] frame ARFrame
/// @param[out] out_depth_image_data The data of depth image
/// @return #ARRESULT_SUCCESS indicating get DepthImage is success, or  #ARRESULT_ERROR_INVALID_ARGUMENT otherwise
ARResult arFrameGetDepthImageData(const ARSession* session, const ARFrame* frame, unsigned char* out_depth_image_data);

/// get DepthImage Format
///
///@param[in] session ARSession
///@param[in] frame ARFrame
/// @param[out] out_format           The image format of depth image
void arFrameGetDepthImageFormat(const ARSession* session, const ARFrame* frame, ARPixelFormat* out_format);

///Get timestamp of current frame
///
///@param[in] session ARSession
///@param[in] frame ARFrame
///@param[out] out_timestamp_ns timestamp will store in this param.
void arFrameGetTimestamp(const ARSession *session, const ARFrame *frame, int64_t *out_timestamp_ns);


/// ------ ARCamera api ------

///Get the pose of given camera
///
///@param[in] session ARSession
///@param[in] camera ARCamera which to get the camera
///@param[out] out_pose the camera's pose
void arCameraGetPose(const ARSession *session, const ARCamera *camera, ARPose *out_pose);

///Get the view matrix of given camera
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[out] out_col_major_4x4 this float array'size must be 16.
///Call this function the out_col_major_4x4 will be filled with the view matrix
void arCameraGetViewMatrix(const ARSession *session, const ARCamera *camera, float *out_col_major_4x4);

///Get the view matrix of AR glass with prediction
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[out] out_col_major_4x4 this float array'size must be 16.
///Call this function the out_col_major_4x4 will be filled with the view matrix
void arCameraGetPredictedViewMatrix(const ARSession *session, const ARCamera *camera, float *out_col_major_4x4);

///Get the tracking state of camera
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[out] out_tracking_state to trakcing state to indicate current ar is successful or failed.
void arCameraGetTrackingState(const ARSession *session, const ARCamera *camera, ARTrackingState *out_tracking_state);

///Get the projection matrix of camera
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[in] near the near clip plane in meters
///@param[in] far the far clip plane in meters
///@param[out] dest_col_major_4x4 a 16 float array pointer which will be filled with projection elements
void arCameraGetProjectionMatrix(const ARSession *session, const ARCamera *camera, float near, float far, float *dest_col_major_4x4);

///Get the projection matrix of ar glass see through devices
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[in] near the near clip plane in meters
///@param[in] far the far clip plane in meters
///@param[out] dest_col_major_4x4 a 16 float array pointer which will be filled with projection elements
void arCameraGetProjectionMatrixSeeThrough(const ARSession *session, const ARCamera *camera, float near, float far, float *dest_col_major_4x4);

///Get the image intrinsics
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[out] out_camera_intrinsics the intrinsics of image
void arCameraGetImageIntrinsics(const ARSession *session, const ARCamera *camera, ARCameraIntrinsics *out_camera_intrinsics);

///Get the texture intrinsics
///
///@param[in] session ARSession
///@param[in] camera ARCamera
///@param[out] out_camera_intrinsics the intrinsics of texture
void arCameraGetTextureIntrinsics(const ARSession *session, const ARCamera *camera, ARCameraIntrinsics *out_camera_intrinsics);

///Release the camera which is acquire from arFrameAcquireCamera
///
///@param[in] camera the ARCamera to be released
void arCameraRelease(ARCamera *camera);


/// ------ ArCameraIntrinsics api ------

///Allocates a camera intrinstics object.
///
///@param[in] session ARSession
///@param[inout] out_camera_intrinsics The camera_intrinsics data
void arCameraIntrinsicsCreate(const ARSession *session, ARCameraIntrinsics **out_camera_intrinsics);

///Get the fx, fy of camera intrinsics
///
///@param[in] session ARSession
///@param[in] intrinsics ARCameraIntrinsics
///@param[out] out_fx
///@param[out] out_fy
void arCameraIntrinsicsGetFocalLength(const ARSession *session, const ARCameraIntrinsics *intrinsics, float *out_fx, float *out_fy);

///Get the cx, cy of camera intrinsics
///
///@param[in] session ARSession
///@param[in] intrinsics ARCameraIntrinsics
///@param[out] out_cx
///@param[out] out_cy
void arCameraIntrinsicsGetPrincipalPoint(const ARSession *session, const ARCameraIntrinsics *intrinsics, float *out_cx, float *out_cy);

///Get the width, height of camera intrinsics
///
///@param[in] session ARSession
///@param[in] intrinsics ARCameraIntrinsics
///@param[out] out_width
///@param[out] out_height
void arCameraIntrinsicsGetImageDimensions(const ARSession *session, const ARCameraIntrinsics *intrinsics, int32_t *out_width, int32_t *out_height);

///Destroy camera intrinsics
///
///@param[in] intrinsics ARCameraIntrinsics
void arCameraIntrinsicsDestroy(ARCameraIntrinsics *camera_intrinsics);


/// ------ ARCameraConfigList api ------

/// Creates a camera config list object.
/// @param[in] target ARSession
/// @param[out] A pointer to an @c ARCameraConfigList* to receive
void arCameraConfigListCreate(const ARSession *session, ARCameraConfigList **out_list);

/// Releases the memory used by a camera config list object,
/// along with all the camera config references it holds.
/// @param[in] target ARCameraConfigList
void arCameraConfigListDestroy(ARCameraConfigList *list);

/// Retrieves the number of camera configs in this list.
/// @param[in] target ARSession
/// @param[in] target ARCameraConfigList
/// @param[inout] size of camera configs
void arCameraConfigListGetSize(const ARSession *session, const ARCameraConfigList *list, int32_t *out_size);

/// Retrieves the specific camera config based on the position in this list.
/// @param[in] target ARSession
/// @param[in] target ARCameraConfigList
/// @param[in] index
/// @param[inout] target ARCameraConfig
void arCameraConfigListGetItem(const ARSession *session, const ARCameraConfigList *list, int32_t index, ARCameraConfig *out_camera_config);


/// === ARCameraConfig api ===

/// Creates a camera config object.
/// @param[in] target ARSession
/// @param[out] A pointer to an @c ARCameraConfig* to receive the address of the newly allocated ArCameraConfig.
void arCameraConfigCreate(const ARSession *session, ARCameraConfig **out_camera_config);

/// Releases the memory used by a camera config object.
/// @param[in] target ARCameraConfig
void arCameraConfigDestroy(ARCameraConfig *camera_config);

/// Obtains the camera image dimensions for the given camera config.
/// @param[in] target ARSession
/// @param[in] target ARCameraConfig
/// @param[inout] width
/// @param[inout] height
void arCameraConfigGetImageDimensions(const ARSession *session, const ARCameraConfig *camera_config, int32_t *out_width, int32_t *out_height);

/// Obtains the texture dimensions for the given camera config.
/// @param[in] target ARSession
/// @param[in] target ARCameraConfig
/// @param[inout] width
/// @param[inout] height
void arCameraConfigGetTextureDimensions(const ARSession *session, const ARCameraConfig *camera_config, int32_t *out_width, int32_t *out_height);


/// ------ ARPointCloud api ------

///Get the number of points of point cloud
///
///@param[in] session ARSession
///@param[in] point_cloud ARPointCloud
///@param[out] out_number_of_points out_number_of_points is the number of points of point_cloud
void arPointCloudGetNumberOfPoints(const ARSession *session, const ARPointCloud *point_cloud, int32_t *out_number_of_points);

///Get the data of point cloud. each point is represented by four elements(x,y,z,w)
///if arPointCloudRelease is called. the out_point_cloud_data is a wild pointer.So copy
///the data before you call arPointCloudRelease
///
///@param[in] session ARSession
///@param[in] point_cloud ARPointCloud
///@param[out] out_point_cloud_data the pointer to receive point cloud data address.
void arPointCloudGetData(const ARSession *session, const ARPointCloud *point_cloud, const float **out_point_cloud_data);

///Release point cloud which is acquired from arFrameAcquirePointCloud
///
///@param[in] point_cloud ARPointCloud to be release
void arPointCloudRelease(ARPointCloud *point_cloud);

/// ------ ARDenseMesh api ------

///Get the vertex format of dense mesh.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_vertex_format the pointer to receive vertex format address.
void arDenseMeshGetFormat(const ARSession* session, const ARDenseMesh* mesh, ARVertexFormat * out_vertex_format);

///Get the vertex count of dense mesh.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_vertex_count the pointer to receive vertex count address.
void arDenseMeshGetVertexCount(const ARSession* session, const ARDenseMesh* mesh, int32_t* out_vertex_count);

///Get the index count of dense mesh, we can use the index to draw dense mesh.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_index_count the pointer to receive index count address.
void arDenseMeshGetIndexCount(const ARSession* session, const ARDenseMesh* mesh, int32_t* out_index_count);

///Get the vertex array of dense mesh.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_vertex_array the pointer to receive vertex array address.
void arDenseMeshGetVertex(const ARSession* session, const ARDenseMesh* mesh, float* out_vertex_array);

///Get the normal array of dense mesh used for rendering.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_index_array the pointer to receive normal array address.
void arDenseMeshGetNormal(const ARSession* session, const ARDenseMesh* mesh, float* out_vertex_normal);

///Get the index array of dense mesh.
///
///@param[in] session ARSession
///@param[in] mesh ARDenseMesh
///@param[out] out_index_array the pointer to receive index array address.
void arDenseMeshGetIndex(const ARSession* session, const ARDenseMesh* mesh, unsigned short* out_index_array);

///Release dense mesh which is acquired from arFrameAcquireDenseMesh
///
///@param[in] mesh ARDenseMesh to be release
void arDenseMeshRelease(ARDenseMesh* mesh);

/// ------ ARIlluminationEstimate api ------

///create ARIlluminationEstimate to get light estimate
///
///@param[in] session                             ARSession
///@param[out] out_illumination_estimate        the pointer to receive ARIlluminationEstimate address.
void arIlluminationEstimateCreate(const ARSession *session, ARIlluminationEstimate **out_illumination_estimate);

///destroy ARIlluminationEstimate
///
///@param[in] illumination_estimate     the ARIlluminationEstimate create by arIlluminationEstimateCreate
void arIlluminationEstimateDestroy(ARIlluminationEstimate *illumination_estimate);

///get ARIlluminationEstimateState of ARIlluminationEstimate
///
///@param[in]       session                               ARSession
///@param[in]       illumination_estimate               ARIlluminationEstimate
///@param[inout]    out_illumination_estimate_state     get the state of illumination_estimate. if the state
///                                                     is ARILLUMINATION_ESTIMATE_STATE_NOT_VALID. the light
///                                                     estimate value can not be used
void arIlluminationEstimateGetState(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, ARIlluminationEstimateState *out_illumination_estimate_state);

///get PixelIntensity of ARIlluminationEstimate
///
///@param[in]       session                   ARSession
///@param[in]       illumination_estimate   ARIlluminationEstimate
///@param[inout]    out_pixel_intensity     the light estimate value of illumination_estimate
void arIlluminationEstimateGetPixelIntensity(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_pixel_intensity);

///get ColorCorrection of ARIlluminationEstimate
///
///@param[in]       session                       ARSession
///@param[in]       illumination_estimate       ARIlluminationEstimate
///@param[inout]    out_color_correction_4      the light estimate value of illumination_estimate
void arIlluminationEstimateGetColorCorrection(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_color_correction_4);

/// Returns the timestamp of the given ArLightEstimate in nanoseconds. This
/// timestamp uses the same time base as arFrameGetTimestamp().
///
///@param[in]       session                   ARSession
///@param[in]       illumination_estimate   ARIlluminationEstimate
///@param[inout]    out_timestamp_ns        the light estimate time of illumination_estimate
void arIlluminationEstimateGetTimestamp(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, int64_t *out_timestamp_ns);

/// Returns the direction of the main directional light based on the inferred
/// Environmental HDR light estimation. If
/// ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR mode is not set, returns
/// [0.0, 1.0, 0.0], representing a light shining straight down from above.
///@param[in]       session                   ARSession
///@param[in]       illumination_estimate   ARIlluminationEstimate
///@param[inout]    out_direction_3         the direction of the main directional light
void arIlluminationEstimateGetEnvironmentalHdrMainLightDirection(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_direction_3);

/// Returns the intensity of the main directional light based on the inferred
/// Environmental HDR lighting estimation. All return values are larger or equal
/// to zero. If ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR mode is not set,
/// returns zero for all elements of the array.
/// @param[in]      session                 ARSession
/// @param[in]      illumination_estimate ARIlluminationEstimate
/// @param[inout]   out_intensity_3       Output lighting intensity.
void arIlluminationEstimateGetEnvironmentalHdrMainLightIntensity(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_intensity_3);

/// Gets the spherical harmonics coefficients for the ambient illumination based
/// on the inferred Environmental HDR lighting.
/// @param[in]      session               ARSession
/// @param[in]      illumination_estimate ARIlluminationEstimate
/// @param[inout]   out_coefficients_27   The output spherical harmonics
///    coefficients for the ambient illumination. This array contains 9 sets of
///    per-channel coefficients, or a total of 27 values of 32-bit floating
///    point type. The coefficients are stored in a channel-major fashion e.g.
///    [r0, g0, b0, r1, g1, b1, ... , r8, g8, b8]. If
///    ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR mode is not set, returns zero
///    for all 27 coefficients.
void arIlluminationEstimateGetEnvironmentalHdrAmbientSphericalHarmonics(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_coefficients_27);

/// Gets the 6 cubemap textures in OpenGL texture format based on the inferred
/// Environmental HDR lighting.
/// @param[in]      session                 ARSession
/// @param[in]      illumination_estimate   ARIlluminationEstimate
/// @param[inout]   out_textures_6          The fixed size array for 6 cubemap textures.
///                                         Fixed size is width * height * 3 * 6, Width
///                                         and height can be obtained by
///                                         arIlluminationEstimateGetEnvironmentTextureSize.
/// If ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR mode is not set, all textures
/// will be assigned with zero pixel values.
void arIlluminationEstimateAcquireEnvironmentalHdrCubemap(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, float *out_textures_6);

/// Get the width and height of the cubemap
/// @param[in]      session                 ARSession
/// @param[in]      illumination_estimate   ARIlluminationEstimate
/// @param[inout]   width                   The width of cubmap
/// @param[inout]   height                  The height of cubmap
/// The width and height will be set zero if there is an error occurs.
void arIlluminationEstimateGetEnvironmentTextureSize(const ARSession *session, const ARIlluminationEstimate *illumination_estimate, int *width, int *height);

/// ------ ARAnchorList api ------

///create ARAnchorList
///
///@param[in] session ARSession
///@param[out] out_anchor_list the pointer to receive ARAnchorList* address
void arAnchorListCreate(const ARSession *session, ARAnchorList **out_anchor_list);

///destroy ARAnchorList
///
///@param[in] session ARSession
///@param[in] anchor_list which to be destroyed
void arAnchorListDestroy(ARAnchorList *anchor_list);

///get ARAnchorList size
///
///@param[in] session ARSession
///@param[in] anchor_list ARAnchorList
///@param[out] out_size anchor_list's size
void arAnchorListGetSize(const ARSession *session, const ARAnchorList *anchor_list, int32_t *out_size);

///get ARAnchorList item with given index
///
///@param[in] session ARSession
///@param[in] anchor_list ARAnchorList
///@param[in] index
///@param[out] out_anchor the indexed entry in the list
void arAnchorListAcquireItem(const ARSession *session, const ARAnchorList *anchor_list, int32_t index, ARAnchor **out_anchor);


/// ------ ARAnchor api ------

///get pose of anchor in session coordinate.
///
///@param[in] session ARSession
///@param[in] anchor ARAnchor
///@param[out] out_pose the indexed entry in the list
void arAnchorGetPose(const ARSession *session, const ARAnchor *anchor, ARPose *out_pose);

///get anchor tracking state
///
///@param[in] session ARSession
///@param[in] anchor ARAnchor
///@param[out] out_tracking_state the tracking state of anchor. anchor is valid if
///out_tracking_state is ARTRACKING_STATE_SUCCESS
void arAnchorGetTrackingState(const ARSession *session, const ARAnchor *anchor, ARTrackingState *out_tracking_state);

///detach anchore.
///@param[in] anchor ARAnchor
void arAnchorDetach(const ARSession *session, ARAnchor *anchor);

///release anchore.
///@param[in] anchor ARAnchor
void arAnchorRelease(ARAnchor *anchor);

/// ------ ARAnchor api ------
///release pose of anchor in session coordinate.
///
///@param[in] session ARSession
///@param[in] anchor ARAnchor
///@param[inout] out_cloud_anchor_id the id of cloud anchor.
///@param[in] anchor id buffer size, at least 64 bytes.
void arAnchorGetCloudAnchorId(ARSession *session, ARAnchor *anchor, int idSize, char *out_cloud_anchor_id);

/// ------ ARAnchor api ------
///release pose of anchor in session coordinate.
///
///@param[in] session ARSession
///@param[in] anchor ARAnchor
///@param[out] out_state the cloud state of cloud anchor. task finished successfully if
///out_state is not ARSTATE_CLOUD_ANCHOR_STATE_SUCCESS.
void arAnchorGetCloudState(const ARSession *session, const ARAnchor *anchor, ARCloudAnchorState *out_state);


/// ------ ARNodeList api ------

///Creates an ARNodeList object.
///@param[in]   session           ARSession
///@param[out]  out_node_list   A pointer to an ARNodeList* to receive the address of the newly allocated object.
void arNodeListCreate(const ARSession *session, ARNodeList **out_node_list);

///Releases the memory used by an ARNodeList object.
///@param[in]   node_list   The ARNodeList object to be released.
void arNodeListDestroy(ARNodeList *node_list);

///Retrieves the number of an ARNodeList.
///@param[in]       session       ARSession
///@param[in]       node_list   ARNodeList
///@param[inout]    out_size    A pointer to a int object, to be filled with size
void arNodeListGetSize(const ARSession *session, const ARNodeList *node_list, int32_t *out_size);

///Retrieves the element of an ARNodeList.
///@param[in]   session       ARSession
///@param[in]   node_list   ARNodeList
///@param[in]   index       Index of the entry to copy
///@param[out]  out_node    A pointer to an ARNode* to receive the address of the newly allocated object
void arNodeListAcquireItem(const ARSession *session, const ARNodeList *node_list, int32_t index, ARNode **out_node);


/// ------ ARNode api ------

///Retrieves the type of the ARNode.
///@param[in]       session           ARSession
///@param[in]       node            ARNode
///@param[inout]    out_node_type   An already-allocated ARNodeType object into which the node's type will be stored
void arNodeGetType(const ARSession *session, const ARNode *node, ARNodeType *out_node_type);

///Retrieves the tracking state of the ARNode.
///@param[in]       session                   ARSession
///@param[in]       node                    ARNode
///@param[inout]    out_tracking_stacking   An already-allocated ARTrackingState object into which the tracking state will be stored
void arNodeGetTrackingState(const ARSession *session, const ARNode *node, ARTrackingState *out_tracking_state);

///Create new anchor of the ARNode.
///@param[in]       session           ARSession
///@param[in]       node            ARNode
///@param[in]       pose            ARPose
///@param[inout]    new created ARAnchor from ARNode, related pos is given by param pose.
ARResult arNodeAcquireNewAnchor(ARSession *session, ARNode *node, ARPose *pose, ARAnchor **out_anchor);

///Retrieves all anchors of the ARNode.
///@param[in]       session           ARSession
///@param[in]       node            ARNode
///@param[inout]    out_anchor_list An already-allocated ARNodeType object into which the all anchors will be stored
void arNodeGetAnchors(const ARSession *session, const ARNode *node, ARAnchorList *out_anchor_list);

///Releases the memory used by an ARNode object.
///@param[in]   node    The ARNode object to be released.
void arNodeRelease(ARNode *node);


/// ------ ARPlaneNode api ------

/// Retrieves the normal (orientation) of the plane.
///@param[in]       session                   ARSession
///@param[in]       plane                   ARPlaneNode
///@param[inout]    out_plane_normal    A pointer to float object, to be filled with normal of the plane
void arPlaneNodeGetNormal(const ARSession *session, const ARPlaneNode *plane, float *out_plane_normal);

///Retrieves the orientation of plane, including horizontal and vertical.
///@param[in]       session                   ARSession
///@param[in]       plane                   ARPlaneNode
///@param[inout]    out_orientation_type    A pointer to int object, to be filled with size orientation type of the plane
void arPlaneNodeGetOrientationType(const ARSession *session, const ARPlaneNode *plane, ARPlaneType* out_orientation_type);

///Retrieves the center pose of the detected plane
///@param[in]       session       ARSession
///@param[in]       plane       ARPlaneNode
///@param[inout]    out_pose    An already-allocated ARPose which the pose will be stored
void arPlaneNodeGetCenterPose(const ARSession *session, const ARPlaneNode *plane, ARPose *out_pose);

///Retrieves the lengths of the plane's bounding box measured along the X-axis, Y-axis and Z-axis.
///@param[in]       session           ARSession
///@param[in]       plane           ARPlaneNode
///@param[inout]    out_extent_xyz  A pointer to float array of 3 floats, to be filled with length
void arPlaneNodeGetExtent(const ARSession *session, const ARPlaneNode *plane, float *out_extent_xyz);

///Retrieves the number of elements in the boundary polygon.
///The number of vertices if 1/3 of this size.
///@param[in]       session               ARSession
///@param[in]       plane               ARPlaneNode
///@param[inout]    out_polygon_size    A pointer to int object, to be filled with size
void arPlaneNodeGetPolygon3DSize(const ARSession* session, const ARPlaneNode* plane, int32_t* out_polygon_size);

///Retrieves the 3D vertices of a convex polygon approximating the detected plane.
///in the form <tt>[x1, y1, z1, x2, y2, z2, ...]</tt>.
///@param[in]       session           ARSession
///@param[in]       plane           ARPlaneNode
///@param[inout]    out_polygon_xyz A pointer to an array of floats.
void arPlaneNodeGetPolygon3D(const ARSession* session, const ARPlaneNode* plane, float* out_polygon_xyz);

///Sets *out_pose_in_extents to non-zero if the given pose is in the plane's rectangular extents.
///@param[in]       session               ARSession
///@param[in]       plane               ARPlaneNode
///@param[in]       pose                ARPose
///@param[inout]    out_pose_in_extents A pointer to a int object, to be filled with the result(non-zero if the given pose is in the plane's rectangular extents)
void arPlaneNodeIsPoseInExtent(const ARSession *session, const ARPlaneNode *plane, const ARPose *pose, int32_t *out_pose_in_extents);

///Sets @c *out_pose_in_extents to non-zero if the given pose is in the plane's polygon.
///@param[in]       session               ARSession
///@param[in]       plane               ARPlaneNode
///@param[in]       pose                ARPose
///@param[inout]    out_pose_in_polygon A pointer to a int object, to be filled with the result(non-zero if the given pose is in the plane's polygon)
void arPlaneNodeIsPoseInPolygon(const ARSession *session, const ARPlaneNode *plane, const ARPose *pose, int32_t *out_pose_in_polygon);


/// ------ ARImageNode api ------

/// Returns the pose of the center of the detected image. The pose's
/// transformed +Y axis will be point normal out of the image.
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_pose        An already-allocated ARPose which the pose will be stored
void arImageNodeGetCenterPose(const ARSession *session, const ARImageNode *image_node, ARPose *out_pose);

/// Retrieves the estimated width, in metres, of the corresponding physical
/// image, as measured along the local X-axis of the coordinate space with
/// origin and axes as defined by arImageNodeGetCenterPose().
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_extent_x    the length of extent in X axis
void arImageNodeGetExtentX(const ARSession *session, const ARImageNode *image_node, float *out_extent_x);

/// Retrieves the estimated height, in metres, of the corresponding physical
/// image, as measured along the local Z-axis of the coordinate space with
/// origin and axes as defined by arImageNodeGetCenterPose().
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_extent_z    the length of extent in Z axis
void arImageNodeGetExtentZ(const ARSession *session, const ARImageNode *image_node, float *out_extent_z);

/// Returns the zero-based positional index of this image from its originating
/// image database.
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_index       the index in reference image database
void arImageNodeGetIndex(const ARSession *session, const ARImageNode *image_node, int32_t *out_index);

/// Returns the name of this image.
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_image_name  the name of the image node
void arImageNodeGetName(const ARSession *session, const ARImageNode *image_node, char **out_image_name);

/// Returns the global id of this image.
///@param[in]       session           ARSession
///@param[in]       image_node      ARImageNode
///@param[inout]    out_global_id   the global id of the image node
void arImageNodeGetGlobalID(const ARSession *session, const ARImageNode *image_node, char **out_global_id);

/// ------ ARReferenceImageDatabase api ------

/// Creates a new empty image database.
///@param[in]   session                ARSession
///@param[out]  out_image_database   A pointer to an ARReferenceImageDatabase* to receive the address of the newly allocated object.
void arReferenceImageDatabaseCreate(const ARSession *session, ARReferenceImageDatabase **out_image_database);

/// Creates a new image database from a byte array.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
///@param[in]   database_raw_bytes       A pointer to raw bytes of database
///@param[in]   database_raw_bytes_size  size of raw bytes
ARResult arReferenceImageDatabaseDeserialize(const ARSession *session, ARReferenceImageDatabase *image_database,
                                             const uint8_t *database_raw_bytes, int64_t database_raw_bytes_size);

/// Loads image database config file.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
///@param[in]   config                   A pointer to raw bytes of config
void arReferenceImageDatabaseLoadConfigure(const ARSession *session, ARReferenceImageDatabase *image_database,
                                           const char* config);

/// Adds a single patt file to an image database.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
///@param[in]   patt_name                name of patt
///@param[in]   patt_raw_bytes           A pointer to raw bytes of patt
///@param[in]   patt_raw_bytes_size      size of raw bytes
ARResult arReferenceImageDatabaseAddPatt(const ARSession *session, ARReferenceImageDatabase *image_database, const char* patt_name,
                                         const uint8_t *patt_raw_bytes, int64_t patt_raw_bytes_size);

/// Adds a single named image of unknown physical size to an image database,
/// from an array of grayscale pixel values. Returns the zero-based positional
/// index of the image within the image database.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
ARResult arReferenceImageDatabaseAddImage(const ARSession *session, ARReferenceImageDatabase *image_database,
                                          const char *image_name, const uint8_t *image_grayscale_pixels,
                                          int32_t image_width_in_pixels, int32_t image_height_in_pixels,
                                          int32_t image_stride_in_pixels, int32_t *out_index);

/// Adds a single named image to an image database, from an array of grayscale
/// pixel values, along with a positive physical width in meters for this image.
/// Returns the zero-based positional index of the image within the image
/// database.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
ARResult arReferenceImageDatabaseAddImageWithPhysicalSize(const ARSession *session, ARReferenceImageDatabase *image_database,
                                                          const char *image_name, const uint8_t *image_grayscale_pixels,
                                                          int32_t image_width_in_pixels, int32_t image_height_in_pixels,
                                                          int32_t image_stride_in_pixels, float image_width_in_meters, int32_t *out_index);

/// Returns the number of images in the image database.
///@param[in]   session                    ARSession
///@param[in]   image_database           ARReferenceImageDatabase
///@param[in]   out_num_images           A pointer to num of images
void arReferenceImageDatabaseGetNumImages(const ARSession *session, const ARReferenceImageDatabase *image_database, int32_t *out_num_images);

/// Releases memory used by an image database.
///@param[in]   image_database           ARReferenceImageDatabase
void arReferenceImageDatabaseDestroy( ARReferenceImageDatabase *image_database);


/// ------ ARQueryResultList api ------

///Allocates an empty ARQueryResultList object.
///@param[in]   session                   ARSession
///@param[out]  out_query_result_list   A pointer to an ArQueryResultList* to receive the address of the newly allocated object
void arQueryResultListCreate(const ARSession *session, ARQueryResultList **out_query_result_list);

///Releases the memory used by an ARQueryResultList object.
///@param[in]   query_result_list   The ARQueryResultList object to be released.
void arQueryResultListDestroy(ARQueryResultList *query_result_list);

///Retrieves the number of an ARQueryResultList.
///@param[in]       session               ARSession
///@param[in]       query_result_list   ARQueryResultList
///@param[inout]    out_size            A pointer to a int object, to be filled with size
void arQueryResultListGetSize(const ARSession *session, const ARQueryResultList *query_result_list, int32_t *out_size);

///Retrieves the element of an ARQueryResultList.
///@param[in]       session               ARSession
///@param[in]       query_result_list   ARQueryResultList
///@param[in]       index               Index of the entry to copy
///@param[inout]    out_query_result    A pointer to already-allocated ARQueryResult object into which the item will be stored
void arQueryResultListGetItem(const ARSession *session, const ARQueryResultList *query_result_list, int32_t index, ARQueryResult *out_query_result);


/// ------ ARQueryResult api ------

///Allocates an empty ARQueryResult object.
///@param[in]   session               ARSession
///@param[out]  out_query_result    A pointer to an ARQueryResult* to receive the address of the newly allocated object
void arQueryResultCreate(const ARSession *session, ARQueryResult **out_query_result);

///Releases the memory used by an ARQueryResult object.
///@param[in]   query_result    The ARQueryResult object to be released
void arQueryResultDestroy(ARQueryResult *query_result);

///Retrieves the distance from the camera to the position of query result, in meters.
///@param[in]       session           ARSession
///@param[in]       query_result    ARQueryResult
///@param[inout]    out_distance    A pointer to a float object, to be filled with distance
void arQueryResultGetDistance(const ARSession *session, const ARQueryResult *query_result, float *out_distance);

///Retrieves the position of the hit.
///@param[in]       session           ARSession
///@param[in]       query_result    ARQueryResult
///@param[inout]    out_pose        An already-allocated ARPose object into which the pose will be stored
void arQueryResultGetHitPose(const ARSession *session, const ARQueryResult *query_result, ARPose *out_pose);

///Acquire a new ARNode at the position of query result.
///@param[in]   session           ARSession
///@param[in]   query_result    ARQueryResult
///@param[out]  out_node        A pointer to an ARNode* to receive the address of the newly allocated node
void arQueryResultAcquireNode(const ARSession *session, const ARQueryResult *query_result, ARNode **out_node);

///Acquire a new anchor at the position of query result.
///@param[in]   session           ARSession
///@param[in]   query_result    ARQueryResult
///@param[out]  out_anchor      A pointer to an ARAnchor* to receive the address of the newly allocated anchor
///@return  #ARRESULT_SUCCESS if successfully acquire a new anchor
ARResult arQueryResultAcquireNewAnchor(ARSession *session, ARQueryResult *query_result, ARAnchor **out_anchor);


/// ------ ARHandGestureNode api ------

///
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]
void arHandGestureNodeRelease(ARHandGestureNode *arHandGesture);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  ARHandGestureType    手势类型
void arHandGestureNodeGetHandGestureType(const ARSession *session, const ARHandGestureNode *arHandGesture, ARHandGestureType* out_ar_hand_gesture_type);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_handType_confidence hand type confidence
void arHandGestureNodeGetGestureTypeConfidence(const ARSession *session, const ARHandGestureNode *arHandGesture, float* out_handType_confidence);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  ARHandType    hand side 0 for left 1 for right
void arHandGestureNodeGetHandType(const ARSession *session, const ARHandGestureNode *arHandGesture, ARHandType * out_ar_hand_side);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  ARHandTowards    hand towards 0 for palm towards camera, 1 for palm towards out
void arHandGestureNodeGetHandTowards(const ARSession *session, const ARHandGestureNode *arHandGesture, ARHandTowards * out_ar_hand_towards);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_palm_center_xyz palm center, xyz, float array of size 3
void arHandGestureNodeGetPalmCenter(const ARSession *session, const ARHandGestureNode *arHandGesture, float* out_palm_center_xyz);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_palm_normal_xyz palm normal, xyz, float array of size 3
void arHandGestureNodeGetPalmNormal(const ARSession *session, const ARHandGestureNode *arHandGesture, float* out_palm_normal_xyz);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_landmark2D_count    number of 2D landmarks
void arHandGestureNodeGetLandMark2DCount(const ARSession *session, const ARHandGestureNode *arHandGesture, int32_t * out_landmark2D_count);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_landMark2DArray 2D landmark array, float array size of number of 2D landmark * 2(x,y)
void arHandGestureNodeGetLandMark2DArray(const ARSession *session, const ARHandGestureNode *arHandGesture, float* out_landMark2DArray);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_Rect        2D hand bounding box, left top right bottom, int array size of 4
void arHandGestureNodeGetRect(const ARSession *session, const ARHandGestureNode *arHandGesture, int* out_Rect);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_landmark3D_count    number of 2D landmarks
void arHandGestureNodeGetLandMark3DCount(const ARSession *session, const ARHandGestureNode *arHandGesture, int32_t * out_landmark3D_count);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_landMark3DArray 3D landmark array, float array size of number of 3D landmark * 3(x,y,z)
void arHandGestureNodeGetLandMark3DArray(const ARSession *session, const ARHandGestureNode *arHandGesture, float* out_landMark3DArray);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_RGBSegArray hand segment mask byte array, char array size of hand segment mask width * hand segment mask height
void arHandGestureNodeGetRGBSegArray(const ARSession *session, const ARHandGestureNode *arHandGesture, char* out_RGBSegArray);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_RGBSegWidth hand segment mask width
void arHandGestureNodeGetRGBSegWidth(const ARSession *session, const ARHandGestureNode *arHandGesture, int32_t* out_RGBSegWidth);

///
///@param[in]   session           ARSession
///@param[in]   arHandGesture   ARHandGestureNode
///@param[out]  out_RGBSegHeight hand segment height
void arHandGestureNodeGetRGBSegHeight(const ARSession *session, const ARHandGestureNode *arHandGesture, int32_t* out_RGBSegHeight);


///
///@param[in]   session           ARSession
///@param[in]   arFaceMeshNode  ARFaceMeshNode
///@param[out]  out_pose        the head's pose
void arFaceMeshNodeGetCenterPose(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, ARPose *out_pose);

///
///@param[in]   session            ARSession
///@param[in]   arFaceMeshNode   ARFaceMeshNode
///@param[out]  out_vertices     face's vertext float array
///@param[out]  out_number_of_vertices   get the number of vertices
void arFaceMeshNodeGetVertices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_vertices, int32_t *out_number_of_vertices);

///
///@param[in]   session            ARSession
///@param[in]   arFaceMeshNode   ARFaceMeshNode
///@param[out]  out_normals    the normals float array
///@param[out]  out_number_of_normals get the number of normals
void arFaceMeshNodeGetNormals(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_normals, int32_t *out_number_of_normals);


///
///@param[in]   session            ARSession
///@param[in]   arFaceMeshNode   ARFaceMeshNode
///@param[out]  out_texture_coordinates    the texture coordinates float array
///@param[out]  out_number_of_texture_coordinates  the number of texture coordinates
void arFaceMeshNodeGetTextureCoordinates(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_texture_coordinates, int32_t *out_number_of_texture_coordinates);



///
///@param[in]   session            ARSession
///@param[in]   arFaceMeshNode   ARFaceMeshNode
///@param[out]  out_triangle_indices    the triangle indices float array
///@param[out]  out_number_of_triangles get the number of triangle indices
void arFaceMeshNodeGetTriangleIndices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, int **out_triangle_indices, int32_t *out_number_of_triangles);

/// @}

/// ------ ARObjectNode api ------
/// Retrieves the boundingBox of the detected object.
///@param[in]   session           ARSession
///@param[in]   object_node     ARObjectNode
///@param[out]  boundingBox     A pointer to vertexes of boundingbox(x0,y0,z0,x1,y1,z1,...)
void arObjecNodeGetBoundingBox(const ARSession *session, const ARObjectNode *object_node, float* boundingBox);

/// Returns the pose of the center of the detected object. The pose's
/// transformed +Y axis will be point normal out of the object.
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_pose        An already-allocated ARPose which the pose will be stored
void arObjectNodeGetCenterPose(const ARSession *session, const ARObjectNode *object_node, ARPose *out_pose);

/// Retrieves the estimated width, in metres, of the corresponding physical
/// object, as measured along the local X-axis of the coordinate space with
/// origin and axes as defined by arObjectNodeGetCenterPose().
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_extent_x    the length of extent in X axis
void arObjectNodeGetExtentX(const ARSession *session, const ARObjectNode *object_node, float *out_extent_x);

/// Retrieves the estimated width, in metres, of the corresponding physical
/// object, as measured along the local Y-axis of the coordinate space with
/// origin and axes as defined by arObjectNodeGetCenterPose().
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_extent_y    the length of extent in Y axis
void arObjectNodeGetExtentY(const ARSession *session, const ARObjectNode *object_node, float *out_extent_y);

/// Retrieves the estimated height, in metres, of the corresponding physical
/// object, as measured along the local Z-axis of the coordinate space with
/// origin and axes as defined by arObjectNodeGetCenterPose().
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_extent_z    the length of extent in Z axis
void arObjectNodeGetExtentZ(const ARSession *session, const ARObjectNode *object_node, float *out_extent_z);

/// Returns the zero-based positional index of this image from its originating
/// object database.
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_index       the index of object in the reference object database
void arObjectNodeGetIndex(const ARSession *session, const ARObjectNode *object_node, int32_t *out_index);

/// Returns the name of this object.
///@param[in]       session           ARSession
///@param[in]       object_node     ARObjectNode
///@param[inout]    out_object_name the name of object
void arObjectNodeGetName(const ARSession *session, const ARObjectNode *object_node, char **out_object_name);

/// ------ ARReferenceObjectDatabase api ------

/// Creates a new empty object database.
///@param[in]   session                ARSession
///@param[out]  out_object_database  A pointer to an ARReferenceObjectDatabase* to receive the address of the newly allocated object.
void arReferenceObjectDatabaseCreate(const ARSession *session, ARReferenceObjectDatabase **out_object_database);

/// Creates a new image database from a byte array.
///@param[in]   session                    ARSession
///@param[in]   object_database          ARReferenceObjectDatabase
///@param[in]   database_raw_bytes       A pointer to raw bytes of database
///@param[in]   database_raw_bytes_size  size of raw bytes
ARResult arReferenceObjectDatabaseDeserialize(const ARSession *session, ARReferenceObjectDatabase *object_database,
                                              const uint8_t *database_raw_bytes, int64_t database_raw_bytes_size);

/// Adds a single object file to an object database.
///@param[in]   session                    ARSession
///@param[in]   object_database          ARReferenceObjectDatabase
ARResult arReferenceObjectDatabaseAddObject(const ARSession *session, ARReferenceObjectDatabase *object_database,
                                            const char *object_name, const uint8_t *object_raw_bytes, int64_t object_raw_bytes_size, int32_t *out_index);

/// Returns the number of objects in the object database.
///@param[in]   session                    ARSession
///@param[in]   object_database          ARReferenceObjectDatabase
///@param[in]   out_num_objects          A pointer to num of objects
void arReferenceObjectDatabaseGetNumObjects(const ARSession *session, const ARReferenceObjectDatabase *object_database, int32_t *out_num_objects);

/// Releases memory used by an object database.
///@param[in]   object_database          ARReferenceObjectDatabase
void arReferenceObjectDatabaseDestroy( ARReferenceObjectDatabase *object_database);


/// Returns the id of body .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_body_id          the body's ID
void arBodyDetectNodeGetBodyId(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_body_id);

/// Returns the body skeleton point count .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_point_count         the body skeleton point count
void arBodyDetectNodeGetSkeletonPoint2dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point_count);


/// Returns the body skeleton 2d point vertexes  .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_point2D         A pointer to vertexes of  body skeleton 2d point( x, y, x, y,...)
void arBodyDetectNodeGetSkeletonPoint2d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2D);


/// Returns the body skeleton 2d point vertexes  .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_point2D         A pointer to vertexes of  body skeleton 2d point( x, y, x, y,...)
void arBodyDetectNodeGetSkeletonPoint2dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2d_confidence);


/// Returns the body skeleton point count .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_point3d_count         the body skeleton point count
void arBodyDetectNodeGetSkeletonPoint3dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point3d_count);


/// Returns the body skeleton 3d point vertexes  .
///@param[in]   session                    ARSession
///@param[in]   object_bodydetect          ARBodyDetectNode
///@param[out]   out_point3D         A pointer to vertexes of  body skeleton 2d point( x, y,z, x, y,z...)
void arBodyDetectNodeGetSkeletonPoint3d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3D);

void arBodyDetectNodeGetSkeletonPoint3dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3d_confidence);



/// @}
#ifdef __cplusplus
}
#endif


#endif  // STANDARD_AR_C_API_H_
