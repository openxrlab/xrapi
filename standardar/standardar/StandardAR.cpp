#include "StandardAR.h"
#include "StandardARCommon.h"

#include "ArPose.h"
#include "ArAnchor.h"
#include "ArTrackable.h"
#include "ArFrame.h"
#include "ArHitResult.h"
#include "ArSession.h"
#include "ArPlane.h"
#include "ArJniHelpler.h"
#include "jni.h"

#include "StandardARInternal.h"

using namespace standardar;

extern "C" {

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
static ARSession *gWorld;
static AREyeSide gBeginEyeId;
static AREyeSide gEndEyeId;
static int gLeftEyeTexId = 0;
static int gRightEyeTexId = 0;

int arJavaEnvOnLoad(void *vm, void *reserved) {
    return impl_arJavaEnv_onLoad(vm, reserved);
}

void arJavaEnvSetClassLoader(void *vm, void *classloader, void *findclassid) {
    impl_arJavaEnv_setClassLoader(vm, classloader, findclassid);
}

// --------------------------------------------------------------------------------------------
void
arCapabilityCheckAvailability(void *env, void *activity_context, ARAvailability *out_availability) {
    return impl_arCapability_checkAvailability(env, activity_context, out_availability);
}


ARResult arCapabilityCheckAlgorithm(void *env, void *application_context, ARAlgorithmType type,
                                    ARStreamMode mode) {
    return impl_arCapability_checkAlgorithm(env, application_context, type, mode);
}

// --------------------------------------------------------------------------------------------
void arPoseCreate(const float *pose_raw, ARPose **out_pose) {
    impl_arPose_create(pose_raw, out_pose);
}

void arPoseDestroy(ARPose *pose) {
    impl_arPose_destroy(pose);
}

void arPoseGetPoseRaw(const ARPose *pose, float *out_pose_raw) {
    impl_arPose_getPoseRaw(pose, out_pose_raw);
}

void arPoseGetMatrix(const ARPose *pose, float *out_matrix_col_major_4x4) {
    impl_arPose_getMatrix(pose, out_matrix_col_major_4x4);
}


// --------------------------------------------------------------------------------------------
///// ------ ARImageFeature api ------
//void arImageFeatureCreate(const char* desc,
//                          int32_t width,
//                          int32_t height,
//                          ARPixelFormat format,
//                          int32_t buffer_size,
//                          const char* buffer,
//                          ARImageFeature **out_image_feature);
//void arImageFeatureDestroy(ARImageFeature *image_feature);
//void arImageFeatureGetDesc(const ARImageFeature *image_feature, char** out_desc);
//void arImageFeatureGetWidth(const ARImageFeature *image_feature, int32_t* out_width);
//void arImageFeatureGetHeight(const ARImageFeature *image_feature, int32_t* out_height);
//void arImageFeatureGetFormat(const ARImageFeature *image_feature, int32_t* out_format);
//void arImageFeatureGetBufferSize(const ARImageFeature *image_feature, int32_t* out_buffersize);
//void arImageFeatureGetBuffer(const ARImageFeature *image_feature, int32_t** out_buffer);
//
//
///// ------ ARObjectFeature api ------
//void arObjectFeatureCreate(const char* desc,
//                           ARVertexFormat format,
//                           int32_t point_count,
//                           float* point_data,
//                           ARObjectFeature **out_object_feature);
//void arObjectFeatureDestroy(ARObjectFeature *object_feature);
//void arObjectFeatureGetDesc(const ARObjectFeature *object_feature, char** out_desc);
//void arObjectFeatureGetFormat(const ARObjectFeature *object_feature, int32_t* out_format);
//void arObjectFeatureGetExtent(const ARObjectFeature *object_feature, float *out_extent_xyz);
//void arObjectFeatureGetCenter(const ARObjectFeature *object_feature, float *out_center_xyz);
//void arObjectFeatureGetNumberOfPoints(const ARObjectFeature *object_feature, int32_t *out_number_of_points);
//void arObjectFeatureGetData(const ARObjectFeature *object_feature, float **out_point_cloud_data);
// --------------------------------------------------------------------------------------------
void arConfigCreate(ARConfig **out_config) {
    impl_arConfig_create(out_config);
}

void arConfigDestroy(ARConfig *config) {
    impl_arConfig_destroy(config);
}

void arConfigGetIlluminationEstimateMode(const ARConfig *config,
                                         ARIlluminationEstimateMode *light_estimation_mode) {
    impl_arConfig_getIlluminationEstimateMode(config, light_estimation_mode);
}

void arConfigSetIlluminationEstimateMode(ARConfig *config,
                                         ARIlluminationEstimateMode light_estimation_mode) {
    impl_arConfig_setIlluminationEstimateMode(config, light_estimation_mode);
}

void arConfigGetMapCoupleMode(ARConfig *config, ARMapCoupleMode *map_couple_mode) {
    impl_arConfig_getMapCoupleMode(config, map_couple_mode);
}

void arConfigSetMapCoupleMode(ARConfig *config, ARMapCoupleMode map_couple_mode) {
    impl_arConfig_setMapCoupleMode(config, map_couple_mode);
}

void arConfigGetPlaneDetectingMode(const ARConfig *config, ARPlaneDetectingMode *plane_detecting_mode) {
    impl_arConfig_getPlaneFindingMode(config, plane_detecting_mode);
}

void arConfigSetPlaneDetectingMode(ARConfig *config, ARPlaneDetectingMode plane_detecting_mode) {
    impl_arConfig_setPlaneFindingMode(config, plane_detecting_mode);
}

void arConfigGetTrackingRunMode(const ARConfig *config, ARTrackingRunMode *tracking_run_mode) {
    impl_arConfig_getTrackingRunMode(config, tracking_run_mode);
}

void arConfigSetTrackingRunMode(ARConfig *config, ARTrackingRunMode tracking_run_mode) {
    impl_arConfig_setTrackingRunMode(config, tracking_run_mode);
}

void
arConfigGetWorldAlignmentMode(const ARConfig *config, ARWorldAlignmentMode *world_alignment_mode) {
    impl_arConfig_getWorldAlignmentMode(config, world_alignment_mode);
}

void arConfigSetWorldAlignmentMode(ARConfig *config, ARWorldAlignmentMode world_alignment_mode) {
    impl_arConfig_setWorldAlignmentMode(config, world_alignment_mode);
}

void arConfigGetVideoPixelFormat(const ARConfig *config, ARPixelFormat *video_pixel_format) {
    impl_arConfig_getVideoPixelFormat(config, video_pixel_format);
}
void arConfigSetVideoPixelFormat(ARConfig *config, ARPixelFormat video_pixel_format) {
    impl_arConfig_setVideoPixelFormat(config, video_pixel_format);
}

void arConfigGetAlgorithmMode(const ARConfig *config, ARAlgorithmType algorithm_type,
                              ARAlgorithmMode *out_algorithm_mode) {
    impl_arConfig_getAlgorithmMode(config, algorithm_type, out_algorithm_mode);
}

void arConfigSetAlgorithmMode(ARConfig *config, ARAlgorithmType algorithm_type,
                              ARAlgorithmMode algorithm_mode) {
    impl_arConfig_setAlgorithmMode(config, algorithm_type, algorithm_mode);
}

void arConfigSetReferenceImageDatabase(const ARSession *session,
                                       ARConfig *config,
                                       const ARReferenceImageDatabase *augmented_image_database){
    impl_arConfig_setAugmentedImageDatabase(session, config, augmented_image_database);
}

void arConfigGetReferenceImageDatabase(const ARSession *session,
                                       const ARConfig *config,
                                       ARReferenceImageDatabase *out_augmented_image_database){
    impl_arConfig_getAugmentedImageDatabase(session, config, out_augmented_image_database);
}

void arConfigSetReferenceObjectDatabase(const ARSession *session, ARConfig *config, const ARReferenceObjectDatabase *object_database){
    impl_arConfig_setReferenceObjectDatabase(session, config, object_database);
}

void arConfigGetReferenceObjectDatabase(const ARSession *session, const ARConfig *config, ARReferenceObjectDatabase *out_object_database){
    impl_arConfig_getReferenceObjectDatabase(session, config, out_object_database);
}

void arConfigSetAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type, ARStreamMode stream_mode){
    impl_arConfig_setAlgorithmStreamMode(config, algorithm_type, stream_mode);
}

void arConfigGetAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type,
                                    ARStreamMode *out_stream_mode) {
    impl_arConfig_getAlgorithmStreamMode(config, algorithm_type, out_stream_mode);
}

void arConfigSetMapServerURL(ARConfig *config, const char *url) {
    impl_arConfig_setServerUrl(config, url);
}

void arConfigSetBeaconUUID(ARConfig *config, const char *uuid) {
    impl_arConfig_setBeaconUUID(config, uuid);
}

void arConfigSetDeviceType(ARConfig *config, ARDeviceType device_type) {
    impl_arConfig_setDeviceType(config, device_type);
}

void arConfigGetDeviceType(ARConfig *config, ARDeviceType *out_device_type) {
    impl_arConfig_getDeviceType(config, out_device_type);
}

//void arConfigGetVideoFormat(const ARConfig* config, ARVideoResolutionMode* video_resolution, int32_t* width, int32_t* height);
//void arConfigSetVideoFormat(ARConfig* config, ARVideoResolutionMode video_resolution, int32_t width, int32_t height);
//void arConfigGetVideoFPS(const ARConfig* config, int32_t* videofps);
//void arConfigSetVideoFPS(ARConfig* config, int32_t videofps);
//void arConfigAddTrackingImage(ARConfig* config, const ARImageFeature* image_feature);
//void arConfigSetMaxNumberOfSyncTrackingImages(ARConfig* config, int32_t max_number);
//void arConfigAddTrackingObject(ARConfig* config, const ARObjectFeature* object_feature);
//void arConfigSetStartupMap(ARConfig* config, ARWorldMap* ar_map);

// --------------------------------------------------------------------------------------------
ARResult arSessionCreate(void *env, void *activity_context, ARSession **out_session) {
    return impl_arSession_create(env == nullptr ?
        ArJniHelpler::getJavaEnv().get() : env, activity_context, out_session);
}

void arSessionDestroy(ARSession *session) {
    impl_arSession_destroy(session);
}

ARResult arSessionConfigure(ARSession *session, const ARConfig *config) {
    return impl_arSession_configure(session, config);
}

ARResult arSessionResume(ARSession *session) {
    pthread_mutex_lock(&gMutex);
    gWorld = session;
    pthread_mutex_unlock(&gMutex);
    return impl_arSession_resume(session);
}

ARResult arSessionStartAlgorithm(ARSession_ *session, ARAlgorithmType type, int32_t option) {
    return impl_arSession_startUp(session, type, option);
}

ARResult arSessionStopAlgorithm(ARSession_ *session, ARAlgorithmType type, int32_t option) {
    return impl_arSession_stop(session, type, option);
}

ARResult arSessionPause(ARSession *session) {
    pthread_mutex_lock(&gMutex);
    gWorld = nullptr;
    pthread_mutex_unlock(&gMutex);
    return impl_arSession_pause(session);
}

ARResult arSessionUpdate(ARSession *session, ARFrame *out_frame) {
    return impl_arSession_update(session, out_frame);
}

void arSessionSetCameraTextureName(ARSession *session, uint32_t texture_id) {
    impl_arSession_setCameraTextureName(session, texture_id);
}

void arSessionSetDisplayGeometry(ARSession *session, int32_t rotation, int32_t width, int32_t height) {
    impl_arSession_setDisplayGeometry(session, rotation, width, height);
}

ARResult arSessionAcquireWorldMap(const ARSession *session, ARWorldMap **out_map) {
    return impl_arSession_acquireMap(session, out_map);
}

void arSessionGetVerticalFov(const ARSession *session, float *vertical_fov) {
    impl_arSession_getVerticalFov(session, vertical_fov);
}

void arSessionGetTrackingState(const ARSession *session, ARTrackingState *out_tracking_state) {
    impl_arSession_getTrackingState(session, out_tracking_state);
}

void arSessionGetVersion(const ARSession *session, char *version, int versionSize) {
    impl_arSession_getVersion(session, version, versionSize);
}

void arSessionSetKeyAndSecret(const ARSession *session, const char *app_key, const char *app_secret) {
    impl_arSession_SetKeyAndSecret(session, app_key, app_secret);
}

ARResult arSessionHostAnchor(const ARSession *session, const ARAnchor *anchor, ARAnchor **out_cloud_anchor) {
    return impl_arSession_hostCloudAnchor(session, anchor, out_cloud_anchor);
}

ARResult arSessionResolveAnchor(const ARSession *session, const char *cloud_anchor_id,
                              ARAnchor **out_cloud_anchor) {
    return impl_arSession_resolveCloudAnchor(session, cloud_anchor_id, out_cloud_anchor);
}

void arSessionGetBackgroundData(const ARSession *session, char **image_data, int *width, int *height) {
    impl_arSession_getBackgroundData(session, image_data, width, height);
}

void
arSessionSetIntValue(const ARSession *session, ARParameterEnum value_type, int32_t *value, int size) {
    impl_arSession_setIntValue(session, value_type, value, size);
}

void
arSessionSetFloatValue(const ARSession *session, ARParameterEnum value_type, float *value, int size) {
    impl_arSession_setFloatValue(session, value_type, value, size);
}

void
arSessionSetStringValue(const ARSession *session, ARParameterEnum value_type, char *value, int size) {
    impl_arSession_setStringValue(session, value_type, value, size);
}

void
arSessionGetIntValue(const ARSession *session, ARParameterEnum value_type, int32_t *value, int size) {
    impl_arSession_getIntValue(session, value_type, value, size);
}

void
arSessionGetFloatValue(const ARSession *session, ARParameterEnum value_type, float *value, int size) {
    impl_arSession_getFloatValue(session, value_type, value, size);
}

void
arSessionGetStringValue(const ARSession *session, ARParameterEnum value_type, char *value, int *length,
                      int size) {
    impl_arSession_getStringValue(session, value_type, value, length, size);
}

void arSessionGetConfig(ARSession *session, ARConfig *out_config) {
    impl_arSession_getConfig(session, out_config);
}

void arSessionGetSupportedCameraConfigs(const ARSession *session, ARCameraConfigList *list) {
    impl_arSession_getSupportedCameraConfigs(session, list);
}

ARResult arSessionSetCameraConfig(const ARSession *session, const ARCameraConfig *camera_config) {
    return impl_arSession_setCameraConfig(session, camera_config);
}

void arSessionGetCameraConfig(const ARSession *session, ARCameraConfig *out_camera_config) {
    impl_arSession_getCameraConfig(session, out_camera_config);
}

void arSessionSetBoundingBox(const ARSession *session, const float* vertex, const float* matrix) {
    impl_arSession_setBoundingBox(session, vertex, matrix);
}

void arSessionGetBoundingBoxScanningResult(const ARSession *session, uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size) {
    impl_arSession_getBoundingBoxScanningResult(session, out_raw_bytes, out_raw_bytes_size);
}

