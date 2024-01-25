#ifndef STANDARD_AR_CAMERAENGINE_C_API_H
#define STANDARD_AR_CAMERAENGINE_C_API_H

#include <jni.h>
#include <functional>
#include "ArMath.h"
#include "ArConfig.h"
#include <string>


namespace standardar {

    class CSession;
    class CameraEngine {
    public:
        explicit CameraEngine();

        ~CameraEngine();
        void OpenCamera(const CConfig& config);
        void CloseCamera();
        void StartPreview();
        void StopPreview();

        float GetFovH();
        float GetFovV();
        void SetScreenRotate(int rotate);
        void SetContext(jobject context);
        void SetSession(int64_t session_ptr);
        void SetTextureId(int id);
        void SetPreviewSize(int width, int height);
        void GetPreviewSize(int& width, int& height);
        void GetProcessSize(int &width, int &height);
        bool IsReady();
        void SetJavaSupportSize(const char* str);
        const char* GetJavaSupportSize();
        void SetDisplaySize(int width, int height);
        void SetNotify(jobject object_client);
        void UpdateTexture();
        void Wait();
        void UpdateImageInfo(int &format, int &stride, int64_t &timestamp);
        bool CopyImageData(unsigned char *y_data, unsigned char *u_data,  unsigned char* v_data);
        bool isImgBufferDirty();

    private:

        void OpenCameraFromJava(const CConfig& config);
        void CloseCameraFromJava();

        void StartPreviewSingleJava();
        void StopPreviewSingleJava();
        void StartPreviewMultipleJava();

    private:
        bool m_CameraReady;
        bool m_SessionCreated;
        int m_Preview_Width;
        int m_Preview_Height;

        jobject m_CameraSource_Obj;

        int m_Texid;
        int64_t m_SesstionPtr;
        std::string m_JavaSupportSizeStr;

        jobject m_Context;

        int m_DisplayWidth;
        int m_DisplayHeight;


    };
}

#endif
