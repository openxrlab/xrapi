
#ifndef STANDARD_AR_INTERNAL_C_API_H_
#define STANDARD_AR_INTERNAL_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <jni.h>

#include "StandardARCommon.h"
#include "StandardAR.h"

int javaEnvOnLoad(void *vm, void *reserved);

extern "C" {

// === Java Env methods ===
int impl_arJavaEnv_onLoad(void *vm, void *reserved);
void impl_arJavaEnv_setClassLoader(void *vm, void *classloader, void *findclassid);

// === ARCapability methods ===
void impl_arCapability_checkAvailability(void *env, void *activity_context, ARAvailability *out_availability);
ARResult impl_arCapability_checkAlgorithm(void *env, void *application_context, ARAlgorithmType type, ARStreamMode mode);

// === ARPose_ methods ===
void impl_arPose_create(const float *pose_raw, ARPose_ **out_pose);
void impl_arPose_destroy(ARPose_ *pose);
void impl_arPose_getPoseRaw(const ARPose_ *pose, float *out_pose_raw);
void impl_arPose_getMatrix(const ARPose_ *pose, float *out_matrix_col_major_4x4);

// === ARConfig_ methods ===
void impl_arConfig_create(ARConfig_ **out_config);
void impl_arConfig_destroy(ARConfig_ *config);
void impl_arConfig_getIlluminationEstimateMode(const ARConfig_ *config, ARIlluminationEstimateMode *light_estimation_mode);
void impl_arConfig_setIlluminationEstimateMode(ARConfig_ *config, ARIlluminationEstimateMode light_estimation_mode);
void impl_arConfig_getMapCoupleMode(const ARConfig_ *config, ARMapCoupleMode *map_couple_mode);
void impl_arConfig_setMapCoupleMode(ARConfig_ *config, ARMapCoupleMode map_couple_mode);
void impl_arConfig_getPlaneFindingMode(const ARConfig_ *config, ARPlaneDetectingMode *plane_finding_mode);
void impl_arConfig_setPlaneFindingMode(ARConfig_ *config, ARPlaneDetectingMode plane_finding_mode);
void impl_arConfig_setCameraId(ARConfig_ *config, ARCameraID cameraId);
void impl_arConfig_getTrackingRunMode(const ARConfig_ *config, ARTrackingRunMode *tracking_run_mode);
void impl_arConfig_setTrackingRunMode(ARConfig_ *config, ARTrackingRunMode tracking_run_mode);
void impl_arConfig_getWorldAlignmentMode(const ARConfig_ *config, ARWorldAlignmentMode *world_alignment_mode);
void impl_arConfig_setWorldAlignmentMode(ARConfig_ *config, ARWorldAlignmentMode world_alignment_mode);
void impl_arConfig_getVideoPixelFormat(const ARConfig* config, ARPixelFormat * video_pixel_format);
void impl_arConfig_setVideoPixelFormat(ARConfig* config, ARPixelFormat video_pixel_format);
void impl_arConfig_getAlgorithmStreamMode(const ARConfig *config, ARAlgorithmType algorithm_type, ARStreamMode *out_stream_mode);
void impl_arConfig_setAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type, ARStreamMode stream_mode);
void impl_arConfig_getAlgorithmMode(const ARConfig *config, ARAlgorithmType algorithm_type, ARAlgorithmMode *out_algorithm_mode);
void impl_arConfig_setAlgorithmMode(ARConfig *config, ARAlgorithmType algorithm_type, ARAlgorithmMode algorithm_mode);
void impl_arConfig_setAugmentedImageDatabase(const ARSession_ *session, ARConfig *config, const ARReferenceImageDatabase_ *augmented_image_database);
void impl_arConfig_getAugmentedImageDatabase(const ARSession_ *session, const ARConfig *config, ARReferenceImageDatabase_ *out_augmented_image_database);
void impl_arConfig_setReferenceObjectDatabase(const ARSession_ *session, ARConfig_ *config, const ARReferenceObjectDatabase_ *object_database);
void impl_arConfig_getReferenceObjectDatabase(const ARSession_ *session, const ARConfig_ *config, ARReferenceObjectDatabase_ *object_database);
void impl_arConfig_setDeviceType(const ARConfig *config, ARDeviceType device_type);
void impl_arConfig_getDeviceType(ARConfig *config, ARDeviceType *out_device_type);
void impl_arConfig_setServerUrl(ARConfig_ *config, const char *url);
void impl_arConfig_setBeaconUUID(ARConfig_ *config, const char *uuid);