void arSessionGetBoundingBoxPointCloud(const ARSession *session, float** points, int* points_num) {
    impl_arSession_getBoundingBoxPointCloud(session, points, points_num);
}

// --------------------------------------------------------------------------------------------
void arCameraConfigListCreate(const ARSession *session, ARCameraConfigList **out_list) {
    impl_arCameraConfigList_create(session, out_list);
}

void arCameraConfigListDestroy(ARCameraConfigList *list) {
    impl_arCameraConfigList_destroy(list);
}

void
arCameraConfigListGetSize(const ARSession *session, const ARCameraConfigList *list, int32_t *out_size) {
    impl_arCameraConfigList_getSize(session, list, out_size);
}

void arCameraConfigListGetItem(const ARSession *session, const ARCameraConfigList *list, int32_t index,
                               ARCameraConfig *out_camera_config) {
    impl_arCameraConfigList_getItem(session, list, index, &out_camera_config);
}

// --------------------------------------------------------------------------------------------
void arCameraConfigCreate(const ARSession *session, ARCameraConfig **out_camera_config) {
    impl_arCameraConfig_create(session, out_camera_config);
}

void arCameraConfigDestroy(ARCameraConfig *camera_config) {
    impl_arCameraConfig_destroy(camera_config);
}

void arCameraConfigGetImageDimensions(const ARSession *session, const ARCameraConfig *camera_config,
                                      int32_t *out_width, int32_t *out_height) {
    impl_arCameraConfig_getImageDimensions(session, camera_config, out_width, out_height);
}

void arCameraConfigGetTextureDimensions(const ARSession *session, const ARCameraConfig *camera_config,
                                        int32_t *out_width, int32_t *out_height) {
    impl_arCameraConfig_getTextureDimensions(session, camera_config, out_width, out_height);
}

// --------------------------------------------------------------------------------------------
ARResult arWorldMapAcquireNewAnchor(ARSession *session, const ARWorldMap *ar_map, const ARPose *pose,
                                    ARAnchor **out_anchor) {
    return impl_arSession_acquireNewAnchor(session, pose, out_anchor);
}

void arWorldMapGetAllAnchors(const ARSession *session, const ARWorldMap *ar_map, ARAnchorList *out_anchor_list) {
    if (!IsARSessionAvalid(session) || !IsARMapAvalid(ar_map) || !IsARAnchorlistAvalid(out_anchor_list))
        return;

    impl_arSession_getAllAnchors(session, out_anchor_list);
}

void arWorldMapGetAllNodes(const ARSession *session, const ARWorldMap *ar_map, ARNodeType filter_type,
                           ARNodeList *out_node_list) {
    if (!IsARSessionAvalid(session) || !IsARMapAvalid(ar_map) || !IsARNodeListAvalid(out_node_list))
        return;

    impl_arSession_getAllNodes(session, filter_type, out_node_list);
}

void arMapGetExtent(const ARSession *session, const ARWorldMap *ar_map, float *out_extent_xyz) {}
void arMapGetCenter(const ARSession *session, const ARWorldMap *ar_map, float *out_center_xyz) {}


void
arWorldMapGetAllPlanesVertexCount(const ARSession *session, const ARWorldMap *ar_map, int32_t *out_vertex_count) {
    if (IsARSessionAvalid(session) == false || IsARMapAvalid(ar_map) == false)
        return;

    impl_arSession_getPlaneVertexCount(session, out_vertex_count);
}

void
arWorldMapGetAllPlanesVertexArray(const ARSession *session, const ARWorldMap *ar_map, float *out_vertex_array) {
    if (IsARSessionAvalid(session) == false || IsARMapAvalid(ar_map) == false)
        return;

    impl_arSession_getPlaneVertexArray(session, out_vertex_array);
}

void
arWorldMapGetAllPlanesIndexCount(const ARSession *session, const ARWorldMap *ar_map, int32_t *out_index_count) {
    if (IsARSessionAvalid(session) == false || IsARMapAvalid(ar_map) == false)
        return;

    impl_arSession_getPlaneIndexCount(session, out_index_count);
}

void arWorldMapGetAllPlanesIndexArray(const ARSession *session, const ARWorldMap *ar_map,
                                      unsigned short *out_index_array) {
    if (IsARSessionAvalid(session) == false || IsARMapAvalid(ar_map) == false)
        return;

    impl_arSession_getPlaneIndexArray(session, out_index_array);
}

//
//ARResult arMapAcquirePointCloud(const ARSession *session, const ARWorldMap *ar_map, ARPointCloud **out_point_cloud);
//ARResult arMapAcquireSurfelCloud(const ARSession *session, const ARWorldMap *ar_map, ARSurfelCloud** out_surfel_cloud);
//ARResult arMapAcquireSurfaceMesh(const ARSession* session, const ARWorldMap *ar_map, ARSurfaceMesh** out_surface_mesh);
void arWorldMapRelease(ARWorldMap *ar_map) {
    if (ar_map)
        free(ar_map);
}

/// ------ ARImageNode api ------

void arImageNodeGetCenterPose(const ARSession *session,
                              const ARImageNode *image_node,
                              ARPose *out_pose) {
    impl_arAugmentedImage_getCenterPose(session, image_node, out_pose);
}

void arImageNodeGetExtentX(const ARSession *session,
                           const ARImageNode *image_node,
                           float *out_extent_x) {
    impl_arAugmentedImage_getExtentX(session, image_node, out_extent_x);
}

void arImageNodeGetExtentZ(const ARSession *session,
                           const ARImageNode *image_node,
                           float *out_extent_z) {
    impl_arAugmentedImage_getExtentZ(session, image_node, out_extent_z);
}

void arImageNodeGetIndex(const ARSession *session,
                         const ARImageNode *image_node,
                         int32_t *out_index) {
    impl_arAugmentedImage_getIndex(session, image_node, out_index);
}

void arImageNodeGetName(const ARSession *session,
                        const ARImageNode *image_node,
                        char **out_augmented_image_name) {
    impl_arAugmentedImage_acquireName(session, image_node, out_augmented_image_name);
}

void arImageNodeGetGlobalID(const ARSession *session, const ARImageNode *image_node, char **out_global_id) {
    impl_arAugmentedImage_getGlobalID(session, image_node, out_global_id);
}

/// ------ ARReferenceImageDatabase api ------

void arReferenceImageDatabaseCreate(const ARSession *session, ARReferenceImageDatabase **out_image_database)
{
    impl_arAugmentedImageDatabase_create(session, out_image_database);
}

ARResult arReferenceImageDatabaseDeserialize(const ARSession *session,
        ARReferenceImageDatabase *image_database,
        const uint8_t *database_raw_bytes,
        int64_t database_raw_bytes_size)
{
    return impl_arAugmentedImageDatabase_deserialize(session, image_database, database_raw_bytes, database_raw_bytes_size);
}

void arReferenceImageDatabaseSerialize(const ARSession *session,
        const ARReferenceImageDatabase *image_database,
        uint8_t **out_image_database_raw_bytes,
        int64_t *out_image_database_raw_bytes_size)
{
    impl_arAugmentedImageDatabase_serialize(session, image_database, out_image_database_raw_bytes, out_image_database_raw_bytes_size);
}

void arReferenceImageDatabaseLoadConfigure(const ARSession *session, ARReferenceImageDatabase *image_database,
                                           const char* config)
{
    impl_arAugmentedImageDatabase_loadConfigure(session, image_database, config);
}

