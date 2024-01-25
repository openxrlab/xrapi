#include <GLES2/gl2.h>
#include <jni.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <android/log.h>
#include <sstream>
#include <set>
#include <MarkerAlgorithmResult.h>
#include "ArSession.h"
#include "ArCommon.h"
#include "StandardARCommon.h"
#include "StandardAR.h"
#include "ArAnchor.h"
#include "ArTrackable.h"
#include "ArPlane.h"
#include "ArMath.h"
#include "ArFrame.h"
#include "ArHitResult.h"
#include "OffscreenRenderTarget.h"
#include "ArEngineJavaClient.h"
#include "ArJniHelpler.h"
#include "picojson.h"
#include "time.h"
#include "ArImageNode.h"
#include "ArImageNodeMgr.h"
#include "miniz.h"
#include "SLAMAlgorithmResult.h"
#include "HandGestureAlgorithmResult.h"
#include "ArFaceMesh.h"
#include "FaceMeshAlgorithmResult.h"
#include "BodyDetectAlgorithmResult.h"
#include "ArJniHelpler.h"
#include "ArWorldLifeCycle.h"
#include "DynamicLoader.h"
#include "ObjectTrackingAlgorithmResult.h"
extern "C" {
#define GL_UNPACK_ROW_LENGTH_EXT          0x0CF2
const static char* standardar_version = "2.4.0.2";
//      tracking_status_detail
//          bit0        :       VIO_INITIALIZING
//          bit1        :       BE_INITIALIZING
//          bit2        :       VIO_TRACKING_FAIL
//          bit3        :       BE_TRACKING_FAIL
//          bit4-31     :       reserved

}

standardar::CSession* g_internal_standard_ar_session = NULL;

namespace standardar
{
    CSession::CSession(void* env, void* context, const char* package_name, int64_t sessionHandle)
    {
        m_appContext = ArJniHelpler::getJavaEnv()->NewGlobalRef(static_cast<jobject>(context));
        m_Mutex = PTHREAD_MUTEX_INITIALIZER;
        m_cloud_Mutex = PTHREAD_MUTEX_INITIALIZER;
        m_Image_Mutex = PTHREAD_MUTEX_INITIALIZER;

        m_pEnv = env;

        m_Config.m_PackageName = package_name;
        //m_Config.m_PackageName = "com.google.ar.core.examples.c.helloar";

        m_Version = -2;

        m_Config.m_CameraID = ARCAMERA_ID_BACK_RGB_CAMERA0;

        m_ScreenRotate = 0;
        m_ScreenWidth = 1920;
        m_ScreenHeight = 1080;
        m_VideoFovX = 60.0f;
        m_VideoFovY = 37.0f;
        m_ScreenAspect = 1.6666667f;

        m_OESTextureId = 0;
        m_VideoWidth = 1280;
        m_VideoHeight = 720;
        m_VideoAspect = 1.6666667f;
        m_CameraConfig = CCameraConfig(m_VideoWidth, m_VideoHeight);

        m_ProcessWidth = 0;
        m_ProcessHeight = 0;

        m_BeginTime = getCurrentSecond();
        m_CurrentTime = getCurrentSecond();
        m_VideoFrameCount = 0;
        m_VideoFPS = 0.0f;

        m_ImageFormat = ARPIXEL_FORMAT_UNKNOWN;
        m_ImageStride = 0;

        m_pArEngine = nullptr;

        m_pAuxRender = NULL;
        m_pBgYUVRender = NULL;

        m_RGBTexture = 0;
        m_pRenderToTexture = NULL;
        m_pFlipTexture = NULL;

        m_SessionPtr = sessionHandle;
        m_IsFirstResume = false;

        m_anchorupdate_count = 0;
        m_anchorupdate_freq = 30;

        m_FrameTimestamp = 0;
        m_result_callback = nullptr;

//        m_MapId[0] = '\0';
        m_AppKey = "";
        m_AppSecret = "";
//        m_CurrentAnchor = nullptr;

        m_isSupportDoubleStream = isArServiceSupportDoubleStream(context);
        m_isArServiceExist = isArServiceExist(m_pEnv, context);
        m_isArServiceSingleAlgorithm = isArServiceSingleAlgorithm(context);
        initAlgorithmResult();

        m_bAlgorithmStart = false;
        m_bSessionResume = false;

        m_isCameraIntrinsicsUpdate = false;
        m_LifeCycle = new ArWorldLifeCycle(this);
    }

    CSession::~CSession()
    {
        destory();
        pthread_mutex_destroy(&m_Mutex);
    }

    int32_t CSession::getVideoWidth()
    {
        if (m_isSupportDoubleStream)
            return m_ProcessWidth;
        return m_VideoWidth;
    }

    int32_t CSession::getVideoHeight()
    {
        if(m_isSupportDoubleStream)
            return m_ProcessHeight;

        return m_VideoHeight;
    }

    ARPixelFormat CSession::getVideoFormat()
    {
        return m_ImageFormat;
    }

    bool CSession::isImgBufferDirty() const
    {
        if (m_pArEngine == nullptr)
            return false;

        return m_pArEngine->isImgBufferDirty();
    }

    bool CSession::isAlgorithmStart()
    {
        return m_bAlgorithmStart;
    }

    void CSession::getUpdatedTrackables(std::vector<ITrackable *> &plist, ARNodeType type)
    {
        ScopeMutex m(&m_Mutex);
        if(type == ARNODE_TYPE_PLANE) {
            for (auto citer = m_Planes.begin(); citer != m_Planes.end(); citer++) {
                ITrackable *plane = citer->second;
                if (plane->m_isUpdated && plane->getType() == type) {
                    plist.push_back(plane);
                    plane->m_isUpdated = false;
                }
            }
        }

        if(type == ARNODE_TYPE_IMAGE) {
            for (auto citer = m_Markers.begin(); citer != m_Markers.end(); citer++) {
                ITrackable *marker = citer->second;
                if (marker->m_isUpdated && marker->getType() == type) {
                    plist.push_back(marker);
                    marker->m_isUpdated = false;
                }
            }
        }
        if(type == ARNODE_TYPE_OBJECT) {
            for (auto citer = m_Objects.begin(); citer != m_Objects.end(); citer++) {
                ITrackable *objects = citer->second;
                if (objects->m_isUpdated && objects->getType() == type) {
                    plist.push_back(objects);
                    objects->m_isUpdated = false;
                }
            }
        }
    }

