#include "CameraEngine.h"
#include "ArCommon.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "ArJniHelpler.h"
#include "StandardARCommon.h"
#include "DynamicLoader.h"
using namespace standardar;

CameraEngine::CameraEngine() : m_CameraReady(false),
                                              m_Preview_Width(1280),
                                              m_Preview_Height(720),
                                              m_CameraSource_Obj(nullptr),
                                              m_Context(nullptr)

{
    m_Texid = 0;
    m_SessionCreated = false;
    m_DisplayWidth = 0;
    m_DisplayHeight = 0;
}

CameraEngine::~CameraEngine()
{
}

void CameraEngine::StartPreview()
{
   StartPreviewSingleJava();
}

void CameraEngine::StopPreview()
{
    StopPreviewSingleJava();
}

void CameraEngine::StartPreviewSingleJava()
{
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    if (m_Texid > 0)
    {
        cameraSourceWrapper.setTextureId(m_Texid);
    }

    if (!m_CameraReady || m_SessionCreated)
    {
        LOGW("start camera preview failed %d %d", m_CameraReady, m_SessionCreated);
        return;
    }
    LOGI("start preview single");

    cameraSourceWrapper.startPreview();
    m_SessionCreated = true;
}

void CameraEngine::StopPreviewSingleJava()
{
    if (!m_CameraReady)
    {
        LOGW("stop camera preview failed %d", m_CameraReady);
        return;
    }
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.stopPreview();
    m_SessionCreated = false;
}

void CameraEngine::StartPreviewMultipleJava()
{
    if (!m_CameraReady || !m_Texid || m_SessionCreated)
    {
        LOGW("mul-java Create session failed %d %d %d", m_CameraReady, m_Texid, m_SessionCreated);
        return;
    }
    LOGI("start preview mul");
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.setTextureId(m_Texid);
    cameraSourceWrapper.startPreview();
    m_SessionCreated = true;
}

bool CameraEngine::IsReady()
{
    return m_CameraReady;
}

void CameraEngine::SetNotify(jobject object_client)
{
    if (object_client == nullptr)
        return;
    if (m_CameraSource_Obj == nullptr)
        return;
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.setCameraNotify(object_client);
}

void CameraEngine::OpenCamera(const CConfig& config)
{
    if (config.m_SLAMDeviceType == ARDEVICE_TYPE_GLASS)
        m_Texid = -1;
    OpenCameraFromJava(config);
}

void CameraEngine::OpenCameraFromJava(const CConfig& config)
{
    if (m_CameraReady)
    {
        LOGW("camera is init already from java");
        return;
    }

    if (m_Context == nullptr)
    {
        LOGW("camera init param is invalid %p %d", m_Context, m_Texid);
        return;
    }

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject camera_source_obj = CameraSourceWrapper::getInstance(m_Context);
    if (camera_source_obj == nullptr)
    {
        LOGI("camera engine can not get camera source object");
        return;
    }

    m_CameraSource_Obj = env->NewGlobalRef(camera_source_obj);
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    std::vector<int> cameraids;
    cameraids.push_back(config.m_CameraTextureTarget);
    cameraSourceWrapper.setRenderCamera(cameraids[0]);
    jintArray cameraids_jintArray = env->NewIntArray(cameraids.size());
    env->SetIntArrayRegion(cameraids_jintArray, 0, cameraids.size(), &cameraids[0]);
    cameraSourceWrapper.openCamera(cameraids);
    env->DeleteLocalRef(cameraids_jintArray);

    std::string support_szie_str = cameraSourceWrapper.getSupportSizeString();
    SetJavaSupportSize(support_szie_str.c_str());
    std::vector<int> size = cameraSourceWrapper.getDisplaySize();
    SetDisplaySize(size[0],size[1]);
    m_CameraReady = true;
}

void CameraEngine::SetContext(jobject context)
{
    m_Context = context;
}

void CameraEngine::SetSession(int64_t session_ptr)
{
    m_SesstionPtr = session_ptr;
}

void CameraEngine::SetTextureId(int id)
{
    m_Texid = id;
}

void CameraEngine::SetPreviewSize(int width, int height)
{
    m_Preview_Width = width;
    m_Preview_Height = height;
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.setPreviewSize(m_Preview_Width, m_Preview_Height);
}

void CameraEngine::GetPreviewSize(int &width, int &height) {
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.getPreviewSize(width, height);
}

void CameraEngine::GetProcessSize(int &width, int &height) {
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.getProcessSize(width, height);
}

void CameraEngine::CloseCameraFromJava()
{
    if (!m_CameraReady)
    {
        LOGW("camera is not opened");
        return;
    }

    if (m_Context == nullptr || m_CameraSource_Obj == nullptr)
    {
        LOGW("camera engine param error:%p %p", m_Context, m_CameraSource_Obj);
        return;
    }
    JNIEnvProxy env(DynamicLoader::s_JVM);
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.closeCamera();
    env->DeleteGlobalRef(m_CameraSource_Obj);
    m_CameraSource_Obj = nullptr;
    m_Context = nullptr;
    m_CameraReady = false;
    m_SessionCreated = false;
}

void CameraEngine::CloseCamera()
{
    CloseCameraFromJava();
}

float CameraEngine::GetFovH()
{
    if(m_CameraSource_Obj != nullptr) {
        CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
        return cameraSourceWrapper.getFovH();
    }
    else {
        return 0.0f;
    }
}



float CameraEngine::GetFovV()
{
    if (m_CameraSource_Obj != nullptr) {
        CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
        return cameraSourceWrapper.getFovV();
    }
    else
        return 0.0f;
}

void CameraEngine::SetJavaSupportSize(const char *str)
{
    m_JavaSupportSizeStr = std::string(str);
}

const char* CameraEngine::GetJavaSupportSize()
{
    return m_JavaSupportSizeStr.c_str();
}

void CameraEngine::SetDisplaySize(int width, int height)
{
    LOGI("set display size:%d %d", width, height);
    m_DisplayWidth = width;
    m_DisplayHeight = height;
}

void CameraEngine::SetScreenRotate(int screenRotate)
{
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    cameraSourceWrapper.setScreenRotate(screenRotate);
}

void CameraEngine::UpdateTexture()
{
    if (m_CameraReady)
    {
        CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
        cameraSourceWrapper.updateTexture();
    }
}

void CameraEngine::Wait()
{
    if (m_CameraReady)
    {
        CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
        cameraSourceWrapper.waitForNextFrame();
    }
}

void CameraEngine::UpdateImageInfo(int &format, int &stride, int64_t &timestamp) {
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    format = cameraSourceWrapper.getCurImageFormat();
    stride = cameraSourceWrapper.getCurImageStride();
    timestamp = cameraSourceWrapper.getCurImageTimestamp();
}

bool CameraEngine::CopyImageData(unsigned char *y_data, unsigned char *u_data, unsigned char *v_data) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    return cameraSourceWrapper.getCurImageData(y_data, u_data, v_data);
}

bool CameraEngine::isImgBufferDirty(){
    CameraSourceWrapper cameraSourceWrapper(m_CameraSource_Obj, true);
    return cameraSourceWrapper.isImageBufferDirty();
}