// === ARWrold_ methods ===
ARResult impl_arSession_create(void *env, void *activity_context, ARSession_ **out_session_pointer);
ARResult impl_arSession_getDisplayLandscapeSize(const ARSession_ *session, int *outwidth, int *outheight);
void impl_arSession_destroy(ARSession_ *session);
ARResult impl_arSession_checkSupported(const ARSession_ *session, const ARConfig_ *config);
ARResult impl_arSession_configure(ARSession_ *session, const ARConfig_ *config);
ARResult impl_arSession_resume(ARSession_ *session);
ARResult impl_arSession_startUp(ARSession_ *session, ARAlgorithmType type, int32_t option);
ARResult impl_arSession_stop(ARSession_ *session, ARAlgorithmType type, int32_t option);
ARResult impl_arSession_pause(ARSession_ *session);
void impl_arSession_getTrackingState(const ARSession_ *session, ARTrackingState *out_tracking_state);
void impl_arSession_setCameraTextureName(ARSession_ *session, uint32_t texture_id);
void impl_arSession_setDisplayGeometry(ARSession_ *session, int32_t rotation, int32_t width, int32_t height);
ARResult impl_arSession_update(ARSession_ *session, ARFrame_ *out_frame);
ARResult impl_arSession_acquireMap(const ARSession_ *session, ARMap_ **out_map);
ARResult impl_arSession_acquireNewAnchor(ARSession_ *session, const ARPose_ *pose, ARAnchor_ **out_anchor);
void impl_arSession_getAllAnchors(const ARSession_ *session, ARAnchorList_ *out_anchor_list);
void impl_arSession_getAllNodes(const ARSession_ *session, ARNodeType filter_type, ARNodeList_ *out_trackable_list);

void impl_arSession_getPlaneVertexCount(const ARSession_ *session, int32_t *out_vertex_count);
void impl_arSession_getPlaneVertexArray(const ARSession_ *session, float *out_vertex_array);
void impl_arSession_getPlaneIndexCount(const ARSession_ *session, int32_t *out_index_count);
void impl_arSession_getPlaneIndexArray(const ARSession_ *session, unsigned short *out_index_array);

void impl_arSession_getVerticalFov(const ARSession_ *session, float *oVerticalFov);
bool impl_arSession_isSLAMInitializing(const ARSession_ *session);
void impl_arSession_getInitProgress(const ARSession_ *session, int *progress);
void impl_arSession_getMapQuality(const ARSession_ *session, float *SLAMMapQuality);
void impl_arSession_getMapCoupleState(const ARSession_ *session, int *state);
void impl_arSession_getSLAMTrackingDetails(const ARSession_ *session, int *details);
void impl_arSession_getVersion(const ARSession_ *session, char *version, int versionSize);
void impl_arSession_SetKeyAndSecret(const ARSession_ *session, const char* app_key, const char* app_secret);
void impl_arSession_getSLAMInfo(const ARSession_ *session, char *slaminfo, int infosize);
void impl_arSession_getSLAMJsonStr(const ARSession_ *session, char *jsonstr, int strsize);
ARResult impl_arSession_hostCloudAnchor(const ARSession_ *session, const ARAnchor_ *anchor, ARAnchor_ **out_anchor);
ARResult impl_arSession_resolveCloudAnchor(const ARSession_ *session, const char *anchor_id, ARAnchor_ **out_anchor);
void impl_arSession_getBackgroundData(const ARSession_ *session, char **image_data, int *width, int *height);

