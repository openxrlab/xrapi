#include "NativeClient.h"
#include "ArJniHelpler.h"
#include "ArSession.h"
#include "StandardARCommon.h"
#include "ArAnchor.h"
#include "DynamicLoader.h"

namespace standardar
{
    NativeClient::NativeClient(const CConfig& config, jobject context) : m_Object_Client(nullptr),
                                                  m_Context(context)
    {
//        JNIEnvProxy env = ArJniHelpler::getJavaEnv();
//        if (env != nullptr)
//        {
//            m_Class_Client = ArJniHelpler::getClientClass();
//            if (m_Class_Client != nullptr)
//            {
//                jmethodID ctor_id = env->GetMethodID(m_Class_Client, "<init>", "(JLandroid/content/Context;Z)V");
//                if (ctor_id != 0)
//                {
//                    jobject object_client = env->NewObject(m_Class_Client, ctor_id, reinterpret_cast<jlong>(this), m_Context, enforceDex(config));
//                    m_Object_Client = env->NewGlobalRef(object_client);
//                }
//
//                // getting timewarp related methods
//                m_PredViewId = env->GetMethodID(m_Class_Client, "getPredictedViewMatrix", "()V");
//                m_SetWindowId = env->GetMethodID(m_Class_Client, "setWindow", "(JJ)V");
//                m_BeginEyeId = env->GetMethodID(m_Class_Client, "beginRenderEye", "(I)V");
//                m_EndEyeId = env->GetMethodID(m_Class_Client, "endRenderEye", "(I)V");
//                m_SubmitFrameId = env->GetMethodID(m_Class_Client, "submitFrame", "(II)V");
//            }
//            else
//                LOGW("can not find class %s", CLIENT_CLASS);
//        }
        JNIEnvProxy env(DynamicLoader::s_JVM);
        jobject client = ClientWrapper::construct(DynamicLoader::s_server_context, m_Context);
        m_Object_Client = env->NewGlobalRef(client);
    }

    NativeClient::~NativeClient()
    {
    }

    jobject NativeClient::getClentObject()
    {
        return m_Object_Client;
    }

    void NativeClient::setSession(int64_t sessionPtr)
    {
        m_SessionPtr = sessionPtr;
    }

    void NativeClient::setContext(jobject context)
    {
        m_Context = context;
    }

