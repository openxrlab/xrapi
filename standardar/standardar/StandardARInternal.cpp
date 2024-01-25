#include "StandardARInternal.h"


#include "ArAnchor.h"
#include "ArFrame.h"
#include "ArHitResult.h"
#include "ArPlane.h"
#include "ArSession.h"
#include "ArTrackable.h"
#include "ArImageNode.h"
#include "ArImageNodeMgr.h"
#include "ArHandGesture.h"

#include <jni.h>
#include <unistd.h>
#include <MarkerAlgorithmResult.h>
#include "ObjectTrackingAlgorithmResult.h"
#include "ArJniHelpler.h"
#include "ArCamera.h"
#include "ArFaceMesh.h"
#include "ArBodyDetctNode.h"
#include "ArWorldLifeCycle.h"
#include "DynamicLoader.h"

#include <android/native_window_jni.h>

using namespace standardar;

static jobject gAppContext;


jclass eClassActivityThread;
static bool isLoadClass = false;


int javaEnvOnLoad(void *vm, void *reserved)
{
    if (isLoadClass)
        return 0;

    //LOGI("Oms_OnLoadcalled [%s] Creating java link vm = %08x\n", __FUNCTION__, vm);

    ArJniHelpler::init(reinterpret_cast<JavaVM *>(vm), nullptr, nullptr);
    JNIEnvProxy jni_env = ArJniHelpler::getJavaEnv();

    if (jni_env == nullptr) {
        LOGW("can not get jni env from omsload");
        return -1;
    }

    jclass activityThread = jni_env->FindClass("android/app/ActivityThread");
    eClassActivityThread = (jclass) jni_env->NewGlobalRef(activityThread);

//    jclass clientProxy = jni_env->FindClass(CLIENTPROXY_CLASS);
//    eClassClientProxy = (jclass) jni_env->NewGlobalRef(clientProxy);

    isLoadClass = true;
    return 0;
}