void impl_arSession_setIntValue(const ARSession_* session, ARParameterEnum type, int32_t* value, int size);
void impl_arSession_setFloatValue(const ARSession_* session, ARParameterEnum type, float* value, int size);
void impl_arSession_setStringValue(const ARSession_* session, ARParameterEnum type, char* value, int size);
void impl_arSession_getIntValue(const ARSession_* session, ARParameterEnum type, int32_t* value, int size);
void impl_arSession_getFloatValue(const ARSession_* session, ARParameterEnum type, float* value, int size);
void impl_arSession_getStringValue(const ARSession_* session, ARParameterEnum type, char* value, int* length, int size);

void impl_arSession_getConfig(ARSession_* session, ARConfig_ *out_config);
void impl_arSession_getSupportedCameraConfigs(const ARSession_* session, ARCameraConfigList_ *list);
ARResult impl_arSession_setCameraConfig(const ARSession_* session, const ARCameraConfig_ *camera_config);
void impl_arSession_getCameraConfig(const ARSession_* session, ARCameraConfig_ *out_camera_config);
void impl_arSession_setBoundingBox(const ARSession_ *session, const float* vertex, const float* matrix);
void impl_arSession_getBoundingBoxScanningResult(const ARSession_ *session, uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size);
void impl_arSession_getBoundingBoxPointCloud(const ARSession_ *session, float** points, int* points_num);
void impl_arSession_getRawCameraIntrinsics(const ARSession_ *session, float* value, int size);

// === ARCamera_ methods ===
void impl_arCamera_getPose(const ARSession_ *session, const ARCamera_ *camera, ARPose_ *out_pose);
void impl_arCamera_getDisplayOrientedPose(const ARSession_ *session, const ARCamera_ *camera, ARPose_ *out_pose);
void impl_arCamera_getViewMatrix(const ARSession_ *session, const ARCamera_ *camera, float *out_col_major_4x4);
void impl_arCamera_getTrackingState(const ARSession_ *session, const ARCamera_ *camera, ARTrackingState *out_tracking_state);
void impl_arCamera_getProjectionMatrix(const ARSession_ *session, const ARCamera_ *camera, float near, float far, float *dest_col_major_4x4);
void impl_arCamera_release(ARCamera_ *camera);
void impl_arCamera_getImageIntrinsics(const ARSession_ *session, const ARCamera_ *camera, ARCameraIntrinsics_ *out_camera_intrinsics);
void impl_arCamera_getTextureIntrinsics(const ARSession_ *session, const ARCamera_ *camera, ARCameraIntrinsics_ *out_camera_intrinsics);
void impl_arCameraIntrinsics_create(const ARSession_ *session, ARCameraIntrinsics_ **out_camera_intrinsics);
void impl_arCameraIntrinsics_getFocalLength(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_fx, float *out_fy);
void impl_arCameraIntrinsics_getFocalLengthSlv(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_fx, float *out_fy);
void impl_arCameraIntrinsics_getPrincipalPoint(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_cx, float *out_cy);
void impl_arCameraIntrinsics_getPrincipalPointSlv(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_cx, float *out_cy);
void impl_arCameraIntrinsics_getImageDimensions(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, int32_t *out_width, int32_t *out_height);
void impl_arCameraIntrinsics_getImageDimensionsSlv(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, int32_t *out_width, int32_t *out_height);
void impl_arCameraIntrinsics_getDistortions(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_distort_arr);
void impl_arCameraIntrinsics_getDistortionsSlv(const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_distort_arr);
void impl_arCameraIntrinsics_destroy(ARCameraIntrinsics_ *camera_intrinsics);

// === ARCameraConfigList_ methods ===
void impl_arCameraConfigList_create(const ARSession_ *session, ARCameraConfigList_ **out_list);
void impl_arCameraConfigList_destroy(ARCameraConfigList_ *list);
void impl_arCameraConfigList_getSize(const ARSession_ *session, const ARCameraConfigList_ *list, int32_t *out_size);
void impl_arCameraConfigList_getItem(const ARSession_ *session, const ARCameraConfigList_ *list, int32_t index, ARCameraConfig **out_camera_config);

// === ARCameraConfig_ methods ===
void impl_arCameraConfig_create(const ARSession_ *session, ARCameraConfig_ **out_camera_config);
void impl_arCameraConfig_destroy(ARCameraConfig_ *camera_config);
void impl_arCameraConfig_getImageDimensions(const ARSession_ *session, const ARCameraConfig_ *camera_config, int32_t *out_width, int32_t *out_height);
void impl_arCameraConfig_getTextureDimensions(const ARSession_ *session, const ARCameraConfig_ *camera_config, int32_t *out_width, int32_t *out_height);