ARResult arReferenceImageDatabaseAddPatt(const ARSession *session, ARReferenceImageDatabase *image_database, const char* patt_name,
                                         const uint8_t *patt_raw_bytes, int64_t patt_raw_bytes_size)
{
    return impl_arAugmentedImageDatabase_addPatt(session, image_database, patt_name, patt_raw_bytes, patt_raw_bytes_size);
}

ARResult arReferenceImageDatabaseAddImage(const ARSession *session,
        ARReferenceImageDatabase *image_database,
        const char *image_name,
        const uint8_t *image_grayscale_pixels,
        int32_t image_width_in_pixels,
        int32_t image_height_in_pixels,
        int32_t image_stride_in_pixels,
        int32_t *out_index)
{
    return impl_arAugmentedImageDatabase_addImageWithPhysicalSize(session, image_database,
                                                                image_name, image_grayscale_pixels,
                                                                image_width_in_pixels,
                                                                image_height_in_pixels,
                                                                image_stride_in_pixels,
                                                                0.2,
                                                                out_index);
}

ARResult arReferenceImageDatabaseAddImageWithPhysicalSize(const ARSession *session, ARReferenceImageDatabase *image_database,
                                                          const char *image_name, const uint8_t *image_grayscale_pixels,
                                                          int32_t image_width_in_pixels, int32_t image_height_in_pixels,
                                                          int32_t image_stride_in_pixels, float image_width_in_meters,
                                                          int32_t *out_index)
{

    return impl_arAugmentedImageDatabase_addImageWithPhysicalSize(session, image_database,
                                                  image_name, image_grayscale_pixels,
                                                  image_width_in_pixels,
                                                  image_height_in_pixels,
                                                  image_stride_in_pixels,
                                                  image_width_in_meters,
                                                  out_index);
}

void arReferenceImageDatabaseGetNumImages(const ARSession *session, const ARReferenceImageDatabase *image_database,
                                          int32_t *out_num_images)
{
    impl_arAugmentedImageDatabase_getNumImages(session, image_database, out_num_images);
}

void arReferenceImageDatabaseDestroy(ARReferenceImageDatabase *image_database)
{
    impl_arAugmentedImageDatabase_destroy(image_database);
}

// --------------------------------------------------------------------------------------------
///// ------ ARSurfelCloud api ------
//void arSurfelCloudGetUpdateState( const ARSession* session, const ARSurfelCloud *surfel_cloud, bool *update_state);
//void arSurfelCloudGetFormat(const ARSession* session, const ARSurfelCloud *surfel_cloud, ARVertexFormat *surfel_format);
//void arSurfelCloudGetCount(const ARSession* session, const ARSurfelCloud *surfel_cloud, int32_t *surfel_count);
//void arSurfelCloudGetData(const ARSession* session, const ARSurfelCloud *surfel_cloud, float **surfel_data);
//void arSurfelCloudRelease(ARSurfelCloud* surfelcloud);
//
//
///// ------ ARDenseMesh api ------
void arDenseMeshGetFormat(const ARSession *session, const ARDenseMesh *mesh,
                          ARVertexFormat *out_vertex_format) {
    if (IsARSessionAvalid(session) == false)
        return;

    impl_arDenseMesh_getFormat(session, mesh, out_vertex_format);
}

void arDenseMeshGetVertexCount(const ARSession *session, const ARDenseMesh *mesh,
                               int32_t *out_vertex_count) {
    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(mesh) == false)
        return;

    impl_arDenseMesh_getVertexCount(session, mesh, out_vertex_count);
}
void
arDenseMeshGetIndexCount(const ARSession *session, const ARDenseMesh *mesh, int32_t *out_index_count) {
    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(mesh) == false)
        return;

    impl_arDenseMesh_getIndexCount(session, mesh, out_index_count);
}

void arDenseMeshGetVertex(const ARSession *session, const ARDenseMesh *mesh, float *out_vertex_array) {
    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(mesh) == false)
        return;

    impl_arDenseMesh_getVertex(session, mesh, out_vertex_array);
}

void arDenseMeshGetNormal(const ARSession *session, const ARDenseMesh *mesh, float *out_vertex_normal) {
    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(mesh) == false)
        return;

    impl_arDenseMesh_getNormal(session, mesh, out_vertex_normal);
}

void arDenseMeshGetIndex(const ARSession *session, const ARDenseMesh *mesh,
                         unsigned short *out_index_array) {
    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(mesh) == false)
        return;

    impl_arDenseMesh_getIndex(session, mesh, out_index_array);
}

void arDenseMeshRelease(ARDenseMesh *mesh) {
    impl_arDenseMesh_release(mesh);
}

// --------------------------------------------------------------------------------------------
void arFrameCreate(const ARSession *session, ARFrame **out_frame) {
    impl_arFrame_create(session, out_frame);
}

void arFrameDestroy(ARFrame *frame) {
    impl_arFrame_destroy(frame);
}

void arFrameGetDisplayGeometryChanged(const ARSession *session, const ARFrame *frame,
                                      int32_t *out_geometry_changed) {
    impl_arFrame_getDisplayGeometryChanged(session, frame, out_geometry_changed);
}

void arFrameTransformDisplayUvCoords(const ARSession *session, const ARFrame *frame,
                                     int32_t num_elements, const float *uvs_in,
                                     float *uvs_out) {
    impl_arFrame_transformDisplayUvCoords(session, frame, num_elements, uvs_in, uvs_out);
}

void arFrameSetPointQueryMode(const ARSession *session, const ARFrame *frame, ARQueryMode mode) {
    impl_arFrame_setHitTestMode(session, frame, mode);
}

void arFramePointQuery(const ARSession *session, const ARFrame *frame, float pixel_x, float pixel_y,
                       ARQueryResultList *query_result_list) {
    impl_arFrame_hitTest(session, frame, pixel_x, pixel_y, query_result_list);
}

void arFrameRayQuery(const ARSession *session, const ARFrame *frame, const float *ray_origin_3,
                     const float *ray_direction_3, ARQueryResultList *hit_result_list) {
    impl_arFrame_hitTestRay(session, frame, ray_origin_3, ray_direction_3, hit_result_list);
}

void arFrameGetIlluminationEstimate(const ARSession *session, const ARFrame *frame,
                                    ARIlluminationEstimate *out_light_estimate) {
    impl_arFrame_getIlluminationEstimate(session, frame, out_light_estimate);
}

ARResult arFrameAcquirePointCloud(const ARSession *session, const ARFrame *frame,
                                  ARPointCloud **out_point_cloud) {
    return impl_arFrame_acquirePointCloud(session, frame, out_point_cloud);
}

ARResult arFrameAcquireDenseMesh(const ARSession *session, const ARFrame *frame, ARDenseMesh **out_dense_mesh) {
    return impl_arFrame_acquireDenseMesh(session, frame, out_dense_mesh);
}

void arFrameAcquireCamera(const ARSession *session, const ARFrame *frame, ARCamera **out_camera) {
    impl_arFrame_acquireCamera(session, frame, out_camera);
}

void arFrameGetUpdatedAnchors(const ARSession *session, const ARFrame *frame,
                              ARAnchorList *out_anchor_list) {
    impl_arFrame_getUpdatedAnchors(session, frame, out_anchor_list);
}
void arFrameGetUpdatedNodes(const ARSession *session, const ARFrame *frame,
                            ARNodeType filter_type, ARNodeList *out_node_list) {
    impl_arFrame_getUpdatedNodes(session, frame, filter_type, out_node_list);
}

void arFrameIsImageDataUpdated(const ARSession* session, const ARFrame* frame, int* data_updated){
    if(data_updated == nullptr)
        return;
    (*data_updated) = impl_arFrame_isYUVImageDataUpdated(session, frame);
}