extern "C" {

// === Java Env methods ===
int impl_arJavaEnv_onLoad(void *vm, void *reserved) {
    return javaEnvOnLoad(vm, reserved);
}

void impl_arJavaEnv_setClassLoader(void *vm, void *classloader, void *findclassid) {
    ArJniHelpler::init((JavaVM *) vm, (jobject) classloader, (jmethodID) findclassid);
}

// === ARCapability methods ===
void impl_arCapability_checkAvailability(void *env, void *activity_context,
                                         ARAvailability *out_availability) {
    std::string brand;
    getPhoneBrand(brand);
    *out_availability = (ARAvailability) isArServiceExist(env, activity_context);
}


ARResult
impl_arCapability_checkAlgorithm(void *env, void *application_context, ARAlgorithmType type,
                                 ARStreamMode mode) {
    if (mode == ARSTREAM_MODE_BACK_RGBD && !isDeviceSupportRGBD(env, application_context))
        return ARRESULT_ERROR_DEVICE_UNSUPPORT;
    switch (type) {
        case ARALGORITHM_TYPE_SLAM:
            if (checkSLAMDeviceSupport(application_context))
                return ARRESULT_SUCCESS;
            else
                return ARRESULT_ERROR_ALGORITHM_UNSUPPORT;
            break;
        case ARALGORITHM_TYPE_HAND_GESTURE:
            if (!isArServiceSupportHandGesture(env, application_context))
                return ARRESULT_ERROR_ALGORITHM_UNSUPPORT;
            break;
        case ARALGORITHM_TYPE_DENSE_RECON:
            break;
        case ARALGORITHM_TYPE_IMAGE_TRACKING:
            break;
        case ARALGORITHM_TYPE_PLANE_DETECTION:
            break;
        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
            break;
        case ARALGORITHM_TYPE_CLOUD_ANCHOR:
            if (!isArServiceSupportCloudAnchor(env, application_context))
                return ARRESULT_ERROR_ALGORITHM_UNSUPPORT;
            break;
        case ARALGORITHM_TYPE_OBJECT_TRACKING:
            break;
        case ARALGORITHM_TYPE_ALL:
            if (!isArServiceSupportCloudAnchor(env, application_context))
                return ARRESULT_ERROR_ALGORITHM_UNSUPPORT;
            break;
        default:
            break;
    }

    return ARRESULT_SUCCESS;
}


void impl_arConfig_create(ARConfig_ **out_config) {
    if (out_config == NULL) {
        return;
    }

    ARConfig_ *pConfig = (ARConfig_ *) malloc(sizeof(ARConfig_));
    pConfig->m_pImpl = new CConfig();

    (*out_config) = pConfig;
}

void impl_arConfig_destroy(ARConfig_ *config) {
    if (IsARConfigAvalid(config) == false)
        return;

    if (config->m_pImpl != nullptr) {
        delete config->m_pImpl;
        config->m_pImpl = nullptr;
    }

    free(config);
}

void impl_arConfig_getIlluminationEstimateMode(const ARConfig_ *config,
                                               ARIlluminationEstimateMode *light_estimation_mode) {

    if (light_estimation_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    (*light_estimation_mode) = config->m_pImpl->m_LightEstimationMode;
}

void impl_arConfig_setIlluminationEstimateMode(ARConfig_ *config,
                                               ARIlluminationEstimateMode light_estimation_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_LightEstimationMode = light_estimation_mode;
}

void impl_arConfig_getMapCoupleMode(const ARConfig_ *config, ARMapCoupleMode *map_couple_mode) {
    if (map_couple_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    (*map_couple_mode) = config->m_pImpl->m_MapCouple_Mode;
}

void impl_arConfig_setMapCoupleMode(ARConfig_ *config, ARMapCoupleMode map_couple_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_MapCouple_Mode = map_couple_mode;
}

void impl_arConfig_getPlaneFindingMode(const ARConfig_ *config,
                                       ARPlaneDetectingMode *plane_finding_mode) {

    if (plane_finding_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    *plane_finding_mode = config->m_pImpl->m_PlaneFindingMode;
}

void impl_arConfig_setPlaneFindingMode(ARConfig_ *config,
                                       ARPlaneDetectingMode plane_finding_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_PlaneFindingMode = plane_finding_mode;
}

void impl_arConfig_setCameraId(ARConfig_ *config, ARCameraID cameraId) {
    if (IsARConfigAvalid(config) == false)
        return;
    config->m_pImpl->m_CameraID = cameraId;
}

void impl_arConfig_getTrackingRunMode(const ARConfig_ *config,
                                      ARTrackingRunMode *tracking_run_mode) {
    if (tracking_run_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    *tracking_run_mode = config->m_pImpl->m_TrackingRunMode;
}

void impl_arConfig_setTrackingRunMode(ARConfig_ *config,
                                      ARTrackingRunMode tracking_run_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_TrackingRunMode = tracking_run_mode;
}

void impl_arConfig_getWorldAlignmentMode(const ARConfig_ *config,
                                         ARWorldAlignmentMode *world_alignment_mode) {
    if (world_alignment_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    *world_alignment_mode = config->m_pImpl->m_WorldAlignmentMode;
}

void impl_arConfig_setWorldAlignmentMode(ARConfig_ *config,
                                         ARWorldAlignmentMode world_alignment_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_WorldAlignmentMode = world_alignment_mode;
}

void impl_arConfig_getVideoPixelFormat(const ARConfig *config, ARPixelFormat *video_pixel_format) {
    if (video_pixel_format == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    *video_pixel_format = config->m_pImpl->m_VideoPixelFormat;
}

void impl_arConfig_setVideoPixelFormat(ARConfig *config, ARPixelFormat video_pixel_format) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_VideoPixelFormat = video_pixel_format;
}

void impl_arConfig_getAlgorithmStreamMode(const ARConfig *config, ARAlgorithmType algorithm_type,
                                          ARStreamMode *out_stream_mode) {
    if (out_stream_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    switch (algorithm_type) {
        case ARALGORITHM_TYPE_SLAM:
            *out_stream_mode = config->m_pImpl->m_SLAM_StreamMode;
            break;
        case ARALGORITHM_TYPE_DENSE_RECON:
            *out_stream_mode = config->m_pImpl->m_DenseRecon_StreamMode;
            break;
        case ARALGORITHM_TYPE_IMAGE_TRACKING:
            *out_stream_mode = config->m_pImpl->m_Marker_StreamMode;
            break;
        case ARALGORITHM_TYPE_HAND_GESTURE:
            *out_stream_mode = config->m_pImpl->m_HandGesture_StreamMode;
            break;
        case ARALGORITHM_TYPE_PLANE_DETECTION:
            *out_stream_mode = config->m_pImpl->m_Plane_StreamMode;
            break;
        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
            *out_stream_mode = config->m_pImpl->m_LightEstimate_StreamMode;
            break;
        case ARALGORITHM_TYPE_CLOUD_ANCHOR:
            *out_stream_mode = config->m_pImpl->m_CloudAnchor_StreamMode;
            break;
        case ARALGORITHM_TYPE_FACE_MESH:
            *out_stream_mode = config->m_pImpl->m_FaceMesh_StreamMode;
            break;
        case ARALGORITHM_TYPE_OBJECT_TRACKING:
            *out_stream_mode = config->m_pImpl->m_ObjectTracking_StreamMode;
            break;
        case ARALGORITHM_TYPE_BODY_DETECT:
            *out_stream_mode = config->m_pImpl->m_BodyDetect_StreamMode;
            break;
        default:
            break;
    }
}

void impl_arConfig_setAlgorithmStreamMode(ARConfig *config, ARAlgorithmType algorithm_type,
                                          ARStreamMode stream_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    switch (algorithm_type) {
        case ARALGORITHM_TYPE_SLAM:
            config->m_pImpl->m_SLAM_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_DENSE_RECON:
            config->m_pImpl->m_DenseRecon_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_IMAGE_TRACKING:
            config->m_pImpl->m_Marker_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_HAND_GESTURE:
            config->m_pImpl->m_HandGesture_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_PLANE_DETECTION:
            config->m_pImpl->m_Plane_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
            config->m_pImpl->m_LightEstimate_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_CLOUD_ANCHOR:
            config->m_pImpl->m_CloudAnchor_StreamMode = stream_mode;
            break;

        case ARALGORITHM_TYPE_FACE_MESH:
            config->m_pImpl->m_FaceMesh_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_OBJECT_TRACKING:
            config->m_pImpl->m_ObjectTracking_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_BODY_DETECT:
            config->m_pImpl->m_BodyDetect_StreamMode = stream_mode;
            break;
        case ARALGORITHM_TYPE_ALL:
        case ARALGORITHM_TYPE_UNKNOWN:
            break;
        default:
            break;
    }
}

void impl_arConfig_setDeviceType(const ARConfig *config, ARDeviceType device_type) {
    if (IsARConfigAvalid(config) == false)
        return;
    config->m_pImpl->m_SLAMDeviceType = device_type;
}

void impl_arConfig_getDeviceType(ARConfig *config, ARDeviceType *out_device_type) {
    if (IsARConfigAvalid(config) == false)
        return;

    *out_device_type = config->m_pImpl->m_SLAMDeviceType;

}

void impl_arConfig_getAlgorithmMode(const ARConfig *config, ARAlgorithmType algorithm_type,
                                    ARAlgorithmMode *out_algorithm_mode) {
    if (out_algorithm_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    switch (algorithm_type) {
        case ARALGORITHM_TYPE_SLAM:
            *out_algorithm_mode = config->m_pImpl->m_SLAMMode;
            break;
        case ARALGORITHM_TYPE_DENSE_RECON:
            *out_algorithm_mode = config->m_pImpl->m_DenseReconMode;
            break;
        case ARALGORITHM_TYPE_IMAGE_TRACKING:
            *out_algorithm_mode = config->m_pImpl->m_ArMarkerMode;
            break;
        case ARALGORITHM_TYPE_HAND_GESTURE:
            *out_algorithm_mode = config->m_pImpl->m_HandGestureMode;
            break;
        case ARALGORITHM_TYPE_PLANE_DETECTION:
            *out_algorithm_mode = config->m_pImpl->m_PlaneAlgorithmMode;
            break;
        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
            *out_algorithm_mode = config->m_pImpl->m_LightEstimateAlgorithmMode;
            break;
        case ARALGORITHM_TYPE_CLOUD_ANCHOR:
            *out_algorithm_mode = config->m_pImpl->m_CloudAnchorMode;
            break;
        case ARALGORITHM_TYPE_FACE_MESH:
            *out_algorithm_mode = config->m_pImpl->m_FaceMeshMode;
        case ARALGORITHM_TYPE_OBJECT_TRACKING:
            *out_algorithm_mode = config->m_pImpl->m_ObjectTrackingMode;
            break;
        case ARALGORITHM_TYPE_OBJECT_SCANNING:
            *out_algorithm_mode = config->m_pImpl->m_ObjectScanningMode;
            break;
        case ARALGORITHM_TYPE_BODY_DETECT:
            *out_algorithm_mode = config->m_pImpl->m_BodyDetectMode;
            break;
        case ARALGORITHM_TYPE_MAP_COUPLE:
            *out_algorithm_mode = config->m_pImpl->m_MapCoupleAlgorithmMode;
        default:
            *out_algorithm_mode = ARALGORITHM_MODE_DISABLE;
            break;
    }
}

void impl_arConfig_setAlgorithmMode(ARConfig *config, ARAlgorithmType algorithm_type,
                                    ARAlgorithmMode algorithm_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    switch (algorithm_type) {
        case ARALGORITHM_TYPE_SLAM:
            config->m_pImpl->m_SLAMMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_DENSE_RECON:
            config->m_pImpl->m_DenseReconMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_IMAGE_TRACKING:
            config->m_pImpl->m_ArMarkerMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_HAND_GESTURE:
            config->m_pImpl->m_HandGestureMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_PLANE_DETECTION:
            config->m_pImpl->m_PlaneAlgorithmMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
            config->m_pImpl->m_LightEstimateAlgorithmMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_CLOUD_ANCHOR:
            config->m_pImpl->m_CloudAnchorMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_FACE_MESH:
            config->m_pImpl->m_FaceMeshMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_OBJECT_TRACKING:
            config->m_pImpl->m_ObjectTrackingMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_OBJECT_SCANNING:
            config->m_pImpl->m_ObjectScanningMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_BODY_DETECT:
            config->m_pImpl->m_BodyDetectMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_MAP_COUPLE:
            config->m_pImpl->m_MapCoupleAlgorithmMode = algorithm_mode;
            break;
        case ARALGORITHM_TYPE_ALL:
        case ARALGORITHM_TYPE_UNKNOWN:
            break;
    }
}

void impl_arConfig_setAugmentedImageDatabase(const ARSession_ *session, ARConfig *config,
                                             const ARReferenceImageDatabase_ *augmented_image_database) {
    if (IsARSessionAvalid(session) == false) {
        return;
    }

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_IMAGE_TRACKING);
    resultWrapper.getResult<MarkerAlgorithmResult *>()->setReferenceImageDatabase(
            augmented_image_database->m_pImpl);
}

void impl_arConfig_getAugmentedImageDatabase(const ARSession_ *session, const ARConfig *config,
                                             ARReferenceImageDatabase_ *out_augmented_image_database) {
    if (IsARSessionAvalid(session) == false) {
        return;
    }

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_IMAGE_TRACKING);
    resultWrapper.getResult<MarkerAlgorithmResult *>()->getReferenceImageDatabase(
            out_augmented_image_database->m_pImpl);
}

void impl_arConfig_setReferenceObjectDatabase(const ARSession_ *session, ARConfig_ *config,
                                              const ARReferenceObjectDatabase_ *object_database) {
    if (IsARSessionAvalid(session) == false) {
        return;
    }

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    resultWrapper.getResult<ObjectTrackingAlgorithmResult *>()->setReferenceObjectDatabase(
            object_database->m_pImpl);
}

void impl_arConfig_getReferenceObjectDatabase(const ARSession_ *session, const ARConfig_ *config,
                                              ARReferenceObjectDatabase_ *object_database) {
    if (IsARSessionAvalid(session) == false) {
        return;
    }

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    resultWrapper.getResult<ObjectTrackingAlgorithmResult *>()->getReferenceObjectDatabase(
            object_database->m_pImpl);
}

void impl_arConfig_getWorldOriginMode(const ARConfig_ *config,
                                      ARWorldOriginMode *world_origin_mode) {
    if (world_origin_mode == NULL)return;

    if (IsARConfigAvalid(config) == false)
        return;

    *world_origin_mode = config->m_pImpl->m_WorldOriginMode;
}

void impl_arConfig_setWorldOriginMode(ARConfig_ *config,
                                      ARWorldOriginMode world_origin_mode) {
    if (IsARConfigAvalid(config) == false)
        return;

    config->m_pImpl->m_WorldOriginMode = world_origin_mode;
}


void impl_arConfig_setServerUrl(ARConfig_ *config, const char *url) {
    if (url == nullptr)
        return;
    memset(config->m_pImpl->m_Url, 0, URL_MAX_LENGTH);
    memcpy(config->m_pImpl->m_Url, url, strlen(url));
}

void impl_arConfig_setBeaconUUID(ARConfig_ *config, const char *uuid) {
    if (uuid == nullptr)
        return;
    memset(config->m_pImpl->m_UUID, 0, URL_MAX_LENGTH);
    memcpy(config->m_pImpl->m_UUID, uuid, strlen(uuid));
}


//// internally used to get surface and window for glass unity apps
jobject GetViewSurface(JNIEnv * javaEnv, jobject activity)
{
    jclass activityClass = javaEnv->GetObjectClass(activity);
    if (activityClass == NULL)
    {
        LOGE("activityClass == NULL!");
        return NULL;
    }

    jfieldID fid = javaEnv->GetFieldID(activityClass, "mUnityPlayer", "Lcom/unity3d/player/UnityPlayer;");
    if (fid == NULL)
    {
        LOGE("mUnityPlayer not found!");
        return NULL;
    }

    jobject unityPlayerObj = javaEnv->GetObjectField(activity, fid);
    if(unityPlayerObj == NULL)
    {
        LOGE("unityPlayer object not found!");
        return NULL;
    }

    jclass unityPlayerClass = javaEnv->GetObjectClass(unityPlayerObj);
    if (unityPlayerClass == NULL)
    {
        LOGE("unityPlayer class not found!");
        return NULL;
    }

    jmethodID mid = javaEnv->GetMethodID(unityPlayerClass, "getChildAt", "(I)Landroid/view/View;");
    if (mid == NULL)
    {
        LOGE("getChildAt methodID not found!");
        return NULL;
    }

    jboolean param = 0;
    jobject surfaceViewObj = javaEnv->CallObjectMethod( unityPlayerObj, mid, param);
    if (surfaceViewObj == NULL)
    {
        LOGE("surfaceView object not found!");
        return NULL;
    }

    jclass surfaceViewClass = javaEnv->GetObjectClass(surfaceViewObj);
    mid = javaEnv->GetMethodID(surfaceViewClass, "getHolder", "()Landroid/view/SurfaceHolder;");
    if (mid == NULL)
    {
        LOGE("getHolder methodID not found!");
        return NULL;
    }

    jobject surfaceHolderObj = javaEnv->CallObjectMethod( surfaceViewObj, mid);
    if (surfaceHolderObj == NULL)
    {
        LOGE("surfaceHolder object not found!");
        return NULL;
    }

    jclass surfaceHolderClass = javaEnv->GetObjectClass(surfaceHolderObj);
    mid = javaEnv->GetMethodID(surfaceHolderClass, "getSurface", "()Landroid/view/Surface;");
    if (mid == NULL)
    {
        LOGE("getSurface methodID not found!");
        return NULL;
    }
    jobject surface = javaEnv->CallObjectMethod( surfaceHolderObj, mid);
    javaEnv->DeleteLocalRef(activityClass);
    javaEnv->DeleteLocalRef(unityPlayerObj);
    javaEnv->DeleteLocalRef(unityPlayerClass);
    javaEnv->DeleteLocalRef(surfaceViewObj);
    javaEnv->DeleteLocalRef(surfaceViewClass);
    javaEnv->DeleteLocalRef(surfaceHolderObj);
    javaEnv->DeleteLocalRef(surfaceHolderClass);
    return surface;
}

ARResult impl_arSession_create(void *env,
                             void *activity_context,
                             ARSession_ **out_session_pointer) {
    DynamicLoader::init((JNIEnv *) env, (jobject) activity_context);
    jclass clazz = ((_JNIEnv *) (env))->FindClass("android/content/ContextWrapper");
    if (clazz == 0) {
        LOGI("find android/content/ContextWrapper error");
    }

    jmethodID method1 = ((_JNIEnv *) (env))->GetMethodID(clazz, "getPackageName",
                                                         "()Ljava/lang/String;");
    if (method1 == 0) {
        LOGI("find getPackageName error");
    }

    jstring str = (jstring) ((_JNIEnv *) (env))->CallObjectMethod((jobject) activity_context,
                                                                  method1);
    const char *package_name = (const char *) ((_JNIEnv *) (env))->GetStringUTFChars(str, NULL);
    LOGI("PackageName:%s", package_name);

    ARSession_ *pSessrion = (ARSession_ *) malloc(sizeof(ARSession_));
    int64_t sessionHandle = static_cast<int64_t>(reinterpret_cast<uintptr_t>(pSessrion));

    pSessrion->m_pImpl = new CSession(env, activity_context, package_name, sessionHandle);
    pSessrion->m_pImpl->intialize();
    (*out_session_pointer) = pSessrion;

    gAppContext = ((_JNIEnv *)(env))->NewGlobalRef((jobject)activity_context);

    return ARRESULT_SUCCESS;
}


ARResult
impl_arSession_getDisplayLandscapeSize(const ARSession_ *session, int *outwidth, int *outheight) {
    if (IsARSessionAvalid(session) == false || outwidth == nullptr || outheight == nullptr) {
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    }
    int width = 0, height = 0;
    bool succ = session->m_pImpl->getDisplayPortraitSize(width, height);
    if (!succ)
        return ARRESULT_ERROR_FATAL;

    *outwidth = width > height ? width : height;
    *outheight = width > height ? height : width;

    return ARRESULT_SUCCESS;
}

void impl_arSession_getVerticalFov(const ARSession_ *session, float *oVerticalFov) {
    if (oVerticalFov == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        return;
    }

    CSession *sessionPtr = session->m_pImpl;
    *oVerticalFov = sessionPtr->getVerticalFov();
}


void impl_arSession_destroy(ARSession_ *session) {
    if (IsARSessionAvalid(session) == false)
        return;

    if (session->m_pImpl) {
        delete session->m_pImpl;
        session->m_pImpl = nullptr;
    }

    free(session);
}

ARResult impl_arSession_checkSupported(const ARSession_ *session,
                                     const ARConfig_ *config) {
    if (IsARSessionAvalid(session) == false || IsARConfigAvalid(config) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    if (session->m_pImpl->checkSupported(*(config->m_pImpl)))
        return ARRESULT_SUCCESS;
    else
        return ARRESULT_ERROR_FATAL;
}

ARResult impl_arSession_configure(ARSession_ *session, const ARConfig_ *config) {
    if (IsARSessionAvalid(session) == false || IsARConfigAvalid(config) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    return session->m_pImpl->configurate(*(config->m_pImpl));
}


ARResult impl_arSession_resume(ARSession_ *session) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    ARResult status = (ARResult) session->m_pImpl->m_LifeCycle->onResume();
    if (status == ARRESULT_SUCCESS) {
        int32_t runmode = session->m_pImpl->getConfig().m_TrackingRunMode;
        if (runmode == ARTRACKING_RUN_MODE_AUTO) {
            session->m_pImpl->m_LifeCycle->onStart();
        }
        return ARRESULT_SUCCESS;
    }

    return ARRESULT_ERROR_FATAL;
}


ARResult impl_arSession_pause(ARSession_ *session) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

//    session->m_pImpl->m_LifeCycle->onStop();
    return (ARResult) session->m_pImpl->m_LifeCycle->onPause();
}


ARResult impl_arSession_startUp(ARSession_ *session, ARAlgorithmType type, int32_t option) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    return (ARResult) session->m_pImpl->m_LifeCycle->onStart();
}

ARResult impl_arSession_stop(ARSession_ *session, ARAlgorithmType type, int32_t option) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    return (ARResult) session->m_pImpl->m_LifeCycle->onStop();
}


void impl_arSession_getTrackingState(const ARSession_ *session, ARTrackingState *out_tracking_state) {
    if (out_tracking_state == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        return;
    }

    int32_t state = session->m_pImpl->getTrackingState();
    *out_tracking_state = (ARTrackingState) state;
}

void impl_arSession_setCameraTextureName(ARSession_ *session, uint32_t texture_id) {
    if (IsARSessionAvalid(session) == false)
        return;

    session->m_pImpl->setCameraTextureName(texture_id);
}


void impl_arSession_setDisplayGeometry(ARSession_ *session, int rotation, int width,
                                     int height) {
    if (IsARSessionAvalid(session) == false)
        return;

    session->m_pImpl->setDisplayGeomtry(rotation, width, height);
}


ARResult impl_arSession_update(ARSession_ *session, ARFrame_ *out_frame) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    return session->m_pImpl->update(out_frame->m_pImpl);
}

ARResult impl_arSession_acquireMap(const ARSession_ *session, ARMap_ **out_map) {
    if (!IsARSessionAvalid(session) || out_map == nullptr)
        return ARRESULT_ERROR_FATAL;

    ARWorldMap *pMap = (ARWorldMap *) malloc(sizeof(ARWorldMap));
    pMap->m_pImpl = session->m_pImpl;
    *out_map = pMap;
    return ARRESULT_SUCCESS;
}

ARResult impl_arSession_acquireNewAnchor(ARSession_ *session, const ARPose_ *pose,
                                       ARAnchor_ **out_anchor) {
    if (IsARSessionAvalid(session) == false || pose == NULL)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    CPose stpose;
    ArPoseTostPose(*pose, stpose);
    CAnchor *pstanchor = session->m_pImpl->acquireNewAnchor(stpose, NULL);
    ARAnchor_ *panchor = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
    panchor->m_pImpl = pstanchor;
    pstanchor->m_Handle = panchor;

    (*out_anchor) = panchor;

    return ARRESULT_SUCCESS;
}

void impl_arSession_getAllAnchors(const ARSession_ *session,
                                ARAnchorList_ *out_anchor_list) {
    if (!IsARSessionAvalid(session) || !IsARAnchorlistAvalid(out_anchor_list))
        return;

    session->m_pImpl->getAllAnchors(out_anchor_list->m_pImpl->m_List);
}

void impl_arSession_getAllNodes(const ARSession_ *session,
                              ARNodeType filter_type,
                              ARNodeList_ *out_node_list) {
    if (IsARSessionAvalid(session) == false || IsARNodeListAvalid(out_node_list) == false)
        return;

    session->m_pImpl->getAllTrackables(filter_type, out_node_list->m_pImpl->m_List);
}


void impl_arSession_getPlaneVertexCount(const ARSession_ *session, int32_t *out_vertex_count) {
    if (out_vertex_count == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    (*out_vertex_count) = session->m_pImpl->getPlaneVetexCount();
}

void impl_arSession_getPlaneVertexArray(const ARSession_ *session, float *out_vertex_array) {
    if (out_vertex_array == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    session->m_pImpl->getPlaneVertexArray(out_vertex_array);
}

void impl_arSession_getPlaneIndexCount(const ARSession_ *session, int32_t *out_index_count) {
    if (out_index_count == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    (*out_index_count) = session->m_pImpl->getPlaneIndexCount();
}

void impl_arSession_getPlaneIndexArray(const ARSession_ *session, unsigned short *out_index_array) {
    if (out_index_array == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    session->m_pImpl->getPlaneIndexArray(out_index_array);
}

bool impl_arSession_isSLAMInitializing(const ARSession_ *session) {
    if (IsARSessionAvalid(session) == false)
        return true;

    return session->m_pImpl->isSLAMIntializing();
}

void impl_arSession_getInitProgress(const ARSession_ *session, int *progress) {
    if (IsARSessionAvalid(session) == false || progress == nullptr)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    *progress = result->getInitProcess();
}


void impl_arSession_getMapQuality(const ARSession_ *session, float *mapQuality) {
    if (IsARSessionAvalid(session) == false || mapQuality == nullptr)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    *mapQuality = result->getSLAMMapQuality();

}

void impl_arSession_getMapCoupleState(const ARSession_ *session, int *state) {
    if (IsARSessionAvalid(session) == false || state == nullptr)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
    *state = result->getMapCoupleState();

}

void impl_arSession_getSLAMTrackingDetails(const ARSession_ *session, int *details) {
    if (IsARSessionAvalid(session) == false || details == nullptr)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    *details = result->getSLAMTrackingDetails();
}

void impl_arSession_getVersion(const ARSession_ *session, char *version, int versionSize) {
    if (IsARSessionAvalid(session) == false || versionSize <= 0)
        return;

    std::string tempstr;
    session->m_pImpl->getVersion(tempstr);

    memset(version, '\0', versionSize);
    if ((int32_t) (tempstr.size()) >= versionSize) {
        memcpy(version, tempstr.c_str(), versionSize);
        version[versionSize - 1] = 0;
    } else {
        memcpy(version, tempstr.c_str(), tempstr.size());
    }
}

void
impl_arSession_SetKeyAndSecret(const ARSession_ *session, const char *app_key, const char *app_secret) {
    if (IsARSessionAvalid(session) == false)
        return;

    session->m_pImpl->setKeyAndSecret(app_key, app_secret);
}

void impl_arSession_getSLAMInfo(const ARSession_ *session, char *infostr, int infosize) {
    if (IsARSessionAvalid(session) == false || infosize <= 0)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();;
    if (slamAlgorithmResult) {
        std::string sdk_info;
        session->m_pImpl->getSDKInfo(sdk_info);
        std::string tempstr = slamAlgorithmResult->getSLAMInfo() + sdk_info;
        memset(infostr, '\0', infosize);
        if ((int32_t) (tempstr.size()) >= infosize) {
            memcpy(infostr, tempstr.c_str(), infosize);
            infostr[infosize - 1] = 0;
        } else {
            memcpy(infostr, tempstr.c_str(), tempstr.size());
        }
    }
}

// get SLAM state string in json format, so string size could be 2048
/// and buffer will be set to 0 at beginning of func.
void impl_arSession_getSLAMJsonStr(const ARSession_ *session, char *jsonstr, int jsonsize) {
    if (IsARSessionAvalid(session) == false || jsonsize <= 0)
        return;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();;
    if (slamAlgorithmResult) {
        std::string tempstr = slamAlgorithmResult->getSLAMJsonInfo();

        memset(jsonstr, '\0', jsonsize);
        if ((int32_t) (tempstr.size()) >= jsonsize) {
            memcpy(jsonstr, tempstr.c_str(), jsonsize);
            jsonstr[jsonsize - 1] = 0;
        } else {
            memcpy(jsonstr, tempstr.c_str(), tempstr.size());
        }
    }

}

ARResult impl_arSession_hostCloudAnchor(const ARSession_ *session, const ARAnchor_ *anchor,
                                      ARAnchor_ **out_anchor) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_NOT_TRACKING;
    if (IsARAnchorAvalid(anchor) == false)
        return ARRESULT_ERROR_ANCHOR_NOT_SUPPORTED_FOR_HOSTING;

    CAnchor *pstanchor;
    ARResult status = session->m_pImpl->hostCloudAnchor(anchor->m_pImpl, &pstanchor);
    if (status == ARRESULT_SUCCESS) {
        ARAnchor_ *paranchor = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
        paranchor->m_pImpl = pstanchor;
        pstanchor->m_Handle = paranchor;
        *out_anchor = paranchor;
    } else
        *out_anchor = nullptr;
    return status;

}

ARResult impl_arSession_resolveCloudAnchor(const ARSession_ *session, const char *anchor_id,
                                         ARAnchor_ **out_anchor) {
    if (IsARSessionAvalid(session) == false)
        return ARRESULT_ERROR_NOT_TRACKING;

    CAnchor *pstanchor;

    ARResult status = session->m_pImpl->resolveCloudAnchor(anchor_id, &pstanchor);
    if (status == ARRESULT_SUCCESS) {
        ARAnchor_ *paranchor = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
        paranchor->m_pImpl = pstanchor;
        pstanchor->m_Handle = paranchor;
        *out_anchor = paranchor;
    } else
        *out_anchor = nullptr;
    return status;
}

void impl_arSession_getBackgroundData(const ARSession_ *session, char **image_data,
                                    int *width, int *height) {
    session->m_pImpl->getBackgroundData(image_data, width, height);
}

void impl_arSession_setIntValue(const ARSession_ *session, ARParameterEnum value_type, int32_t *value,
                              int size) {

    if (IsARSessionAvalid(session) == false)
        return;

    switch (value_type) {
        case ARPARAMETER_CAMERA_TEXTURE_TARGET: {
            session->m_pImpl->setCameraTextureTarget(*value);
        }
        default:
            break;
    }
}

void impl_arSession_setFloatValue(const ARSession_ *session, ARParameterEnum value_type, float *value,
                                int size) {

}

void impl_arSession_setStringValue(const ARSession_ *session, ARParameterEnum value_type, char *value,
                                 int size) {
    if(IsARSessionAvalid(session) == false)
        return;

    switch (value_type){
        case ARPARAMETER_ALGORITHM_MAP_COUPLE_PARAM_JSON:
        {
            session->m_pImpl->setUserJson(value);
            break;
        }
        default:
            break;
    }

}

void impl_arSession_getIntValue(const ARSession_ *session, ARParameterEnum value_type, int32_t *value,
                              int size) {
    if (value == nullptr)
        return;
    switch (value_type) {
        case ARPARAMETER_ALGORITHM_SLAM_DETAIL_STATE: {
            impl_arSession_getSLAMTrackingDetails(session, value);
            break;
        }
        case ARPARAMETER_CAMERA_TEXTURE_TARGET: {
            *value = session->m_pImpl->getCameraTextureTarget();
            break;
        }
        case ARPARAMETER_ALGORITHM_MAP_COUPLE_STATE:
        {
            impl_arSession_getMapCoupleState(session, value);
            break;
        }
        default:
            break;
    }
}

void impl_arSession_getFloatValue(const ARSession_ *session, ARParameterEnum value_type, float *value,
                                int size) {
    if (value == nullptr)
        return;
    switch (value_type) {
        case ARPARAMETER_VIDEO_VERTICAL_FOV: {
            impl_arSession_getVerticalFov(session, value);
            break;
        }
        case ARPRARMETER_ALGORITHM_SLAM_MAP_QUALITY: {
            impl_arSession_getMapQuality(session, value);
            break;
        }
        case ARPARAMETER_CAMERA_INTRINSICS: {
            impl_arSession_getRawCameraIntrinsics(session, value, size);
            break;
        }
        default:
            break;
    }
}

void impl_arSession_getStringValue(const ARSession_ *session, ARParameterEnum value_type, char *value,
                                 int *length, int size) {
    if (value == nullptr || length == nullptr)
        return;

    if (IsARSessionAvalid(session) == false || size <= 0)
        return;

    switch (value_type) {
        case ARPARAMETER_ALGORITHM_SLAM_INFO: {

            AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
                    ARALGORITHM_TYPE_SLAM);
            SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
            if (slamAlgorithmResult) {
                std::string sdk_info;
                session->m_pImpl->getSDKInfo(sdk_info);
                std::string tempstr = slamAlgorithmResult->getSLAMInfo() + sdk_info;
                memset(value, '\0', size);
                if ((int32_t) (tempstr.size()) >= size) {
                    memcpy(value, tempstr.c_str(), size);
                    value[size - 1] = 0;
                    *length = size;
                } else {
                    memcpy(value, tempstr.c_str(), tempstr.size());
                    *length = (int) tempstr.size();
                }
            }
            break;
        }
        case ARPARAMETER_ALGORITHM_OBJECT_TRACKING: {

            AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
                    ARALGORITHM_TYPE_OBJECT_TRACKING);
            ObjectTrackingAlgorithmResult *objectTrackingAlgorithmResult = algorithmResultWrapper.getResult<ObjectTrackingAlgorithmResult *>();
            if (objectTrackingAlgorithmResult) {
                std::string tempstr = objectTrackingAlgorithmResult->getObjectTrackingJsonInfo();
                memset(value, '\0', size);
                if ((int32_t) (tempstr.size()) >= size) {
                    memcpy(value, tempstr.c_str(), size);
                    value[size - 1] = 0;
                    *length = size;
                } else {
                    memcpy(value, tempstr.c_str(), tempstr.size());
                    *length = (int) tempstr.size();
                }
            }
            break;
        }
        case ARPARAMETER_ALGORITHM_MAP_COUPLE_RESULT_JSON:
        {
            AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
            SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
            if (slamAlgorithmResult) {
                std::string tempstr = slamAlgorithmResult->getRegionInfo();
                memset(value, '\0', size);
                if ((int32_t) (tempstr.size()) >= size) {
                    memcpy(value, tempstr.c_str(), size);
                    value[size - 1] = 0;
                    *length = size;
                } else {
                    memcpy(value, tempstr.c_str(), tempstr.size());
                    *length = (int)tempstr.size();
                }
            }
            break;
        }
        default:
            break;
    }
}

void impl_arSession_getConfig(ARSession_ *session, ARConfig_ *out_config) {
    if (IsARSessionAvalid(session) == false || IsARConfigAvalid(out_config) == false)
        return;

    (*(out_config->m_pImpl)) = session->m_pImpl->getConfig();
}

void impl_arSession_getSupportedCameraConfigs(const ARSession_ *session, ARCameraConfigList_ *list) {
    if (IsARSessionAvalid(session) == false || IsARCameraConfigListAvalid(list) == false)
        return;

    session->m_pImpl->getSupportedCameraConfigs((*list->m_pImpl));
}

ARResult impl_arSession_setCameraConfig(const ARSession_ *session, const ARCameraConfig_ *camera_config) {
    if (IsARSessionAvalid(session) == false || IsARCameraConfigAvalid(camera_config) == false)
        return ARRESULT_ERROR_FATAL;

    return session->m_pImpl->setCameraConfig(*(camera_config->m_pImpl));
}

void impl_arSession_getCameraConfig(const ARSession_ *session, ARCameraConfig_ *out_camera_config) {
    if (IsARSessionAvalid(session) == false || IsARCameraConfigAvalid(out_camera_config) == false)
        return;

    *(out_camera_config->m_pImpl) = session->m_pImpl->getCameraConfig();
}

void impl_arSession_setBoundingBox(const ARSession_ *session, const float *vertex, const float *matrix) {
    if (IsARSessionAvalid(session) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    resultWrapper.getResult<SLAMAlgorithmResult *>()->setBoundingBox(vertex, matrix);
}

void impl_arSession_getBoundingBoxScanningResult(const ARSession_ *session, uint8_t **out_raw_bytes,
                                               int64_t *out_raw_bytes_size) {
    if (IsARSessionAvalid(session) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    resultWrapper.getResult<SLAMAlgorithmResult *>()->getScanningResult(out_raw_bytes,
                                                                        out_raw_bytes_size);
}

void impl_arSession_getBoundingBoxPointCloud(const ARSession_ *session, float **points, int *points_num) {
    if (IsARSessionAvalid(session) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    resultWrapper.getResult<SLAMAlgorithmResult *>()->getBoundingBoxPointCloud(points, points_num);
}

void impl_arSession_getRawCameraIntrinsics(const ARSession_ *session, float* value, int size) {
    if (IsARSessionAvalid(session) == false)
        return;

    if (size != 14 && size != 28) {
        LOGE("error: intrinsics size must be 14 or 28");
        return;
    }

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
        ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    CCameraIntrinsics intrinsics = result->getCameraIntrinsics();

    value[0] = intrinsics.m_ImageWidth;
    value[1] = intrinsics.m_ImageHeight;
    value[2] = intrinsics.m_FocalLengthX;
    value[3] = intrinsics.m_FocalLengthY;
    value[4] = intrinsics.m_PrincipalPTX;
    value[5] = intrinsics.m_PrincipalPTY;
    if (intrinsics.m_Distortion != nullptr)
        memcpy(&value[6], intrinsics.m_Distortion, 8 * sizeof(float));

    if (size == 14) {
        return;
    }
    // assign slave intrinsics also
    value[14] = intrinsics.m_ImageWidthSlv;
    value[15] = intrinsics.m_ImageHeightSlv;
    value[16] = intrinsics.m_FocalLengthSlvX;
    value[17] = intrinsics.m_FocalLengthSlvY;
    value[18] = intrinsics.m_PrincipalPTSlvX;
    value[19] = intrinsics.m_PrincipalPTSlvY;
    if (intrinsics.m_DistortionSlv != nullptr)
        memcpy(&value[20], intrinsics.m_DistortionSlv, 8 * sizeof(float));
}

// === ARCameraConfigList_ methods ===
void impl_arCameraConfigList_create(const ARSession_ *session, ARCameraConfigList_ **out_list) {
    if (out_list == NULL)
        return;

    if (IsARSessionAvalid(session) == false) {
        *out_list = NULL;
        return;
    }

    ARCameraConfigList_ *presultlist = (ARCameraConfigList_ *) malloc(sizeof(ARCameraConfigList_));
    presultlist->m_pImpl = new CCameraConfigList();
    *out_list = presultlist;
}

void impl_arCameraConfigList_destroy(ARCameraConfigList_ *list) {
    if (list == NULL)
        return;

    if (list->m_pImpl) {
        delete list->m_pImpl;
        list->m_pImpl = NULL;
    }

    free(list);
}

void impl_arCameraConfigList_getSize(const ARSession_ *session, const ARCameraConfigList_ *list,
                                     int32_t *out_size) {
    if (out_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraConfigListAvalid(list) == false) {
        *out_size = 0;
        return;
    }

    *out_size = (int32_t) list->m_pImpl->m_List.size();
}

void impl_arCameraConfigList_getItem(const ARSession_ *session, const ARCameraConfigList_ *list,
                                     int32_t index, ARCameraConfig **out_camera_config) {
    if (out_camera_config == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraConfigListAvalid(list) == false ||
        (int32_t) (list->m_pImpl->m_List.size()) <= index) {
        *out_camera_config = NULL;
        return;
    }

    ARCameraConfig_ *presult = (ARCameraConfig_ *) malloc(sizeof(ARCameraConfig_));
    presult->m_pImpl = list->m_pImpl->m_List[index];
    *out_camera_config = presult;
}

// === ARCameraConfig_ methods ===
void impl_arCameraConfig_create(const ARSession_ *session, ARCameraConfig_ **out_camera_config) {
    if (out_camera_config == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        *out_camera_config = NULL;
        return;
    }

    ARCameraConfig_ *presult = (ARCameraConfig_ *) malloc(sizeof(ARCameraConfig_));
    presult->m_pImpl = NULL;
    *out_camera_config = presult;
}

void impl_arCameraConfig_destroy(ARCameraConfig *camera_config) {
    if (IsARCameraConfigAvalid(camera_config) == false)
        return;

    if (camera_config->m_pImpl != nullptr) {
        delete camera_config->m_pImpl;
        camera_config->m_pImpl = nullptr;
    }

    free(camera_config);
}

void
impl_arCameraConfig_getImageDimensions(const ARSession_ *session, const ARCameraConfig_ *camera_config,
                                       int32_t *out_width, int32_t *out_height) {
    if (!IsARSessionAvalid(session) || !IsARCameraConfigAvalid(camera_config))
        return;

    *out_width = camera_config->m_pImpl->m_Width;
    *out_height = camera_config->m_pImpl->m_Height;
}

void impl_arCameraConfig_getTextureDimensions(const ARSession_ *session,
                                              const ARCameraConfig_ *camera_config,
                                              int32_t *out_width, int32_t *out_height) {
    if (!IsARSessionAvalid(session) || !IsARCameraConfigAvalid(camera_config))
        return;

    *out_width = camera_config->m_pImpl->m_Width;
    *out_height = camera_config->m_pImpl->m_Height;
}

// === ARPose_ methods ===
void impl_arPose_create(const float *pose_raw, ARPose_ **out_pose) {
    if (out_pose == NULL)return;

    ARPose_ *ppose = (ARPose_ *) malloc(sizeof(ARPose_));
    memset(&(ppose->qx), 0, sizeof(ARPose_));

    if (pose_raw != NULL)
        memcpy(&(ppose->qx), pose_raw, sizeof(float) * 7);

    (*out_pose) = ppose;
}

/// Releases memory used by a pose object.
void impl_arPose_destroy(ARPose_ *pose) {
    if (pose == NULL)
        return;

    free(pose);
}

void impl_arPose_getPoseRaw(const ARPose_ *pose, float *out_pose_raw) {
    if (out_pose_raw == NULL)
        return;

    if (pose == NULL)
        return;

    memcpy(out_pose_raw, &(pose->qx), sizeof(ARPose_));
}

void impl_arPose_getMatrix(const ARPose_ *pose, float *out_matrix_col_major_4x4) {
    if (out_matrix_col_major_4x4 == NULL)
        return;

    if (pose == NULL)
        return;

    using namespace standardar;

    mat4f translate = mat4f::createTranslation(pose->tx, pose->ty, pose->tz);
    Quatf quad;
    quad.v.x = pose->qx;
    quad.v.y = pose->qy;
    quad.v.z = pose->qz;
    quad.w = pose->qw;

    mat4f rotate = quad.transform();
    mat4f finalmat = translate * rotate;
    memcpy(out_matrix_col_major_4x4, finalmat.data, sizeof(float) * 16);
}


// === ARCamera_ methods ===
void impl_arCamera_getPose(const ARSession_ *session,
                           const ARCamera_ *camera,
                           ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    float out_col_major_4x4[16];
    session->m_pImpl->getViewMatrix(out_col_major_4x4);

    mat4f viewmat(out_col_major_4x4);
    mat4f worldmat = viewmat.inverse();

    Quatf quat = Quatf::frommat(worldmat);
    out_pose->qx = quat.v.x;
    out_pose->qy = quat.v.y;
    out_pose->qz = quat.v.z;
    out_pose->qw = quat.w;
    out_pose->tx = worldmat[12];
    out_pose->ty = worldmat[13];
    out_pose->tz = worldmat[14];
}

void impl_arCamera_getDisplayOrientedPose(const ARSession_ *session, const ARCamera_ *camera,
                                          ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    float out_col_major_4x4[16];
    session->m_pImpl->getViewMatrix(out_col_major_4x4);

    mat4f viewmat(out_col_major_4x4);
    mat4f worldmat = viewmat.inverse();

    Quatf quat = Quatf::frommat(worldmat);
    out_pose->qx = quat.v.x;
    out_pose->qy = quat.v.y;
    out_pose->qz = quat.v.z;
    out_pose->qw = quat.w;
    out_pose->tx = worldmat[12];
    out_pose->ty = worldmat[13];
    out_pose->tz = worldmat[14];
}

void impl_arCamera_getViewMatrix(const ARSession_ *session, const ARCamera_ *camera,
                                 float *out_col_major_4x4) {
    if (out_col_major_4x4 == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    //camera->m_pImpl->getViewMatrix(out_col_major_4x4);
    session->m_pImpl->getViewMatrix(out_col_major_4x4);
}

void impl_arCamera_getTrackingState(const ARSession_ *session, const ARCamera_ *camera,
                                    ARTrackingState *out_tracking_state) {
    if (out_tracking_state == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    *out_tracking_state = camera->m_pImpl->getTrackingState();
}

void impl_arCamera_getProjectionMatrix(const ARSession_ *session,
                                       const ARCamera_ *camera, float near, float far,
                                       float *dest_col_major_4x4) {
    if (dest_col_major_4x4 == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    //camera->m_pImpl->getProjectionMatrix(near, far, dest_col_major_4x4);
    session->m_pImpl->getProjectionMatrix(near, far, dest_col_major_4x4);
}


void impl_arCamera_release(ARCamera_ *camera) {
    if (camera == NULL)
        return;

    free(camera);
}

void impl_arCamera_getImageIntrinsics(const ARSession_ *session, const ARCamera_ *camera,
                                      ARCameraIntrinsics_ *out_camera_intrinsics) {
    if (out_camera_intrinsics == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    CCameraIntrinsics intrinsics = camera->m_pImpl->getImageIntrinsics();
    out_camera_intrinsics->m_pImpl->m_ImageWidth = intrinsics.m_ImageWidth;
    out_camera_intrinsics->m_pImpl->m_ImageHeight = intrinsics.m_ImageHeight;
    out_camera_intrinsics->m_pImpl->m_FocalLengthX = intrinsics.m_FocalLengthX;
    out_camera_intrinsics->m_pImpl->m_FocalLengthY = intrinsics.m_FocalLengthY;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTX = intrinsics.m_PrincipalPTX;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTY = intrinsics.m_PrincipalPTY;
    out_camera_intrinsics->m_pImpl->m_Distortion = intrinsics.m_Distortion;

    out_camera_intrinsics->m_pImpl->m_ImageWidthSlv = intrinsics.m_ImageWidthSlv;
    out_camera_intrinsics->m_pImpl->m_ImageHeightSlv = intrinsics.m_ImageHeightSlv;
    out_camera_intrinsics->m_pImpl->m_FocalLengthSlvX = intrinsics.m_FocalLengthSlvX;
    out_camera_intrinsics->m_pImpl->m_FocalLengthSlvY = intrinsics.m_FocalLengthSlvY;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTSlvX = intrinsics.m_PrincipalPTSlvX;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTSlvY = intrinsics.m_PrincipalPTSlvY;
    out_camera_intrinsics->m_pImpl->m_DistortionSlv = intrinsics.m_DistortionSlv;
}

void impl_arCamera_getTextureIntrinsics(const ARSession_ *session, const ARCamera_ *camera,
                                        ARCameraIntrinsics_ *out_camera_intrinsics) {
    if (out_camera_intrinsics == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    CCameraIntrinsics intrinsics = camera->m_pImpl->getTextureIntrinsics();
    out_camera_intrinsics->m_pImpl->m_ImageWidth = intrinsics.m_ImageWidth;
    out_camera_intrinsics->m_pImpl->m_ImageHeight = intrinsics.m_ImageHeight;
    out_camera_intrinsics->m_pImpl->m_FocalLengthX = intrinsics.m_FocalLengthX;
    out_camera_intrinsics->m_pImpl->m_FocalLengthY = intrinsics.m_FocalLengthY;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTX = intrinsics.m_PrincipalPTX;
    out_camera_intrinsics->m_pImpl->m_PrincipalPTY = intrinsics.m_PrincipalPTY;
}

void impl_arCameraIntrinsics_create(const ARSession_ *session,
                                    ARCameraIntrinsics_ **out_camera_intrinsics) {
    if (IsARSessionAvalid(session) == false) {
        return;
    }

    ARCameraIntrinsics_ *pCameraIntrinsics = (ARCameraIntrinsics_ *) malloc(
            sizeof(ARCameraIntrinsics_));
    pCameraIntrinsics->m_pImpl = new CCameraIntrinsics();

    (*out_camera_intrinsics) = pCameraIntrinsics;
}

void impl_arCameraIntrinsics_getFocalLength(const ARSession_ *session,
                                            const ARCameraIntrinsics_ *intrinsics, float *out_fx,
                                            float *out_fy) {
    if (out_fx == NULL || out_fy == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_fx) = intrinsics->m_pImpl->m_FocalLengthX;
    (*out_fy) = intrinsics->m_pImpl->m_FocalLengthY;
}

void impl_arCameraIntrinsics_getFocalLengthSlv(const ARSession_ *session,
                                               const ARCameraIntrinsics_ *intrinsics, float *out_fx,
                                               float *out_fy) {
    if (out_fx == NULL || out_fy == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_fx) = intrinsics->m_pImpl->m_FocalLengthSlvX;
    (*out_fy) = intrinsics->m_pImpl->m_FocalLengthSlvY;
}

void impl_arCameraIntrinsics_getPrincipalPoint(const ARSession_ *session,
                                               const ARCameraIntrinsics_ *intrinsics, float *out_cx,
                                               float *out_cy) {
    if (out_cx == NULL || out_cy == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_cx) = intrinsics->m_pImpl->m_PrincipalPTX;
    (*out_cy) = intrinsics->m_pImpl->m_PrincipalPTY;
}

void impl_arCameraIntrinsics_getPrincipalPointSlv(
    const ARSession_ *session, const ARCameraIntrinsics_ *intrinsics, float *out_cx, float *out_cy) {
    if (out_cx == NULL || out_cy == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_cx) = intrinsics->m_pImpl->m_PrincipalPTSlvX;
    (*out_cy) = intrinsics->m_pImpl->m_PrincipalPTSlvY;
}

void impl_arCameraIntrinsics_getImageDimensions(const ARSession_ *session,
                                                const ARCameraIntrinsics_ *intrinsics,
                                                int32_t *out_width, int32_t *out_height) {
    if (out_width == NULL || out_height == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_width) = intrinsics->m_pImpl->m_ImageWidth;
    (*out_height) = intrinsics->m_pImpl->m_ImageHeight;
}

void impl_arCameraIntrinsics_getImageDimensionsSlv(const ARSession_ *session,
                                                   const ARCameraIntrinsics_ *intrinsics,
                                                   int32_t *out_width, int32_t *out_height) {
    if (out_width == NULL || out_height == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    (*out_width) = intrinsics->m_pImpl->m_ImageWidthSlv;
    (*out_height) = intrinsics->m_pImpl->m_ImageHeightSlv;
}

void impl_arCameraIntrinsics_getDistortions(const ARSession_ *session,
                                            const ARCameraIntrinsics_ *intrinsics,
                                            float *out_distort_arr) {
    if (out_distort_arr == NULL || intrinsics->m_pImpl->m_Distortion == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    memcpy(out_distort_arr, intrinsics->m_pImpl->m_Distortion, sizeof(float) * 8);
}

void impl_arCameraIntrinsics_getDistortionsSlv(const ARSession_ *session,
                                               const ARCameraIntrinsics_ *intrinsics,
                                               float *out_distort_arr) {
    if (out_distort_arr == NULL || intrinsics->m_pImpl->m_DistortionSlv == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARCameraIntrinsicsAvalid(intrinsics) == false) {
        return;
    }

    memcpy(out_distort_arr, intrinsics->m_pImpl->m_DistortionSlv, sizeof(float) * 8);
}


void impl_arCameraIntrinsics_destroy(ARCameraIntrinsics_ *camera_intrinsics) {
    if (IsARCameraIntrinsicsAvalid(camera_intrinsics) == false)
        return;

    if (camera_intrinsics->m_pImpl != nullptr) {
        delete camera_intrinsics->m_pImpl;
        camera_intrinsics->m_pImpl = nullptr;
    }

    free(camera_intrinsics);
}


// === ARFrame_ methods ===


void impl_arFrame_create(const ARSession_ *session, ARFrame_ **out_frame) {
    if (out_frame == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        (*out_frame) = NULL;
        return;
    }

    CFrame *pstFrame = session->m_pImpl->createFrame(session->m_pImpl);
    ARFrame_ *pFrame = (ARFrame_ *) malloc(sizeof(ARFrame_));
    pstFrame->m_Handle = pFrame;
    pFrame->m_pImpl = pstFrame;
    *out_frame = pFrame;
}

void impl_arFrame_destroy(ARFrame_ *frame) {
    if (IsARFrameAvalid(frame) == false)
        return;

    if (!frame->m_pImpl)
        return;

    CSession *pstSession = frame->m_pImpl->getSession();
    if (pstSession)
        pstSession->removeFrame(frame->m_pImpl);

    free(frame);
}

void impl_arFrame_getDisplayGeometryChanged(const ARSession_ *session,
                                            const ARFrame_ *frame,
                                            int32_t *out_geometry_changed) {
    if (out_geometry_changed == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        *out_geometry_changed = 0;
        return;
    }

    (*out_geometry_changed) = frame->m_pImpl->getDisplayGeometryChanged();
}


void impl_arFrame_transformDisplayUvCoords(const ARSession_ *session,
                                           const ARFrame_ *frame,
                                           int32_t num_elements, const float *uvs_in,
                                           float *uvs_out) {
    if (uvs_in == NULL || uvs_out == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    frame->m_pImpl->transformDisplayUvCoords(num_elements, uvs_in, uvs_out);
}

void impl_arFrame_setHitTestMode(const ARSession_ *session,
                                 const ARFrame_ *frame,
                                 ARQueryMode mode) {
    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    frame->m_pImpl->setHitTestMode(mode);
}

void impl_arFrame_hitTest(const ARSession_ *session, const ARFrame_ *frame,
                          float pixel_x, float pixel_y,
                          ARQueryResultList_ *hit_result_list) {
    if (hit_result_list == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    frame->m_pImpl->hitTest(pixel_x, pixel_y, hit_result_list->m_pImpl->m_List);
}

void impl_arFrame_getIlluminationEstimate(const ARSession_ *session, const ARFrame_ *frame,
                                          ARIlluminationEstimate_ *out_light_estimate) {
    if (out_light_estimate == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    out_light_estimate->m_pImpl = frame->m_pImpl;
}

ARResult impl_arFrame_acquirePointCloud(const ARSession_ *session,
                                        const ARFrame_ *frame,
                                        ARPointCloud_ **out_point_cloud) {
    if (out_point_cloud == NULL)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    }

    ARPointCloud_ *pPointCloud = (ARPointCloud_ *) malloc(sizeof(ARPointCloud_));
    pPointCloud->m_pImpl = frame->m_pImpl;

    (*out_point_cloud) = pPointCloud;
    return ARRESULT_SUCCESS;
}

ARResult impl_arFrame_acquireDenseMesh(const ARSession_ *session, const ARFrame_ *frame,
                                       ARDenseMesh_ **out_dense_mesh) {
    if (out_dense_mesh == NULL)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    }

    ARDenseMesh_ *pDenseMesh = (ARDenseMesh_ *) malloc(sizeof(ARDenseMesh_));
    pDenseMesh->m_pImpl = frame->m_pImpl;

    (*out_dense_mesh) = pDenseMesh;
    return ARRESULT_SUCCESS;
}


void impl_arFrame_acquireCamera(const ARSession_ *session, const ARFrame_ *frame,
                                ARCamera_ **out_camera) {
    if (out_camera == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    ARCamera_ *pcamera = (ARCamera_ *) malloc(sizeof(ARCamera_));
    pcamera->m_pImpl = frame->m_pImpl;
    (*out_camera) = pcamera;
}

void impl_arFrame_getUpdatedAnchors(const ARSession_ *session,
                                    const ARFrame_ *frame,
                                    ARAnchorList_ *out_anchor_list) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame) ||
        !IsARAnchorlistAvalid(out_anchor_list))
        return;

    session->m_pImpl->getUpdatedAnchors(out_anchor_list->m_pImpl->m_List);
}

void impl_arFrame_getUpdatedNodes(const ARSession_ *session,
                                  const ARFrame_ *frame,
                                  ARNodeType filter_type,
                                  ARNodeList_ *out_node_list) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame) || !IsARNodeListAvalid(out_node_list))
        return;
    session->m_pImpl->getUpdatedTrackables(out_node_list->m_pImpl->m_List, filter_type);
}

void impl_arFrame_getImageFormat(const ARSession_ *session, const ARFrame_ *frame,
                                 ARPixelFormat *video_image_format) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return;

    if (video_image_format == NULL)
        return;

    *video_image_format = session->m_pImpl->getVideoFormat();
}

ARResult
impl_arFrame_getPreviewSize(const ARSession_ *session, const ARFrame_ *frame, int *w, int *h) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return ARRESULT_ERROR_FATAL;
    if (w == NULL || h == NULL)
        return ARRESULT_ERROR_FATAL;

    *w = session->m_pImpl->getVideoWidth();
    *h = session->m_pImpl->getVideoHeight();
    return ARRESULT_SUCCESS;
}

bool impl_arFrame_isYUVImageDataUpdated(const ARSession_ *session, const ARFrame_ *frame) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return false;

    CSession *sessionPtr = session->m_pImpl;
    return sessionPtr->isImgBufferDirty();
}

ARResult impl_arFrame_getYUVImageData(const ARSession_ *session, const ARFrame_ *frame,
                                      unsigned char *gray_data, unsigned char *u_data,
                                      unsigned char *v_data) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return ARRESULT_ERROR_FATAL;
    if (gray_data == NULL || u_data == NULL)
        return ARRESULT_ERROR_FATAL;

    bool bcopy = session->m_pImpl->copyYUVImageDataLocked(gray_data, u_data, v_data);
    if (bcopy == false)
        return ARRESULT_ERROR_FATAL;

    return ARRESULT_SUCCESS;
}

ARResult impl_arFrame_isDepthImageAvaliable(const ARSession_ *session, const ARFrame_ *frame) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return ARRESULT_ERROR_FATAL;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    if (result) {
        bool isDepthBufBack = result->isDepthImageAvaliable();
        if (!isDepthBufBack)
            return ARRESULT_ERROR_FATAL;
        return ARRESULT_SUCCESS;
    }
    return ARRESULT_ERROR_FATAL;
}

ARResult
impl_arFrame_getDepthImageResolution(const ARSession_ *session, const ARFrame_ *frame, int *out_width,
                                     int *out_height) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return ARRESULT_ERROR_FATAL;
    if (out_width == NULL || out_height == NULL)
        return ARRESULT_ERROR_FATAL;
    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *result = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    if (result) {
        bool isDepthBufBack = result->isDepthImageAvaliable();
        if (!isDepthBufBack)
            return ARRESULT_ERROR_FATAL;
        *out_width = result->getDepthWidth();
        *out_height = result->getDepthHeight();
        return ARRESULT_SUCCESS;
    }
    return ARRESULT_ERROR_FATAL;
}

ARResult impl_arFrame_getDepthImageData(const ARSession_ *session, const ARFrame_ *frame,
                                        unsigned char *out_depth_image_data) {
    if (!IsARSessionAvalid(session) || !IsARFrameAvalid(frame))
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    if (out_depth_image_data == NULL)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    AlgorithmResultWrapper algorithmResultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
    bool bcopy = false;
    if (slamAlgorithmResult)
        bcopy = slamAlgorithmResult->getDepthImage(out_depth_image_data);
    if (bcopy == false)
        return ARRESULT_ERROR_INVALID_ARGUMENT;

    return ARRESULT_SUCCESS;
}

void impl_arFrame_getDepthImageFormat(const ARSession_ *session, const ARFrame_ *frame,
                                      ARPixelFormat *out_format) {
    *out_format = ARPIXEL_FORMAT_DEPTH16;
}

void impl_arFrame_getTimestamp(const ARSession_ *session, const ARFrame_ *frame,
                               int64_t *out_timestamp_ns) {
    if (out_timestamp_ns == NULL)
        return;;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false)
        return;

    (*out_timestamp_ns) = frame->m_pImpl->getTimeStamp();
}

void impl_arFrame_getAndroidSensorPose(const ARSession_ *session, const ARFrame_ *frame,
                                       ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    float out_col_major_4x4[16];
    session->m_pImpl->getViewMatrix(out_col_major_4x4);

    mat4f viewmat(out_col_major_4x4);
    mat4f worldmat = viewmat.inverse();

    Quatf quat = Quatf::frommat(worldmat);
    out_pose->qx = quat.v.x;
    out_pose->qy = quat.v.y;
    out_pose->qz = quat.v.z;
    out_pose->qw = quat.w;
    out_pose->tx = worldmat[12];
    out_pose->ty = worldmat[13];
    out_pose->tz = worldmat[14];
}

void
impl_arFrame_hitTestRay(const ARSession_ *session, const ARFrame_ *frame, const float *ray_origin_3,
                        const float *ray_direction_3, ARQueryResultList_ *hit_result_list) {
    if (hit_result_list == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARFrameAvalid(frame) == false) {
        return;
    }

    frame->m_pImpl->hitTest(ray_origin_3, ray_direction_3, hit_result_list->m_pImpl->m_List);
}

// === ARPointCloud_ methods ===

void impl_arPointCloud_getNumberOfPoints(const ARSession_ *session,
                                         const ARPointCloud_ *point_cloud,
                                         int32_t *out_number_of_points) {
    if (out_number_of_points == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARPointCloudAvalid(point_cloud) == false) {
        return;
    }

    *out_number_of_points = point_cloud->m_pImpl->getPointCloudPointCount();
}

void impl_arPointCloud_getData(const ARSession_ *session,
                               const ARPointCloud_ *point_cloud,
                               const float **out_point_cloud_data) {
    if (out_point_cloud_data == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARPointCloudAvalid(point_cloud) == false) {
        return;
    }

    //int ptcount = point_cloud->m_pImpl->getPointCloudPointCount();
    //memcpy(point_cloud->m_pImpl->getPointCloudData(), *out_point_cloud_data, ptcount*sizeof(float));
    (*out_point_cloud_data) = point_cloud->m_pImpl->getPointCloudData();
}


void impl_arPointCloud_release(ARPointCloud_ *point_cloud) {
    if (point_cloud == NULL)
        return;

    free(point_cloud);
}

// === ARDenseMesh_ methods ===
void impl_arDenseMesh_getFormat(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                                ARVertexFormat *out_vertex_format) {
    if (out_vertex_format == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    *out_vertex_format = dense_mesh->m_pImpl->getDenseMeshFormat();
}

void impl_arDenseMesh_getVertexCount(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                                     int32_t *out_vertex_count) {
    if (out_vertex_count == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    *out_vertex_count = dense_mesh->m_pImpl->getDenseMeshVertexCount();
}

void impl_arDenseMesh_getVertex(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                                float *out_vertex_array) {
    if (out_vertex_array == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    dense_mesh->m_pImpl->getDenseMeshVertex(out_vertex_array);
}

void impl_arDenseMesh_getNormal(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                                float *out_vertex_normal) {
    if (out_vertex_normal == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    dense_mesh->m_pImpl->getDenseMeshNormal(out_vertex_normal);
}

void impl_arDenseMesh_getIndexCount(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                                    int32_t *out_index_count) {
    if (out_index_count == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    *out_index_count = dense_mesh->m_pImpl->getDenseMeshIndexCount();
}

void impl_arDenseMesh_getIndex(const ARSession_ *session, const ARDenseMesh_ *dense_mesh,
                               unsigned short *out_index_array) {
    if (out_index_array == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    if (IsARSessionAvalid(session) == false || IsARDenseMeshAvalid(dense_mesh) == false) {
        return;
    }

    dense_mesh->m_pImpl->getDenseMeshIndex(out_index_array);
}

void impl_arDenseMesh_release(ARDenseMesh_ *dense_mesh) {
    if (dense_mesh == NULL)
        return;

    free(dense_mesh);
}

// === ARIlluminationEstimate_ methods ===

void impl_arIlluminationEstimate_create(const ARSession_ *session,
                                        ARIlluminationEstimate_ **out_light_estimate) {
    if (out_light_estimate == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    ARIlluminationEstimate_ *plightestimate = (ARIlluminationEstimate_ *) malloc(
            sizeof(ARIlluminationEstimate_));
    plightestimate->m_pImpl = NULL;
    *out_light_estimate = plightestimate;
}

void impl_arIlluminationEstimate_destroy(ARIlluminationEstimate_ *light_estimate) {
    if (light_estimate == NULL)
        return;

    free(light_estimate);
}

void impl_arIlluminationEstimate_getState(const ARSession_ *session,
                                          const ARIlluminationEstimate_ *light_estimate,
                                          ARIlluminationEstimateState *out_light_estimate_state) {
    if (out_light_estimate_state == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE) {
        *out_light_estimate_state = ARILLUMINATION_ESTIMATE_STATE_NOT_VALID;
        return;
    }
    *out_light_estimate_state = light_estimate->m_pImpl->getLightEstimateState();
}

void impl_arIlluminationEstimate_getMode(const ARSession_ *session,
                                         const ARIlluminationEstimate_ *light_estimate,
                                         ARIlluminationEstimateMode *out_light_estimate_state) {
    if (out_light_estimate_state == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        return;
    }

    *out_light_estimate_state = light_estimate->m_pImpl->getLightEstimateMode();
}

void impl_arIlluminationEstimate_getPixelIntensity(const ARSession_ *session,
                                                   const ARIlluminationEstimate_ *light_estimate,
                                                   float *out_pixel_intensity) {
    if (out_pixel_intensity == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        *out_pixel_intensity = 0;
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_AMBIENT_INTENSITY) {
        *out_pixel_intensity = 0;
        return;
    }

    *out_pixel_intensity = light_estimate->m_pImpl->getPixelIntensity();
}

void impl_arIlluminationEstimate_getSphericalHarmonicLighting(const ARSession_ *session,
                                                              const ARIlluminationEstimate_ *light_estimate,
                                                              float *out_sh_arr) {
    if (out_sh_arr == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        memset(out_sh_arr, 0, sizeof(float) * 27);
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR) {
        memset(out_sh_arr, 0, sizeof(float) * 27);
        return;
    }

    light_estimate->m_pImpl->getSphericalHarmonicLighting(out_sh_arr);

}

void impl_arIlluminationEstimate_getEnvironmentTextureSize(const ARSession_ *session,
                                                           const ARIlluminationEstimate_ *light_estimate,
                                                           int *width, int *height) {
    if (width == nullptr || height == nullptr)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        *width = 0;
        *height = 0;
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR) {
        *width = 0;
        *height = 0;
        return;
    }

    light_estimate->m_pImpl->getEnvironmentTextureSize(width, height);
}

void impl_arIlluminationEstimate_getColorCorrection(const ARSession_ *session,
                                                    const ARIlluminationEstimate_ *light_estimate,
                                                    float *out_color_correction_4) {
    if (out_color_correction_4 == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        memset(out_color_correction_4, 0, sizeof(float) * 4);
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_AMBIENT_INTENSITY) {
        memset(out_color_correction_4, 0, sizeof(float) * 4);
        return;
    }

    light_estimate->m_pImpl->getColorCorrection(out_color_correction_4);
}

void impl_arIlluminationEstimate_getEnvironmentalHdrMainLightDirection(const ARSession_ *session,
                                                                       const ARIlluminationEstimate_ *light_estimate,
                                                                       float *out_direction_3) {
    if (out_direction_3 == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        memset(out_direction_3, 0, sizeof(float) * 3);
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR) {
        memset(out_direction_3, 0, sizeof(float) * 3);
        out_direction_3[1] = 1;
        return;
    }
    light_estimate->m_pImpl->getEnvironmentalHdrMainLightDirection(out_direction_3);
}

void impl_arIlluminationEstimate_getEnvironmentalHdrMainLightIntensity(const ARSession_ *session,
                                                                       const ARIlluminationEstimate_ *light_estimate,
                                                                       float *out_intensity_3) {
    if (out_intensity_3 == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        memset(out_intensity_3, 0, sizeof(float) * 3);
        return;
    }

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR) {
        memset(out_intensity_3, 0, sizeof(float) * 3);
        return;
    }

    light_estimate->m_pImpl->getEnvironmentalHdrMainLightIntensity(out_intensity_3);
}

void impl_arIlluminationEstimate_acquireEnvironmentalHdrCubemap(const ARSession_ *session,
                                                                const ARIlluminationEstimate_ *light_estimate,
                                                                float *out_textures_6) {
    if (out_textures_6 == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        return;
    }

//    int width, height;
//    light_estimate->m_pImpl->getEnvironmentTextureSize(&width, &height);

    if (session->m_pImpl->m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE ||
        session->m_pImpl->m_Config.m_LightEstimationMode !=
        ARILLUMINATION_ESTIMATE_MODE_ENVIRONMENTAL_HDR) {
//        memset(out_textures_6, 0, sizeof(float) * width * height * 3 * 6);
        return;
    }
    light_estimate->m_pImpl->getEnvironmentalHdrCubemap(out_textures_6);

}

void impl_arIlluminationEstimate_getLightTimestamp(const ARSession_ *session,
                                                   const ARIlluminationEstimate_ *light_estimate,
                                                   int64_t *out_timestamp_ns) {
    if (out_timestamp_ns == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARIlluminationEstimateAvalid(light_estimate) == false) {
        return;
    }
    (*out_timestamp_ns) = light_estimate->m_pImpl->getLightTimeStamp();
}


void impl_arAnchor_getPose(const ARSession_ *session, const ARAnchor_ *anchor,
                           ARPose_ *out_pose) {
    if (out_pose == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARAnchorAvalid(anchor) == false) {
        return;
    }

    memcpy(out_pose, &(anchor->m_pImpl->m_Pose), sizeof(ARPose_));
}

void impl_arAnchor_getTrackingState(const ARSession_ *session, const ARAnchor_ *anchor,
                                    ARTrackingState *out_tracking_state) {
    if (out_tracking_state == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARAnchorAvalid(anchor) == false) {
        return;
    }

    *out_tracking_state = (ARTrackingState) anchor->m_pImpl->m_State;
}

void impl_arAnchor_detach(const ARSession_ *session, ARAnchor_ *anchor) {
    if (IsARSessionAvalid(session) == false || IsARAnchorAvalid(anchor) == false) {
        return;
    }

    session->m_pImpl->detachAnchor(anchor->m_pImpl);
}


void impl_arAnchor_release(ARAnchor_ *anchor) {
//    if(anchor == NULL)
//        return;
//
//    free(anchor);
}

void impl_arAnchor_getCloudAnchorId(const ARSession_ *session, const ARAnchor_ *anchor,
                                    char *out_anchor_id, int length) {
    if (IsARSessionAvalid(session) == false || IsARAnchorAvalid(anchor) == false || length < 64) {
        return;
    }

    std::string tempstr;
    ((CCloudAnchor *) anchor->m_pImpl)->getCloudAnchorId(tempstr);

    memset(out_anchor_id, '\0', length);
    memcpy(out_anchor_id, tempstr.c_str(), tempstr.size());
}

void impl_arAnchor_getCloudAnchorState(const ARSession_ *session, const ARAnchor_ *anchor,
                                       ARCloudAnchorState *out_state) {
    if (IsARSessionAvalid(session) == false || IsARAnchorAvalid(anchor) == false) {
        return;
    }

    *out_state = ((CCloudAnchor *) anchor->m_pImpl)->getCloudAnchorState();
}

// === ARAnchorList_ methods ===

void impl_arAnchorList_create(const ARSession_ *session, ARAnchorList_ **out_anchor_list) {
    if (out_anchor_list == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        (*out_anchor_list) = NULL;
        return;
    }

    ARAnchorList_ *plist = (ARAnchorList_ *) malloc(sizeof(ARAnchorList_));
    plist->m_pImpl = new CAnchorList();
    (*out_anchor_list) = plist;
}

void impl_arAnchorList_destroy(ARAnchorList_ *anchor_list) {
    if (anchor_list == NULL)
        return;

    if (anchor_list->m_pImpl) {
        delete anchor_list->m_pImpl;
        anchor_list->m_pImpl = NULL;
    }

    free(anchor_list);

}

void impl_arAnchorList_getSize(const ARSession_ *session, const ARAnchorList_ *anchor_list,
                               int32_t *out_size) {
    if (out_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARAnchorlistAvalid(anchor_list) == false) {
        *out_size = 0;
        return;
    }

    (*out_size) = (int32_t) (anchor_list->m_pImpl->m_List.size());
}

void impl_arAnchorList_acquireItem(const ARSession_ *session, const ARAnchorList_ *anchor_list,
                                   int32_t index, ARAnchor_ **out_anchor) {
    if (out_anchor == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARAnchorlistAvalid(anchor_list) == false ||
        (int32_t) (anchor_list->m_pImpl->m_List.size()) <= index) {
        *out_anchor = NULL;
        return;
    }

    CAnchor *pstAnchor = anchor_list->m_pImpl->m_List[index];
    if (!pstAnchor->m_Handle) {
        pstAnchor->m_Handle = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
        pstAnchor->m_Handle->m_pImpl = pstAnchor;
    }
    *out_anchor = pstAnchor->m_Handle;
}


void
impl_arHandGesture_getHandGestureType(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                      ARHandGestureType *out_ar_handgesture_type) {
    if (out_ar_handgesture_type == NULL)
        return;
    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;
//    (*out_ar_handgesture_type) = (ARHandGestureType)(handGesture->m_pImpl->m_gesture_type);
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    (*out_ar_handgesture_type) = static_cast<ARHandGestureType>(cHandGesture->getHandGestureType(
            resultWrapper.getResult<HandGestureAlgorithmResult *>()));
}

void impl_arHandGesture_getHandTypeConfidence(const ARSession_ *session,
                                              const ARHandGestureNode_ *handGesture,
                                              float *out_hand_type_confidence) {
//    LOGI("czz in get hand gesture hand type confidence");

    if (out_hand_type_confidence == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    // (*out_hand_type_confidence) = (float) (handGesture->m_pImpl->m_gesture_type_confidence);

//    LOGI("czz out get hand gesture hand type confidence");

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    (*out_hand_type_confidence) = cHandGesture->getHandTypeConfidence(
            resultWrapper.getResult<HandGestureAlgorithmResult *>());
}


void impl_arHandGesture_getHandSide(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                    ARHandType *out_ar_hand_side) {
//    LOGI("czz in get hand gesture hand side");

    if (out_ar_hand_side == NULL)
        return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;


    // (*out_ar_hand_side) =  (ARHandType)(handGesture->m_pImpl->m_side);

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    (*out_ar_hand_side) = static_cast<ARHandType >(cHandGesture->getHandSide(
            resultWrapper.getResult<HandGestureAlgorithmResult *>()));

//    LOGI("czz out get hand gesture hand type");
}

void impl_arHandGesture_getHandTowards(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                       ARHandTowards *out_ar_hand_towards) {
//    LOGI("czz in get hand gesture hand towards");
    if (out_ar_hand_towards == NULL) {
        return;
    }

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;


    // (*out_ar_hand_towards) = (ARHandTowards) (handGesture->m_pImpl->m_towards);

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    (*out_ar_hand_towards) = static_cast<ARHandTowards >(cHandGesture->getHandToward(
            resultWrapper.getResult<HandGestureAlgorithmResult *>()));

//    LOGI("czz out get hand gesture hand towards");
}


void
impl_arHandGesture_getPalmCenter(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                 float *out_palm_center_xyz) {
//    LOGI("czz in get hand gesture palm center");

    if (out_palm_center_xyz == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    //  memcpy(out_palm_center_xyz, handGesture->m_pImpl->m_palm_center, sizeof(float)*3);

//    LOGI("czz out get hand gesture palm center");
}

void
impl_arHandGesture_getPalmNormal(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                 float *out_palm_normal_xyz) {
//    LOGI("czz in get hand gesture palm normal");

    if (out_palm_normal_xyz == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    //  memcpy(out_palm_normal_xyz, handGesture->m_pImpl->m_palm_normal, sizeof(float)*3);

//    LOGI("czz out get hand gesture palm normal");
}

void
impl_arHandGesture_getLandMark2DCount(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                      int32_t *out_landmark_2d_count) {
//    LOGI("czz in get hand gesture landmark 2d count");

    if (out_landmark_2d_count == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    //   (*out_landmark_2d_count) = (int32_t) (handGesture->m_pImpl->m_landmark2d_count);

//    LOGI("czz out get hand gesture landmark 2d count");

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    (*out_landmark_2d_count) = cHandGesture->getLandMark2DCount(
            resultWrapper.getResult<HandGestureAlgorithmResult *>());
}

void
impl_arHandGesture_getLandMark2DArray(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                      float *out_landmark2d_array) {
//    LOGI("czz in get hand gesture landmark2d");

    if (out_landmark2d_array == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

//    memcpy(out_landmark2d_array, handGesture->m_pImpl->m_landmark_2D, sizeof(float)*handGesture->m_pImpl->m_landmark2d_count*2);


//    LOGI("czz out get hand gesture landmark2d");
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_HAND_GESTURE);

    CHandGesture *cHandGesture = handGesture->m_pImpl;
    cHandGesture->getLandMark2DArray(
            resultWrapper.getResult<HandGestureAlgorithmResult *>(), out_landmark2d_array);
}

void impl_arHandGesture_getLandMark3DCount(const ARSession_ *session,
                                           const ARHandGestureNode_ *handGesture,
                                           int32_t *out_landmark_3d_count) {
//    LOGI("czz in get hand gesture landmark 3d count");

    if (out_landmark_3d_count == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    //  (*out_landmark_3d_count) = (int32_t) (handGesture->m_pImpl->m_landmark3d_count);

//    LOGI("czz out get hand gesture landmark 3d count");
}

void impl_arHandGesture_getLandMark3DArray(const ARSession_ *session,
                                           const ARHandGestureNode_ *handGesture,
                                           float *out_landmark3d_array) {
//    LOGI("czz in get hand gesture landmark3d");

    if (out_landmark3d_array == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    memcpy(out_landmark3d_array, handGesture->m_pImpl->m_landmark_3D,
           sizeof(float) * handGesture->m_pImpl->m_landmark3d_count * 3);


//    LOGI("czz out get hand gesture landmark3d");
}


void impl_arHandGesture_getRect(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                int *out_rect) {
//    LOGI("czz in get hand gesture bbox rect");

    if (out_rect == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    out_rect[0] = (int32_t) (handGesture->m_pImpl->m_rect_left);
    out_rect[1] = (int32_t) (handGesture->m_pImpl->m_rect_top);
    out_rect[2] = (int32_t) (handGesture->m_pImpl->m_rect_right);
    out_rect[3] = (int32_t) (handGesture->m_pImpl->m_rect_bottom);


//    LOGI("czz out get hand gesture bbox rect");
}


void
impl_arHandGesture_getRGBSegArray(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                  char *out_RGBSeg_array) {
//    LOGI("czz in get hand gesture rgb seg array");

    if (out_RGBSeg_array == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    memcpy(out_RGBSeg_array, handGesture->m_pImpl->m_rgb_seg_array,
           sizeof(char) * handGesture->m_pImpl->m_rgb_seg_width *
           handGesture->m_pImpl->m_rgb_seg_height);

//    LOGI("czz out get hand gesture rgb seg array");

}

void
impl_arHandGesture_getRGBSegWidth(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                  int32_t *out_RGBSeg_width) {
//    LOGI("czz in get hand gesture rgb seg array");

    if (out_RGBSeg_width == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    (*out_RGBSeg_width) = (int32_t) (handGesture->m_pImpl->m_rgb_seg_width);

//    LOGI("czz out get hand gesture rgb seg array");
}

void
impl_arHandGesture_getRGBSegHeight(const ARSession_ *session, const ARHandGestureNode_ *handGesture,
                                   int32_t *out_RGBSeg_height) {
//    LOGI("czz in get hand gesture rgb seg array");

    if (out_RGBSeg_height == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARHandGestureAvalid(handGesture) == false)
        return;

    (*out_RGBSeg_height) = (int32_t) (handGesture->m_pImpl->m_rgb_seg_height);

//    LOGI("czz out get hand gesture rgb seg array");
}

// === ARNodeList_ methods ===

void impl_arNodeList_create(const ARSession_ *session,
                            ARNodeList_ **out_node_list) {
    if (out_node_list == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        (*out_node_list) = NULL;
        return;
    }

    ARNodeList_ *plist = (ARNodeList_ *) malloc(sizeof(ARNodeList_));
    plist->m_pImpl = new CTrackableList();
    (*out_node_list) = plist;
}

void impl_arNodeList_destroy(ARNodeList_ *node_list) {
    if (node_list == NULL)
        return;

    if (node_list->m_pImpl) {
        delete node_list->m_pImpl;
        node_list->m_pImpl = NULL;
    }

    free(node_list);
}

void impl_arNodeList_getSize(const ARSession_ *session,
                             const ARNodeList_ *node_list,
                             int32_t *out_size) {
    if (out_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARNodeListAvalid(node_list) == false) {
        *out_size = 0;
        return;
    }

    (*out_size) = (int32_t) (node_list->m_pImpl->m_List.size());
}

void impl_arNodeList_acquireItem(const ARSession_ *session,
                                 const ARNodeList_ *node_list,
                                 int32_t index, ARNode_ **out_node) {
    if (out_node == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARNodeListAvalid(node_list) == false ||
        (int32_t) (node_list->m_pImpl->m_List.size()) <= index) {
        *out_node = NULL;
        return;
    }
    ITrackable *pstTrackable = node_list->m_pImpl->m_List[index];
    if (pstTrackable) {
        if (!pstTrackable->m_Handle) {
            pstTrackable->m_Handle = (ARNode_ *) malloc(sizeof(ARNode_));
            pstTrackable->m_Handle->m_pImpl = pstTrackable;
        }
        *out_node = pstTrackable->m_Handle;
        return;
    }
    *out_node = NULL;
}

// === ARNode_ methods ===

void impl_arNode_release(ARNode_ *node) {
//    if(node == NULL)
//        return;
//
//    free(node);
}

void impl_arNode_getType(const ARSession_ *session, const ARNode_ *node,
                         ARNodeType *out_node_type) {
    if (out_node_type == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARNodeAvalid(node) == false) {
        *out_node_type = ARNODE_TYPE_UNKNOWN;
        return;
    }

    *out_node_type = (ARNodeType) (node->m_pImpl->getType());
}


void impl_arNode_getTrackingState(const ARSession_ *session,
                                  const ARNode_ *node,
                                  ARTrackingState *out_tracking_state) {
    if (out_tracking_state == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARNodeAvalid(node) == false) {
        *out_tracking_state = ARTRACKING_STATE_STOPPED;
        return;
    }

    *out_tracking_state = ARTRACKING_STATE_SUCCESS;
}

ARResult impl_arNode_acquireNewAnchor(ARSession_ *session,
                                      ARNode_ *node,
                                      ARPose_ *pose,
                                      ARAnchor_ **out_anchor) {
    if (out_anchor == NULL)return ARRESULT_ERROR_INVALID_ARGUMENT;

    if (IsARSessionAvalid(session) == false || IsARNodeAvalid(node) == false) {
        *out_anchor = NULL;
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    }

    CPose stpose;
    if (pose != NULL)
        ArPoseTostPose(*pose, stpose);

    CAnchor *pstAnchor = session->m_pImpl->acquireNewAnchor(stpose, node->m_pImpl);
    ARAnchor_ *paranchor = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
    paranchor->m_pImpl = pstAnchor;
    pstAnchor->m_Handle = paranchor;

    *out_anchor = paranchor;

    return ARRESULT_SUCCESS;
}

void impl_arNode_getAnchors(const ARSession_ *session,
                            const ARNode_ *node,
                            ARAnchorList_ *out_anchor_list) {
    if (IsARSessionAvalid(session) == false || IsARNodeAvalid(node) == false) {
        return;
    }
    session->m_pImpl->getAnchorsByTrackable(node->m_pImpl, out_anchor_list->m_pImpl->m_List);
}

// === ARPlaneNode_ methods ===

void impl_arPlaneNode_getType(const ARSession_ *session, const ARPlaneNode_ *plane,
                              ARPlaneType *out_plane_type) {
    if (out_plane_type == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    //(*out_plane_type) = AR_PLANE_HORIZONTAL_UPWARD_FACING;
    CPlane *pPlane = (CPlane *) (plane->m_pImpl);
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    (*out_plane_type) = pPlane->getPlaneType(resultWrapper.getResult<SLAMAlgorithmResult *>(),
                                             session->m_pImpl->m_Config.m_AxisUpMode);
//    (*out_plane_type) = session->m_pImpl->getPlaneType(pPlane);
}

void impl_arPlaneNode_getNormal(const ARSession_ *session, const ARPlaneNode_ *plane,
                                float *out_plane_normal) {
    if (out_plane_normal == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    CPlane *pPlane = (CPlane *) (plane->m_pImpl);
//    session->m_pImpl->getPlaneNormal(pPlane, out_plane_normal);
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_SLAM);
    pPlane->getPlaneNormal(resultWrapper.getResult<SLAMAlgorithmResult *>(), out_plane_normal);
}

void impl_arPlaneNode_getCenterPose(const ARSession_ *session, const ARPlaneNode_ *plane,
                                    ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);

        CPose stpose;
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        pPlane->getCenterPose(resultWrapper.getResult<SLAMAlgorithmResult *>(), stpose);
        memcpy(out_pose, &stpose, sizeof(ARPose_));
    }
}

void impl_arPlaneNode_getExtentX(const ARSession_ *session, const ARPlaneNode_ *plane,
                                 float *out_extent_x) {
    if (out_extent_x == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        (*out_extent_x) = pPlane->getExtentX(resultWrapper.getResult<SLAMAlgorithmResult *>());
//        (*out_extent_x) = session->m_pImpl->getExtentX(pPlane);
    } else {
        (*out_extent_x) = 0.0f;
    }
}

void impl_arPlaneNode_getExtentY(const ARSession_ *session, const ARPlaneNode_ *plane,
                                 float *out_extent_y) {
    if (out_extent_y == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        (*out_extent_y) = pPlane->getExtentY(resultWrapper.getResult<SLAMAlgorithmResult *>());
//        (*out_extent_y) = session->m_pImpl->getExtentY(pPlane);
    } else {
        (*out_extent_y) = 0.0f;
    }
}

void impl_arPlaneNode_getExtentZ(const ARSession_ *session, const ARPlaneNode_ *plane,
                                 float *out_extent_z) {
    if (out_extent_z == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }
    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        (*out_extent_z) = pPlane->getExtentZ(resultWrapper.getResult<SLAMAlgorithmResult *>());
    } else {
        (*out_extent_z) = 0.0f;
    }
}

void impl_arPlaneNode_getPolygonSize(const ARSession_ *session, const ARPlaneNode_ *plane,
                                     int32_t *out_polygon_size) {
    if (out_polygon_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        *out_polygon_size = pPlane->getPolygonSize(
                resultWrapper.getResult<SLAMAlgorithmResult *>());
//        *out_polygon_size = session->m_pImpl->getPolygonSize(pPlane);
    } else {
        *out_polygon_size = 0;
    }

}

void impl_arPlaneNode_getPolygon(const ARSession_ *session, const ARPlaneNode_ *plane,
                                 float *out_polygon_xz) {
    if (out_polygon_xz == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        pPlane->getPolygon(resultWrapper.getResult<SLAMAlgorithmResult *>(), out_polygon_xz);
//        session->m_pImpl->getPolygon(pPlane, out_polygon_xz);

    }

}

void impl_arPlaneNode_getPolygon3DSize(const ARSession_ *session, const ARPlaneNode_ *plane,
                                       int32_t *out_polygon_size) {
    if (out_polygon_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        *out_polygon_size = 0;
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        *out_polygon_size = pPlane->getPolygon3DSize(
                resultWrapper.getResult<SLAMAlgorithmResult *>());
//        *out_polygon_size = session->m_pImpl->getPolygon3DSize(pPlane);
    } else {
        *out_polygon_size = 0;
    }
}

void impl_arPlaneNode_getPolygon3D(const ARSession_ *session, const ARPlaneNode_ *plane,
                                   float *out_polygon_xyz) {
    if (out_polygon_xyz == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        pPlane->getPolygon3D(resultWrapper.getResult<SLAMAlgorithmResult *>(), out_polygon_xyz);
//        session->m_pImpl->getPolygon3D(pPlane, out_polygon_xyz);
    }
}

void impl_arPlaneNode_acquireSubsumedBy(const ARSession_ *session, const ARPlaneNode_ *plane,
                                        ARPlaneNode_ **out_subsumed_by) {
    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    *out_subsumed_by = const_cast<ARPlaneNode_ *>(plane);
}

void impl_arPlaneNode_isPoseInExtents(const ARSession_ *session,
                                      const ARPlaneNode_ *plane,
                                      const ARPose_ *pose,
                                      int32_t *out_pose_in_extents) {
    if (!pose || !out_pose_in_extents)
        return;
    if (!IsARSessionAvalid(session) || !IsARPlaneNodeAvalid(plane))
        return;

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pstPlane = (CPlane *) plane->m_pImpl;
        CPose stpose;
        ArPoseTostPose(*pose, stpose);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        (*out_pose_in_extents) = pstPlane->isPoseInExtents(
                resultWrapper.getResult<SLAMAlgorithmResult *>(), stpose);
//        (*out_pose_in_extents) = session->m_pImpl->isPoseInExtents(pstPlane, stpose);
    } else {
        (*out_pose_in_extents) = 0;
    }
}

void impl_arPlaneNode_isPoseInPolygon(const ARSession_ *session, const ARPlaneNode_ *plane,
                                      const ARPose_ *pose, int32_t *out_pose_in_polygon) {
    if (pose == NULL || out_pose_in_polygon == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARPlaneNodeAvalid(plane) == false) {
        return;
    }

    if (plane->m_pImpl->getType() == ARNODE_TYPE_PLANE) {
        CPlane *pPlane = (CPlane *) (plane->m_pImpl);

        CPose stpose;
        ArPoseTostPose((*pose), stpose);
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_SLAM);
        (*out_pose_in_polygon) = pPlane->isPoseInPolygon(
                resultWrapper.getResult<SLAMAlgorithmResult *>(), stpose);
//        (*out_pose_in_polygon) = session->m_pImpl->isPoseInPolygon(pPlane, stpose);
    } else {
        (*out_pose_in_polygon) = 0;
    }
}


// === ArAugmentedImage methods ===
void impl_arAugmentedImage_getCenterPose(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                         ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false)
        return;

    if (augmented_image->m_pImpl->getType() == ARNODE_TYPE_IMAGE) {
        CImageNode *pMarker = augmented_image->m_pImpl;

        CPose cpose;
        int screen_rotate = session->m_pImpl->getScreenRotate();
        float viewArray[16];
        session->m_pImpl->getViewMatrix(viewArray);

        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_IMAGE_TRACKING);
        pMarker->getCenterPose(resultWrapper.getResult<MarkerAlgorithmResult *>(), screen_rotate,
                               viewArray, cpose);
        memcpy(out_pose, &cpose, sizeof(ARPose_));
    }
}

void impl_arAugmentedImage_getExtentX(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                      float *out_extent_x) {
    if (out_extent_x == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false)
        return;

    if (augmented_image->m_pImpl->getType() == ARNODE_TYPE_IMAGE) {
        CImageNode *pMarker = augmented_image->m_pImpl;
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_IMAGE_TRACKING);
        (*out_extent_x) = pMarker->getExtentX(resultWrapper.getResult<MarkerAlgorithmResult *>());
    } else {
        (*out_extent_x) = 0.0f;
    }
}

void impl_arAugmentedImage_getExtentY(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                      float *out_extent_y) {
    if (out_extent_y == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false)
        return;

    if (augmented_image->m_pImpl->getType() == ARNODE_TYPE_IMAGE) {
        CImageNode *pMarker = augmented_image->m_pImpl;
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_IMAGE_TRACKING);
        (*out_extent_y) = pMarker->getExtentY(resultWrapper.getResult<MarkerAlgorithmResult *>());
    } else {
        (*out_extent_y) = 0.0f;
    }
}

void impl_arAugmentedImage_getExtentZ(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                      float *out_extent_z) {
    if (out_extent_z == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false) {
        return;
    }

    if (augmented_image->m_pImpl->getType() == ARNODE_TYPE_IMAGE) {
        CImageNode *pMarker = augmented_image->m_pImpl;
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_IMAGE_TRACKING);
        (*out_extent_z) = pMarker->getExtentZ(resultWrapper.getResult<MarkerAlgorithmResult *>());
    } else {
        (*out_extent_z) = 0.0f;
    }
}

void impl_arAugmentedImage_getIndex(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                    int32_t *out_index) {
    if (out_index == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false) {
        return;
    }

    *out_index = augmented_image->m_pImpl->m_MarkerId;
}

void impl_arAugmentedImage_acquireName(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                       char **out_augmented_image_name) {
    if (out_augmented_image_name == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false) {
        return;
    }

    if (augmented_image->m_pImpl->m_Name.length() > 0) {
        *out_augmented_image_name = new char[augmented_image->m_pImpl->m_Name.length() + 1];
        strcpy(*out_augmented_image_name, augmented_image->m_pImpl->m_Name.c_str());
        (*out_augmented_image_name)[augmented_image->m_pImpl->m_Name.length()] = '\0';
    } else {
        *out_augmented_image_name = nullptr;
    }
}

void impl_arAugmentedImage_get2DCorners(const ARSession_ *session, const ARImageNode_ *augmented_image,
                                        float *out_2d_corners) {
    if (out_2d_corners == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false) {
        return;
    }

    if (augmented_image->m_pImpl->getType() == ARNODE_TYPE_IMAGE) {
        CImageNode *pMarker = augmented_image->m_pImpl;
        AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
                ARALGORITHM_TYPE_IMAGE_TRACKING);
        pMarker->get2DCorners(resultWrapper.getResult<MarkerAlgorithmResult *>(), out_2d_corners);
    }
}

void impl_arAugmentedImage_getGlobalID(const ARSession_ *session, const ARImageNode_ *augmented_image, char **out_global_id)
{
    if (out_global_id == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARImageNodeAvalid(augmented_image) == false) {
        return;
    }

    if (augmented_image->m_pImpl->m_GlobalId.length() > 0) {
        *out_global_id = new char[augmented_image->m_pImpl->m_GlobalId.length() + 1];
        strcpy(*out_global_id, augmented_image->m_pImpl->m_GlobalId.c_str());
        (*out_global_id)[augmented_image->m_pImpl->m_GlobalId.length()] = '\0';
    } else {
        *out_global_id = nullptr;
    }
}

// === ArAugmentedImageDatabase methods ===
void impl_arAugmentedImageDatabase_create(
        const ARSession_ *session,
        ARReferenceImageDatabase_ **out_augmented_image_database) {
    if (out_augmented_image_database == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        return;
    }

    ARReferenceImageDatabase_ *imageNodeMgr = (ARReferenceImageDatabase_ *) malloc(
            sizeof(ARReferenceImageDatabase_));
    imageNodeMgr->m_pImpl = new CImageNodeMgr();
    *out_augmented_image_database = imageNodeMgr;
}


ARResult impl_arAugmentedImageDatabase_deserialize(
        const ARSession_ *session,
        ARReferenceImageDatabase_ *augmented_image_database,
        const uint8_t *database_raw_bytes,
        int64_t database_raw_bytes_size) {
    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return ARRESULT_ERROR_FATAL;

    augmented_image_database->m_pImpl->deserialize(database_raw_bytes, database_raw_bytes_size);
    return ARRESULT_SUCCESS;
}


void impl_arAugmentedImageDatabase_serialize(
        const ARSession_ *session,
        const ARReferenceImageDatabase_ *augmented_image_database,
        uint8_t **out_image_database_raw_bytes,
        int64_t *out_image_database_raw_bytes_size) {
    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return;

    //session->m_pImpl->serializeAugImgDatabase(augmented_image_database->m_pImpl, out_image_database_raw_bytes, out_image_database_raw_bytes_size);
}

void impl_arAugmentedImageDatabase_loadConfigure(const ARSession_ *session,
                                                 const ARReferenceImageDatabase_ *augmented_image_database,
                                                 const char *config) {
    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return;

    augmented_image_database->m_pImpl->loadConfigure(config);
}

ARResult impl_arAugmentedImageDatabase_addPatt(const ARSession_ *session,
                                               const ARReferenceImageDatabase_ *augmented_image_database,
                                               const char *patt_name, const uint8_t *patt_raw_bytes,
                                               int64_t patt_raw_bytes_size) {
    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return ARRESULT_ERROR_FATAL;

    augmented_image_database->m_pImpl->addPatt(patt_name, patt_raw_bytes, patt_raw_bytes_size);
    return ARRESULT_SUCCESS;
}


ARResult impl_arAugmentedImageDatabase_addImageWithPhysicalSize(
        const ARSession_ *session,
        ARReferenceImageDatabase_ *augmented_image_database,
        const char *image_name,
        const uint8_t *image_grayscale_pixels,
        int32_t image_width_in_pixels,
        int32_t image_height_in_pixels,
        int32_t image_stride_in_pixels,
        float image_width_in_meters,
        int32_t *out_index) {
    if (out_index == NULL)return ARRESULT_ERROR_FATAL;

    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return ARRESULT_ERROR_FATAL;

    augmented_image_database->m_pImpl->addImage(image_name, image_grayscale_pixels,
                                                image_width_in_pixels,
                                                image_height_in_pixels, image_stride_in_pixels,
                                                image_width_in_meters,
                                                out_index);
    return ARRESULT_SUCCESS;
}

void impl_arAugmentedImageDatabase_getNumImages(const ARSession_ *session,
                                                const ARReferenceImageDatabase_ *augmented_image_database,
                                                int32_t *out_num_images) {
    if (out_num_images == NULL)return;

    if (IsARSessionAvalid(session) == false ||
        IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return;

    *out_num_images = augmented_image_database->m_pImpl->getImageNum();
}

void impl_arAugmentedImageDatabase_destroy(ARReferenceImageDatabase_ *augmented_image_database) {
    if (IsARReferenceImageDatabaseAvalid(augmented_image_database) == false)
        return;

    if (augmented_image_database->m_pImpl) {
        delete augmented_image_database->m_pImpl;
        augmented_image_database->m_pImpl = NULL;
    }

    free(augmented_image_database);
}

// === ARObjectNode methods ===
void impl_arObjectNode_getCenterPose(const ARSession_ *session, const ARObjectNode_ *object_node,
                                     ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    CPose cpose;
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    object_node->m_pImpl->getCenterPose(resultWrapper.getResult<ObjectTrackingAlgorithmResult *>(),
                                        cpose);
    memcpy(out_pose, &cpose, sizeof(ARPose_));
}

void impl_arObjectNode_getBoundingBox(const ARSession_ *session, const ARObjectNode_ *object_node,
                                      float *out_boundingBox) {
    if (out_boundingBox == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    object_node->m_pImpl->getBoundingBox(resultWrapper.getResult<ObjectTrackingAlgorithmResult *>(),
                                         out_boundingBox);
}

void impl_arObjectNode_getExtentX(const ARSession_ *session, const ARObjectNode_ *object_node,
                                  float *out_extent_x) {
    if (out_extent_x == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    *out_extent_x = object_node->m_pImpl->getExtentX(
            resultWrapper.getResult<ObjectTrackingAlgorithmResult *>());
}

void impl_arObjectNode_getExtentY(const ARSession_ *session, const ARObjectNode_ *object_node,
                                  float *out_extent_y) {
    if (out_extent_y == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    *out_extent_y = object_node->m_pImpl->getExtentY(
            resultWrapper.getResult<ObjectTrackingAlgorithmResult *>());
}

void impl_arObjectNode_getExtentZ(const ARSession_ *session, const ARObjectNode_ *object_node,
                                  float *out_extent_z) {
    if (out_extent_z == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_OBJECT_TRACKING);
    *out_extent_z = object_node->m_pImpl->getExtentZ(
            resultWrapper.getResult<ObjectTrackingAlgorithmResult *>());
}

void impl_arObjectNode_getIndex(const ARSession_ *session, const ARObjectNode_ *object_node,
                                int32_t *out_index) {
    if (out_index == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    *out_index = object_node->m_pImpl->getIndex();
}

void impl_arObjectNode_getName(const ARSession_ *session, const ARObjectNode_ *object_node,
                               char **out_object_name) {
    if (out_object_name == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARObjectNodeAvalid(object_node) == false)
        return;

    std::string name = object_node->m_pImpl->getName();
    if (name.length() > 0) {
        *out_object_name = new char[name.length() + 1];
        strcpy(*out_object_name, name.c_str());
        (*out_object_name)[name.length()] = '\0';
    } else {
        *out_object_name = nullptr;
    }
}

// === ARReferenceObjectDatabase methods ===
void impl_arReferenceObjectDatabase_create(const ARSession_ *session,
                                           ARReferenceObjectDatabase_ **out_object_database) {
    if (out_object_database == NULL)return;

    if (IsARSessionAvalid(session) == false)
        return;

    ARReferenceObjectDatabase_ *objectDatabase = (ARReferenceObjectDatabase_ *) malloc(
            sizeof(ARReferenceObjectDatabase_));
    objectDatabase->m_pImpl = new CReferenceObjectDatabase();
    *out_object_database = objectDatabase;
}

ARResult impl_arReferenceObjectDatabase_deserialize(const ARSession_ *session,
                                                    ARReferenceObjectDatabase_ *object_database,
                                                    const uint8_t *database_raw_bytes,
                                                    int64_t database_raw_bytes_size) {
    if (database_raw_bytes == NULL)
        return ARRESULT_ERROR_FATAL;

    if (IsARSessionAvalid(session) == false ||
        IsARReferenceObjectDatabaseAvalid(object_database) == false)
        return ARRESULT_ERROR_FATAL;

    object_database->m_pImpl->deserialize(database_raw_bytes, database_raw_bytes_size);
    return ARRESULT_SUCCESS;
}

ARResult impl_arReferenceObjectDatabase_addObject(const ARSession_ *session,
                                                  ARReferenceObjectDatabase_ *object_database,
                                                  const char *object_name,
                                                  const uint8_t *object_raw_bytes,
                                                  int64_t object_raw_bytes_size,
                                                  int32_t *out_index) {
    if (out_index == NULL)
        return ARRESULT_ERROR_FATAL;

    if (IsARSessionAvalid(session) == false ||
        IsARReferenceObjectDatabaseAvalid(object_database) == false)
        return ARRESULT_ERROR_FATAL;

    object_database->m_pImpl->addReferenceObject(object_name, object_raw_bytes,
                                                 object_raw_bytes_size, out_index);
    return ARRESULT_SUCCESS;
}

void impl_arReferenceObjectDatabase_getNumObjects(const ARSession_ *session,
                                                  const ARReferenceObjectDatabase_ *object_database,
                                                  int32_t *out_num_objects) {
    if (out_num_objects == NULL)
        return;

    if (IsARSessionAvalid(session) == false ||
        IsARReferenceObjectDatabaseAvalid(object_database) == false)
        return;

    *out_num_objects = object_database->m_pImpl->getObjectNum();
}

void impl_arReferenceObjectDatabase_destroy(ARReferenceObjectDatabase_ *object_database) {
    if (IsARReferenceObjectDatabaseAvalid(object_database) == false)
        return;

    if (object_database->m_pImpl) {
        delete object_database->m_pImpl;
        object_database->m_pImpl = NULL;
    }

    free(object_database);
}

// === ARQueryResultList_ methods ===

void impl_arQueryResultList_create(const ARSession_ *session,
                                   ARQueryResultList_ **out_hit_result_list) {
    if (out_hit_result_list == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        *out_hit_result_list = NULL;
        return;
    }

    ARQueryResultList_ *presultlist = (ARQueryResultList_ *) malloc(sizeof(ARQueryResultList_));
    presultlist->m_pImpl = new CHitResultList();
    *out_hit_result_list = presultlist;
}

void impl_arQueryResultList_destroy(ARQueryResultList_ *hit_result_list) {
    if (hit_result_list == NULL)
        return;

    if (hit_result_list->m_pImpl) {
        delete hit_result_list->m_pImpl;
        hit_result_list->m_pImpl = NULL;
    }

    free(hit_result_list);
}

void impl_arQueryResultList_getSize(const ARSession_ *session,
                                    const ARQueryResultList_ *hit_result_list,
                                    int32_t *out_size) {
    if (out_size == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARQueryResultListAvalid(hit_result_list) == false) {
        *out_size = 0;
        return;
    }

    *out_size = (int32_t) hit_result_list->m_pImpl->m_List.size();
}

void impl_arQueryResultList_getItem(const ARSession_ *session,
                                    const ARQueryResultList_ *hit_result_list,
                                    int32_t index, ARQueryResult_ *out_hit_result) {
    if (out_hit_result == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARQueryResultListAvalid(hit_result_list) == false) {
        return;
    }

    if ((int32_t) (hit_result_list->m_pImpl->m_List.size()) <= index) {
        return;
    }

    out_hit_result->m_pImpl = hit_result_list->m_pImpl->m_List[index];
}

// === ARQueryResult_ methods ===

void impl_arQueryResult_create(const ARSession_ *session, ARQueryResult_ **out_hit_result) {
    if (out_hit_result == NULL)return;

    if (IsARSessionAvalid(session) == false) {
        *out_hit_result = NULL;
        return;
    }

    ARQueryResult_ *presult = (ARQueryResult_ *) malloc(sizeof(ARQueryResult_));
    presult->m_pImpl = NULL;
    *out_hit_result = presult;
}

void impl_arQueryResult_destroy(ARQueryResult_ *hit_result) {
    if (hit_result == NULL)return;

    free(hit_result);
}

void impl_arQueryResult_getDistance(const ARSession_ *session,
                                    const ARQueryResult_ *hit_result,
                                    float *out_distance) {
    if (!IsARSessionAvalid(session) || !IsARQueryResultAvalid(hit_result))
        return;

    *out_distance = session->m_pImpl->getDistanceFromCamera(session->m_pImpl, hit_result->m_pImpl);

}

void impl_arQueryResult_getHitPose(const ARSession_ *session,
                                   const ARQueryResult_ *hit_result, ARPose_ *out_pose) {

    if (IsARSessionAvalid(session) == false || IsARQueryResultAvalid(hit_result) == false) {
        return;
    }

    const CPose &stpose = hit_result->m_pImpl->getHitPose();

    out_pose->qx = stpose.qx;
    out_pose->qy = stpose.qy;
    out_pose->qz = stpose.qz;
    out_pose->qw = stpose.qw;
    out_pose->tx = stpose.tx;
    out_pose->ty = stpose.ty;
    out_pose->tz = stpose.tz;
}

void impl_arQueryResult_acquireNode(const ARSession_ *session,
                                    const ARQueryResult_ *hit_result,
                                    ARNode_ **out_node) {
    if (out_node == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARQueryResultAvalid(hit_result) == false) {
        *out_node = NULL;
        return;
    }

    ITrackable *psttrackable = hit_result->m_pImpl->getTrackable();
    if (psttrackable) {
        if (!psttrackable->m_Handle) {
            psttrackable->m_Handle = (ARNode_ *) malloc(sizeof(ARNode_));
            psttrackable->m_Handle->m_pImpl = psttrackable;
        }
        (*out_node) = psttrackable->m_Handle;
    } else {
        (*out_node) = NULL;
    }
}

ARResult impl_arQueryResult_acquireNewAnchor(ARSession_ *session,
                                             ARQueryResult_ *hit_result,
                                             ARAnchor_ **out_anchor) {
    if (out_anchor == NULL)return ARRESULT_ERROR_INVALID_ARGUMENT;

    if (IsARSessionAvalid(session) == false || IsARQueryResultAvalid(hit_result) == false) {
        *out_anchor = NULL;
        return ARRESULT_ERROR_INVALID_ARGUMENT;
    }

    CPose stpose = hit_result->m_pImpl->getHitPose();
    ITrackable *ptrackable = hit_result->m_pImpl->getTrackable();
    CAnchor *pstanchor = session->m_pImpl->acquireNewAnchor(stpose, ptrackable);

    if (!pstanchor->m_Handle) {
        pstanchor->m_Handle = (ARAnchor_ *) malloc(sizeof(ARAnchor_));
        pstanchor->m_Handle->m_pImpl = pstanchor;
    }
    (*out_anchor) = pstanchor->m_Handle;
    return ARRESULT_SUCCESS;
}


void impl_arFaceMesh_getCenterPose(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                   ARPose_ *out_pose) {
    if (out_pose == NULL)return;

    if (IsARSessionAvalid(session) == false || IsARFaceMeshAvalid(arFaceMeshNode) == false)
        return;
    CPose cpose;
    float poseArr[16];

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_FACE_MESH);
    ArFaceMesh *arFaceMesh = arFaceMeshNode->m_pImpl;
    arFaceMesh->getPose(resultWrapper.getResult<FaceMeshAlgorithmResult *>(), poseArr);

    mat4<float> result(poseArr);

    cpose.tx = result.at(3, 0);
    cpose.ty = result.at(3, 1);
    cpose.tz = result.at(3, 2);
    Quatf q = Quatf::frommat(result);
    cpose.qw = q.w, cpose.qx = q.v.x, cpose.qy = q.v.y, cpose.qz = q.v.z;

    memcpy(out_pose, &cpose, sizeof(ARPose_));

}

void impl_arFaceMesh_getVertices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                 float **out_vertices, int32_t *out_number_of_vertices) {
    if (IsARSessionAvalid(session) == false || IsARFaceMeshAvalid(arFaceMeshNode) == false)
        return;
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_FACE_MESH);
    ArFaceMesh *arFaceMesh = arFaceMeshNode->m_pImpl;
    FaceMeshAlgorithmResult * result =resultWrapper.getResult<FaceMeshAlgorithmResult *>();
    *out_number_of_vertices = arFaceMesh->getVertexCount(result);
    *out_vertices =arFaceMesh->getVertices(result);
}

void impl_arFaceMesh_getNormals(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                float **out_normals, int32_t *out_number_of_normals) {
    if (IsARSessionAvalid(session) == false || IsARFaceMeshAvalid(arFaceMeshNode) == false)
        return;
//    *out_normals = arFaceMeshNode->m_pImpl->normals;
//    *out_number_of_normals = arFaceMeshNode->m_pImpl->normal_count;
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_FACE_MESH);
    ArFaceMesh *arFaceMesh = arFaceMeshNode->m_pImpl;
    FaceMeshAlgorithmResult * result =resultWrapper.getResult<FaceMeshAlgorithmResult *>();
    *out_number_of_normals = arFaceMesh->getNormalCount(result);
    *out_normals =arFaceMesh->getNormals(result);

}

void
impl_arFaceMesh_getTextureCoordinates(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                      float **out_texture_coordinates,
                                      int32_t *out_number_of_texture_coordinates) {
    if (IsARSessionAvalid(session) == false || IsARFaceMeshAvalid(arFaceMeshNode) == false)
        return;
//    *out_texture_coordinates = arFaceMeshNode->m_pImpl->texture_coordinates;
//    *out_number_of_texture_coordinates = arFaceMeshNode->m_pImpl->texture_coordinates_count;
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_FACE_MESH);
    ArFaceMesh *arFaceMesh = arFaceMeshNode->m_pImpl;
    FaceMeshAlgorithmResult * result =resultWrapper.getResult<FaceMeshAlgorithmResult *>();
    *out_number_of_texture_coordinates = arFaceMesh->getTextureCoordinateCount(result);
    *out_texture_coordinates =arFaceMesh->getTextureCoordinates(result);
}

void impl_arFaceMesh_getTriangleIndices(const ARSession *session, const ARFaceMeshNode *arFaceMeshNode,
                                        int **out_triangle_indices,
                                        int32_t *out_number_of_triangles) {
    if (IsARSessionAvalid(session) == false || IsARFaceMeshAvalid(arFaceMeshNode) == false)
        return;
//    *out_triangle_indices = arFaceMeshNode->m_pImpl->triangle_indices;
//    *out_number_of_triangles = arFaceMeshNode->m_pImpl->triangle_indices_count;

    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_FACE_MESH);
    ArFaceMesh *arFaceMesh = arFaceMeshNode->m_pImpl;
    FaceMeshAlgorithmResult * result =resultWrapper.getResult<FaceMeshAlgorithmResult *>();
    *out_number_of_triangles = arFaceMesh->getIndexCount(result);
    *out_triangle_indices =arFaceMesh->getIndices(result);
}


void impl_arBodyDetect_getBodyId(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode,
                                 int32_t *out_body_id) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
  //  *out_body_id = arBodyDetectNode->m_pImpl->getBodyId();
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_BODY_DETECT);
    CBodyDetectNode *cBodyDetectNode = arBodyDetectNode->m_pImpl;
    *out_body_id = cBodyDetectNode->getBodyId();
}


void impl_arBodyDetect_getSkeletonPoint2dCount(const ARSession *session,
                                               const ARBodyDetectNode *arBodyDetectNode,
                                               int32_t *out_point_count) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
    // *out_point_count = arBodyDetectNode->m_pImpl->getSkeletonPointCount();
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_BODY_DETECT);
    CBodyDetectNode *cBodyDetectNode = arBodyDetectNode->m_pImpl;
    *out_point_count = cBodyDetectNode->getSkeletonPoint2dCount(
            resultWrapper.getResult<BodyDetectAlgorithmResult *>());

}

void
impl_arBodyDetect_getSkeletonPoint2d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode,
                                     float *out_point2d) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
    // arBodyDetectNode->m_pImpl->getSkeletonPoint2D(out_point2D);
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_BODY_DETECT);
    CBodyDetectNode *cBodyDetectNode = arBodyDetectNode->m_pImpl;
    cBodyDetectNode->getSkeletonPoint2d(resultWrapper.getResult<BodyDetectAlgorithmResult *>(),
                                        out_point2d);

}