// === ARFrame_ methods ===
void impl_arFrame_create(const ARSession_ *session, ARFrame_ **out_frame);
void impl_arFrame_destroy(ARFrame_ *frame);
void impl_arFrame_getDisplayGeometryChanged(const ARSession_ *session, const ARFrame_ *frame, int32_t *out_geometry_changed);
void impl_arFrame_transformDisplayUvCoords(const ARSession_ *session, const ARFrame_ *frame, int32_t num_elements, const float *uvs_in, float *uvs_out);
void impl_arFrame_setHitTestMode(const ARSession_ *session, const ARFrame_ *frame, ARQueryMode mode);
void impl_arFrame_hitTest(const ARSession_ *session, const ARFrame_ *frame, float pixel_x, float pixel_y, ARQueryResultList_ *hit_result_list);
void impl_arFrame_getIlluminationEstimate(const ARSession_ *session, const ARFrame_ *frame, ARIlluminationEstimate_ *out_light_estimate);
ARResult impl_arFrame_acquirePointCloud(const ARSession_ *session, const ARFrame_ *frame, ARPointCloud_ **out_point_cloud);
ARResult impl_arFrame_acquireDenseMesh(const ARSession_ *session, const ARFrame_ *frame, ARDenseMesh_ **out_dense_mesh);
void impl_arFrame_acquireCamera(const ARSession_ *session, const ARFrame_ *frame, ARCamera_ **out_camera);
void impl_arFrame_getUpdatedAnchors(const ARSession_ *session, const ARFrame_ *frame, ARAnchorList_ *out_anchor_list);
void impl_arFrame_getUpdatedNodes(const ARSession_ *session, const ARFrame_ *frame, ARNodeType filter_type, ARNodeList_ *out_trackable_list);

void impl_arFrame_getImageFormat(const ARSession_ *session, const ARFrame_ *frame, ARPixelFormat *video_image_format);
ARResult impl_arFrame_getPreviewSize(const ARSession_ *session, const ARFrame_ *frame, int *outwidth, int *outheight);
bool impl_arFrame_isYUVImageDataUpdated(const ARSession_ *session, const ARFrame_ *frame);
ARResult impl_arFrame_getYUVImageData(const ARSession_ *session, const ARFrame_ *frame, unsigned char *out_gray_data, unsigned char *out_u_data, unsigned char *out_v_data);
ARResult impl_arFrame_isDepthImageAvaliable(const ARSession_* session, const ARFrame_* frame);
ARResult impl_arFrame_getDepthImageResolution(const ARSession_* session, const ARFrame_* frame, int* out_width, int* out_height);
ARResult impl_arFrame_getDepthImageData(const ARSession_* session, const ARFrame_* frame, unsigned char* out_depth_image_data);
void impl_arFrame_getDepthImageFormat(const ARSession_* session, const ARFrame_* frame, ARPixelFormat* out_format);
void impl_arFrame_getTimestamp(const ARSession_ *session, const ARFrame_ *frame, int64_t *out_timestamp_ns);
void impl_arFrame_getAndroidSensorPose(const ARSession_ *session, const ARFrame_ *frame, ARPose_ *out_pose);
void impl_arFrame_hitTestRay(const ARSession_ *session, const ARFrame_ *frame, const float *ray_origin_3, const float *ray_direction_3, ARQueryResultList_ *hit_result_list);

// === ARPointCloud_ methods ===
void impl_arPointCloud_getNumberOfPoints(const ARSession_ *session, const ARPointCloud_ *point_cloud, int32_t *out_number_of_points);
void impl_arPointCloud_getData(const ARSession_ *session, const ARPointCloud_ *point_cloud, const float **out_point_cloud_data);
void impl_arPointCloud_release(ARPointCloud_ *point_cloud);

