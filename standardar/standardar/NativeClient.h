#ifndef STANDARD_AR_NATIVECLIENT_C_API_H
#define STANDARD_AR_NATIVECLIENT_C_API_H

#include <jni.h>
#include <functional>
#include "ArConfig.h"
#include "ArEngineBase.h"
#include <pthread.h>

namespace standardar
{
    class NativeClient
    {
    public:
        NativeClient(const CConfig& config, jobject context);
        ~NativeClient();
        jobject getClentObject();
        void setSession(int64_t sessionPtr);
        void setContext(jobject context);
        void initAlgorithm(const CConfig& config);
        void stopAlgorithm(int type);
        void startAlgorithm(int type);
        void destroyAlgorithm();
        void hostAnchor(bool override, float *anchor, std::string mapId, std::string app_key, std::string app_secret);
        void resolveAnchor(std::string anchorid, std::string app_key, std::string app_secret);
        void setUserJson(std::string json);
        void changeLightingEstimateMode(int mode);
        void setUrl(const CConfig& config);
        void setBluetoothUUID(const CConfig& config);
        void setSLAMMode(const CConfig& config);
        int getScreenRotate();
        jobject setEnable(int type, bool enable);

        void pushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                          double *rot, double *trans);
        void runDenseRecon();

        void destroy();
        void queryResult();
        void setResumeFlag();
        void updateAlgorithmResult();
        void setAxisUpMode(int mode);
    protected:
        void setPackageName(const char* packageName);
        void setPlaneFindingMode(int mode);
        void setCloudAnchorMode(int mode);
        void setLightingEstimateMode(int mode);
        void setWorldAligmentMode(int mode);
        void setDenseReconMode(int mode);
        void setObjectTrackingMode(int mode);
        void setObjectScanningMode(int mode);
        void setAlgorithmStreamMode(int algorithmType, int mode);
        void setDeviceType(int algorithmType, int mode);
    private:
        jobject m_Object_Client;
        jobject m_Context;
        int64_t m_SessionPtr;
        bool checkJavaValid();
        // timewarp related ids

    };

}





#endif //STANDARD_AR_NATIVECLIENT_C_API_H