    void NativeClient::setPackageName(const char* packageName)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setPackageName(m_Context, packageName);
    }

    void NativeClient::setPlaneFindingMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setPlaneFindingMode(mode);
    }

    void NativeClient::setCloudAnchorMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setCloudAnchorMode(mode);
    }

    void NativeClient::setLightingEstimateMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setLightingEstimateMode(mode);
    }

    void NativeClient::changeLightingEstimateMode(int mode)
    {
        if (!checkJavaValid())
            return;

        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.changeLightingEstimateMode(mode);
    }

    void NativeClient::setUrl(const CConfig& config)
    {
        if (!checkJavaValid() && config.m_MapCoupleAlgorithmMode == ARALGORITHM_MODE_DISABLE)
            return;

        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setUrl(config.m_Url);
    }

    void NativeClient::setBluetoothUUID(const CConfig& config)
    {
        if (!checkJavaValid())
            return;

        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setBluetoothUUID(config.m_UUID);
    }

    void NativeClient::setSLAMMode(const CConfig& config)
    {
        if (!checkJavaValid())
            return;

        ClientWrapper clientWrapper(m_Object_Client, true);
        if(config.m_MapCoupleAlgorithmMode == ARALGORITHM_MODE_DISABLE)
            clientWrapper.setSLAMMode(-1);
        else
            clientWrapper.setSLAMMode(config.m_MapCouple_Mode);
    }


    void NativeClient::setWorldAligmentMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setWorldAligmentMode(mode);
    }

    void NativeClient::setDenseReconMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setDenseReconMode(mode);
    }

    void NativeClient::setObjectTrackingMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setObjectTrackingMode(mode);
    }

    void NativeClient::setObjectScanningMode(int mode)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setObjectScanningMode(mode);
    }

    void NativeClient::setAlgorithmStreamMode(int algorithmType, int mode)
    {
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setAlgorithmStreamMode(algorithmType, mode);
    }

    // set device type for dexclassloader to load proper slam class
    void NativeClient::setDeviceType(int algorithmType, int mode)
    {
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setDeviceType(algorithmType, mode);
    }

    void NativeClient::initAlgorithm(const CConfig& config)
    {
        if (!checkJavaValid())
            return;

        // package name
        setPackageName(config.m_PackageName.c_str());
        setAxisUpMode(config.m_AxisUpMode);
        // SLAM config
        if (config.m_SLAMMode != ARALGORITHM_MODE_DISABLE) {
            setAlgorithmStreamMode(ARALGORITHM_TYPE_SLAM, config.m_SLAM_StreamMode);
            setDeviceType(ARALGORITHM_TYPE_SLAM, config.m_SLAMDeviceType);
            setPlaneFindingMode(config.m_PlaneFindingMode);
            setLightingEstimateMode(config.m_LightEstimateAlgorithmMode == ARALGORITHM_MODE_ENABLE ? (int) config.m_LightEstimationMode : 0);
            setWorldAligmentMode(config.m_WorldAlignmentMode);
            setCloudAnchorMode(config.m_CloudAnchorMode);
        }

        // hand guesture config
        if (config.m_HandGestureMode != ARALGORITHM_MODE_DISABLE)
        {
//            setHandGestureMode(config.chooseStreamMode());
            setAlgorithmStreamMode(ARALGORITHM_TYPE_HAND_GESTURE, config.m_CameraTextureTarget);
        }

        if (config.m_DenseReconMode != ARALGORITHM_MODE_DISABLE)
        {
            setDenseReconMode(config.m_DenseRecon_StreamMode);
        }

        if (config.m_ArMarkerMode != ARALGORITHM_MODE_DISABLE)
        {
            setAlgorithmStreamMode(ARALGORITHM_TYPE_IMAGE_TRACKING, config.m_CameraTextureTarget);
        }

        if (config.m_FaceMeshMode != ARALGORITHM_MODE_DISABLE)
        {
            setAlgorithmStreamMode(ARALGORITHM_TYPE_FACE_MESH, config.m_FaceMesh_StreamMode);
        }

        if(config.m_ObjectScanningMode != ARALGORITHM_MODE_DISABLE)
        {
            setObjectScanningMode(config.m_ObjectScanningMode);
        }

        if (config.m_ObjectTrackingMode != ARALGORITHM_MODE_DISABLE)
        {
            setObjectTrackingMode(config.m_ObjectTrackingMode);
            //setAlgorithmStreamMode(ARALGORITHM_TYPE_OBJECT_TRACKING, config.m_ObjectTracking_StreamMode);
        }

        if (config.m_BodyDetectMode != ARALGORITHM_MODE_DISABLE)
        {
            setAlgorithmStreamMode(ARALGORITHM_TYPE_BODY_DETECT, config.m_BodyDetect_StreamMode);
        }

        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.initAlgorithm();
    }

    void NativeClient::stopAlgorithm(int type)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.stopAlgorithm(type);
    }

    void NativeClient::startAlgorithm(int type)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.startAlgorithm(type);
    }

    void NativeClient::destroyAlgorithm()
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.destroyAlgorithm();
    }

    bool NativeClient::checkJavaValid()
    {
        return m_Object_Client != nullptr;
    }

    void NativeClient::hostAnchor(bool override, float *anchor, std::string mapid, std::string app_key, std::string app_secret)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.hostAnchor(m_Context, override, anchor, mapid, app_key, app_secret);
    }

    void NativeClient::resolveAnchor(std::string anchorid, std::string app_key, std::string app_secret)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.resolveAnchor(m_Context, anchorid, app_key, app_secret);
    }

    void NativeClient::setUserJson(std::string json)
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setUserJson(json);
    }

    jobject NativeClient::setEnable(int type, bool enable)
    {
        if (!checkJavaValid())
            return nullptr;
        ClientWrapper clientWrapper(m_Object_Client, true);
        if (enable) {
            return clientWrapper.enableAlgorithm(type);
        } else {
            clientWrapper.disableAlgorithm(type);
            return nullptr;
        }
    }

    int NativeClient::getScreenRotate() {
        ARSession_* session = reinterpret_cast<ARSession_*>(m_SessionPtr);
        int screenRotate =  session->m_pImpl->getScreenRotate();
        return screenRotate;
    }

    void NativeClient::pushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                                    double *rot, double *trans) {
        if (!checkJavaValid()) {
            return;
        }
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.pushKeyFrame(imgBuff, timestamp, width, height, rot, trans);
    }

    void NativeClient::runDenseRecon() {
        if (!checkJavaValid()) {
            return;
        }
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.runDenseRecon();
    }

    void NativeClient::destroy()
    {
        m_Context = nullptr;
    }

    void NativeClient::queryResult()
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.getSLAMResult();
    }

    void NativeClient::setResumeFlag()
    {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setResumeFlag();
    }

    void NativeClient::updateAlgorithmResult() {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.updateAlgorithmResult();
    }

    void NativeClient::setAxisUpMode(int mode) {
        if (!checkJavaValid())
            return;
        ClientWrapper clientWrapper(m_Object_Client, true);
        clientWrapper.setAxisUpMode(mode);
    }
}