// === ARDenseMesh_ methods ===
void impl_arDenseMesh_getFormat(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, ARVertexFormat *out_vertex_format);
void impl_arDenseMesh_getVertexCount(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, int32_t *out_vertex_count);
void impl_arDenseMesh_getVertex(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, float *out_vertex_array);
void impl_arDenseMesh_getNormal(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, float *out_vertex_normal);
void impl_arDenseMesh_getIndexCount(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, int32_t *out_index_count);
void impl_arDenseMesh_getIndex(const ARSession_ *session, const ARDenseMesh_ *dense_mesh, unsigned short *out_index_array);
void impl_arDenseMesh_release(ARDenseMesh_ *dense_mesh);

// === ARIlluminationEstimate_ methods ===
void impl_arIlluminationEstimate_create(const ARSession_ *session, ARIlluminationEstimate_ **out_light_estimate);
void impl_arIlluminationEstimate_destroy(ARIlluminationEstimate_ *light_estimate);
void impl_arIlluminationEstimate_getState(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, ARIlluminationEstimateState *out_light_estimate_state);
void impl_arIlluminationEstimate_getMode(const ARSession_ *session,const ARIlluminationEstimate_ *light_estimate,ARIlluminationEstimateMode *out_light_estimate_state);
void impl_arIlluminationEstimate_getPixelIntensity(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, float *out_pixel_intensity);
void impl_arIlluminationEstimate_getColorCorrection(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, float *out_color_correction_4);
void impl_arIlluminationEstimate_getSphericalHarmonicLighting(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate,float* out_sh_arr);
void impl_arIlluminationEstimate_getEnvironmentTextureSize(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, int * width,int * height);
void impl_arIlluminationEstimate_getEnvironmentalHdrMainLightDirection(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, float *out_direction_3);
void impl_arIlluminationEstimate_getEnvironmentalHdrMainLightIntensity(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, float *out_intensity_3);
void impl_arIlluminationEstimate_acquireEnvironmentalHdrCubemap(const ARSession_ *word, const ARIlluminationEstimate_ *light_estimate, float* out_textures_6);
void impl_arIlluminationEstimate_getLightTimestamp(const ARSession_ *session, const ARIlluminationEstimate_ *light_estimate, int64_t *out_timestamp_ns);

// === ARAnchorList_ methods ===
void impl_arAnchorList_create(const ARSession_ *session, ARAnchorList_ **out_anchor_list);
void impl_arAnchorList_destroy(ARAnchorList_ *anchor_list);
void impl_arAnchorList_getSize(const ARSession_ *session, const ARAnchorList_ *anchor_list, int32_t *out_size);
void impl_arAnchorList_acquireItem(const ARSession_ *session, const ARAnchorList_ *anchor_list, int32_t index, ARAnchor_ **out_anchor);


// === ARAnchor_ methods ===
void impl_arAnchor_getPose(const ARSession_ *session, const ARAnchor_ *anchor, ARPose_ *out_pose);
void impl_arAnchor_getTrackingState(const ARSession_ *session, const ARAnchor_ *anchor, ARTrackingState *out_tracking_state);
void impl_arAnchor_detach(const ARSession_ *session, ARAnchor_ *anchor);
void impl_arAnchor_release(ARAnchor_ *anchor);
void impl_arAnchor_getCloudAnchorId(const ARSession_ *session, const ARAnchor_ *anchor, char *out_anchor_id, int length);
void impl_arAnchor_getCloudAnchorState(const ARSession_ *session, const ARAnchor_ *anchor, ARCloudAnchorState *out_state);


// === ARNodeList_ methods ===
void impl_arNodeList_create(const ARSession_ *session, ARNodeList_ **out_node_list);
void impl_arNodeList_destroy(ARNodeList_ *node_list);
void impl_arNodeList_getSize(const ARSession_ *session, const ARNodeList_ *node_list, int32_t *out_size);
void impl_arNodeList_acquireItem(const ARSession_ *session, const ARNodeList_ *node_list, int32_t index, ARNode_ **out_node);

