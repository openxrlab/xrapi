

#ifndef STANDARD_AR_ARENGINEBASE_C_API_H_
#define STANDARD_AR_ARENGINEBASE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <jni.h>

#include "ArConfig.h"

namespace standardar {

    typedef void (*SLAMResultCallback) (void *Imgbuf, int Bufsize, void *Result, int Resultsize, void *info);

    typedef enum {
        AR_CMD_START_ALGORITHM,
        AR_CMD_STOP_ALGORITHM,
        AR_CMD_SET_FOCUS_MODE,
        AR_CMD_UPDATE,
        AR_CMD_HOST,
        AR_CMD_RESOLVE,
        AR_CMD_INIT_ALGORITHM,
        AR_CMD_DESTROY_ALGORITM,
        AR_CMD_WAIT,
		AR_CMD_SET_RESUME_FLAG,
        AR_CMD_SET_MODE
    } ARENGINE_COMMAND;


    class IArEngineBase
    {
    public:
        virtual ~IArEngineBase(){};

        virtual int32_t initialize() = 0;
        virtual void destroy() = 0;

        virtual int32_t arEngineInit(int halVersion, const char *packageid, const CConfig& config, SLAMResultCallback imgpreview) = 0;
        virtual int32_t arEngineDeinit() = 0;

        virtual int arConfig(const char *certVersion, const char *appPackage, const char* authorityStatusWord) = 0;
        virtual void arGetVersion(char *version, size_t len) = 0;

        virtual void arEngineStart() = 0;
        virtual void arEngineStartSimp() = 0;
        virtual void arEngineStop() = 0;

        virtual void arSendCommand(int cmd, int32_t arg1, float arg2) = 0;
        virtual void arStoreData(const char* name, void* data) = 0;

        // sdk virtual function, and will not pass cmd to ARServer.
        virtual void arSetPrviewSize(int width, int height) = 0;
        virtual void arGetPreviewSize(int& width, int& height) = 0;
        virtual void arGetProcessSize(int& width, int& height){};
        virtual void arGetCameraFovH(float& fovH) = 0;
        virtual const char *arGetSupportPreviewSize() = 0;
        virtual void arGetDisplayInfo(int& width, int& height) = 0;

        virtual void setLightEstimateMode(int mode) = 0;
        virtual jobject setEnable(int type, bool enable){return nullptr;}

        virtual void arPushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                                    double *rot, double *trans) {}
        virtual void arRunDenseRecon() {}
        virtual void arQueryResult(){}
        virtual void getPredictedViewMatrix() {}
        virtual void setWindow(void *window, void *context) {}
        virtual void beginRenderEye(int eyeId) {}
        virtual void endRenderEye(int eyeId) {}
        virtual void submitFrame(int leftEyeTexId, int rightEyeTexId) {}
        virtual void updateImageInfo(int& format, int& stride, int64_t& timestamp){}
        virtual void updateAlgorithmResult(){}
        virtual bool copyImageData(unsigned char *y_data, unsigned char *u_data,  unsigned char* v_data){return false;}
        virtual bool isImgBufferDirty(){return false;}
        virtual void setUserJson(std::string json) {}
    };
}


#endif  // STANDARD_AR_ARENGINEBASE_C_API_H_