void impl_arBodyDetect_getSkeletonPoint2dConfidence(const ARSession *session,
                                                    const ARBodyDetectNode *arBodyDetectNode,
                                                    float *out_point2d_confidence) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
   // arBodyDetectNode->m_pImpl->getSkeletonPoint2dConfidence(out_point2d_confidence);
    AlgorithmResultWrapper resultWrapper = session->m_pImpl->getAlgorithmResult(
            ARALGORITHM_TYPE_BODY_DETECT);
    CBodyDetectNode *cBodyDetectNode = arBodyDetectNode->m_pImpl;
    cBodyDetectNode->getSkeletonPoint2dConfidence(resultWrapper.getResult<BodyDetectAlgorithmResult *>(),
                                        out_point2d_confidence);
}


void impl_arBodyDetect_getSkeletonPoint3dCount(const ARSession *session,
                                               const ARBodyDetectNode *arBodyDetectNode,
                                               int32_t *out_point3d_count) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
    *out_point3d_count = arBodyDetectNode->m_pImpl->getSkeletonPoint3dCount();

}

void
impl_arBodyDetect_getSkeletonPoint3d(const ARSession *session, const ARBodyDetectNode *arBodyDetectNode,
                                     float *out_point3d) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
    arBodyDetectNode->m_pImpl->getSkeletonPoint3d(out_point3d);
}