void impl_arHandGesture_getHandGestureType(const ARSession_ *session, const ARHandGestureNode_ *handGesture,ARHandGestureType* out_ar_handgesture_type);
void impl_arHandGesture_getHandTypeConfidence(const ARSession_ *session, const ARHandGestureNode_ *handGesture, float* out_hand_type_confidence);
void impl_arHandGesture_getHandSide(const ARSession_ *session, const ARHandGestureNode_ *handGesture, ARHandType* out_ar_hand_side);
void impl_arHandGesture_getHandTowards(const ARSession_ *session, const ARHandGestureNode_ *handGesture,ARHandTowards* out_ar_hand_towards);
void impl_arHandGesture_getPalmCenter(const ARSession_ *session, const ARHandGestureNode_ *handGesture, float* out_palm_center_xyz);
void impl_arHandGesture_getPalmNormal(const ARSession_ *session, const ARHandGestureNode_ *handGesture, float* out_palm_palm_xyz);
void impl_arHandGesture_getLandMark2DCount(const ARSession_ *session, const ARHandGestureNode_ *handGesture, int32_t * out_landmark_2d_count);
void impl_arHandGesture_getLandMark2DArray(const ARSession_ *session, const ARHandGestureNode_ *handGesture, float* out_landmark2d_array);
void impl_arHandGesture_getRect(const ARSession_ *session, const ARHandGestureNode_ *handGesture, int* out_rect);
void impl_arHandGesture_getLandMark3DCount(const ARSession_ *session, const ARHandGestureNode_ *handGesture, int32_t * out_landmark_3d_count);
void impl_arHandGesture_getLandMark3DArray(const ARSession_ *session, const ARHandGestureNode_ *handGesture, float* out_landmark3d_array);
void impl_arHandGesture_getRGBSegArray(const ARSession_ *session, const ARHandGestureNode_ *handGesture, char* out_RGBSeg_array);
void impl_arHandGesture_getRGBSegWidth(const ARSession_ *session, const ARHandGestureNode_ *handGesture, int32_t* out_RGBSeg_width);
void impl_arHandGesture_getRGBSegHeight(const ARSession_ *session, const ARHandGestureNode_ *handGesture, int32_t* out_RGBSeg_height);


void impl_arFaceMesh_getCenterPose(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, ARPose_ *out_pose);
void impl_arFaceMesh_getVertices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_vertices, int32_t *out_number_of_vertices);
void impl_arFaceMesh_getNormals(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_normals, int32_t *out_number_of_normals);
void impl_arFaceMesh_getTextureCoordinates(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_texture_coordinates, int32_t *out_number_of_texture_coordinates);
void impl_arFaceMesh_getTriangleIndices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, int **out_triangle_indices, int32_t *out_number_of_triangles);

// === ARNBodyode_ methods ===
void impl_arBodyDetect_getBodyId(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_body_id);
void impl_arBodyDetect_getSkeletonPoint2dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point_count);
void impl_arBodyDetect_getSkeletonPoint2d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2d);
void impl_arBodyDetect_getSkeletonPoint2dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2d_confidence);


void impl_arBodyDetect_getSkeletonPoint3dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point3d_count);
void impl_arBodyDetect_getSkeletonPoint3d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3d);
void impl_arBodyDetect_getSkeletonPoint3dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3d_confidence);




// === ARNode_ methods ===
void impl_arNode_release(ARNode_ *node);
void impl_arNode_getType(const ARSession_ *session, const ARNode_ *node, ARNodeType *out_node_type);
void impl_arNode_getTrackingState(const ARSession_ *session, const ARNode_ *node, ARTrackingState *out_tracking_state);
ARResult impl_arNode_acquireNewAnchor(ARSession_ *session, ARNode_ *node, ARPose_ *pose, ARAnchor_ **out_anchor);
void impl_arNode_getAnchors(const ARSession_ *session, const ARNode_ *node, ARAnchorList_ *out_anchor_list);


