#include <vector>
#include <jni.h>
#include <string>
#include <fstream>
#include "ArEngineJavaClient.h"
#include "ArCommon.h"
#include "NativeClient.h"
#include "ArJniHelpler.h"
#include "StandardARCommon.h"

namespace standardar
{
    int ArEngineJavaClient::m_InstanceCount = 0;
    ArEngineJavaClient::ArEngineJavaClient()
    {
        m_yuv_cache_size = 1024 * 1014;
        m_yuv_cache = new unsigned char[m_yuv_cache_size];
        memset(m_yuv_cache, 0, m_yuv_cache_size);

        m_SesstionPtr = 0;
        m_Context = nullptr;

        m_CameraEngine = nullptr;
        m_SensorEngine = nullptr;

        m_NativeClient = nullptr;
    }

    ArEngineJavaClient::~ArEngineJavaClient()
    {

    }
    int32_t ArEngineJavaClient::initialize()
    {
        if (m_CameraEngine)
        {
            m_CameraEngine->SetTextureId(0);
        }
        return 0;
    }
    void ArEngineJavaClient::destroy()
    {
        if (m_NativeClient)
            m_NativeClient->destroy();
    }

    bool ArEngineJavaClient::checkCameraPermission()
    {
        jclass utiClz = ArJniHelpler::getUtilClass();
        JNIEnvProxy env = ArJniHelpler::getJavaEnv();
        jmethodID checkid = env->GetStaticMethodID(utiClz, "checkCameraPermission",
                                                   "(Landroid/content/Context;)Z");
        jboolean res = env->CallStaticBooleanMethod(utiClz, checkid,
                                                    m_Context);
        return res;
    }

    bool ArEngineJavaClient::checkIMURequired()
    {
        if (m_Config.m_SLAMMode == ARALGORITHM_MODE_ENABLE)
            return true;
        else
            return false;
    }

    int32_t ArEngineJavaClient::arEngineInit(int halVersion, const char *packageid, const CConfig& config,
                                          SLAMResultCallback imgpreview)
    {
        LOGI("ArEngine Java Client Init");

        m_Context = ArJniHelpler::getApplicationContextGlobalRef();

        m_SlamResultCallBack = imgpreview;

        m_Config = config;

        if(!checkCameraPermission()) {
            LOGE("camera permission is not required");
            return ARRESULT_ERROR_FATAL;
        }

        openClient(config);
        openCameraDevice();
        openSensorDevice();
        return ARRESULT_SUCCESS;
    }

    void ArEngineJavaClient::openCameraDevice()
    {
        if (!m_CameraEngine)
        {
            LOGI("create camera engine");
            m_CameraEngine  = new CameraEngine();
        }

        if (m_CameraEngine)
        {
            LOGI("open camera engine");
            m_CameraEngine->SetContext(m_Context);
            m_CameraEngine->SetSession(m_SesstionPtr);
            m_CameraEngine->OpenCamera(m_Config);
            setCameraNotify();
        }
    }

    void ArEngineJavaClient::openSensorDevice()
    {
        if (!m_SensorEngine)
        {
//            LOGI("create sensor engine %d", SENSOR_SOURCE_JAVA);
            m_SensorEngine = new SensorEngine(SENSOR_SOURCE_JAVA);
        }

        if (m_SensorEngine && checkIMURequired())
        {
            LOGI("open sensor engine");
            m_SensorEngine->SetContext(m_Context);
            m_SensorEngine->Init();
            setSensorNotify();
        }
    }

    void ArEngineJavaClient::openClient(const CConfig& config)
    {
        if (m_NativeClient == nullptr)
        {
            m_NativeClient = new NativeClient(config, m_Context);
        }
        if (m_NativeClient)
        {
            m_NativeClient->setSession(m_SesstionPtr);
            m_NativeClient->setContext(m_Context);
        }
    }

    void ArEngineJavaClient::processResult(const char *result, int size, const char* img, int imgsize, int64_t timestamp)
    {
        m_SlamResultCallBack((void*)img, imgsize, (void*)result, size, nullptr);
    }

    void ArEngineJavaClient::setCameraNotify()
    {
        if (m_NativeClient && m_CameraEngine)
        {
            m_CameraEngine->SetNotify(m_NativeClient->getClentObject());
        }
    }

    void ArEngineJavaClient::setSensorNotify()
    {
        if (m_NativeClient && m_SensorEngine)
        {
            m_SensorEngine->SetNotify(m_NativeClient->getClentObject());
        }
    }

    void ArEngineJavaClient::closeCameraDevice()
    {
        if (m_CameraEngine)
        {
            LOGI("close camera engine");
            m_CameraEngine->CloseCamera();
        }
    }

