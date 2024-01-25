#ifndef HELLO_AR_C_SRC_SLAMCOMMON_H
#define HELLO_AR_C_SRC_SLAMCOMMON_H


#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <assert.h>
#include <jni.h>
#include "log.h"
#include "ArCommon.h"
#include "StandardAR.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum StandardARSLAMState
{
    StandardAR_SLAM_INITIALIZING,        /* SLAM is in initialize state */
    StandardAR_SLAM_TRACKING_SUCCESS,    /* SLAM is in tracking state and track success */
    StandardAR_SLAM_TRACKING_FAIL,        /* SLAM is in tracking state and track fail */
    StandardAR_SLAM_TRACKING_RESET,       /* SLAM system need to reset */
    StandardAR_SLAM_TRACKING_RELOCALIZING, /* SLAM is in reloc state */
    StandardAR_SLAM_TRACKING_SUCCESS_NO_MAP /* SLAM Tracking succ, but map not expand OK */
} StandardARSLAMState;

typedef struct StandardARSLAMCamera
{
    float quaternion_u3d[4];    /* for unity3d */
    float center_u3d[3];        /* for unity3d */
    float quaternion[4];
    float center[3];
    float rotation[3][3];    /* rotation matrix R */
    float translation[3];    /* translation vector T */
    float depth;
} StandardARSLAMCamera;                /* for a 3D point in world coordinate X_w, its 3D coordinate in the camera frame X_c = R * X_w + T */


/* landmark 3d position */
typedef struct StandardARSLAMLandmark
{
    float X, Y, Z, W;       /* X, Y, Z is world position of the point, W is weight of point */
} StandardARSLAMLandmark;

typedef struct StandardARSLAMPlane
{
    int plane_id;
    char is_updated;
    int plane_f_index[2];
    int plane_v_index[2];
    float plane_normal[3];
    float plane_origin_point[3];
    float plane_x_axis[3];
    unsigned char reserved[7];
} StandardARSLAMPlane;

typedef enum StandardARSLAMVertexFormat
{
    StandardAR_SLAM_FORMAT_3D_POSITION = 0,                 // 3 float (position)
    StandardAR_SLAM_FORMAT_3D_POSITION_NORMAL,          // 6 float (position, normal)
    StandardAR_SLAM_FORMAT_3D_POSITION_NORMAL_COLOR     // 9 float (position, normal, RGB), the range of color is [0, 1]
} StandardARSLAMVertexFormat;



typedef enum StandardARSubVersion{
    StandardARSubVersion_0_Origin = 0,
    StandardARSubVersion_1_Marker = 1
} StandardARSubVersion;

typedef enum StandardARSLAMImageMarkerState{
    StandardAR_SLAM_DETECT_SUCCEED = 0,
    StandardAR_SLAM_DETECT_FAIL = 1
} StandardARSLAMImageMarkerState;

typedef enum StandardARSLAMImageMarkerType{
    StandardAR_SLAM_Marker_Type_PreTraining,
    StandardAR_SLAM_Marker_Type_Raw_Image_Gray,
    StandardAR_SLAM_Marker_Type_Raw_Image_RGB24,
    StandardAR_SLAM_Marker_Type_Raw_Image_RGB32,
    StandardAR_SLAM_Marker_Type_Compressed_Image
} StandardARSLAMImageMarkerType;

/* corner in image coordinate */
typedef struct StandardARSLAMCorner
{
    float x, y;
} StandardARSLAMCorner;

/* corners for  */
typedef struct StandardARSLAMMarker
{
    StandardARSLAMCorner corners[4];
} StandardARSLAMMarker;

typedef struct StandardARSLAMImageMarkerDetectResult{
    int marker_id;                      /* ID of Marker */
    int marker_state;                   /* marker detect result state, type: StandardARSLAMImageMarkerState */
    float corners[12];                     /* X_i = corners[i*3], Y_i = corners[i*3+1], Z_i = corners[i*3+2], i belong to [0, corners_len) */
} StandardARSLAMImageMarkerDetectResult;