// === ARPlaneNode_ methods ===
void impl_arPlaneNode_getType(const ARSession_ *session, const ARPlaneNode_ *plane, ARPlaneType *out_plane_type);
void impl_arPlaneNode_getNormal(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_plane_normal);
void impl_arPlaneNode_getCenterPose(const ARSession_ *session, const ARPlaneNode_ *plane, ARPose_ *out_pose);
void impl_arPlaneNode_getExtentX(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_extent_x);
void impl_arPlaneNode_getExtentY(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_extent_y);
void impl_arPlaneNode_getExtentZ(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_extent_z);
void impl_arPlaneNode_getPolygonSize(const ARSession_ *session, const ARPlaneNode_ *plane, int32_t *out_polygon_size);
void impl_arPlaneNode_getPolygon(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_polygon_xz);
void impl_arPlaneNode_isPoseInExtents(const ARSession_ *session, const ARPlaneNode_ *plane, const ARPose_ *pose, int32_t *out_pose_in_extents);
void impl_arPlaneNode_isPoseInPolygon(const ARSession_ *session, const ARPlaneNode_ *plane, const ARPose_ *pose, int32_t *out_pose_in_polygon);
void impl_arPlaneNode_getPolygon3DSize(const ARSession_ *session, const ARPlaneNode_ *plane, int32_t *out_polygon_size);
void impl_arPlaneNode_getPolygon3D(const ARSession_ *session, const ARPlaneNode_ *plane, float *out_polygon_xyz);
void impl_arPlaneNode_acquireSubsumedBy(const ARSession_ *session, const ARPlaneNode_ *plane, ARPlaneNode_ **out_subsumed_by);

// === ArAugmentedImage methods ===
void impl_arAugmentedImage_getCenterPose(const ARSession_ *session, const ARImageNode_ *augmented_image, ARPose_ *out_pose);
void impl_arAugmentedImage_getExtentX(const ARSession_ *session, const ARImageNode_ *augmented_image, float *out_extent_x);
void impl_arAugmentedImage_getExtentY(const ARSession_ *session, const ARImageNode_ *augmented_image, float *out_extent_y);
void impl_arAugmentedImage_getExtentZ(const ARSession_ *session, const ARImageNode_ *augmented_image, float *out_extent_z);
void impl_arAugmentedImage_getIndex(const ARSession_ *session, const ARImageNode_ *augmented_image, int32_t *out_index);
void impl_arAugmentedImage_acquireName(const ARSession_ *session, const ARImageNode_ *augmented_image, char **out_augmented_image_name);
void impl_arAugmentedImage_get2DCorners(const ARSession_ *session, const ARImageNode_ *augmented_image, float *out_2d_corners);
void impl_arAugmentedImage_getGlobalID(const ARSession_ *session, const ARImageNode_ *augmented_image, char **out_global_id);

// === ArAugmentedImageDatabase methods ===
void impl_arAugmentedImageDatabase_create(const ARSession_ *session, ARReferenceImageDatabase_ **out_augmented_image_database);
ARResult impl_arAugmentedImageDatabase_deserialize(const ARSession_ *session, ARReferenceImageDatabase_ *augmented_image_database, const uint8_t *database_raw_bytes, int64_t database_raw_bytes_size);
void impl_arAugmentedImageDatabase_serialize(const ARSession_ *session, const ARReferenceImageDatabase_ *augmented_image_database, uint8_t **out_image_database_raw_bytes, int64_t *out_image_database_raw_bytes_size);
ARResult impl_arAugmentedImageDatabase_addPatt(const ARSession_ *session, const ARReferenceImageDatabase_ *augmented_image_database, const char* patt_name, const uint8_t *patt_raw_bytes, int64_t patt_raw_bytes_size);
void impl_arAugmentedImageDatabase_loadConfigure(const ARSession_ *session, const ARReferenceImageDatabase_ *augmented_image_database, const char* config);
ARResult impl_arAugmentedImageDatabase_addImageWithPhysicalSize(const ARSession_ *session, ARReferenceImageDatabase_ *augmented_image_database, const char *image_name, const uint8_t *image_grayscale_pixels, int32_t image_width_in_pixels,
                                                                int32_t image_height_in_pixels, int32_t image_stride_in_pixels, float image_width_in_meters, int32_t *out_index);
void impl_arAugmentedImageDatabase_getNumImages(const ARSession_ *session, const ARReferenceImageDatabase_ *augmented_image_database, int32_t *out_num_images);
void impl_arAugmentedImageDatabase_destroy(ARReferenceImageDatabase_ *augmented_image_database);

