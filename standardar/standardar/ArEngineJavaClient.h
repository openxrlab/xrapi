#ifndef STANDARD_AR_ARENGINEJAVACLIENT_C_API_H
#define STANDARD_AR_ARENGINEJAVACLIENT_C_API_H

#include <pthread.h>
#include <jni.h>
#include "ArEngineBase.h"
#include "ArCommon.h"
#include "SensorEngine.h"
#include "CameraEngine.h"
#include "NativeClient.h"

namespace standardar {

    class ArEngineJavaClient : public IArEngineBase {
    public:
        static int m_InstanceCount;
        static ArEngineJavaClient* getInstance();
        virtual ~ArEngineJavaClient();
        virtual int32_t initialize();
        virtual void destroy();

        virtual int32_t arEngineInit(int halVersion, const char *packageid, const CConfig& config, SLAMResultCallback imgpreview);
        virtual int32_t arEngineDeinit();

        virtual int arConfig(const char *certVersion, const char *appPackage, const char* authorityStatusWord);
        virtual void arGetVersion(char *version, size_t len);

        virtual void arEngineStart();
        virtual void arEngineStartSimp();
        virtual void arEngineStop();

        virtual void arSendCommand(int cmd, int32_t arg1, float arg2);
        virtual void arStoreData(const char* name, void* data);

        // sdk virtual function, and will not pass cmd to ARServer.
        virtual void arSetPrviewSize(int width, int height);
        virtual void arGetPreviewSize(int& width, int& height);
        virtual void arGetProcessSize(int& width, int& height);
        virtual void arGetCameraFovH(float& fovH);

        virtual const char *arGetSupportPreviewSize();
        virtual void arGetDisplayInfo(int& width, int& height);

        virtual void setLightEstimateMode(int mode);
        virtual jobject setEnable(int type, bool enable);
        virtual void arPushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                                    double *rot, double *trans);
        virtual void arRunDenseRecon();
        virtual void arQueryResult();
        virtual void updateImageInfo(int& format, int& stride, int64_t& timestamp);
        virtual bool copyImageData(unsigned char *y_data, unsigned char *u_data,  unsigned char* v_data);
        virtual bool isImgBufferDirty();
        virtual void setUserJson(std::string json);

        virtual void updateAlgorithmResult();
    private:
        ArEngineJavaClient();
        void openCameraDevice();
        void openSensorDevice();
        void openClient(const CConfig& config);
        void setCameraNotify();
        void setSensorNotify();
        void closeCameraDevice();
        void closeSensorDevice();
        void processResult(const char* result, int size, const char* img, int imgsize, int64_t timestamp);
        bool checkCameraPermission();
        bool checkIMURequired();
        private:
        int m_Preview_Width;
        int m_Preview_Height;
        SLAMResultCallback m_SlamResultCallBack;

        unsigned char* m_yuv_cache;
        int m_yuv_cache_size;

        CameraEngine* m_CameraEngine;
        SensorEngine* m_SensorEngine;
        NativeClient* m_NativeClient;

        int64_t m_SesstionPtr;
        jobject m_Context;
//        int m_Engine_Type;
        std::string m_AnchorId;
        std::string m_MapId;
        std::string m_appKey;
        std::string m_appSecret;
        float m_Pose[7];
        bool m_IsOverride;

        CConfig m_Config;
        std::string m_PackageName;
        std::string m_JavaSupportSizeStr;

    };
}

#ifdef __cplusplus
extern "C" {
#endif
bool isSupportCamera2Java();
#ifdef __cplusplus
}
#endif

#endif //STANDARD_AR_ARENGINEJAVACLIENT_C_API_H