// 注意：！！！！！！！！！！！！！！
// 0.属性顺序不能调整，出reserve属性，其他属性偏移位置不能调整；
// 1.结构采用32位对其，就是4字节对齐，否则结构指针可能都会导致异常；
// 2.要确保字符串后的结构指针4字节对齐，字符串必须四字节对齐；
// 3.目前StandardARSubVersion_1_Marker后（包括）的版本的版本保证字符串info4字节对齐；
// 4.每个结构内的长度必须是4字节对齐！！！
typedef struct StandardARSLAMTrackingResult
{
    int version;
    int sub_version;
    int state;	// StandardARSLAMState
    StandardARSLAMMarker marker;
    StandardARSLAMCamera camera;
    float light_intensity;

    int track_confidence;
    int track_state;
    int num_features;
    int num_landmarks;
    /////////////////////////////////////////////////////////
    // Plane Example
    // 0 ----- 1       4 ----- 5
    //  |   / |         |   /
    //  |  /  |         |  /
    //  | /   |         | /
    // 3 ----- 2       6
    //
    // plane_count: 2
    // dense_mesh_v: [x0,y0,z0,nx0,ny0,nz0,r0,g0,b0,
    //                x1,y1,z1,nx1,ny1,nz1,r1,g1,b1,
    //                x2,y2,z2,...,
    //                x3,y3,z3,...,
    //                x4,y4,z4,...,
    //                x5,y5,z5,...,
    //                x6,y6,z6,...]
    // dense_mesh_f: [0,3,1,1,3,2,4,6,5]
    // dense_mesh_v_size: 63
    // dense_mesh_f_size: 9

    // plane_v_index: [0, 36, 63], size is plane_count + 1
    //                - [0, 36): the [start, end) verts data of the 1st plane in dense_mesh_v array
    //                - [36,63): the [start, end) verts data of the 2nd plane in dense_mesh_v array
    // plane_f_index: [0, 6, 9], size is plane_count + 1
    //                - [0,6): (0,3,1,1,3,2), the [start, end) triangles data of the 1st plane in dense_mesh_f array
    //                - [6,9): (4,6,5), the [start, end) triangle data of the 2nd plane in dense_mesh_f array
    ////////////////////////////////////////////////////////
    int plane_count;        // the number of planes
    int dense_mesh_v_size;  // the length of dense_mesh_v
    int dense_mesh_f_size;  // the length of dense_mesh_f
    int info_length;        // 必须4字节对齐，目前StandardARSubVersion_1_Marker后（包括）的版本的版本保证4字节对齐
    int image_marker_count;         /// num of marker detector result
    char reverved[60];

    float *dense_mesh_v;
    int *dense_mesh_f;
    int *plane_v_index;
    int *plane_f_index;
    StandardARSLAMPlane *planes;      // null if plane_count = 0
    StandardARSLAMCorner * features;  // 2D feature in current frame
    StandardARSLAMLandmark *landmarks;// 3D landmarks or point cloud in world coordinate

    const char* info;       // 必须4字节对齐，目前StandardARSubVersion_1_Marker后（包括）的版本保证4字节对齐
    StandardARSLAMImageMarkerDetectResult* image_markers;   /// marker detect result, null if marker_count = 0
} StandardARSLAMTrackingResult;

typedef struct StandardARLightEstimationResult
{
    int width;
    int height;
    int estimate_state;
    int estimate_valid;
    int64_t light_timestamp;
    float * environment_texture;
    float * spherical_harmonic_params;//27
    float * hdr_main_light_direction;//3
    float * hdr_main_light_intensity;//3
    float * color_correction;//4
}StandardARLightEstimationResult;


typedef enum StandardARImuType
{
    StandardAR_SLAM_ACCELEROMETER_UNCALIBRATED = 0,
    StandardAR_SLAM_GYROSCOPE_UNCALIBRATED = 1,
    StandardAR_SLAM_GRAVITY = 2,
    StandardAR_SLAM_ROTATION_VECTOR = 3
}StandardARImuType;

typedef struct StandardARSLAMImage
{
    unsigned char *data;    /* contains the intensity value for each image pixel */
    double timeStamp;       /* timestamp in second */
} StandardARSLAMImage;

typedef struct StandardARSLAMIMU
{
    double acceleration[3];    /* raw acceleration data in gravity unit. */
    double gyroscope[3];    /* raw angular velocity data in radian */
    double timeStamp;       /* timestamp in second */
} StandardARSLAMIMU;

typedef struct StandardARSLAMAttitude
{
    double quaternion[4];    /* attitude of the device */
    double gravity[3];        /* gravity direction */
    double timeStamp;       /* timestamp in second */
} StandardARSLAMAttitude;

typedef struct StandardARDensePlaneDesc {
    int plane_count;                        /// the number of planes
    int dense_mesh_v_size;                  /// the length of dense_mesh_v
    int dense_mesh_f_size;                  /// the length of dense_mesh_f
    char reserved[12];
} StandardARDensePlaneDesc;

typedef struct StandardARDenseReconPlane
{
    int plane_id;
    char is_updated;
    int plane_f_index[2];
    int plane_v_index[2];
    float plane_normal[3];
    float plane_origin_point[3];
    float plane_x_axis[3];
    unsigned char reserved[7];
} StandardARDenseReconPlane;