    void ArEngineJavaClient::closeSensorDevice()
    {
        if (m_SensorEngine)
        {
            LOGI("close sensor engine");
            m_SensorEngine->DeInit();
        }
    }


    int32_t ArEngineJavaClient::arConfig(const char *certVersion, const char *appPackage,
                                      const char *authorityStatusWord)
    {
        return 0;
    }

    void ArEngineJavaClient::arGetVersion(char *version, size_t len)
    {

    }

    void ArEngineJavaClient::arSetPrviewSize(int width, int height)
    {
        m_Preview_Width = width;
        m_Preview_Height = height;
        if(m_CameraEngine)
            m_CameraEngine->SetPreviewSize(width, height);

        LOGD("set preview size %d %d", m_Preview_Width, m_Preview_Height);
    }

    void ArEngineJavaClient::arGetPreviewSize(int& width, int& height)
    {
        if (m_CameraEngine)
            m_CameraEngine->GetPreviewSize(width, height);
    }

    void ArEngineJavaClient::arGetProcessSize(int &width, int &height) {
        if (m_CameraEngine)
            m_CameraEngine->GetProcessSize(width, height);
    }

    void ArEngineJavaClient::arGetCameraFovH(float& fovH)
    {
        if (m_CameraEngine)
            fovH = m_CameraEngine->GetFovH();
    }



    const char *ArEngineJavaClient::arGetSupportPreviewSize()
    {
        if (m_CameraEngine)
            return m_CameraEngine->GetJavaSupportSize();
        return "";
    }

    void ArEngineJavaClient::arGetDisplayInfo(int& width, int& height)
    {
        jobject context = ArJniHelpler::getApplicationContextGlobalRef();
        jclass utilClz = ArJniHelpler::getUtilClass();
        JNIEnvProxy env = ArJniHelpler::getJavaEnv();
        jmethodID getWidthId = env->GetStaticMethodID(utilClz, "getLandscapeDisplayWidth", "(Landroid/content/Context;)I");
        jmethodID getHeightId = env->GetStaticMethodID(utilClz, "getLandscapeDisplayHeight", "(Landroid/content/Context;)I");
        width = env->CallStaticIntMethod(utilClz, getWidthId, context);
        height = env->CallStaticIntMethod(utilClz, getHeightId, context);
        env->DeleteGlobalRef(context);
    }

    int32_t ArEngineJavaClient::arEngineDeinit()
    {
        closeCameraDevice();
        closeSensorDevice();
        JNIEnvProxy env = ArJniHelpler::getJavaEnv();
        if (m_Context)
        {
            env->DeleteGlobalRef(m_Context);
            m_Context = nullptr;
        }
        return 0;
    }

    void ArEngineJavaClient::arEngineStart()
    {
        if (m_CameraEngine && m_CameraEngine->IsReady())
        {
            m_CameraEngine->StartPreview();
        }
        if (m_SensorEngine)
        {
            m_SensorEngine->Start();
        }
    }

    void ArEngineJavaClient::arEngineStartSimp()
    {
        if (m_NativeClient != nullptr) {
            m_NativeClient->initAlgorithm(m_Config);
            LOGI("Simplified ArEngine started");
            return;
        }
        LOGI("Simplified ArEngine not started, m_NativeClient instance not created");
    }

    void ArEngineJavaClient::arEngineStop()
    {
        if (m_CameraEngine && m_CameraEngine->IsReady())
        {
            m_CameraEngine->StopPreview();
        }
        if (m_SensorEngine)
        {
            m_SensorEngine->Stop();
        }
    }

    void ArEngineJavaClient::arSendCommand(int cmd, int32_t arg1, float arg2)
    {
        switch (cmd)
        {
            case AR_CMD_START_ALGORITHM:
            {
                if (m_NativeClient)
                    m_NativeClient->startAlgorithm(arg1);
                break;
            }
            case AR_CMD_STOP_ALGORITHM:
            {
                if (m_NativeClient)
                    m_NativeClient->stopAlgorithm(arg1);
                break;
            }
            case AR_CMD_INIT_ALGORITHM:
            {
                if(m_NativeClient) {
                    m_NativeClient->setSLAMMode(m_Config);
                    m_NativeClient->initAlgorithm(m_Config);
                    m_NativeClient->setUrl(m_Config);
                    m_NativeClient->setBluetoothUUID(m_Config);
                }
                break;
            }
            case AR_CMD_DESTROY_ALGORITM:
            {
                if(m_NativeClient && m_InstanceCount == 1)
                    m_NativeClient->destroyAlgorithm();
                --m_InstanceCount;
                break;
            }
            case AR_CMD_SET_FOCUS_MODE:
            {
                break;
            }
            case AR_CMD_HOST:
            {
                if (m_NativeClient)
                    m_NativeClient->hostAnchor(m_IsOverride, m_Pose, m_MapId, m_appKey, m_appSecret);
                break;
            }
            case AR_CMD_RESOLVE:
            {
                if(m_NativeClient)
                    m_NativeClient->resolveAnchor(m_AnchorId, m_appKey, m_appSecret);
                break;
            }
            case AR_CMD_UPDATE:
            {
                if (m_CameraEngine)
                {
                    m_CameraEngine->UpdateTexture();
                }
                break;
            }
            case AR_CMD_WAIT:
            {
                if (m_CameraEngine)
                {
                    m_CameraEngine->Wait();
                }
                break;
            }
			case AR_CMD_SET_RESUME_FLAG:
            {
                if (m_NativeClient) {
                    m_NativeClient->setResumeFlag();
                }
                break;
            }
            case AR_CMD_SET_MODE: {
                if (m_NativeClient)
                    m_NativeClient->setSLAMMode(m_Config);
            }
            default:
                break;
        }
    }