void arFrameGetImageFormat(const ARSession *session, const ARFrame *frame,
                           ARPixelFormat *video_image_format) {
    impl_arFrame_getImageFormat(session, frame, video_image_format);
}

ARResult arFrameGetImageResolution(const ARSession *session, const ARFrame *frame, int *outwidth,
                                   int *outheight) {
    return impl_arFrame_getPreviewSize(session, frame, outwidth, outheight);
}

ARResult arFrameGetImageData(const ARSession *session, const ARFrame *frame, unsigned char *out_data1,
                             unsigned char *out_data2, unsigned char *out_data3) {
    return impl_arFrame_getYUVImageData(session, frame, out_data1, out_data2, out_data3);
}

ARResult arFrameIsDepthImageAvaliable(const ARSession *session, const ARFrame *frame) {
    return impl_arFrame_isDepthImageAvaliable(session, frame);
}

ARResult arFrameGetDepthImageResolution(const ARSession *session, const ARFrame *frame, int *out_width,
                                        int *out_height) {
    return impl_arFrame_getDepthImageResolution(session, frame, out_width, out_height);
}

ARResult arFrameGetDepthImageData(const ARSession *session, const ARFrame *frame,
                                  unsigned char *out_depth_image_data) {
    return impl_arFrame_getDepthImageData(session, frame, out_depth_image_data);
}

void
arFrameGetDepthImageFormat(const ARSession *session, const ARFrame *frame, ARPixelFormat *out_format) {
    impl_arFrame_getDepthImageFormat(session, frame, out_format);
}

void arFrameGetTimestamp(const ARSession *session, const ARFrame *frame, int64_t *out_timestamp_ns) {
    impl_arFrame_getTimestamp(session, frame, out_timestamp_ns);
}


// --------------------------------------------------------------------------------------------
void arCameraGetPose(const ARSession *session, const ARCamera *camera, ARPose *out_pose) {
    impl_arCamera_getPose(session, camera, out_pose);
}
void arCameraGetViewMatrix(const ARSession *session, const ARCamera *camera,
                           float *out_col_major_4x4) {
    impl_arCamera_getViewMatrix(session, camera, out_col_major_4x4);
}

void arCameraGetPredictedViewMatrix(const ARSession *session, const ARCamera *camera,
                                    float *out_col_major_4x4) {
    impl_arCamera_getPredictedViewMatrix(session, camera, out_col_major_4x4);
}

void arCameraGetTrackingState(const ARSession *session, const ARCamera *camera,
                              ARTrackingState *out_tracking_state) {
    impl_arCamera_getTrackingState(session, camera, out_tracking_state);
}

void
arCameraGetProjectionMatrix(const ARSession *session, const ARCamera *camera, float near, float far,
                            float *dest_col_major_4x4) {
    impl_arCamera_getProjectionMatrix(session, camera, near, far, dest_col_major_4x4);
}

void arCameraGetProjectionMatrixSeeThrough(const ARSession *session, const ARCamera *camera,
                                           float near, float far, float *dest_col_major_4x4) {
    impl_arCamera_getProjectionMatrixSeeThrough(session, camera, near, far, dest_col_major_4x4);
}

void arCameraRelease(ARCamera *camera) {
    impl_arCamera_release(camera);
}

void
arCameraGetDisplayOrientedPose(const ARSession *session, const ARCamera *camera, ARPose *out_pose) {
    impl_arCamera_getDisplayOrientedPose(session, camera, out_pose);
}

void arCameraGetImageIntrinsics(const ARSession *session, const ARCamera *camera,
                                ARCameraIntrinsics *out_camera_intrinsics) {
    impl_arCamera_getImageIntrinsics(session, camera, out_camera_intrinsics);
}

void arCameraGetTextureIntrinsics(const ARSession *session, const ARCamera *camera,
                                  ARCameraIntrinsics *out_camera_intrinsics) {
    impl_arCamera_getTextureIntrinsics(session, camera, out_camera_intrinsics);
}

// --------------------------------------------------------------------------------------------
void arCameraIntrinsicsCreate(const ARSession *session, ARCameraIntrinsics **out_camera_intrinsics) {
    impl_arCameraIntrinsics_create(session, out_camera_intrinsics);
}

void arCameraIntrinsicsGetFocalLength(const ARSession *session, const ARCameraIntrinsics *intrinsics,
                                      float *out_fx, float *out_fy) {
    impl_arCameraIntrinsics_getFocalLength(session, intrinsics, out_fx, out_fy);
}

void arCameraIntrinsicsGetPrincipalPoint(const ARSession *session, const ARCameraIntrinsics *intrinsics,
                                         float *out_cx, float *out_cy) {
    impl_arCameraIntrinsics_getPrincipalPoint(session, intrinsics, out_cx, out_cy);
}

void
arCameraIntrinsicsGetImageDimensions(const ARSession *session, const ARCameraIntrinsics *intrinsics,
                                     int32_t *out_width, int32_t *out_height) {
    impl_arCameraIntrinsics_getImageDimensions(session, intrinsics, out_width, out_height);
}

void arCameraIntrinsicsDestroy(ARCameraIntrinsics *camera_intrinsics) {
    impl_arCameraIntrinsics_destroy(camera_intrinsics);
}

// --------------------------------------------------------------------------------------------
void arPointCloudGetNumberOfPoints(const ARSession *session, const ARPointCloud *point_cloud,
                                   int32_t *out_number_of_points) {
    impl_arPointCloud_getNumberOfPoints(session, point_cloud, out_number_of_points);
}
void arPointCloudGetData(const ARSession *session, const ARPointCloud *point_cloud,
                         const float **out_point_cloud_data) {
    impl_arPointCloud_getData(session, point_cloud, out_point_cloud_data);
}
void arPointCloudRelease(ARPointCloud *point_cloud) {
    impl_arPointCloud_release(point_cloud);
}

// --------------------------------------------------------------------------------------------
void arIlluminationEstimateCreate(const ARSession *session,
                                  ARIlluminationEstimate **out_illumination_estimate) {
    impl_arIlluminationEstimate_create(session, out_illumination_estimate);
}
void arIlluminationEstimateDestroy(ARIlluminationEstimate *illumination_estimate) {
    impl_arIlluminationEstimate_destroy(illumination_estimate);
}
void arIlluminationEstimateGetState(const ARSession *session,
                                    const ARIlluminationEstimate *illumination_estimate,
                                    ARIlluminationEstimateState *out_illumination_estimate_state) {
    impl_arIlluminationEstimate_getState(session, illumination_estimate,
                                         out_illumination_estimate_state);
}

void arIlluminationEstimateGetMode(const ARSession *session,
                                   const ARIlluminationEstimate *illumination_estimate,
                                   ARIlluminationEstimateMode *out_illumination_estimate_mode) {
    impl_arIlluminationEstimate_getMode(session, illumination_estimate,
                                        out_illumination_estimate_mode);
}

void arIlluminationEstimateGetPixelIntensity(const ARSession *session,
                                             const ARIlluminationEstimate *illumination_estimate,
                                             float *out_pixel_intensity) {
    impl_arIlluminationEstimate_getPixelIntensity(session, illumination_estimate,
                                                  out_pixel_intensity);
}

void arIlluminationEstimateGetColorCorrection(const ARSession *session,
                                              const ARIlluminationEstimate *illumination_estimate,
                                              float *out_color_correction_4) {
    impl_arIlluminationEstimate_getColorCorrection(session, illumination_estimate,
                                                   out_color_correction_4);
}

void arIlluminationEstimateGetTimestamp(const ARSession *session,
                                        const ARIlluminationEstimate *illumination_estimate,
                                        int64_t *out_timestamp_ns)
{
    impl_arIlluminationEstimate_getLightTimestamp(session,illumination_estimate,out_timestamp_ns);
}