typedef struct StandardARSurfelCloudDesc {
    int surfel_format;                      /// the format of surfel, refer to STDenseVertexFormat
    int surfel_size;                        /// the length of surfel
    char reserved[20];
} StandardARSurfelCloudDesc;

typedef struct StandardARSurfaceMeshDesc {
    int vertex_format;                      /// the format of surface_mesh_v, refer to STDenseVertexFormat
    int surface_mesh_v_size;                /// the length of surface_mesh_v
    int surface_mesh_f_size;                /// the length of surface_mesh_f
    char reserved[12];
} StandardARSurfaceMeshDesc;

/////////////////////////////////////////////////////////
/// Plane Example
/// 0 ----- 1       4 ----- 5
///  |   / |         |   /
///  |  /  |         |  /
///  | /   |         | /
/// 3 ----- 2       6
///
/// plane_count: 2
/// dense_mesh_v: [x0,y0,z0,nx0,ny0,nz0,r0,g0,b0,
///                x1,y1,z1,nx1,ny1,nz1,r1,g1,b1,
///                x2,y2,z2,...,
///                x3,y3,z3,...,
///                x4,y4,z4,...,
///                x5,y5,z5,...,
///                x6,y6,z6,...]
/// dense_mesh_f: [0,3,1,1,3,2,4,6,5]
/// dense_mesh_v_size: 63
/// dense_mesh_f_size: 9

/// plane_v_index: [0, 36, 63], size is plane_count + 1
///                - [0, 36): the [start, end) verts data of the 1st plane in dense_mesh_v array
///                - [36,63): the [start, end) verts data of the 2nd plane in dense_mesh_v array
/// plane_f_index: [0, 6, 9], size is plane_count + 1
///                - [0,6): (0,3,1,1,3,2), the [start, end) triangles data of the 1st plane in dense_mesh_f array
///                - [6,9): (4,6,5), the [start, end) triangle data of the 2nd plane in dense_mesh_f array
////////////////////////////////////////////////////////
typedef struct StandardARDenseReconResult
{
    int version;
    int sub_version;
    int info_length;
    char reserved[48];

    /// debug info of dense reconstruction
    const char* info;

    /// dense plane
    StandardARDensePlaneDesc *plane_desc;                       /// should not be null
    float *dense_mesh_v;                                /// null if plane_desc->plane_count == 0
    int *dense_mesh_f;                                  /// null if plane_desc->plane_count == 0
    int *plane_v_index;                                 /// null if plane_desc->plane_count == 0
    int *plane_f_index;                                 /// null if plane_desc->plane_count == 0
    StandardARDenseReconPlane *planes;                          /// null if plane_desc->plane_count == 0

    /// surfel cloud
    StandardARSurfelCloudDesc *surfel_cloud_desc;               /// should not be null
    float *surfel_cloud;                                /// null if surfel_cloud_desc->surfel_size == 0

    /// surface mesh
    StandardARSurfaceMeshDesc *surface_mesh_desc;               /// should not be null
    float *surface_mesh_v;                              /// null if surface_mesh_desc->surface_mesh_v_size == 0
    int *surface_mesh_f;                                /// null if surface_mesh_desc->surface_mesh_f_size == 0

    /// more here...
} StandardARDenseReconResult;

typedef struct StandardARObjectTrackingResult
{
    int num;
    int frame_index;
    double time_stamp;

    /// each digits corresponds to a object id
    int *id_buffer;
    /// each digits corresponds to a object state
    /// 0: good
    /// -1: bad
    int *state_buffer;
    /// object pose: Rco tco
    /// [0, 8] [9, 17] ... [9*n, 8+9*n]; every 9 digits is the rotation matrix (row major)
    float *R_obj_buffer;
    /// [0, 2] [3, 5] ... [3*n, 2+3*n]; every 3 digits is the translation vector
    float *t_obj_buffer;

    /// camera pose: Rcw tcw
    float R_camera[3][3];
    float t_camera[3];

    float *bounding_box;

    int info_length;
    const char* info;
} StandardARObjectTrackingResult;

// !!!!!!!!!!!!!!! 注意小心，序列化和反序列化必须同时修改，上线后已经序列化的顺序不能再进行修改  !!!!!!!!!!!!!!!
extern int32_t calculateSLAMTrackingResult(const StandardARSLAMTrackingResult* result);
extern int32_t SLAMTrackingResultSerializeToBuffer(const StandardARSLAMTrackingResult* result, char* pbuffer, int bufsize);
extern int32_t UnserializeToSLAMTrackingAndDenseReconResult(const char* pbuffer, int32_t bufsize, StandardARSLAMTrackingResult* slam_result, StandardARDenseReconResult* dense_recon_result);
extern int32_t UnserializeToSLAMTrackingResult(const char* pbuffer, int32_t bufsize, StandardARSLAMTrackingResult* result);
extern int32_t UnserializeToDenseReconResult(const char* pbuffer, int32_t bufsize, int32_t offset, StandardARDenseReconResult* result);
extern int32_t UnserializeToLightEstimationResult(const char* pbuffer, int bufsize, int32_t offset, StandardARLightEstimationResult* result, ARIlluminationEstimateMode mode);
extern int32_t UnserializeToObjectTrackingResult(const char* pbuffer, int32_t bufsize, int32_t offset, StandardARObjectTrackingResult* result);