// === ARObjectNode methods ===
void impl_arObjectNode_getCenterPose(const ARSession_ *session, const ARObjectNode_ *object_node, ARPose_ *out_pose);
void impl_arObjectNode_getBoundingBox(const ARSession_ *session, const ARObjectNode_ *object_node, float *out_boundingBox);
void impl_arObjectNode_getExtentX(const ARSession_ *session, const ARObjectNode_ *object_node,  float *out_extent_x);
void impl_arObjectNode_getExtentY(const ARSession_ *session, const ARObjectNode_ *object_node, float *out_extent_y);
void impl_arObjectNode_getExtentZ(const ARSession_ *session, const ARObjectNode_ *object_node, float *out_extent_z);
void impl_arObjectNode_getIndex(const ARSession_ *session, const ARObjectNode_ *object_node, int32_t *out_index);
void impl_arObjectNode_getName(const ARSession_ *session, const ARObjectNode_ *object_node, char **out_object_name);

// === ARReferenceObjectDatabase methods ===
void impl_arReferenceObjectDatabase_create( const ARSession_ *session, ARReferenceObjectDatabase_ **out_object_database);
ARResult impl_arReferenceObjectDatabase_deserialize(const ARSession_ *session, ARReferenceObjectDatabase_ *object_database, const uint8_t *database_raw_bytes,int64_t database_raw_bytes_size);
ARResult impl_arReferenceObjectDatabase_addObject( const ARSession_ *session,  ARReferenceObjectDatabase_ *object_database, const char *object_name, const uint8_t *object_raw_bytes,int64_t object_raw_bytes_size, int32_t *out_index);
void impl_arReferenceObjectDatabase_getNumObjects( const ARSession_ *session, const ARReferenceObjectDatabase_ *object_database, int32_t *out_num_objects);
void impl_arReferenceObjectDatabase_destroy( ARReferenceObjectDatabase_ *Object_database);

// === ARQueryResultList_ methods ===
void impl_arQueryResultList_create(const ARSession_ *session, ARQueryResultList_ **out_hit_result_list);
void impl_arQueryResultList_destroy(ARQueryResultList_ *hit_result_list);
void impl_arQueryResultList_getSize(const ARSession_ *session, const ARQueryResultList_ *hit_result_list, int32_t *out_size);
void impl_arQueryResultList_getItem(const ARSession_ *session, const ARQueryResultList_ *hit_result_list, int32_t index, ARQueryResult_ *out_hit_result);


// === ARQueryResult_ methods ===
void impl_arQueryResult_create(const ARSession_ *session, ARQueryResult_ **out_hit_result);
void impl_arQueryResult_destroy(ARQueryResult_ *hit_result);
void impl_arQueryResult_getDistance(const ARSession_ *session, const ARQueryResult_ *hit_result, float *out_distance);
void impl_arQueryResult_getHitPose(const ARSession_ *session, const ARQueryResult_ *hit_result, ARPose_ *out_pose);
void impl_arQueryResult_acquireNode(const ARSession_ *session, const ARQueryResult_ *hit_result, ARNode_ **out_node);
ARResult impl_arQueryResult_acquireNewAnchor(ARSession_ *session, ARQueryResult_ *hit_result, ARAnchor_ **out_anchor);


// === timewarp & prediction related methods ===
ARResult impl_arSession_beginRenderEye(const ARSession_ *session, int eyeId);
ARResult impl_arSession_endRenderEye(const ARSession_ *session, int eyeId);
ARResult impl_arSession_submitFrame(const ARSession_ *session, const int32_t leftEyeTextureId, const int32_t rightEyeTextureId);
ARResult impl_arSession_setWindow(const ARSession_ *session, void *window);
void impl_arSession_getPredictedDisplayTime(const ARSession_ *session, uint64_t *time);
void impl_arCamera_getPredictedViewMatrix(const ARSession_ *session, const ARCamera_ *camera, float *out_col_major_4x4);
void impl_arCamera_getProjectionMatrixSeeThrough(const ARSession_ *session, const ARCamera_ *camera, float near, float far, float *dest_col_major_4x4);

}

#endif  // STANDARD_AR_INTERNAL_C_API_H_