void arIlluminationEstimateGetEnvironmentalHdrMainLightDirection(const ARSession *session,
                                                                 const ARIlluminationEstimate *illumination_estimate,
                                                                 float *out_direction_3)
{
    impl_arIlluminationEstimate_getEnvironmentalHdrMainLightDirection(session,illumination_estimate,out_direction_3);
}

void arIlluminationEstimateGetEnvironmentalHdrMainLightIntensity(const ARSession *session,
                                                                 const ARIlluminationEstimate *illumination_estimate,
                                                                 float *out_intensity_3)
{
    impl_arIlluminationEstimate_getEnvironmentalHdrMainLightIntensity(session,illumination_estimate,out_intensity_3);
}

void arIlluminationEstimateGetEnvironmentalHdrAmbientSphericalHarmonics(const ARSession *session,
                                                                        const ARIlluminationEstimate *illumination_estimate,
                                                                        float *out_coefficients_27)
{
    impl_arIlluminationEstimate_getSphericalHarmonicLighting(session,illumination_estimate,out_coefficients_27);
}

void arIlluminationEstimateAcquireEnvironmentalHdrCubemap(const ARSession *session,
                                                          const ARIlluminationEstimate *illumination_estimate,
                                                          float* out_textures_6)
{
    impl_arIlluminationEstimate_acquireEnvironmentalHdrCubemap(session,illumination_estimate,out_textures_6);
}

void arIlluminationEstimateGetEnvironmentTextureSize(const ARSession * session,
                                                     const ARIlluminationEstimate * illumination_estimate,
                                                     int * width,int * height)
{
    impl_arIlluminationEstimate_getEnvironmentTextureSize(session,illumination_estimate,width,height);
}

// --------------------------------------------------------------------------------------------
void arAnchorListCreate(const ARSession *session, ARAnchorList **out_anchor_list) {
    impl_arAnchorList_create(session, out_anchor_list);
}
void arAnchorListDestroy(ARAnchorList *anchor_list) {
    impl_arAnchorList_destroy(anchor_list);
}

void arAnchorListGetSize(const ARSession *session, const ARAnchorList *anchor_list, int32_t *out_size) {
    impl_arAnchorList_getSize(session, anchor_list, out_size);
}

void arAnchorListAcquireItem(const ARSession *session, const ARAnchorList *anchor_list, int32_t index,
                             ARAnchor **out_anchor) {
    impl_arAnchorList_acquireItem(session, anchor_list, index, out_anchor);
}

// --------------------------------------------------------------------------------------------
void arAnchorGetPose(const ARSession *session, const ARAnchor *anchor, ARPose *out_pose) {
    impl_arAnchor_getPose(session, anchor, out_pose);
}

void arAnchorGetTrackingState(const ARSession *session, const ARAnchor *anchor,
                              ARTrackingState *out_tracking_state) {
    impl_arAnchor_getTrackingState(session, anchor, out_tracking_state);
}

void arAnchorDetach(const ARSession *session, ARAnchor *anchor) {
    impl_arAnchor_detach(session, anchor);
}

void arAnchorRelease(ARAnchor *anchor) {
    impl_arAnchor_release(anchor);
}

void
arAnchorGetCloudAnchorId(ARSession *session, ARAnchor *anchor, int idSize, char *out_cloud_anchor_id) {
    impl_arAnchor_getCloudAnchorId(session, anchor, out_cloud_anchor_id, idSize);
}

void
arAnchorGetCloudState(const ARSession *session, const ARAnchor *anchor, ARCloudAnchorState *out_state) {
    impl_arAnchor_getCloudAnchorState(session, anchor, out_state);
}

// --------------------------------------------------------------------------------------------
void arNodeListCreate(const ARSession *session, ARNodeList **out_node_list) {
    impl_arNodeList_create(session, out_node_list);
}

void arNodeListDestroy(ARNodeList *node_list) {
    impl_arNodeList_destroy(node_list);
}

void arNodeListGetSize(const ARSession *session, const ARNodeList *node_list, int32_t *out_size) {
    impl_arNodeList_getSize(session, node_list, out_size);
}

void arNodeListAcquireItem(const ARSession *session, const ARNodeList *node_list, int32_t index,
                           ARNode **out_node) {
    impl_arNodeList_acquireItem(session, node_list, index, out_node);
}

void arNodeRelease(ARNode *node) {
    impl_arNode_release(node);
}

void arNodeGetType(const ARSession *session, const ARNode *node, ARNodeType *out_node_type) {
    impl_arNode_getType(session, node, out_node_type);
}

void arNodeGetTrackingState(const ARSession *session, const ARNode *node,
                            ARTrackingState *out_tracking_state) {
    impl_arNode_getTrackingState(session, node, out_tracking_state);
}

//void arNodeGetUUID(const ARSession *session, const ARNode *node, ARUUID *uuid) {}

ARResult arNodeAcquireNewAnchor(ARSession *session, ARNode *node, ARPose *pose, ARAnchor **out_anchor) {
    return impl_arNode_acquireNewAnchor(session, node, pose, out_anchor);
}

void arNodeGetAnchors(const ARSession *session, const ARNode *node, ARAnchorList *out_anchor_list) {
    impl_arNode_getAnchors(session, node, out_anchor_list);
}


// --------------------------------------------------------------------------------------------
void arPlaneNodeGetNormal(const ARSession *session, const ARPlaneNode *plane, float *out_plane_normal) {
    impl_arPlaneNode_getNormal(session, plane, out_plane_normal);
}

void arPlaneNodeGetOrientationType(const ARSession *session, const ARPlaneNode *plane,
                                   ARPlaneType *out_orientation_type) {
    impl_arPlaneNode_getType(session, plane, out_orientation_type);
}
void arPlaneNodeGetCenterPose(const ARSession *session, const ARPlaneNode *plane, ARPose *out_pose) {
    impl_arPlaneNode_getCenterPose(session, plane, out_pose);
}

void arPlaneNodeGetExtent(const ARSession *session, const ARPlaneNode *plane, float *out_extent_xyz) {
    if (out_extent_xyz == nullptr)
        return;

    if (!IsARSessionAvalid(session) || !IsARPlaneNodeAvalid(plane))
        return;

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();

        *out_extent_xyz = pPlane->getExtentX(slamAlgorithmResult);
//        *out_extent_xyz = session->m_pImpl->getExtentX(pPlane);
        *(out_extent_xyz + 1) = pPlane->getExtentY(slamAlgorithmResult);
//        *(out_extent_xyz + 1) = session->m_pImpl->getExtentY(pPlane);
        *(out_extent_xyz + 2) = pPlane->getExtentZ(slamAlgorithmResult);
//        *(out_extent_xyz + 2) = session->m_pImpl->getExtentZ(pPlane);

    }
}

void arPlaneNodeGetPolygon3DSize(const ARSession *session, const ARPlaneNode *plane,
                                 int32_t *out_polygon_size) {
    impl_arPlaneNode_getPolygon3DSize(session, plane, out_polygon_size);
}
void
arPlaneNodeGetPolygon3D(const ARSession *session, const ARPlaneNode *plane, float *out_polygon_xyz) {
    impl_arPlaneNode_getPolygon3D(session, plane, out_polygon_xyz);
}
void arPlaneNodeIsPoseInExtent(const ARSession *session, const ARPlaneNode *plane, const ARPose *pose,
                               int32_t *out_pose_in_extents) {
    impl_arPlaneNode_isPoseInExtents(session, plane, pose, out_pose_in_extents);
}
void arPlaneNodeIsPoseInPolygon(const ARSession *session, const ARPlaneNode *plane, const ARPose *pose,
                                int32_t *out_pose_in_polygon) {
    impl_arPlaneNode_isPoseInPolygon(session, plane, pose, out_pose_in_polygon);
}