void impl_arBodyDetect_getSkeletonPoint3dConfidence(const ARSession *session,
                                                    const ARBodyDetectNode *arBodyDetectNode,
                                                    float *out_point3d_confidence) {
    if (IsARSessionAvalid(session) == false || IsARBodyDetectAvalid(arBodyDetectNode) == false)
        return;
    arBodyDetectNode->m_pImpl->getSkeletonPoint3dConfidence(out_point3d_confidence);
}


// === timewarp & prediction related methods ===
ARResult impl_arSession_beginRenderEye(const ARSession_ *session, int eyeId) {
    if (!IsARSessionAvalid(session)) {
        return ARRESULT_ERROR_FATAL;;
    }
    session->m_pImpl->beginRenderEye(eyeId);
    return ARRESULT_SUCCESS;
}

ARResult impl_arSession_endRenderEye(const ARSession_ *session, int eyeId) {
    if (!IsARSessionAvalid(session)) {
        return ARRESULT_ERROR_FATAL;
    }
    session->m_pImpl->endRenderEye(eyeId);
    return ARRESULT_SUCCESS;
}

ARResult impl_arSession_submitFrame(const ARSession_ *session,
                                  const int32_t leftEyeTextureId, const int32_t rightEyeTextureId) {
    if (!IsARSessionAvalid(session)) {
        return ARRESULT_ERROR_FATAL;
    }
    session->m_pImpl->submitRenderFrame(leftEyeTextureId, rightEyeTextureId);
    return ARRESULT_SUCCESS;
}