extern bool isMemoryEqual(const char* tag, const char* pleftbuf, const char* prightbuf, int bufsize);
extern bool isSLAMTrackingResultEqual(const StandardARSLAMTrackingResult* pleft, const StandardARSLAMTrackingResult* pright);
extern void dumpSLAMResult(const StandardARSLAMTrackingResult* result);
extern void convertSLAMResultPointToYAxisUp(StandardARSLAMTrackingResult& result);
extern void convertDenseReconResultPointToYAxisUp(StandardARDenseReconResult& result);

extern void getYAxisUpViewMatrix(const StandardARSLAMCamera& slamcamera, float* out_col_major_4x4, MatrixConvertFlag convertFlag);
extern void getZAxisUpViewMatrix(const StandardARSLAMCamera& slamcamera, float* out_col_major_4x4, MatrixConvertFlag convertFlag);
extern void SLAMMatrixToRenderingMatrix(const StandardARSLAMCamera& slamcamera, float* out_col_major_4x4);
extern void convertAlgorithmMatrixToRenderingMatrix(const float* rotation, const float* translation, float* out_col_major_4x4);

typedef StandardARSLAMTrackingResult (*StandardARSLAMRun)(const StandardARSLAMImage I,                            /* input image */
                                                          const StandardARSLAMIMU *zs_vio, const int N_vio,    /* N IMU measurements during last and current frame */
                                                          const StandardARSLAMAttitude *r );
typedef StandardARSLAMTrackingResult (*StandardARSLAMRunWithImage)(const StandardARSLAMImage I,                            /* input image */
                                                                   double exposure_time);

// https://developer.android.com/reference/android/hardware/Sensor.html#TYPE_ACCELEROMETER_UNCALIBRATED
typedef struct StandardARSLAMAcceleration
{
    double data[6];             /* index [0-2] is raw data, index [3-5] is bias*/
    double timestamp;
} StandardARSLAMAcceleration;

// https://developer.android.com/reference/android/hardware/Sensor.html#TYPE_GYROSCOPE_UNCALIBRATED
typedef struct StandardARSLAMGyroscope
{
    double data[6];             /* index [0-2] is raw data, index [3-5] is bias */
    double timestamp;
} StandardARSLAMGyroscope;

typedef struct StandardARSLAMGravity {
    double data[3];
    double timestamp;
} StandardARSLAMGravity;

typedef struct StandardARSLAMRotationVector {
    double data[4];
    double timestamp;
} StandardARSLAMRotationVector;

/* which layout this device */
/* Android app just set to StandardAR_SLAM_LandscapeRight */
typedef enum StandardARSLAMOrientation
{
    StandardAR_SLAM_Portrait,
    StandardAR_SLAM_UpsideDown,
    StandardAR_SLAM_LandscapeLeft,
    StandardAR_SLAM_LandscapeRight
} StandardAROrientation;

typedef enum StandardARSLAMDevice {
    DEVICE_APPLE = 0,
    DEVICE_ANDROID = 1
} StandardARSLAMDevice;

typedef enum StandardARSLAMCameraType {
    MONO_CAMERA = 0,
    DUAL_CAMERA = 1
} StandardARSLAMCameraType;

/*
 * Create SLAM system
 *
 * */
struct SLAMConfig {
    int width, height;              /* image resolution */
    double fovx;                    /* horizontal field of view, e.g., for iPhone 6/6p, fovx = 58.04 */
    StandardARSLAMOrientation ori;   /* device orientation  */
    StandardARSLAMDevice device;     /* input device is Apple or Android? for different IMU format*/
    unsigned char* calib_buffer;    /* dual camera calib buffer data */
    int calib_buffer_size;          /* dual camera calib buffer size */
    int64_t ability_flag;           /* ability flag, show what ability should open to developer*/
    int platform_flag;              /* which platform, MTK or Qualcomm */
    float rolling_shutter_time;     /* rolling shutter time */
    char reserved[16];              /* reserved field */
};
#ifdef __cplusplus
}
#endif
#endif //HELLO_AR_C_SRC_SLAMCOMMON_H