void arPlaneNodeAcquireSubsumedBy(const ARSession_ *session, const ARPlaneNode_ *plane,
                                  ARPlaneNode_ **out_subsumed_by) {
    impl_arPlaneNode_acquireSubsumedBy(session, plane, out_subsumed_by);
}

// --------------------------------------------------------------------------------------------
//
//
///// ------ ARImageNode api ------
//void arImageNodeGetCenterPose(const ARSession *session, const ARImageNode *imagenode, ARPose *out_pose);
//void arImageNodeGetExtentX(const ARSession *session, const ARImageNode *imagenode, float *out_extent_x);
//void arImageNodeGetExtentZ(const ARSession *session, const ARImageNode *imagenode, float *out_extent_z);
//void arImageNodeGetIndex(const ARSession *session, const ARImageNode *imagenode, int32_t *out_index);
//void arImageNodeGetImageFeature(const ARSession *session, const ARImageNode *imagenode, ARImageFeature *out_image_feature);
//
//
///// ------ ARObjectNode api ------
//void arObjectNodeGetCenterPose(const ARSession *session, const ARObjectNode *objectnode, ARPose *out_pose);
//void arObjectNodeGetExtent(const ARSession *session, const ARObjectNode *objectnode, float *out_extent_xyz);
//void arObjectNodeGetObjectFeature(const ARSession *session, const ARObjectNode *objectnode, ARObjectFeature *out_object_feature);
//
//
///// ------ AREnvironmentProbeNode api ------
//void arEnvironmentProbeNodeGetPose(const ARSession *session, const AREnvironmentProbeNode *env_probe, ARPose *out_pose);
//void arEnvironmentProbeNodeGetExtent(const ARSession *session, const AREnvironmentProbeNode *env_probe, float *out_extent);
//void arEnvironmentProbeNodeSetExtent(const ARSession *session, const AREnvironmentProbeNode *env_probe, float extent[3]);
//void arEnvironmentProbeNodeGetTexture(const ARSession *session, const AREnvironmentProbeNode *env_probe, int32_t *out_texture);
//void arEnvironmentProbeNodeSetTexture(const ARSession *session, const AREnvironmentProbeNode *env_probe, int32_t texture);

// --------------------------------------------------------------------------------------------
void arQueryResultListCreate(const ARSession *session, ARQueryResultList **out_query_result_list) {
    impl_arQueryResultList_create(session, out_query_result_list);
}

void arQueryResultListDestroy(ARQueryResultList *query_result_list) {
    impl_arQueryResultList_destroy(query_result_list);
}

void arQueryResultListGetSize(const ARSession *session, const ARQueryResultList *query_result_list,
                              int32_t *out_size) {
    impl_arQueryResultList_getSize(session, query_result_list, out_size);
}

void arQueryResultListGetItem(const ARSession *session, const ARQueryResultList *query_result_list,
                              int32_t index, ARQueryResult *out_query_result) {
    impl_arQueryResultList_getItem(session, query_result_list, index, out_query_result);
}

// --------------------------------------------------------------------------------------------
void arQueryResultCreate(const ARSession *session, ARQueryResult **out_query_result) {
    impl_arQueryResult_create(session, out_query_result);
}

void arQueryResultDestroy(ARQueryResult *query_result) {
    impl_arQueryResult_destroy(query_result);
}

void arQueryResultGetDistance(const ARSession *session, const ARQueryResult *query_result,
                              float *out_distance) {
    impl_arQueryResult_getDistance(session, query_result, out_distance);
}

void arQueryResultGetHitPose(const ARSession *session, const ARQueryResult *query_result,
                             ARPose *out_pose) {
    impl_arQueryResult_getHitPose(session, query_result, out_pose);
}

void arQueryResultAcquireNode(const ARSession *session, const ARQueryResult *query_result,
                              ARNode **out_node) {
    impl_arQueryResult_acquireNode(session, query_result, out_node);
}

ARResult
arQueryResultAcquireNewAnchor(ARSession *session, ARQueryResult *query_result, ARAnchor **out_anchor) {
    return impl_arQueryResult_acquireNewAnchor(session, query_result, out_anchor);
}


// --------------------------------------------------------------------------------------------

///// ------ ARUtility api ------
//ARResult arUtilityLoadMap(const char* url, ARWorldMap** out_map);
//ARResult arUtilitySaveMap(const ARWorldMap* ar_map, const char* url);
//void arUnitlityLoadImageFeature(const ARSession* session, const char* path, ARImageFeature** out_image_feature);
//void arUnitlitySaveImageFeature(const ARSession* session, const char* path, ARImageFeature* image_feature);
//void arUnitlityLoadObjectFeature(const ARSession* session, const char* path, ARObjectFeature** out_object_feature);
//void arUnitlitySaveObjectFeature(const ARSession* session, const char* path, ARObjectFeature* object_feature);
//void arUnitlityLoadMap(const ARSession* session, const char* path, ARWorldMap** out_ar_map);
//void arUnitlitySaveMap(const ARSession* session, const char* path, ARWorldMap* ar_map);



void arHandGestureNodeRelease(ARHandGestureNode *arHandGesture) {

}

void arHandGestureNodeGetHandGestureType(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                     ARHandGestureType *out_ar_handgesture_type) {
    impl_arHandGesture_getHandGestureType(session, arHandGesture, out_ar_handgesture_type);
}

void
arHandGestureNodeGetGestureTypeConfidence(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                      float *out_handType_confidence) {
    impl_arHandGesture_getHandTypeConfidence(session, arHandGesture, out_handType_confidence);
}


void arHandGestureNodeGetHandType(const ARSession *session, const ARHandGestureNode *arHandGesture,
                              ARHandType *out_ar_hand_side) {
    impl_arHandGesture_getHandSide(session, arHandGesture, out_ar_hand_side);
}

void arHandGestureNodeGetHandTowards(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                 ARHandTowards *out_ar_hand_towards) {
    impl_arHandGesture_getHandTowards(session, arHandGesture, out_ar_hand_towards);
}


void arHandGestureNodeGetPalmCenter(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                float *out_palm_center_xyz) {
    impl_arHandGesture_getPalmCenter(session, arHandGesture, out_palm_center_xyz);
}

void arHandGestureNodeGetPalmNormal(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                float *out_palm_normal_xyz) {
    impl_arHandGesture_getPalmNormal(session, arHandGesture, out_palm_normal_xyz);
}

void arHandGestureNodeGetLandMark2DCount(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                     int32_t *out_landmark2D_count) {
    impl_arHandGesture_getLandMark2DCount(session, arHandGesture, out_landmark2D_count);
}

void arHandGestureNodeGetLandMark2DArray(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                     float *out_landMark2DArray) {
    impl_arHandGesture_getLandMark2DArray(session, arHandGesture, out_landMark2DArray);
}

void
arHandGestureNodeGetRect(const ARSession *session, const ARHandGestureNode *arHandGesture, int *out_Rect) {
    impl_arHandGesture_getRect(session, arHandGesture, out_Rect);
}

void arHandGestureNodeGetLandMark3DCount(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                     int32_t *out_landmark3D_count) {
    impl_arHandGesture_getLandMark3DCount(session, arHandGesture, out_landmark3D_count);
}