    void CSession::getUpdatedAnchors(std::vector<CAnchor *> &plist)
    {
        ScopeMutex m(&m_Mutex);
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            CAnchor* anchor = citer->second;
            if (anchor->m_isUpdated)
            {
                plist.push_back(anchor);
                anchor->m_isUpdated = false;
            }
        }
    }

    void CSession::getAnchorsByTrackable(ITrackable *trackable, std::vector<CAnchor *> &plist)
    {
        ScopeMutex mutext(&m_Mutex);
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            CAnchor* anchor = citer->second;
            if (anchor->m_Trackable == trackable)
                plist.push_back(anchor);
        }
    }

    ITrackable* CSession::getTrackableById(int32_t id)
    {
        auto citer = m_Planes.find(id);
        if(citer == m_Planes.end())
            return NULL;
        else
            return citer->second;
    }

    float CSession::getVerticalFov()
    {
        if(m_ScreenRotate==CFrame::ROTATION_0 ||
           m_ScreenRotate==CFrame::ROTATION_180)
        {
            return m_VideoFovX;
        }

        return m_VideoFovY;
    }

    int CSession::getScreenRotate()
    {
        return m_ScreenRotate;
    }

    bool CSession::intialize()
    {
        LOGI("AREngineInit");
        if (m_pArEngine)
            m_pArEngine->initialize();

        return true;
    }

    void CSession::destory()
    {
        {
            ScopeMutex m1(&m_Image_Mutex);
            SAFE_DELETE_TEXTURE(m_RGBTexture)
            StandardAR_SAFE_DELETE(m_pAuxRender)
            StandardAR_SAFE_DELETE(m_pBgYUVRender)
            StandardAR_SAFE_DELETE(m_pRenderToTexture)
            StandardAR_SAFE_DELETE(m_pFlipTexture)
        }

        {
            ScopeMutex m(&m_Mutex);
            clearAnchor();
            clearTrackable();
            clearFrames();
        }

        m_AlgorithmResultPack.deleteAlgorithmResult();
        if(m_pArEngine)
        {
            m_pArEngine->arSendCommand(AR_CMD_DESTROY_ALGORITM, 0, 0);
            m_pArEngine->destroy();
            m_pArEngine = NULL;
        }

    }

    void CSession::clearFrames()
    {
        auto citer = m_Frames.begin();
        for(; citer!=m_Frames.end(); citer++)
        {
            CFrame* pframe = citer->second;
            if(pframe != NULL)
            {
                pframe->clearSLAMResult();
                pframe->m_Handle->m_pImpl = NULL;
                delete pframe;
            }
        }
        m_Frames.clear();
    }

    CFrame* CSession::createFrame(CSession* pSession)
    {

        CFrame* pstFrame = new CFrame();
        pstFrame->setSession(pSession);
        addFrame(pstFrame);

        return pstFrame;
    }

    void CSession::clearResult()
    {
//        memset(m_CachedBuf, 0, m_CachedSize);
//        memset(m_ResultBuff, 0, m_ResultBuffSize);
//        memset(&m_SLAMResult, 0, sizeof(m_SLAMResult));
//        memset(&m_DenseReconResult, 0, sizeof(m_DenseReconResult));
//
//        m_SLAMJsonStr = "";
//        m_SLAMInfo = "";
//        m_initProgress = 0;
        clearTrackable();
        clearAnchor();
        m_AlgorithmResultPack.clearAlgorithmResult();
        auto citer = m_Frames.begin();
        for(; citer!=m_Frames.end(); citer++)
        {
            CFrame* pframe = citer->second;
            if(pframe != NULL)
            {
                pframe->clearSLAMResult();
            }
        }
    }

    void CSession::clearAnchor()
    {
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            if(citer->second != NULL)
            {
                if (citer->second->m_Handle)
                    citer->second->m_Handle->m_pImpl = NULL;
                delete citer->second;
            }
        }
        m_Anchors.clear();
    }

    void CSession::clearTrackable()
    {
        for(auto citer = m_Planes.begin(); citer!=m_Planes.end(); citer++)
        {
            if(citer->second != NULL)
            {
                if (citer->second->m_Handle)
                    citer->second->m_Handle->m_pImpl = NULL;
                delete citer->second;
            }
        }
        m_Planes.clear();

        m_Markers.clear();

        m_Objects.clear();

    }

    void CSession::resetResult()
    {
        resetAnchor();
        resetTrackable();
        m_AlgorithmResultPack.clearAlgorithmResult();
    }

    void CSession::resetAnchor()
    {
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            if(citer->second != NULL)
            {
                citer->second->m_State = ARTRACKING_STATE_LOST;
            }
        }
    }

    void CSession::resetTrackable()
    {
        for(auto citer = m_Planes.begin(); citer!=m_Planes.end(); citer++)
        {
            if(citer->second != NULL)
            {
                citer->second->m_State = ARTRACKING_STATE_LOST;
            }
        }
    }

    bool CSession::getDisplayPortraitSize(int& width, int&height)
    {
        if(m_pArEngine== nullptr)
            return false;

        m_pArEngine->arGetDisplayInfo(width, height);

        if(width>height)
            std::swap(width, height);

        return true;
    }

    void CSession::getSupportedCameraConfigs(CCameraConfigList& list)
    {
        if(m_pArEngine == nullptr)
            return;

        const char *sizestr = m_pArEngine->arGetSupportPreviewSize();
        if (sizestr) {
            std::vector<vec2i> videosizes;
            parseSizesList(sizestr, videosizes);
            for (int i = 0; i < videosizes.size(); ++i) {
                list.m_List.push_back(new CCameraConfig(videosizes[i].x, videosizes[i].y));
            }
        }
    }

    int CSession::getCameraTextureTarget()
    {
        return m_Config.m_CameraTextureTarget;
    }

    void CSession::setCameraTextureTarget(int type)
    {
        m_Config.m_CameraTextureTarget = (ARStreamMode) type;
    }

    int32_t CSession::checkSupported(const CConfig& config)
    {
        return true;
    }

    ARResult CSession::configurate(const CConfig& config)
    {
        if (config.m_CloudAnchorMode == ARALGORITHM_MODE_ENABLE && config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            LOGW("can not enabled cloud anchor feature without slam!");
            return ARRESULT_ERROR_INVALID_ARGUMENT;
        }
        if (config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_ENABLE && config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            LOGW("can not enabled cloud anchor feature without slam!");
            return ARRESULT_ERROR_INVALID_ARGUMENT;
        }

        {
            ScopeMutex m(&m_Mutex);
            updateConfig(config);
        }

        createArEngine();

        return ARRESULT_SUCCESS;
    }

    void CSession::updateConfig(const CConfig& config)
    {
        std::set<ARAlgorithmType> disable_types;
        std::set<ARAlgorithmType> enable_types;

        if(config.m_SLAMMode != m_Config.m_SLAMMode){
            disable_types.insert(config.m_SLAMMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_SLAM: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_SLAMMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_SLAM: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_DenseReconMode != m_Config.m_DenseReconMode){
            disable_types.insert(config.m_DenseReconMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_DENSE_RECON: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_DenseReconMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_DENSE_RECON: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_ArMarkerMode != m_Config.m_ArMarkerMode){
            disable_types.insert(config.m_ArMarkerMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_IMAGE_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_ArMarkerMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_IMAGE_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_HandGestureMode != m_Config.m_HandGestureMode){
            disable_types.insert(config.m_HandGestureMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_HAND_GESTURE: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_HandGestureMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_HAND_GESTURE: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_FaceMeshMode != m_Config.m_FaceMeshMode){
            disable_types.insert(config.m_FaceMeshMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_FACE_MESH: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_FaceMeshMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_FACE_MESH: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_BodyDetectMode != m_Config.m_BodyDetectMode){
            disable_types.insert(config.m_BodyDetectMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_BODY_DETECT: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_BodyDetectMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_BODY_DETECT: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_ObjectTrackingMode != m_Config.m_ObjectTrackingMode){
            disable_types.insert(config.m_ObjectTrackingMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_OBJECT_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_ObjectTrackingMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_OBJECT_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_ObjectScanningMode != m_Config.m_ObjectScanningMode){
            disable_types.insert(config.m_ObjectScanningMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_OBJECT_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_ObjectScanningMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_OBJECT_TRACKING: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_PlaneAlgorithmMode != m_Config.m_PlaneAlgorithmMode){
            disable_types.insert(config.m_PlaneAlgorithmMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_PLANE_DETECTION: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_PlaneAlgorithmMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_PLANE_DETECTION: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_LightEstimateAlgorithmMode != m_Config.m_LightEstimateAlgorithmMode){
            disable_types.insert(config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_CloudAnchorMode != m_Config.m_CloudAnchorMode){
            disable_types.insert(config.m_CloudAnchorMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_CLOUD_ANCHOR: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_CloudAnchorMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_CLOUD_ANCHOR: ARALGORITHM_TYPE_UNKNOWN);
        }

        if(config.m_MapCoupleAlgorithmMode != m_Config.m_MapCoupleAlgorithmMode){
            disable_types.insert(config.m_MapCoupleAlgorithmMode == ARALGORITHM_MODE_DISABLE? ARALGORITHM_TYPE_MAP_COUPLE: ARALGORITHM_TYPE_UNKNOWN);
            enable_types.insert(config.m_MapCoupleAlgorithmMode == ARALGORITHM_MODE_ENABLE? ARALGORITHM_TYPE_MAP_COUPLE: ARALGORITHM_TYPE_UNKNOWN);
        }

        if (disable_types.size() == 0 && enable_types.size() == 0) {
            if (m_Config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_ENABLE) {
                if (m_Config.m_LightEstimationMode != config.m_LightEstimationMode) {
                    m_pArEngine->setLightEstimateMode(config.m_LightEstimationMode);
                }
            }
            if (m_Config.m_MapCoupleAlgorithmMode == ARALGORITHM_MODE_ENABLE) {
                if (m_Config.m_MapCouple_Mode != config.m_MapCouple_Mode) {
                    m_pArEngine->arSendCommand(AR_CMD_SET_MODE, config.m_MapCouple_Mode, 0.0f);
                }
            }
            m_Config = config;
            return;
        }

        if (disable_types.size() == 1) {
            ARAlgorithmType type = *(disable_types.begin());
            if (type == ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION) {
                m_pArEngine->setLightEstimateMode(m_Config.m_LightEstimateAlgorithmMode);
                disable_types.clear();
                m_Config = config;
                return;
            }
            if (type == ARALGORITHM_TYPE_MAP_COUPLE) {
                m_pArEngine->arSendCommand(AR_CMD_SET_MODE, config.m_MapCoupleAlgorithmMode, 0.0f);
                disable_types.clear();
                m_Config = config;
                return;
            }
        }

        //判断与slam相关的算法
        if (m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE) {
            disable_types.insert(ARALGORITHM_TYPE_SLAM);
        }
        bool startSLAM = false;
        for (auto enable_type : enable_types) {
            if (enable_type == ARALGORITHM_TYPE_CLOUD_ANCHOR ||
                enable_type == ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION ||
                enable_type == ARALGORITHM_TYPE_DENSE_RECON ||
                enable_type == ARALGORITHM_TYPE_OBJECT_TRACKING
                ) {
                startSLAM = true;
            }
        }
        if (startSLAM || config.m_SLAMMode == ARALGORITHM_MODE_ENABLE) {
            enable_types.insert(ARALGORITHM_TYPE_SLAM);
        }

        m_Config = config;
        m_Config.chooseStreamMode(m_isSupportDoubleStream);

        if (m_bSessionResume) {
            for (auto disable_type : disable_types) {
                if (disable_type == ARALGORITHM_TYPE_UNKNOWN) {
                    continue;
                }
                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, disable_type, 0.0f);
                AlgorithmResultWrapper resultWrapper(&m_AlgorithmResultPack, disable_type);
                AlgorithmResult* algorithmResult = resultWrapper.getResult<AlgorithmResult*>();
                if(algorithmResult)
                    algorithmResult->setState(false);
                switch (disable_type) {
                    case ARALGORITHM_TYPE_SLAM:
                        for (auto citer = m_Planes.begin(); citer != m_Planes.end(); citer++) {
                            if (citer->second != NULL) {
                                if (citer->second->m_Handle)
                                    citer->second->m_Handle->m_pImpl = NULL;
                                delete citer->second;
                            }
                        }
                        m_Planes.clear();
                        clearAnchor();
                        break;
                    case ARALGORITHM_TYPE_HAND_GESTURE:
                        break;
                    case ARALGORITHM_TYPE_DENSE_RECON:
                        break;
                    case ARALGORITHM_TYPE_IMAGE_TRACKING:
                        m_Markers.clear();
                        break;
                    case ARALGORITHM_TYPE_PLANE_DETECTION:
                        break;
                    case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION:
                        m_pArEngine->setLightEstimateMode(m_Config.m_LightEstimationMode);
                        break;
                    case ARALGORITHM_TYPE_CLOUD_ANCHOR:
                        break;
                    case ARALGORITHM_TYPE_FACE_MESH:
                        break;
                    case ARALGORITHM_TYPE_OBJECT_TRACKING:
                        m_Objects.clear();
                        break;
                    case ARALGORITHM_TYPE_OBJECT_SCANNING:
                        break;
                    case ARALGORITHM_TYPE_BODY_DETECT:
                        break;
                    case ARALGORITHM_TYPE_MAP_COUPLE:
                        m_pArEngine->arSendCommand(AR_CMD_SET_MODE, config.m_MapCouple_Mode, 0.0f);
                        break;
                    default:
                        break;
                }
                AlgorithmResult *result = m_AlgorithmResultPack.getAlgorithmResult(disable_type);
                if (result)
                    result->cleanResult();
            }

            if (!disable_types.empty() && !enable_types.empty()) {
                //destroy active algorithm and disable all algorithm
                m_pArEngine->arSendCommand(AR_CMD_DESTROY_ALGORITM, 0, 0);
                pause();
                m_IsFirstResume = false;

                resume();

                for (auto enable_type : enable_types) {
                    m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, enable_type, 0.0f);
                    AlgorithmResultWrapper resultWrapper(&m_AlgorithmResultPack, enable_type);
                    AlgorithmResult* algorithmResult = resultWrapper.getResult<AlgorithmResult*>();
                    if(algorithmResult)
                        algorithmResult->setState(true);
                    switch (enable_type) {
                        case ARALGORITHM_TYPE_IMAGE_TRACKING: {
                            MarkerAlgorithmResult *marker_result = (MarkerAlgorithmResult *) m_AlgorithmResultPack.getAlgorithmResult(
                                    ARALGORITHM_TYPE_IMAGE_TRACKING);
                            marker_result->sendReferenceImageDatabase();
                            break;
                        }
                        case ARALGORITHM_TYPE_OBJECT_TRACKING: {
                            ObjectTrackingAlgorithmResult *object_result = (ObjectTrackingAlgorithmResult *) m_AlgorithmResultPack.getAlgorithmResult(
                                    ARALGORITHM_TYPE_OBJECT_TRACKING);
                            object_result->sendReferenceObjectDatabase();
                            break;
                        }
                        case ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION: {
                            m_pArEngine->setLightEstimateMode(m_Config.m_LightEstimationMode);
                            AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(
                                    ARALGORITHM_TYPE_SLAM);
                            SLAMAlgorithmResult *slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult *>();
                            slamAlgorithmResult->m_LightMode = m_Config.m_LightEstimationMode;
                            break;
                        }
                        case ARALGORITHM_TYPE_MAP_COUPLE: {
                            m_pArEngine->arSendCommand(AR_CMD_SET_MODE, config.m_MapCouple_Mode, 0.0f);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
        
    }

    void CSession::initAlgorithmResult()
    {
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_SLAM, new SLAMAlgorithmResult());
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_HAND_GESTURE, new HandGestureAlgorithmResult());
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_IMAGE_TRACKING, new MarkerAlgorithmResult());
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_FACE_MESH, new FaceMeshAlgorithmResult());
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_BODY_DETECT, new BodyDetectAlgorithmResult());
        m_AlgorithmResultPack.addAlgorithmResult(ARALGORITHM_TYPE_OBJECT_TRACKING, new ObjectTrackingAlgorithmResult());
    }

    ARResult CSession::setCameraConfig(const CCameraConfig& config)
    {
        m_CameraConfig = config;
        return ARRESULT_SUCCESS;
    }

    ARResult CSession::startAlgorithm()
    {
        if(m_pArEngine)
        {
            pthread_mutex_lock(&m_Mutex);
            if(m_bAlgorithmStart == false)
            {
                m_bAlgorithmStart = true;

                pthread_mutex_unlock(&m_Mutex);
                LOGI("start algorithm");
                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_SLAM, 0.0f);
                AlgorithmResultWrapper slamAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_SLAM);
                SLAMAlgorithmResult* slamAlgorithmResult = slamAlgorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
                slamAlgorithmResult->setState(m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE);

                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_IMAGE_TRACKING, 0.0f);
                AlgorithmResultWrapper markerAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_IMAGE_TRACKING);
                MarkerAlgorithmResult* markerAlgorithmResult = markerAlgorithmResultWrapper.getResult<MarkerAlgorithmResult*>();
                markerAlgorithmResult->setState(m_Config.m_ArMarkerMode == ARALGORITHM_MODE_ENABLE);

                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_HAND_GESTURE, 0.0f);
                AlgorithmResultWrapper handAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_HAND_GESTURE);
                HandGestureAlgorithmResult* handAlgorithmResult = handAlgorithmResultWrapper.getResult<HandGestureAlgorithmResult*>();
                handAlgorithmResult->setState(m_Config.m_HandGestureMode == ARALGORITHM_MODE_ENABLE);

                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_FACE_MESH, 0.0f);
                AlgorithmResultWrapper faceAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_FACE_MESH);
                FaceMeshAlgorithmResult* faceAlgorithmResult = faceAlgorithmResultWrapper.getResult<FaceMeshAlgorithmResult*>();
                faceAlgorithmResult->setState(m_Config.m_FaceMeshMode == ARALGORITHM_MODE_ENABLE);

                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_BODY_DETECT, 0.0f);
                AlgorithmResultWrapper bodyAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_BODY_DETECT);
                BodyDetectAlgorithmResult* bodyDetectAlgorithmResult = bodyAlgorithmResultWrapper.getResult<BodyDetectAlgorithmResult*>();
                bodyDetectAlgorithmResult->setState(m_Config.m_BodyDetectMode == ARALGORITHM_MODE_ENABLE);


                m_pArEngine->arSendCommand(AR_CMD_START_ALGORITHM, ARALGORITHM_TYPE_OBJECT_TRACKING, 0.0f);
                AlgorithmResultWrapper objectTrackingAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_OBJECT_TRACKING);
                ObjectTrackingAlgorithmResult* objectTrackingAlgorithmResult = objectTrackingAlgorithmResultWrapper.getResult<ObjectTrackingAlgorithmResult*>();
                objectTrackingAlgorithmResult->setState(m_Config.m_ObjectTrackingMode == ARALGORITHM_MODE_ENABLE);

                if(m_Config.m_ArMarkerMode == ARALGORITHM_MODE_ENABLE) {
                    MarkerAlgorithmResult* marker_result = (MarkerAlgorithmResult*)m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_IMAGE_TRACKING);
                    marker_result->sendReferenceImageDatabase();
                }

                if(m_Config.m_ObjectTrackingMode== ARALGORITHM_MODE_ENABLE) {
                    ObjectTrackingAlgorithmResult* object_result = (ObjectTrackingAlgorithmResult*)m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_OBJECT_TRACKING);
                    object_result->sendReferenceObjectDatabase();
                }
            }
            else
            {
                pthread_mutex_unlock(&m_Mutex);
            }
        }

        return ARRESULT_SUCCESS;
    }

    ARResult CSession::stopAlgorithm()
    {
        if(m_pArEngine)
        {
            pthread_mutex_lock(&m_Mutex);
            if(m_bAlgorithmStart == true)
            {
                LOGI("stop algorithm");
                m_bAlgorithmStart = false;

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_SLAM, 0.0f);
                AlgorithmResultWrapper slamAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_SLAM);
                SLAMAlgorithmResult* slamAlgorithmResult = slamAlgorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
                slamAlgorithmResult->setState(false);
                slamAlgorithmResult->m_MapId[0] = '\0';

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_IMAGE_TRACKING, 0.0f);
                AlgorithmResultWrapper markerAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_IMAGE_TRACKING);
                MarkerAlgorithmResult* markerAlgorithmResult = markerAlgorithmResultWrapper.getResult<MarkerAlgorithmResult*>();
                markerAlgorithmResult->setState(false);

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_HAND_GESTURE, 0.0f);
                AlgorithmResultWrapper handAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_HAND_GESTURE);
                HandGestureAlgorithmResult* handAlgorithmResult = handAlgorithmResultWrapper.getResult<HandGestureAlgorithmResult*>();
                handAlgorithmResult->setState(false);

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_FACE_MESH, 0.0f);
                AlgorithmResultWrapper faceAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_FACE_MESH);
                FaceMeshAlgorithmResult* faceAlgorithmResult = faceAlgorithmResultWrapper.getResult<FaceMeshAlgorithmResult*>();
                faceAlgorithmResult->setState(false);

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_BODY_DETECT, 0.0f);

                m_pArEngine->arSendCommand(AR_CMD_STOP_ALGORITHM, ARALGORITHM_TYPE_OBJECT_TRACKING, 0.0f);
                AlgorithmResultWrapper objectTrackingAlgorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_OBJECT_TRACKING);
                ObjectTrackingAlgorithmResult* objectTrackingAlgorithmResult = objectTrackingAlgorithmResultWrapper.getResult<ObjectTrackingAlgorithmResult*>();
                objectTrackingAlgorithmResult->setState(false);

                m_AppKey = "";
                m_AppSecret = "";

                clearResult();
                pthread_mutex_unlock(&m_Mutex);

            }
            else
            {
                pthread_mutex_unlock(&m_Mutex);
            }
        }

        return ARRESULT_SUCCESS;
    }

    void CSession::setAlgorithmProxy()
    {
        SLAMAlgorithmResult* slam_result = (SLAMAlgorithmResult*)m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        slam_result->setConfig(m_Config);
        slam_result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_SLAM, m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE));


        AlgorithmResult* result = m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_HAND_GESTURE);
        result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_HAND_GESTURE, m_Config.m_HandGestureMode == ARALGORITHM_MODE_ENABLE));

        result = m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_IMAGE_TRACKING);
        result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_IMAGE_TRACKING, m_Config.m_ArMarkerMode == ARALGORITHM_MODE_ENABLE));


        result = m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_FACE_MESH);
        result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_FACE_MESH, m_Config.m_FaceMeshMode == ARALGORITHM_MODE_ENABLE));


        result = m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_BODY_DETECT);
        result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_BODY_DETECT, m_Config.m_BodyDetectMode == ARALGORITHM_MODE_ENABLE));

        result = m_AlgorithmResultPack.getAlgorithmResult(ARALGORITHM_TYPE_OBJECT_TRACKING);
        result->setAlgorithmCore(m_pArEngine->setEnable(ARALGORITHM_TYPE_SLAM, m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE));

    }

    ARResult CSession::resume()
    {
        if(m_pArEngine== nullptr)
            return ARRESULT_ERROR_FATAL;

        LOGI("resume package:%s", m_Config.m_PackageName.c_str());
        g_internal_standard_ar_session = this;

        if (m_pArEngine) {
            m_pArEngine->arStoreData(STR_SESSION, &m_SessionPtr);
        }

        //open camera and imu sensor
        int32_t hresult = m_pArEngine->arEngineInit(m_Version, m_Config.m_PackageName.c_str(), m_Config, m_result_callback);
        if(hresult != ARRESULT_SUCCESS)
        {
            LOGE("AREngineInit package:%s, return failed:%i", m_Config.m_PackageName.c_str(), hresult);
            return (ARResult)hresult;
        }
        LOGD("AREngineInit package:%s cameraId:%d success", m_Config.m_PackageName.c_str(), m_Config.m_CameraID);

        if(m_IsFirstResume) {
            m_pArEngine->arSendCommand(AR_CMD_SET_RESUME_FLAG, 0, 0);
        }
        //get display and preview information to
        displayInfoConfigure();
        //enable algorithm and store native pointer in java code
        setAlgorithmProxy();
        //start camera preview and register imu sensor listener
        m_pArEngine->arEngineStart();

        LOGI("sdk version is %s", standardar_version);

        if (!m_IsFirstResume)
        {
            LOGD("init algorithm");
            m_pArEngine->arSendCommand(AR_CMD_INIT_ALGORITHM, 0, 0);
            m_IsFirstResume = true;
        }

        if (m_OESTextureId != 0) {
            m_pArEngine->arStoreData(STR_TEXID, (void *) (&m_OESTextureId));
        }
        m_bSessionResume = true;
        return ARRESULT_SUCCESS;
    }

    void CSession::displayInfoConfigure()
    {
        int displayWidth = 0;
        int displayHeight = 0;
        m_pArEngine->arGetDisplayInfo(displayWidth, displayHeight);
        LOGD("display width:%i, height:%i\n", displayWidth, displayHeight);

        m_pArEngine->arGetPreviewSize(m_VideoWidth, m_VideoHeight);
        m_pArEngine->arGetProcessSize(m_ProcessWidth, m_ProcessHeight);
        m_CameraConfig.m_Width = m_VideoWidth;
        m_CameraConfig.m_Height = m_VideoHeight;
        LOGD("get preview size:%i, %i", m_VideoWidth, m_VideoHeight);

        m_pArEngine->arSendCommand(AR_CMD_SET_FOCUS_MODE, 0, 0.0f);
        m_pArEngine->arSendCommand(100, m_Config.m_WorldAlignmentMode, 0.0f);
        m_pArEngine->arSendCommand(101, m_Config.m_WorldOriginMode, 0.0f);

        m_pArEngine->arGetCameraFovH(m_VideoFovX);
        float cameraFocal =
                m_VideoWidth * 0.5f / (float) tan((double) m_VideoFovX * 0.5 * 3.1415926 / 180.0);
        float half_tan_fovy = 0.5f * m_VideoHeight / cameraFocal;
        m_VideoFovY = 2 * atanf(half_tan_fovy) * 180.0f / 3.1415726f;
        LOGD("camera fovx:%f, calculate fovy:%f", m_VideoFovX, m_VideoFovY);

        auto citer = m_Frames.begin();
        for(; citer!=m_Frames.end(); citer++)
        {
            CFrame* pframe = citer->second;
            updateFrameSLAMResult(pframe);
        }
    }

    ARResult CSession::pause()
    {
        m_bSessionResume = false;
        if (m_pArEngine == nullptr)
        {
            return ARRESULT_ERROR_INVALID_ARGUMENT;
        }
        //stop camera preview and unregister imu sensor listener
        m_pArEngine->arEngineStop();
        //close camera and close imu sensor
        m_pArEngine->arEngineDeinit();

        g_internal_standard_ar_session = NULL;

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult) {
            pthread_mutex_lock(&m_cloud_Mutex);
            slamAlgorithmResult->m_MapId[0] = '\0';
            m_AppKey = "";
            m_AppSecret = "";
            if (slamAlgorithmResult->m_CurrentAnchor != nullptr)
                ((CCloudAnchor*)(slamAlgorithmResult->m_CurrentAnchor))->setCloudAnchorState(ARCLOUD_ANCHOR_STATE_ERROR_INTERNAL);
            slamAlgorithmResult->m_CloudMode = 0;
            slamAlgorithmResult->m_CurrentAnchor = nullptr;
            pthread_mutex_unlock(&m_cloud_Mutex);
        }

        AlgorithmResultWrapper faceMeshResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_FACE_MESH);
        FaceMeshAlgorithmResult* faceMeshAlgorithmResult = faceMeshResultWrapper.getResult<FaceMeshAlgorithmResult*>();
        if(faceMeshAlgorithmResult){
            faceMeshAlgorithmResult->cleanResult();
        }
        m_isCameraIntrinsicsUpdate = false;
        return ARRESULT_SUCCESS;
    }

    ARResult CSession::update(CFrame* pframe, bool brender)
    {
        if (m_pArEngine == nullptr)
            return ARRESULT_ERROR_INVALID_ARGUMENT;
        m_pArEngine->updateImageInfo((int &) m_ImageFormat, m_ImageStride, m_FrameTimestamp);
        m_pArEngine->arSendCommand(AR_CMD_UPDATE, 0, 0);
//        if(m_bSessionResume)
//            m_pArEngine->arQueryResult();

        m_pArEngine->updateAlgorithmResult();
        m_AlgorithmResultPack.updateAlgorithmResult();
        {
            ScopeMutex mutex(&m_Mutex);
            if (m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE &&  getTrackingState() == ARTRACKING_STATE_STOPPED) {
                resetResult();
                return ARRESULT_ERROR_NOT_TRACKING;
            }
            updateFrameSLAMResult(pframe);
            if (m_bAlgorithmStart)
            {
                updateTrackable();
                updateFOV();
            }
        }
        return ARRESULT_SUCCESS;
    }


    void CSession::updateFOV()
    {
        if(!m_isCameraIntrinsicsUpdate) {
            SLAMAlgorithmResult *result = (SLAMAlgorithmResult *) m_AlgorithmResultPack.getAlgorithmResult(
                    ARALGORITHM_TYPE_SLAM);
            if (result && result->getState())
            {
                CCameraIntrinsics intrinsics = result->getCameraIntrinsics();
                if (intrinsics.m_FocalLengthX != 0)
                {
                    m_VideoFovX = (float) (atan(intrinsics.m_ImageWidth * 0.5f / intrinsics.m_FocalLengthX) * 180.0 * 2.0 / 3.1415926);
                    m_VideoFovY = (float) (atan(intrinsics.m_ImageHeight * 0.5f / intrinsics.m_FocalLengthY) * 180.0 * 2.0 / 3.1415926);
                    m_isCameraIntrinsicsUpdate = true;
                }
            }
        }
    }



    void CSession::updateFrameSLAMResult(CFrame* pframe)
    {
        if(m_ScreenRotate==CFrame::ROTATION_0 || m_ScreenRotate==CFrame::ROTATION_180)
        {
            pframe->setHorizontalFov(m_VideoFovY);
            pframe->setVerticalFov(m_VideoFovX);
        } else
        {
            pframe->setHorizontalFov(m_VideoFovX);
            pframe->setVerticalFov(m_VideoFovY);
        }

        pframe->setPreviewAspect(m_VideoAspect);
        pframe->setPreviewSize(m_VideoWidth, m_VideoHeight);
        pframe->setScreenAspect(m_ScreenAspect);
        pframe->setScreenSize(m_ScreenWidth, m_ScreenHeight);
        pframe->setConfig(m_Config);
        pframe->setTimeStamp(m_FrameTimestamp);

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();

        if (slamAlgorithmResult && slamAlgorithmResult->getState())
        {
            pframe->setImageIntrinsics(slamAlgorithmResult->getCameraIntrinsics());
            pframe->updateSLAMAndDenseReconResult(slamAlgorithmResult);
        }
    }

    void CSession::updateTrackable()
    {
        updatePlaneNode();
        updateImageNode();
        updateObjectNode();
    }

    void CSession::updatePlaneNode()
    {
        AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if(!slamAlgorithmResult || !slamAlgorithmResult->getState())
            return;
        std::vector<int> planeids = slamAlgorithmResult->getAllPlaneId();

        int size = planeids.size() / 2;
        for (int i = 0; i < size; ++i) {
            int plane_id = planeids[2*i];
            int is_updated = planeids[2*i+1];
            auto pos = m_Planes.find(plane_id);
            if(pos==m_Planes.end())
            {
                CPlane* pstplane = new CPlane(plane_id);
                pstplane->m_isUpdated = is_updated;
                m_Planes.insert(std::make_pair(plane_id, pstplane));
                LOGI("Insert a plane:%i", plane_id);
            }
            else if (!pos->second->m_isUpdated && is_updated)
            {
                pos->second->m_isUpdated = true;
            }
            else
            {
                pos->second->m_State = ARTRACKING_STATE_SUCCESS;
            }
        }

        m_anchorupdate_count++;
        if (m_anchorupdate_count < m_anchorupdate_freq)
            return;
        m_anchorupdate_count = 0;

        for (int i = 0; i < size; ++i) {
            int plane_id = planeids[2 * i];
            int is_updated = planeids[2 * i + 1];

            if(is_updated)
            {
                auto pos = m_Planes.find(plane_id);
                tagAnchorUpdateFlag(pos->second, slamAlgorithmResult);
                //LOGI("update a plane:%i", plane.plane_id);
            }
        }
    }

    void CSession::updateImageNode()
    {
        AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_IMAGE_TRACKING);
        MarkerAlgorithmResult *markerAlgorithmResult = algorithmResultWrapper.getResult<MarkerAlgorithmResult *>();
        if(!markerAlgorithmResult || !markerAlgorithmResult->getState())
            return;
        std::vector<int> markerids = markerAlgorithmResult->getAllMarkerId();

        for (std::map<int32_t, ITrackable *>::iterator iter = m_Markers.begin(); iter != m_Markers.end(); iter++) {
            iter->second->m_State = ARTRACKING_STATE_STOPPED;
        }

        for (int i = 0; i < markerids.size(); ++i) {
            auto pos = m_Markers.find(markerids[i]);
            if (pos == m_Markers.end()) {
                CImageNode *pstMarker = markerAlgorithmResult->getReferenceImageDatabase()->getReferenceImageById(
                        markerids[i]);
                if (pstMarker != nullptr) {
                    pstMarker->m_Type = ARNODE_TYPE_IMAGE;
                    pstMarker->m_isUpdated = true;
                    m_Markers.insert(std::make_pair(markerids[i], pstMarker));
                }
                LOGI("Inserted a marker:%i", markerids[i]);
            } else {
                pos->second->m_isUpdated = true;
                pos->second->m_State = ARTRACKING_STATE_SUCCESS;
            }
        }
    }

    void CSession::updateObjectNode()
    {
        AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack, ARALGORITHM_TYPE_OBJECT_TRACKING);
        ObjectTrackingAlgorithmResult *objectTrackingAlgorithmResult = algorithmResultWrapper.getResult<ObjectTrackingAlgorithmResult *>();
        if(!objectTrackingAlgorithmResult || !objectTrackingAlgorithmResult->getState())
            return;
        std::vector<int> objectids = objectTrackingAlgorithmResult->getAllObjectId();

        for (std::map<int32_t, ITrackable *>::iterator iter = m_Objects.begin(); iter != m_Objects.end(); iter++) {
            iter->second->m_State = ARTRACKING_STATE_STOPPED;
        }

        for (int i = 0; i < objectids.size(); ++i) {
            auto pos = m_Objects.find(objectids[i]);
            if (pos == m_Objects.end()) {
                CObjectNode *pObject = objectTrackingAlgorithmResult->getReferenceObjectDatabase()->getReferenceObjectById(objectids[i]);
                if (pObject != nullptr) {
                    pObject->m_Type = ARNODE_TYPE_OBJECT;
                    pObject->m_isUpdated = true;
                    m_Objects.insert(std::make_pair(objectids[i], pObject));
                }
                LOGI("Inserted a Object:%i", objectids[i]);
            } else {
                pos->second->m_isUpdated = true;
                pos->second->m_State = ARTRACKING_STATE_SUCCESS;
            }
        }
    }

    void CSession::updateStatistic()
    {
        m_CurrentTime = getCurrentSecond();
        double elapsetime = m_CurrentTime-m_BeginTime;
        if(elapsetime>3.0)
        {
            m_VideoFPS = (float)m_VideoFrameCount/(float)elapsetime;
            m_BeginTime = m_CurrentTime;
            m_VideoFrameCount = 0;
            LOGI("debugtime [client] video_fps=%f", m_VideoFPS);
        }
    }

    void CSession::tagAnchorUpdateFlag(ITrackable* ptrackable, SLAMAlgorithmResult* result)
    {
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            CAnchor* anchor = citer->second;
            if(anchor->m_Trackable && ptrackable) {
                if (((CPlane *) anchor->m_Trackable)->m_PlaneID == ((CPlane *) ptrackable)->m_PlaneID) {
                    anchor->m_isUpdated = true;
                    if (ptrackable->getType() == ARNODE_TYPE_PLANE) {
                        CPlane *pplane = (CPlane *) ptrackable;
                        int planeIndex = pplane->getPlaneIndex(result);
                        if (planeIndex == -1)
                            continue;

                        CPose planeorigin;
                        pplane->getPlaneOriginPoint(result, planeorigin, m_Config.m_AxisUpMode);

                        anchor->m_Pose.tx = anchor->m_LocalPose.tx + planeorigin.tx;
                        anchor->m_Pose.ty = anchor->m_LocalPose.ty + planeorigin.ty;
                        anchor->m_Pose.tz = anchor->m_LocalPose.tz + planeorigin.tz;

                        //LOGI("update anchor: %f, %f, %f to %f, %f, %f", oldpos.tx, oldpos.ty, oldpos.tz, anchor->m_Pose.tx, anchor->m_Pose.ty, anchor->m_Pose.tz);
                    }
                }
            }
        }
    }

    ARTrackingState CSession::getTrackingState()
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result && result->getState())
        {
            return result->getTrackingState();
        }
        return ARTRACKING_STATE_STOPPED;
    }

    void CSession::setCameraTextureName(int texid)
    {
        m_OESTextureId = texid;
        ScopeMutex m1(&m_Image_Mutex);
        if(m_pArEngine)
            m_pArEngine->arStoreData(STR_TEXID, (void *) (&texid));
    }

    void CSession::setDisplayGeomtry(int rotation, int width, int height)
    {
        m_ScreenRotate = rotation;
        m_ScreenWidth = width;
        m_ScreenHeight = height;
        m_ScreenAspect = (float)m_ScreenWidth/(float)m_ScreenHeight;
        m_VideoAspect = (float)m_VideoWidth/(float)m_VideoHeight;

        ScopeMutex mutext(&m_Mutex);
        for (auto citer = m_Frames.begin(); citer != m_Frames.end(); citer++) {
            citer->second->setDisplayGeometryChanged();
            citer->second->setScreenRotation(m_ScreenRotate);
        }
        if(m_pArEngine)
            m_pArEngine->arStoreData(SCREEN_ROTATE, (void*)&(rotation));

    }

    CAnchor* CSession::acquireNewAnchor(const CPose& pose, ITrackable* ptrackable)
    {
        ScopeMutex mutex(&m_Mutex);
        CAnchor* panchor = new CAnchor();
        panchor->m_State = ARTRACKING_STATE_SUCCESS;
        panchor->m_Pose = pose;
        panchor->m_Trackable = ptrackable;

        m_Anchors.insert(std::make_pair(panchor, panchor));

        if(ptrackable && ptrackable->getType()==ARNODE_TYPE_PLANE)
        {

            AlgorithmResultWrapper resultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
            SLAMAlgorithmResult* slam_result = resultWrapper.getResult<SLAMAlgorithmResult*>();
            if (slam_result)
            {
                CPlane* pPlane = (CPlane*)ptrackable;
                int planeIndex = pPlane->getPlaneIndex(slam_result);
                if(planeIndex==-1)
                    return panchor;

                CPose planeOrigin;
                pPlane->getPlaneOriginPoint(slam_result, planeOrigin, (ArAxisUpMode)m_Config.m_AxisUpMode);
                ARPlaneType planeType = pPlane->getPlaneType(slam_result, (ArAxisUpMode)m_Config.m_AxisUpMode);
                if(planeType == ARPLANE_TYPE_UPWARD_FACING) {
                    panchor->m_LocalPose.tx = pose.tx - planeOrigin.tx;
                    panchor->m_LocalPose.ty = pose.ty - planeOrigin.ty;
                    panchor->m_LocalPose.tz = pose.tz - planeOrigin.tz;
                }else if(planeType == ARPLANE_TYPE_VERTICAL){
                    panchor->m_LocalPose.tx = pose.tx - planeOrigin.tx;
                    panchor->m_LocalPose.ty = pose.ty - planeOrigin.ty;
                    panchor->m_LocalPose.tz = pose.tz - planeOrigin.tz;
                }
            }


//            LOGI("update local pose: %f, %f, %f ", panchor->m_LocalPose.tx, panchor->m_LocalPose.ty, panchor->m_LocalPose.tz);
        }

        return panchor;
    }

    void CSession::detachAnchor(CAnchor* anchor)
    {
        anchor->m_State = ARTRACKING_STATE_STOPPED;
    }

    void CSession::getAllTrackables(ARNodeType filtertype, std::vector<ITrackable*>& trackalbe_array) {
        ScopeMutex mutex(&m_Mutex);
        switch (filtertype) {
            case ARNODE_TYPE_PLANE: {
                if(m_Config.m_PlaneAlgorithmMode == ARALGORITHM_MODE_DISABLE)
                    return;

                for (auto citer = m_Planes.begin(); citer != m_Planes.end(); citer++) {
                    if(citer->second->m_State == ARTRACKING_STATE_SUCCESS)
                        trackalbe_array.push_back(citer->second);
                }
            }
                break;
            case ARNODE_TYPE_HAND_GESTURE: {

                if(m_Config.m_HandGestureMode == ARALGORITHM_MODE_DISABLE)
                    return;
                AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack,
                                                              ARALGORITHM_TYPE_HAND_GESTURE);
                HandGestureAlgorithmResult *handGestureAlgorithmResult = algorithmResultWrapper.getResult<HandGestureAlgorithmResult *>();
                if (handGestureAlgorithmResult) {
                    if (handGestureAlgorithmResult->resultIsEmpty()) {
                        return;
                    }
                      handGestureAlgorithmResult->getResult(trackalbe_array);
                }
            }
                break;

            case ARNODE_TYPE_IMAGE: {
                for (auto citer = m_Markers.begin(); citer != m_Markers.end(); citer++) {
                    if (citer->second->m_State == ARTRACKING_STATE_SUCCESS)
                        trackalbe_array.push_back(citer->second);
                }
            }
                break;
            case ARNODE_TYPE_FACE_MESH: {

                AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack,
                                                              ARALGORITHM_TYPE_FACE_MESH);
                FaceMeshAlgorithmResult *faceMeshAlgorithmResult = algorithmResultWrapper.getResult<FaceMeshAlgorithmResult *>();
                if (faceMeshAlgorithmResult) {
                    if (faceMeshAlgorithmResult->resultIsEmpty()) {
                        return;
                    }
                    faceMeshAlgorithmResult->getResult(trackalbe_array);

                }

            }
				break;
            case ARNODE_TYPE_OBJECT: {
                for (auto citer = m_Objects.begin(); citer != m_Objects.end(); citer++) {
                    if (citer->second->m_State == ARTRACKING_STATE_SUCCESS)
                        trackalbe_array.push_back(citer->second);
                }
            }
                break;
            case ARNODE_TYPE_BODY_DETECT:{
                AlgorithmResultWrapper algorithmResultWrapper(&m_AlgorithmResultPack,
                                                              ARALGORITHM_TYPE_BODY_DETECT);
                BodyDetectAlgorithmResult *bodyDetectAlgorithmResult = algorithmResultWrapper.getResult<BodyDetectAlgorithmResult *>();
                if (bodyDetectAlgorithmResult) {
                    if (bodyDetectAlgorithmResult->resultIsEmpty()) {
                        return;
                    }
                    bodyDetectAlgorithmResult->getResult(trackalbe_array);
                }
            }
                break;
            default:
                break;

        }
    }

    void CSession::getAllAnchors(std::vector<CAnchor*>& anchor_array)
    {
        ScopeMutex mutex(&m_Mutex);
        for(auto citer = m_Anchors.begin(); citer!=m_Anchors.end(); citer++)
        {
            if(citer->second->m_State == ARTRACKING_STATE_SUCCESS)
                anchor_array.push_back(citer->second);
        }
    }

    void CSession::getViewMatrix( float* out_col_major_4x4 )
    {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            if(m_Config.m_ObjectTrackingMode == ARALGORITHM_MODE_ENABLE)
            {
                AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_OBJECT_TRACKING);
                ObjectTrackingAlgorithmResult* result = algorithmResultWrapper.getResult<ObjectTrackingAlgorithmResult*>();
                if (!result && !result->getState())
                    return;
                result->getCameraViewMatrix(out_col_major_4x4, m_ScreenRotate);
                return;
            }
            mat4f viewMatrix;
            memcpy(out_col_major_4x4, viewMatrix.data, sizeof(float) * 16);
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result && !result->getState())
            return;
        result->getCameraViewMatrix(out_col_major_4x4, m_ScreenRotate);
    }

    void CSession::getProjectionMatrix(float znear, float zfar, float* dest_col_major_4x4)
    {
        float aspect = m_ScreenAspect < 1 ? 1.0f / m_ScreenAspect : m_ScreenAspect;
        float fov = getVerticalFov();
        float realFov = fov;
        if (m_ScreenRotate == CFrame::ROTATION_90 || m_ScreenRotate == CFrame::ROTATION_270) {
            if (m_VideoAspect < aspect) {
                double focal = m_VideoHeight * 0.5f / tan(fov * 0.5f * M_PI / 180.0f);
                float realH = m_VideoWidth / aspect;
                realFov = (float) (2.0f * atan(0.5f * realH / focal) * 180.0f / M_PI);
            }
        } else {
            if (m_VideoAspect > aspect) {
                double focal = m_VideoWidth * 0.5f / tan(fov * 0.5f * M_PI / 180.0f);
                float realW = m_VideoHeight * aspect;
                realFov = (float) (2.0f * atan(0.5f * realW / focal) * 180.0f / M_PI);
            }
        }

        mat4f projmat = mat4f::createPerspective(realFov,
            m_Config.m_SLAMDeviceType == ARDEVICE_TYPE_GLASS ? m_ScreenAspect / 2 : m_ScreenAspect,
            znear, zfar);

        memcpy(dest_col_major_4x4, projmat.data, sizeof(float)*16);
//        mat4f projmat = mat4f::createPerspective(m_FovY, m_Aspect,znear, zfar);
//        memcpy(dest_col_major_4x4, projmat.data, sizeof(float)*16);
    }

    void CSession::getProjectionMatrixSeeThrough(float near, float far, float* dest_col_major_4x4) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            mat4f viewMatrix;
            memcpy(dest_col_major_4x4, viewMatrix.data, sizeof(float) * 16);
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->getProjectionMatrixSeeThrough(near, far, dest_col_major_4x4);
    }

    void CSession::getPredictedViewMatrix(float *out_col_major_4x4) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            mat4f viewMatrix;
            memcpy(out_col_major_4x4, viewMatrix.data, sizeof(float) * 16);
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->getPredictedViewMatrix(out_col_major_4x4);
    }

    void CSession::setWindow(void *window, bool newPbuffer) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->setWindow(window, m_appContext, newPbuffer);
    }

    void CSession::beginRenderEye(int eyeId) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->beginRenderEye(eyeId);
    }

    void CSession::endRenderEye(int eyeId) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->endRenderEye(eyeId);
    }

    void CSession::submitRenderFrame(int leftEyeTextureId, int rightEyeTextureId) {
        if(m_Config.m_SLAMMode == ARALGORITHM_MODE_DISABLE) {
            return;
        }

        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (!result)
            return;
        result->submitRenderFrame(leftEyeTextureId, rightEyeTextureId);
    }

    AlgorithmResultWrapper CSession::getAlgorithmResult(int type)
    {
        return AlgorithmResultWrapper(&m_AlgorithmResultPack, (ARAlgorithmType) type);
    }

    void CSession::createArEngine()
    {
        if (!m_pArEngine)
        {
            m_pArEngine = ArEngineJavaClient::getInstance();
            m_pArEngine->arStoreData(STR_SESSION, &m_SessionPtr);
            LOGI("create ar service engine");
        }
        if (m_pArEngine)
            m_pArEngine->initialize();
    }

    float CSession::getDistanceFromCamera(CSession* pSession, CHitResult *hitresult)
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result)
        {
            vec3f hitPose;
            hitPose.x = hitresult->getHitPose().tx;
            hitPose.y = hitresult->getHitPose().ty;
            hitPose.z = hitresult->getHitPose().tz;
            return result->getDistanceFromCamera(hitPose);
        }
        else
            return 0.0f;
    }

    int32_t CSession::getPlaneVetexCount()
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result)
        {
            return result->getPlaneVertexCount();
        }
        return 0;
    }

    bool CSession::getPlaneVertexArray(float* vertex_array)
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result)
        {
            result->getPlaneVertexArray(vertex_array);
            return true;
        }
        return false;
    }

    int32_t CSession::getPlaneIndexCount()
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result) {
            return result->getPlaneIndexCount();
        }
        return 0;
    }

    bool CSession::getPlaneIndexArray(unsigned short* index_array)
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* result = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (result) {
            result->getPlaneIndexArray(index_array);
        }

        return false;
    }

    void CSession::addFrame(CFrame* pframe)
    {
        ScopeMutex mutext(&m_Mutex);
        auto pos = m_Frames.find(pframe);
        if(pos == m_Frames.end())
        {
            m_Frames.insert(std::make_pair(pframe, pframe));
        }
    }

    void CSession::removeFrame(CFrame* pframe)
    {
        if (!pframe)
            return;
        ScopeMutex mutext(&m_Mutex);
        auto pos = m_Frames.find(pframe);
        if(pos != m_Frames.end())
        {
            m_Frames.erase(pframe);
            pframe->m_Handle->m_pImpl = NULL;
            delete pframe;
        }
    }

    void CSession::getSDKInfo(std::string &infostr)
    {
        std::ostringstream buffer;
        buffer << m_VideoFPS;
        std::string videoFPS = buffer.str();
        std::string sdk_version  = standardar_version;
        infostr = "sdk version:"+ sdk_version + "\nvideo fps:"+videoFPS;
    }

    void CSession::getVersion(std::string& version)
    {
        const int TEMP_STR_LEN = 1024;
        char tempstr[TEMP_STR_LEN];
        memset(tempstr, '\0', TEMP_STR_LEN);
        if(m_pArEngine)
        {
            m_pArEngine->arGetVersion(tempstr, TEMP_STR_LEN);
            version = tempstr;
        }
    }

    bool CSession::isSLAMIntializing()
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if (slamAlgorithmResult)
            return slamAlgorithmResult->isSLAMIntializing();
        return false;
    }

    int CSession::parse_pair(const char *str, int *first, int *second, char delim, char **endptr )
    {
        // Find the first integer.
        char *end;
        int w = (int)strtol(str, &end, 10);
        // If a delimeter does not immediately follow, give up.
        if (*end != delim) {
            LOGI("Cannot find delimeter (%c) in str=%s", delim, str);
            return -1;
        }

        // Find the second integer, immediately after the delimeter.
        int h = (int)strtol(end+1, &end, 10);

        *first = w;
        *second = h;

        if (endptr) {
            *endptr = end;
        }

        return 0;
    }

    void CSession::parseSizesList(const char *sizesStr, std::vector<vec2i> &sizes)
    {
        if (sizesStr == 0) {
            return;
        }

        char *sizeStartPtr = (char *)sizesStr;

        while (true) {
            int width, height;
            int success = parse_pair(sizeStartPtr, &width, &height, 'x',
                                     &sizeStartPtr);
            if (success == -1 || (*sizeStartPtr != ',' && *sizeStartPtr != '\0')) {
                LOGW("Picture sizes string \"%s\" contains invalid character.", sizesStr);
                return;
            }

            sizes.push_back(vec2i(width, height));

            if (*sizeStartPtr == '\0') {
                return;
            }
            sizeStartPtr++;
        }
    }

    bool CSession::copyYUVImageDataLocked(unsigned char *y_data, unsigned char *u_data,  unsigned char* v_data)
    {
        if (m_pArEngine == nullptr)
            return false;

        return m_pArEngine->copyImageData(y_data, u_data, v_data);
    }

    ARResult CSession::hostCloudAnchor(CAnchor* anchor, CAnchor** out_anchor)
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if(slamAlgorithmResult->m_CurrentAnchor != nullptr){
            *out_anchor = nullptr;
            return ARRESULT_ERROR_RESOURCE_EXHAUSTED;
        }
        if(m_Config.m_CloudAnchorMode == ARALGORITHM_MODE_DISABLE) {
            *out_anchor = nullptr;
            return ARRESULT_ERROR_CLOUD_ANCHORS_NOT_CONFIGURED;
        }
        if(slamAlgorithmResult->getTrackingState() != ARTRACKING_STATE_SUCCESS) {
            *out_anchor = nullptr;
            return ARRESULT_ERROR_NOT_TRACKING;
        }
        CAnchor* cloudanchor = new CCloudAnchor(*anchor);
        slamAlgorithmResult->m_CurrentAnchor = cloudanchor;
        slamAlgorithmResult->m_CloudMode = CLOUD_HOST;