ARResult impl_arSession_setWindow(const ARSession_ *session, void *window) {
    if (!IsARSessionAvalid(session)) {
        return ARRESULT_ERROR_FATAL;
    }
    if (window == nullptr) {
        JNIEnvProxy jEnv = ArJniHelpler::getJavaEnv();
        jobject surface = GetViewSurface(jEnv.get(), gAppContext);
        ANativeWindow *nWindow = ANativeWindow_fromSurface(jEnv.get(), surface);
        LOGI("window not set, get nativewindow %d", nWindow == nullptr);
        session->m_pImpl->setWindow(nWindow, true);
        return ARRESULT_SUCCESS;
    }
    session->m_pImpl->setWindow(window, false);
    return ARRESULT_SUCCESS;
}

void impl_arSession_getPredictedDisplayTime(const ARSession_ *session, uint64_t *time) {
    if (!IsARSessionAvalid(session)) {
        *time = UINT64_MAX;
        return;
    }
}

void impl_arCamera_getPredictedViewMatrix(const ARSession_ *session, const ARCamera_ *camera,
                                          float *out_col_major_4x4) {
    if (out_col_major_4x4 == NULL) {
        return;
    }

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

//    session->m_pImpl->getPredictedViewMatrix(out_col_major_4x4, 0, prediction_source);
    session->m_pImpl->getPredictedViewMatrix(out_col_major_4x4);
}

void impl_arCamera_getProjectionMatrixSeeThrough(const ARSession_ *session, const ARCamera_ *camera,
                                                 float near, float far, float *dest_col_major_4x4) {
    if (dest_col_major_4x4 == NULL) {
        return;
    }

    if (IsARSessionAvalid(session) == false || IsARCameraAvalid(camera) == false) {
        return;
    }

    session->m_pImpl->getProjectionMatrixSeeThrough(near, far, dest_col_major_4x4);
}


}

/// @}