void arHandGestureNodeGetLandMark3DArray(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                     float *out_landMark3DArray) {
    impl_arHandGesture_getLandMark3DArray(session, arHandGesture, out_landMark3DArray);
}

void arHandGestureNodeGetRGBSegArray(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                 char *out_RGBSegArray) {
    impl_arHandGesture_getRGBSegArray(session, arHandGesture, out_RGBSegArray);
}

void arHandGestureNodeGetRGBSegWidth(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                 int32_t *out_RGBSegWidth) {
    impl_arHandGesture_getRGBSegWidth(session, arHandGesture, out_RGBSegWidth);
}

void arHandGestureNodeGetRGBSegHeight(const ARSession *session, const ARHandGestureNode *arHandGesture,
                                  int32_t *out_RGBSegHeight) {
    impl_arHandGesture_getRGBSegHeight(session, arHandGesture, out_RGBSegHeight);
}


void arFaceMeshNodeGetCenterPose(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                             ARPose *out_pose) {
    impl_arFaceMesh_getCenterPose(session, arFaceMeshNode, out_pose);

}

void arFaceMeshNodeGetVertices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                           float **out_vertices, int32_t *out_number_of_vertices) {
    impl_arFaceMesh_getVertices(session, arFaceMeshNode, out_vertices, out_number_of_vertices);
}

void arFaceMeshNodeGetNormals(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode, float **out_normals, int32_t *out_number_of_normals){
    impl_arFaceMesh_getNormals(session, arFaceMeshNode, out_normals, out_number_of_normals);

}

void arFaceMeshNodeGetTextureCoordinates(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                     float **out_texture_coordinates,
                                     int32_t *out_number_of_texture_coordinates) {
    impl_arFaceMesh_getTextureCoordinates(session, arFaceMeshNode, out_texture_coordinates,out_number_of_texture_coordinates);
}
void arFaceMeshNodeGetTriangleIndices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                  int **out_triangle_indices, int32_t *out_number_of_triangles) {
    impl_arFaceMesh_getTriangleIndices(session, arFaceMeshNode, out_triangle_indices,out_number_of_triangles);
}

ARResult arSessionRenderBeginEye(ARSession* session, AREyeSide eye)
{
    return impl_arSession_beginRenderEye(session, (int)eye);
}

ARResult arSessionRenderEndEye(ARSession* session, AREyeSide eye)
{
    return impl_arSession_endRenderEye(session, (int)eye);
}

ARResult arSessionRenderSubmitFrame(ARSession* session, const int32_t leftEyeTextureId, const int32_t rightEyeTextureId)
{
    return impl_arSession_submitFrame(session, leftEyeTextureId, rightEyeTextureId);
}

ARResult arSessionRenderSetWindow(ARSession *session, void *window)
{
    return impl_arSession_setWindow(session, window);
}

void arSessionRenderBeginEyeUnity(ARSession* session, AREyeSide eye)
{
    LOGI("arSessionRenderBeginEye()");
    gBeginEyeId = eye;
}

void arSessionRenderEndEyeUnity(ARSession* session, AREyeSide eye)
{
    LOGI("arSessionRenderEndEye");
    gEndEyeId = eye;
}

void arSessionRenderSubmitFrameUnity(ARSession* session, const int32_t leftEyeTextureId, const int32_t rightEyeTextureId)
{
    LOGI("arSessionRenderSubmitFrame");
    gLeftEyeTexId = leftEyeTextureId, gRightEyeTexId = rightEyeTextureId;
}

void arObjecNodeGetBoundingBox(const ARSession *session, const ARObjectNode *object_node, float* boundingBox) {
    if(IsARSessionAvalid(session) == false|| IsARObjectNodeAvalid(object_node) == false)
        return;

    impl_arObjectNode_getBoundingBox(session, object_node, boundingBox);
}

void arObjectNodeGetCenterPose(const ARSession *session, const ARObjectNode *object_node, ARPose *out_pose) {
    impl_arObjectNode_getCenterPose(session, object_node, out_pose);
}

void arObjectNodeGetExtentX(const ARSession *session, const ARObjectNode *object_node, float *out_extent_x) {
    impl_arObjectNode_getExtentX(session, object_node, out_extent_x);

}

void arObjectNodeGetExtentY(const ARSession *session, const ARObjectNode *object_node, float *out_extent_y) {
    impl_arObjectNode_getExtentY(session, object_node, out_extent_y);
}

void arObjectNodeGetExtentZ(const ARSession *session, const ARObjectNode *object_node, float *out_extent_z) {
    impl_arObjectNode_getExtentZ(session, object_node, out_extent_z);
}

void arObjectNodeGetIndex(const ARSession *session, const ARObjectNode *object_node, int32_t *out_index) {
    impl_arObjectNode_getIndex(session, object_node, out_index);
}

void arObjectNodeGetName(const ARSession *session, const ARObjectNode *object_node, char **out_object_name) {
    impl_arObjectNode_getName(session, object_node, out_object_name);
}

void arReferenceObjectDatabaseCreate(const ARSession *session, ARReferenceObjectDatabase **out_object_database) {
    impl_arReferenceObjectDatabase_create(session, out_object_database);
}

ARResult arReferenceObjectDatabaseDeserialize(const ARSession *session, ARReferenceObjectDatabase *object_database,
                                              const uint8_t *database_raw_bytes, int64_t database_raw_bytes_size) {
    return impl_arReferenceObjectDatabase_deserialize(session, object_database, database_raw_bytes, database_raw_bytes_size);
}

ARResult arReferenceObjectDatabaseAddObject(const ARSession *session, ARReferenceObjectDatabase *object_database,
                                            const char *object_name, const uint8_t *object_raw_bytes, int64_t object_raw_bytes_size, int32_t *out_index) {
    return impl_arReferenceObjectDatabase_addObject(session, object_database, object_name, object_raw_bytes, object_raw_bytes_size, out_index);
}

void arReferenceObjectDatabaseGetNumObjects(const ARSession *session, const ARReferenceObjectDatabase *object_database, int32_t *out_num_objects) {
    impl_arReferenceObjectDatabase_getNumObjects(session, object_database, out_num_objects);
}

void arReferenceObjectDatabaseDestroy( ARReferenceObjectDatabase *object_database) {
    impl_arReferenceObjectDatabase_destroy(object_database);
}

void arBodyDetectNodeGetBodyId(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_body_id) {
    impl_arBodyDetect_getBodyId(session,arBodyDetectNode,out_body_id);
}

void arBodyDetectNodeGetSkeletonPoint2dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point_count){
    impl_arBodyDetect_getSkeletonPoint2dCount(session,arBodyDetectNode,out_point_count);
}

void arBodyDetectNodeGetSkeletonPoint2d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2D){
    impl_arBodyDetect_getSkeletonPoint2d(session,arBodyDetectNode,out_point2D);
}

void arBodyDetectNodeGetSkeletonPoint2dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point2d_confidence){
    impl_arBodyDetect_getSkeletonPoint2dConfidence(session,arBodyDetectNode,out_point2d_confidence);

}


void arBodyDetectNodeGetSkeletonPoint3dCount(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, int32_t *out_point3d_count){
    impl_arBodyDetect_getSkeletonPoint3dCount(session,arBodyDetectNode,out_point3d_count);
}

void arBodyDetectNodeGetSkeletonPoint3d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3D){
    impl_arBodyDetect_getSkeletonPoint3d(session,arBodyDetectNode,out_point3D);
}


void arBodyDetectNodeGetSkeletonPoint3dConfidence(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode, float *out_point3d_confidence){
    impl_arBodyDetect_getSkeletonPoint3dConfidence(session,arBodyDetectNode,out_point3d_confidence);
}
}
