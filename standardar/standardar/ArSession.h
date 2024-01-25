
#ifndef STANDARD_AR_ARSESSION_C_API_H_
#define STANDARD_AR_ARSESSION_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <pthread.h>
#include <map>
#include "ArCommon.h"
#include "ArConfig.h"
#include "ArPose.h"
#include "ArMath.h"
#include "ArAuxRender.h"
#include "OffscreenRenderTarget.h"
#include "ArEngineBase.h"
#include "StandardARCommon.h"
#include "ArCamera.h"
#include "AlgorithmResult.h"
#include "SLAMAlgorithmResult.h"
#include "HandGestureAlgorithmResult.h"
#include "ArBgYUVRender.h"

namespace standardar
{

    class CFrame;
    class CAnchor;
    class ITrackable;
    class CPlane;
    class CHitResult;
    class CHandGesture;
    class ArWorldLifeCycle;

    class CSession
    {
    public:
        CSession(void* env, void* context, const char* package_name, int64_t sessionHandle);
        virtual ~CSession();

        virtual bool intialize();

        virtual bool getDisplayPortraitSize(int& width, int&height);

        virtual int32_t checkSupported(const CConfig& config);
        virtual ARResult configurate(const CConfig& config);

        virtual ARResult update(CFrame* pframe, bool brender = true);

        virtual const CConfig& getConfig()const{return m_Config;}
        virtual const CCameraConfig& getCameraConfig()const{return m_CameraConfig;}
        virtual ARResult setCameraConfig(const CCameraConfig& config);
        virtual void  getSupportedCameraConfigs(CCameraConfigList& list);
        virtual int getCameraTextureTarget();
        virtual void setCameraTextureTarget(int type);

        virtual ARTrackingState getTrackingState();
        virtual void setCameraTextureName(int texid);
        virtual void setDisplayGeomtry(int rotation, int width, int height);

        virtual CAnchor* acquireNewAnchor(const CPose& pose, ITrackable* ptrackable);
        virtual void detachAnchor(CAnchor* anchor);
        virtual void getAllTrackables(ARNodeType filtertype, std::vector<ITrackable*>& plist);
        virtual void getAllAnchors(std::vector<CAnchor*>& plist);

        virtual void getViewMatrix( float* out_col_major_4x4 );
        virtual void getProjectionMatrix(float near, float far, float* dest_col_major_4x4);

        /**
         * 获取一个AlgorithmResultWrapper，这个类利用局部变量自动释放的原理
         * 构造时持有session.m_ResultMutex锁，析构时释放该锁
         * @param type
         * @return
         */
        virtual AlgorithmResultWrapper getAlgorithmResult(int type);
        AlgorithmResultPack m_AlgorithmResultPack;

        virtual int32_t getPlaneVetexCount();
        virtual bool getPlaneVertexArray(float* vertex_array);
        virtual int32_t getPlaneIndexCount();
        virtual bool getPlaneIndexArray(unsigned short* index_arrayu);

        virtual void addFrame(CFrame* pframe);
        virtual void removeFrame(CFrame* pframe);

        virtual void getVersion(std::string& version);

        virtual bool isSLAMIntializing();
        virtual bool isAlgorithmStart();
        virtual float getVerticalFov();
        virtual int getScreenRotate();

        virtual int32_t getVideoWidth();
        virtual int32_t getVideoHeight();
        virtual ARPixelFormat getVideoFormat();
        virtual bool isImgBufferDirty()const;

        virtual void getUpdatedTrackables(std::vector<ITrackable*>& plist, ARNodeType type);
        virtual void getUpdatedAnchors(std::vector<CAnchor*> &plist);
        virtual void getAnchorsByTrackable(ITrackable* trackable, std::vector<CAnchor*>& plist);
        virtual ITrackable* getTrackableById(int32_t id);
        virtual float getDistanceFromCamera(CSession* pSession, CHitResult* hitresult);
        virtual CFrame* createFrame(CSession* pSession);
        virtual bool copyYUVImageDataLocked(unsigned char* y_data, unsigned char* u_data, unsigned char* v_data);

        virtual void setValue(ARParameterEnum type, void* value){}
        virtual void beginEye(uint32_t whicheye){}
        virtual void endEye(uint32_t whicheye){}
        virtual void submitFrame(const int32_t leftEyeTextureId, const int32_t rightEyeTextureId){}
        virtual void getFloatValue(ARParameterEnum type, float* value, int size);
        virtual void getIntValue(ARParameterEnum type, int32_t * value, int size);
        virtual void getBackgroundData(char **image_data, int *width, int *height);