//        m_CurrentAnchor = cloudanchor;
        float pose[7];
        pose[0] = cloudanchor->m_Pose.tx;
        pose[1] = cloudanchor->m_Pose.ty;
        pose[2] = cloudanchor->m_Pose.tz;
        pose[3] = cloudanchor->m_Pose.qx;
        pose[4] = cloudanchor->m_Pose.qy;
        pose[5] = cloudanchor->m_Pose.qz;
        pose[6] = cloudanchor->m_Pose.qw;

        m_pArEngine->arStoreData(STR_POSE, (void *)pose);
        bool isOverride = !(slamAlgorithmResult->m_MapId[0] == '\0');
        m_pArEngine->arStoreData(BOOL_IS_OVERRIDE, (void*)(&isOverride));
        if(isOverride){
            m_pArEngine->arStoreData(STR_MAP_ID, (void *)slamAlgorithmResult->m_MapId);
        }
        m_pArEngine->arStoreData(APP_KEY, (void *)(m_AppKey.c_str()));
        m_pArEngine->arStoreData(APP_SECRET, (void *)(m_AppSecret.c_str()));
        m_pArEngine->arSendCommand(AR_CMD_HOST, 0, 0);

        *out_anchor = cloudanchor;
        return ARRESULT_SUCCESS;
    }

    ARResult CSession::resolveCloudAnchor(std::string anchorId, CAnchor** out_anchor)
    {
        AlgorithmResultWrapper algorithmResultWrapper = getAlgorithmResult(ARALGORITHM_TYPE_SLAM);
        SLAMAlgorithmResult* slamAlgorithmResult = algorithmResultWrapper.getResult<SLAMAlgorithmResult*>();
        if(slamAlgorithmResult->m_CurrentAnchor != nullptr){
            *out_anchor = nullptr;
            return ARRESULT_ERROR_RESOURCE_EXHAUSTED;
        }
        if(m_Config.m_CloudAnchorMode == ARALGORITHM_MODE_DISABLE) {
            *out_anchor = nullptr;
            return ARRESULT_ERROR_CLOUD_ANCHORS_NOT_CONFIGURED;
        }

        if(slamAlgorithmResult->getTrackingState() != ARTRACKING_STATE_SUCCESS) {
            *out_anchor = nullptr;
            return ARRESULT_ERROR_NOT_TRACKING;
        }

        CAnchor* cloudanchor = new CCloudAnchor();
        slamAlgorithmResult->m_CurrentAnchor = cloudanchor;
        slamAlgorithmResult->m_CloudMode = CLOUD_RESOLVE;
        m_pArEngine->arStoreData(STR_ANCHOR_ID, (void *)anchorId.c_str());
        m_pArEngine->arStoreData(APP_KEY, (void *)(m_AppKey.c_str()));
        m_pArEngine->arStoreData(APP_SECRET, (void *)(m_AppSecret.c_str()));
        m_pArEngine->arSendCommand(AR_CMD_RESOLVE, 0, 0);

        *out_anchor = cloudanchor;
        return ARRESULT_SUCCESS;
    }

    void CSession::setKeyAndSecret(const char* key, const char* secret)
    {
        m_AppKey = std::string(key);
        m_AppSecret = std::string(secret);
    }

    void CSession::setUserJson(const char* json)
    {
        if(m_pArEngine)
            m_pArEngine->setUserJson(std::string(json));
    }

    void CSession::getIntValue(ARParameterEnum type, int32_t * value, int size)
    {
        switch (type)
        {
            case ARPARAMETER_ALGORITHM_SLAM_DETAIL_STATE:
            {
                break;
            }
            default:
                break;
        }
    }

    void CSession::getFloatValue(ARParameterEnum type, float* value, int size)
    {
        switch (type)
        {
            case ARPARAMETER_VIDEO_VERTICAL_FOV:
            {
                *value = getVerticalFov();
                break;
            }
            default:
                break;
        }
    }

    void CSession::getBackgroundData(char **image_data, int *width, int *height) {
        return;
    }

}