    ArEngineJavaClient* ArEngineJavaClient::getInstance()
    {
        static ArEngineJavaClient instance;
        ++m_InstanceCount;
        return &instance;
    }

    void ArEngineJavaClient::arStoreData(const char *name, void *data)
    {
        if (strcmp(name, STR_SESSION) == 0)
        {
            int64_t* data_ptr = (int64_t*)data;
            m_SesstionPtr = *data_ptr;
        }
        else if (strcmp(name, STR_TEXID) == 0)
        {
            if (m_CameraEngine)
            {
                m_CameraEngine->SetTextureId(*((int*)data));
                m_CameraEngine->StartPreview();
            }
        }
        else if (strcmp(name, STR_POSE) == 0)
        {
            float* floatdata = (float*)data;
            for(int i = 0; i < 7; i++)
                m_Pose[i] = floatdata[i];
        }
        else if (strcmp(name, STR_ANCHOR_ID) == 0)
        {
            m_AnchorId = std::string((const char*)data);
        }
        else if (strcmp(name, STR_MAP_ID) == 0)
        {
            m_MapId = std::string((const char*)data);
        }
        else if (strcmp(name, BOOL_IS_OVERRIDE) == 0)
        {
            m_IsOverride = *((bool*)data);
        }
        else if (strcmp(name, APP_KEY) == 0)
        {
            m_appKey = std::string((const char*)data);
        }
        else if (strcmp(name, APP_SECRET) == 0)
        {
            m_appSecret = std::string((const char*)data);
        } else if(strcmp(name, SCREEN_ROTATE) == 0){
            int screen_rotate = *((int*)data);
            if (m_CameraEngine)
            {
                m_CameraEngine->SetScreenRotate(screen_rotate);

            }
        }
    }

    void ArEngineJavaClient::setLightEstimateMode(int mode)
    {
        if(m_NativeClient){
            m_NativeClient->changeLightingEstimateMode(mode);
        }
    }

    jobject ArEngineJavaClient::setEnable(int type, bool enable)
    {
        if (m_NativeClient)
        {
            return m_NativeClient->setEnable(type, enable);
        }
        return nullptr;
    }

    void ArEngineJavaClient::arPushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                        double *rot, double *trans) {
        if (m_NativeClient) {
            m_NativeClient->pushKeyFrame(imgBuff, timestamp, width, height, rot, trans);
        }
    }

    void ArEngineJavaClient::arRunDenseRecon() {
        if (m_NativeClient) {
            m_NativeClient->runDenseRecon();
        }
    }

	void ArEngineJavaClient::arQueryResult()
	{
        if (m_NativeClient) {
            m_NativeClient->queryResult();
        }
    }

    void ArEngineJavaClient::updateImageInfo(int &format, int &stride, int64_t &timestamp) {
        if (m_CameraEngine) {
            m_CameraEngine->UpdateImageInfo(format, stride, timestamp);
        }
    }

    bool ArEngineJavaClient::copyImageData(unsigned char *y_data,
            unsigned char *u_data, unsigned char *v_data) {
        if (m_CameraEngine)
            return m_CameraEngine->CopyImageData(y_data, u_data, v_data);
        return false;
    }

    bool ArEngineJavaClient::isImgBufferDirty()
    {
        if (m_CameraEngine)
            return m_CameraEngine->isImgBufferDirty();
        return false;
    }

    void ArEngineJavaClient::updateAlgorithmResult() {
        if (m_NativeClient)
            m_NativeClient->updateAlgorithmResult();
    }

    void ArEngineJavaClient::setUserJson(std::string json)
    {
        if(m_NativeClient) {
            m_NativeClient->setUserJson(json);
        }
    }

}