        //// timewarp / prediction related
        void getProjectionMatrixSeeThrough(float near, float far, float* dest_col_major_4x4);
        void getPredictedViewMatrix(float *out_col_major_4x4);
        void beginRenderEye(int eyeId);
        void endRenderEye(int eyeId);
        void submitRenderFrame(int leftEyeTextureId, int rightEyeTextureId);
        void setWindow(void *window, bool newPbuffer);

        ARResult hostCloudAnchor(CAnchor* anchor, CAnchor** out_anchor);
        ARResult resolveCloudAnchor(std::string anchorId, CAnchor** out_anchor);

        void setKeyAndSecret(const char* key, const char* secret);
        void setUserJson(const char* json);
        void getSDKInfo(std::string &infostr);

    protected:

        virtual void tagAnchorUpdateFlag(ITrackable* ptrackable, SLAMAlgorithmResult* result);

        virtual void updateFrameSLAMResult(CFrame* pframe);
        virtual void updateTrackable();
        virtual void updatePlaneNode();
        virtual void updateImageNode();
        void updateObjectNode();


        virtual void updateStatistic();


        virtual void clearResult();
        virtual void clearAnchor();
        virtual void clearTrackable();
        virtual void clearFrames();
        virtual void resetResult();
        virtual void resetAnchor();
        virtual void resetTrackable();

        virtual int parse_pair(const char *str, int *first, int *second, char delim, char **endptr = NULL);
        virtual void parseSizesList(const char *sizesStr, std::vector<vec2i> &sizes);

        virtual void createArEngine();

        SLAMResultCallback m_result_callback;
        virtual void initAlgorithmResult();
        virtual void setAlgorithmProxy();
        virtual void updateFOV();
        void updateConfig(const CConfig& config);
        void displayInfoConfigure();

        virtual ARResult startAlgorithm();
        virtual ARResult stopAlgorithm();
        virtual ARResult resume();
        virtual ARResult pause();
        virtual void destory();

        friend class OnCreateState;
        friend class OnResumeState;
        friend class OnPauseState;
        friend class OnStartState;
        friend class OnStopState;

    public:
        pthread_mutex_t m_Mutex;
        pthread_mutex_t m_cloud_Mutex;
        pthread_mutex_t m_Image_Mutex;

        ARPixelFormat m_ImageFormat;
        int         m_ImageStride;

        int32_t     m_VideoFrameCount;
        int64_t     m_FrameTimestamp;

//        char        m_MapId[128];
        std::string m_AppKey;
        std::string m_AppSecret;
//        CAnchor*    m_CurrentAnchor;

        bool m_bAlgorithmStart;
        bool m_bSessionResume;
        CConfig    m_Config;
        
        ArWorldLifeCycle* m_LifeCycle;
    protected:
        void*       m_appContext;

        void*       m_pEnv;
        CCameraConfig m_CameraConfig;

        int32_t     m_Version;

        int32_t     m_ScreenRotate;
        int32_t     m_ScreenWidth;
        int32_t     m_ScreenHeight;

        float       m_ScreenAspect;

        int32_t     m_VideoWidth;
        int32_t     m_VideoHeight;
        int32_t     m_ProcessWidth;
        int32_t     m_ProcessHeight;
        float       m_VideoAspect;
        float       m_VideoFovX;
        float       m_VideoFovY;

        double      m_BeginTime;
        double      m_CurrentTime;
        float       m_VideoFPS;

        int         m_OESTextureId;

        std::map<CFrame*, CFrame*>        m_Frames;
        std::map<CAnchor*, CAnchor*>      m_Anchors;
        std::map<int32_t , ITrackable*>    m_Planes;
        std::map<int32_t, ITrackable*>     m_Markers;
        std::map<int32_t, ITrackable*>     m_Objects;

        IArEngineBase*          m_pArEngine;

        CAuxRender*             m_pAuxRender;
        CBgYUVRender*           m_pBgYUVRender;

        GLuint m_RGBTexture;
        COffscreenRenderTarget*  m_pRenderToTexture;
        COffscreenRenderTarget*  m_pFlipTexture;

        int64_t m_SessionPtr;
        bool m_IsFirstResume;

        int m_anchorupdate_count;
        int m_anchorupdate_freq;

//        int32_t m_EngineType;

        bool m_isCameraIntrinsicsUpdate;
        bool m_isSupportDoubleStream;
        int  m_isArServiceExist;
        bool m_isArServiceSingleAlgorithm;
    };
}

#endif  // ARSESSION_C_API_H_
