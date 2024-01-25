#include <map>
#include "jni.h"
//#define EGL_EGLEXT_PROTOTYPES
//#include <EGL/egl.h>
//#include <EGL/eglext.h>
//#include <android/hardware_buffer.h>
//#include <android/hardware_buffer_jni.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <pthread.h>
#include <vector>
#include <log.h>
#include "OffscreenRenderTarget.h"
#include "ArBgYUVRender.h"
#include "ShaderUtils.h"
#include "picojson.h"
#include "Utils.h"
#define DEVICE_PARAM_NAME           "device_params.yaml"
#ifdef __cplusplus
extern "C" {
#endif

//OES Texture
//#define AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420 0x23
//static char* image_buffer = nullptr;
//static int buffer_size = 5;
//static std::vector<AHardwareBuffer*> graphic_buffer;
//static std::vector<EGLImageKHR> imagekhr_buffer;
static pthread_mutex_t image_lock = PTHREAD_MUTEX_INITIALIZER;
static bool image_dirty = false;
//static int index = 0;
//
//
//EGLint attrs[] = {
//        EGL_IMAGE_PRESERVED_KHR,    EGL_TRUE,
//        EGL_NONE,
//};
//
//static void createImageBuffer(int height, int width, int stride) {
//    AHardwareBuffer_Desc usage;
//    usage.format =  AHARDWAREBUFFER_FORMAT_Y8Cb8Cr8_420;
//    usage.height = height;
//    usage.width = width;
//    usage.layers = 1;
//    usage.rfu0 = 0;
//    usage.rfu1 = 0;
//    usage.usage = AHARDWAREBUFFER_USAGE_CPU_READ_NEVER | AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN |
//                  AHARDWAREBUFFER_USAGE_GPU_COLOR_OUTPUT;
//    usage.stride = stride;
//    EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//    for (int i = 0; i < buffer_size; ++i) {
//        AHardwareBuffer* temp_graphic_buf;
//        AHardwareBuffer_allocate(&usage, &temp_graphic_buf);
//        EGLClientBuffer clientBuf = eglGetNativeClientBufferANDROID(temp_graphic_buf);
//        graphic_buffer.push_back(temp_graphic_buf);
//        imagekhr_buffer.push_back(eglCreateImageKHR(disp, EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuf, attrs));
//    }
//}
//
//static void destroyImageBuffer() {
//    pthread_mutex_lock(&image_lock);
//    if(graphic_buffer.size() == buffer_size) {
//        for (int i = 0; i < buffer_size; ++i) {
//            AHardwareBuffer_release(graphic_buffer[i]);
//        }
//        graphic_buffer.clear();
//    }
//    if (imagekhr_buffer.size() == buffer_size) {
//        EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//        for (int i = 0; i < buffer_size; ++i) {
//            eglDestroyImageKHR(disp, imagekhr_buffer[i]);
//        }
//        imagekhr_buffer.clear();
//    }
//    index = 0;
//    pthread_mutex_unlock(&image_lock);
//}

//RGB Texture
using namespace standardar;
static CBgYUVRender* m_pBgYUVRender = nullptr;
static COffscreenRenderTarget*  m_pRenderToTexture = nullptr;
static COffscreenRenderTarget*  m_pFlipTexture = nullptr;
static GLuint m_RGBTexture = 0;
static int32_t m_YTexId = 0;
static int32_t m_UTexId = 0;
static int32_t m_VTexId = 0;
static unsigned char* image_buffer = nullptr;
static int buffer_size = 0;
static int image_width = 0;
static int image_height = 0;
static int image_stride = 0;
static jlong image_timestamp =0;
static SImageFormat image_format = SIMAGE_FORMAT_NON;
static int frameCount = 0;
static double beginTime = 0;
static double currentTime = 0;
static float fps = 0;

void setCameraYUVTextureName(int ytexid, int utexid, int vtexid)
{
    m_YTexId = ytexid;
    m_UTexId = utexid;
    m_VTexId = vtexid;

    glBindTexture(GL_TEXTURE_2D, m_YTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_width, image_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    if(image_format == SIAMGE_FORMAT_YUV_I420 || image_format == SIMAGE_FORMAT_YUV_YV12) {
        glBindTexture(GL_TEXTURE_2D, m_UTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_width / 2, image_height / 2,
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    } else if(image_format == SIMAGE_FORMAT_YUV_NV21 || image_format == SIMAGE_FORMAT_YUV_NV12) {
        glBindTexture(GL_TEXTURE_2D, m_UTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, image_width / 2, image_height / 2,
                     0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, NULL);
    }

    glBindTexture(GL_TEXTURE_2D, m_VTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_width / 2, image_height / 2,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
}

static void updateYUVTexture()
{
    glBindTexture(GL_TEXTURE_2D, m_YTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    unsigned char *frameRenderBuffer = image_buffer;

    glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, image_stride);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height,
                    GL_LUMINANCE, GL_UNSIGNED_BYTE, frameRenderBuffer);
    glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, 0);

    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
        LOGI("OpenGL error:%i", err);
    }

    if (image_format == SIAMGE_FORMAT_YUV_I420 || image_format == SIMAGE_FORMAT_YUV_YV12) {
        glBindTexture(GL_TEXTURE_2D, m_UTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        frameRenderBuffer = image_buffer + (4 * (image_stride / 2) * (image_height / 2)) -
                            (image_stride - image_width);
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, image_stride / 2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width / 2, image_height / 2,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, frameRenderBuffer);
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, 0);

        glBindTexture(GL_TEXTURE_2D, m_VTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        frameRenderBuffer = image_buffer + (5 * (image_stride / 2) * (image_height / 2)) -
                            (image_stride - image_width) * 3 / 2;
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, image_stride / 2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width / 2, image_height / 2,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, frameRenderBuffer);
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, 0);
    } else if(image_format == SIMAGE_FORMAT_YUV_NV21 || image_format == SIMAGE_FORMAT_YUV_NV12){
        glBindTexture(GL_TEXTURE_2D, m_UTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        frameRenderBuffer = image_buffer + (4 * (image_stride / 2) * (image_height / 2)) -
                            (image_stride - image_width);
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, image_stride / 2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width / 2, image_height / 2,
                        GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, frameRenderBuffer);
        glPixelStorei(GL_UNPACK_ROW_LENGTH_EXT, 0);
    }
}

static void renderYUVToRGB()
{
    if (m_pRenderToTexture == nullptr || m_pBgYUVRender == nullptr)
        return;

    m_pRenderToTexture->Enable();

    glClearColor(1.0, 0.0, 1.0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    m_pBgYUVRender->DrawYUV();
    m_pRenderToTexture->Disable();
}

static void flipTexture()
{
    if(m_pFlipTexture == nullptr || m_pBgYUVRender == nullptr || m_pRenderToTexture == nullptr)
        return;

    m_pFlipTexture->Enable();

    glClearColor(1.0, 0.0, 1.0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);

    m_pBgYUVRender->DrawFullScreen(m_pRenderToTexture->GetColorTexId());

    m_pFlipTexture->Disable();
}

int64_t getHPCounter()
{
#ifdef __ANDROID_API__
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t i64 = now.tv_sec*int64_t(1000000000L) + now.tv_nsec;
#else
    struct timeval now;
        gettimeofday(&now, 0);
        int64_t i64 = now.tv_sec*int64_t(1000000L) + now.tv_usec;
#endif
    return i64;
}

int64_t getHPFrequency()
{
#ifdef __ANDROID_API__
    return int64_t(1000000000L);
#else
    return int64_t(1000000L);
#endif
}

static double getCurrentSecond()
{
    static double appstart = getHPCounter();
    double hpcounter = getHPCounter();
    double hpfrequency = getHPFrequency();
    return (double)((hpcounter-appstart)/hpfrequency);
}

static void updateStatistic()
{
    currentTime = getCurrentSecond();
    double elapsetime = currentTime-beginTime;
    if(elapsetime>3.0)
    {
        fps = (float)frameCount/(float)elapsetime;
        beginTime = currentTime;
        frameCount = 0;
        LOGI("debugtime [client] video_fps=%f", fps);
    }
}

JNIEXPORT void JNICALL Java_com_standardar_sensor_camera_CameraUtils_cropImageNv12(JNIEnv *env,
                                                                                   jobject self,
                                                                                   jobject buf,
                                                                                   jint width,
                                                                                   jint height,
                                                                                   jint stride,
                                                                                   jint scanline,
                                                                                   jbyteArray out_) {
    void *image = env->GetDirectBufferAddress(buf);
    jbyte *out = env->GetByteArrayElements(out_, NULL);

    unsigned char *image_y = (unsigned char *) image;
    unsigned char *image_uv = image_y + stride * scanline;
    unsigned char *out_image_y = (unsigned char *) out;
    unsigned char *out_image_uv = out_image_y + width * height;

    for (int i = 0; i < height; i++) {

        int offsetSrc = i * stride + 68;
        int offsetDst = i * width;
        int uv_offsetSrc = (i >> 1) * stride + 68;
        int uv_offsetDst = (i >> 1) * width;

        memcpy(out_image_y + offsetDst, image_y + offsetSrc, width);

        if (i % 2 == 0) {
            unsigned char *out_uv = out_image_uv + uv_offsetDst;
            unsigned char *in_uv = image_uv + uv_offsetSrc;

            for (int j = 0; j < width; j += 2) {
                out_uv[j] = in_uv[j + 1];
                out_uv[j + 1] = in_uv[j];
            }
        }
    }

    env->ReleaseByteArrayElements(out_, out, 0);
}

JNIEXPORT void JNICALL
Java_com_standardar_sensor_camera_CameraUtils_cropImageNv12Buffer(JNIEnv *env,
                                                                  jobject self,
                                                                  jobject buf,
                                                                  jint width,
                                                                  jint height,
                                                                  jint stride,
                                                                  jint scanline,
                                                                  jint tof_width,
                                                                  jint tof_height,
                                                                  jobject out_rgb_buffer,
                                                                  jobject out_tof_buffer) {
    jbyte *image = (jbyte *) env->GetDirectBufferAddress(buf);
    jbyte *out_rgb = (jbyte *) env->GetDirectBufferAddress(out_rgb_buffer);
    jbyte *out_tof = (jbyte *) env->GetDirectBufferAddress(out_tof_buffer);
    if (width == stride) {
        memcpy(out_rgb, image + 68, stride * height);
        memcpy(out_rgb + width * height, image + 68 + stride * scanline, width * height / 2);
    } else {
        unsigned char *image_y = (unsigned char *) image;
        unsigned char *image_uv = image_y + stride * scanline;
        unsigned char *out_image_y = (unsigned char *) out_rgb;
        unsigned char *out_image_uv = out_image_y + width * height;

        for (int i = 0; i < height; i++) {
            int offsetSrc = i * stride + 68;
            int offsetDst = i * width;
            int uv_offsetSrc = (i >> 1) * stride + 68;
            int uv_offsetDst = (i >> 1) * width;
            memcpy(out_image_y + offsetDst, image_y + offsetSrc, width);
            if (i % 2 == 0) {
                unsigned char *out_uv = out_image_uv + uv_offsetDst;
                unsigned char *in_uv = image_uv + uv_offsetSrc;
                for (int j = 0; j < width; j += 2) {
                    out_uv[j] = in_uv[j + 1];
                    out_uv[j + 1] = in_uv[j];
                }
            }
        }
    }
    memcpy(out_tof, image + 68 + stride * scanline * 3 / 2, tof_width * tof_height * 2);
}

JNIEXPORT void JNICALL
Java_com_standardar_sensor_camera_CameraUtils_copyImageBuffer(JNIEnv *env,
                                                                  jobject self,
                                                                  jobject buf,
                                                                  jint width,
                                                                  jint height,
                                                                  jint stride,
                                                                  jint format,
                                                                  jlong timestamp) {
//    TODO: OES Texture
//    jbyte* img_ptr = (jbyte*) env->GetDirectBufferAddress(buf);
//    pthread_mutex_lock(&image_lock);
//    int size = stride * height * 3 / 2;
//    if (graphic_buffer.size() == 0 && imagekhr_buffer.size() == 0)
//    {
//        createImageBuffer(height, width, stride);
//    }
//    AHardwareBuffer_lock(graphic_buffer[index], AHARDWAREBUFFER_USAGE_CPU_WRITE_OFTEN, -1, nullptr, (void**)&image_buffer);
//    memcpy(image_buffer, img_ptr, size);
//    AHardwareBuffer_unlock(graphic_buffer[index], nullptr);
//    image_dirty = true;
//    pthread_mutex_unlock(&image_lock);
    pthread_mutex_lock(&image_lock);
    jbyte* img_ptr = (jbyte*) env->GetDirectBufferAddress(buf);
    int size = stride * height * 3 / 2;
    if (image_buffer == nullptr || buffer_size != size)
    {
        delete[] image_buffer;
        image_buffer = new unsigned char[size];
        buffer_size = size;
    }
    memcpy(image_buffer, img_ptr, size);
    image_width = width;
    image_height = height;
    image_stride = stride;
    image_format = (SImageFormat) format;
    image_dirty = true;
    image_timestamp = timestamp;
    ++frameCount;
    pthread_mutex_unlock(&image_lock);
}

JNIEXPORT void JNICALL
Java_com_standardar_sensor_camera_CameraUtils_updateOESTexture(JNIEnv *env,
                                                                    jobject self, jint texture_id) {
//    TODO: OES Texture
//    pthread_mutex_lock(&image_lock);
//    if (image_dirty) {
//        if (imagekhr_buffer.size() < index && imagekhr_buffer[index] == nullptr) {
//            pthread_mutex_unlock(&image_lock);
//            return;
//        }
//        glBindTexture(GL_TEXTURE_EXTERNAL_OES, (GLuint)texture_id);
//        glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, (GLeglImageOES)imagekhr_buffer[index]);
//        glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
//        index = (index + 1) % buffer_size;
//        image_dirty = false;
//        pthread_mutex_unlock(&image_lock);
//        return;
//    }
//    pthread_mutex_unlock(&image_lock);
    pthread_mutex_lock(&image_lock);
    if(image_buffer == nullptr || texture_id == 0) {
        pthread_mutex_unlock(&image_lock);
        return;
    }

    if(m_pBgYUVRender == nullptr){
        m_pBgYUVRender = new CBgYUVRender(image_format);
        m_pBgYUVRender->InitializeGlContent();
        setCameraYUVTextureName(m_pBgYUVRender->GetyuvYTextureId(),
                                m_pBgYUVRender->GetyuvUTextureId(),
                                m_pBgYUVRender->GetyuvVTextureId());
    }

    if(m_pRenderToTexture == nullptr)
    {
        if(m_RGBTexture != 0)
        {
            SAFE_DELETE_TEXTURE(m_RGBTexture);
        }

        glGenTextures(1, &m_RGBTexture);
        glBindTexture(GL_TEXTURE_2D, m_RGBTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        m_pRenderToTexture = new COffscreenRenderTarget();
        m_pRenderToTexture->InitializeWithExt(m_RGBTexture, kCONTEXT_COLOR | kCONTEXT_DEPTH, image_width, image_height);
    }

    if(m_pFlipTexture == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        m_pFlipTexture = new COffscreenRenderTarget();
        m_pFlipTexture->InitializeWithExt(texture_id, kCONTEXT_COLOR | kCONTEXT_DEPTH, image_width, image_height);
    }

    if(image_dirty) {
        updateYUVTexture();
        image_dirty = false;
    }

    renderYUVToRGB();
    flipTexture();

    updateStatistic();

    pthread_mutex_unlock(&image_lock);
}

JNIEXPORT void JNICALL
Java_com_standardar_sensor_camera_CameraUtils_releaseImageBuffer(JNIEnv *env,
                                                              jobject self){
    //destroyImageBuffer();
    StandardAR_SAFE_DELETE(image_buffer)
    buffer_size = 0;
    SAFE_DELETE_TEXTURE(m_RGBTexture)
    StandardAR_SAFE_DELETE(m_pBgYUVRender)
    StandardAR_SAFE_DELETE(m_pRenderToTexture)
    StandardAR_SAFE_DELETE(m_pFlipTexture)
}

JNIEXPORT jlong JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageTimestamp(JNIEnv *env,
                                                                   jobject self){
    pthread_mutex_lock(&image_lock);
    jlong timestamp = image_timestamp;
    pthread_mutex_unlock(&image_lock);
    return timestamp;
}

JNIEXPORT jint JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageWidth(JNIEnv *env,
                                                                   jobject self){
    pthread_mutex_lock(&image_lock);
    jint width = image_width;
    pthread_mutex_unlock(&image_lock);
    return width;
}

JNIEXPORT jint JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageHeight(JNIEnv *env,
                                                                   jobject self){
    pthread_mutex_lock(&image_lock);
    jint height = image_height;
    pthread_mutex_unlock(&image_lock);
    return height;
}

JNIEXPORT jint JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageStride(JNIEnv *env,
                                                                jobject self){
    pthread_mutex_lock(&image_lock);
    jint stride = image_stride;
    pthread_mutex_unlock(&image_lock);
    return stride;
}

JNIEXPORT jint JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageFormat(JNIEnv *env,
                                                                jobject self){
    pthread_mutex_lock(&image_lock);
    jint format = image_format;
    pthread_mutex_unlock(&image_lock);
    return format;
}

JNIEXPORT jboolean JNICALL
Java_com_standardar_sensor_camera_CameraUtils_getCurImageData(JNIEnv *env,
                                                                jobject self, jlong y_ptr, jlong u_ptr, jlong v_ptr){
    pthread_mutex_lock(&image_lock);
    unsigned char *y_data = reinterpret_cast<unsigned char*>(y_ptr);
    unsigned char *u_data = reinterpret_cast<unsigned char*>(u_ptr);
    unsigned char *v_data = reinterpret_cast<unsigned char*>(v_ptr);
    int size = image_width * image_height;
    if(image_buffer == nullptr || buffer_size < size) {
        pthread_mutex_unlock(&image_lock);
        return JNI_FALSE;
    }

    if(y_data == NULL || u_data == NULL){
        pthread_mutex_unlock(&image_lock);
        return JNI_FALSE;
    }

    switch(image_format) {
        case SIMAGE_FORMAT_YUV_NV21:
        case SIMAGE_FORMAT_YUV_NV12:
            if (image_width == image_stride) {
                memcpy(y_data, image_buffer, size);
                memcpy(u_data, image_buffer + size, size / 2);
            } else {
                for (int i = 0; i < image_height; ++i) {
                    memcpy(y_data, image_buffer + i * image_stride, image_width);
                }

                for (int i = 0; i < image_height / 2; ++i) {
                    memcpy(u_data, image_buffer + (image_height + i) * image_stride, image_width);
                }
            }
            break;
        case SIMAGE_FORMAT_YUV_YV12:
            if (v_data == NULL) {
                pthread_mutex_unlock(&image_lock);
                return JNI_FALSE;
            }

            if (image_width == image_stride) {
                memcpy(y_data, image_buffer, size);
                memcpy(v_data, image_buffer + size, size / 4);
                memcpy(u_data, image_buffer + size * 5 / 4, size / 4);
            } else {
                for (int i = 0; i < image_height; ++i) {
                    memcpy(y_data, image_buffer + i * image_stride, image_width);
                }

                for (int i = 0; i < image_height / 4; ++i) {
                    memcpy(v_data, image_buffer + (image_height + i) * image_stride, image_width);
                }

                for (int i = 0; i < image_height / 4; ++i) {
                    memcpy(u_data, image_buffer + (image_height * 5 / 4 + i) * image_stride, image_width);
                }
            }
            break;
        case SIAMGE_FORMAT_YUV_I420:
            if (v_data == NULL) {
                pthread_mutex_unlock(&image_lock);
                return JNI_FALSE;
            }

            if (image_width == image_stride) {
                memcpy(y_data, image_buffer, size);
                memcpy(u_data, image_buffer + size, size / 4);
                memcpy(v_data, image_buffer + size * 5 / 4, size / 4);
            } else {
                for (int i = 0; i < image_height; ++i) {
                    memcpy(y_data, image_buffer + i * image_stride, image_width);
                }

                for (int i = 0; i < image_height / 4; ++i) {
                    memcpy(u_data, image_buffer + (image_height + i) * image_stride, image_width);
                }

                for (int i = 0; i < image_height / 4; ++i) {
                    memcpy(v_data, image_buffer + (image_height * 5 / 4 + i) * image_stride, image_width);
                }
            }
            break;
        default:
            break;
    }

    pthread_mutex_unlock(&image_lock);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_com_standardar_sensor_camera_CameraUtils_isImageBufferDirty(JNIEnv *env,
                                                                jobject self){
    pthread_mutex_lock(&image_lock);
    jboolean result = (jboolean)image_dirty;
    pthread_mutex_unlock(&image_lock);
    return result;
}
JNIEXPORT jstring JNICALL
Java_com_standardar_sensor_imu_IMUReader_parserDeviceParmaPath(JNIEnv* env, jobject self, jstring support_txt) {
    jboolean isCopy;
    const char* str_str = env->GetStringUTFChars(support_txt, &isCopy);
    std::string jsonStr(str_str);
    env->ReleaseStringUTFChars(support_txt, str_str);
    picojson::value v;
    std::string err = picojson::parse(v, jsonStr);
    if (!err.empty()) {
        LOGI("parser support file error %s", err.c_str());
        return  env->NewStringUTF("");
    }

    const auto& array = v.get<picojson::array>();
    std::string brand = Utils::getPhoneBrand();
    std::string model = Utils::getPhoneModel();
    bool isSupportDoubleStream = false;
    for (picojson::array::const_iterator it = array.begin(); it != array.end(); it++)
    {
        picojson::object obj = it->get<picojson::object>();
        picojson::array& slam_array = obj["SLAM"].get<picojson::array>();
        for (picojson::array::const_iterator slam_it = slam_array.begin(); slam_it != slam_array.end(); slam_it++)
        {
            picojson::object brand_obj = slam_it->get<picojson::object>();
            for(auto brand_obj_iter = brand_obj.begin(); brand_obj_iter != brand_obj.end(); ++brand_obj_iter)
            {
                picojson::array brand_array = brand_obj_iter->second.get<picojson::array>();
                picojson::array::const_iterator brand_array_iter = brand_array.begin();
                picojson::object name_obj = brand_array_iter->get<picojson::object>();
                std::string name_attr = name_obj["name"].get<std::string>();
//                m_Brand_Map[brand_obj_iter->first] = name_attr;
                if (brand_obj_iter->first != brand)
                    continue;

                ++brand_array_iter;
                picojson::object model_obj = brand_array_iter->get<picojson::object>();
                picojson::array model_obj_array = model_obj["Model"].get<picojson::array>();
                for (picojson::array::const_iterator model_iter = model_obj_array.begin();
                     model_iter != model_obj_array.end(); ++model_iter)
                {
                    picojson::object per_model_obj = model_iter->get<picojson::object>();
//                    m_Model_Map[per_model_obj["model"].get<std::string>().c_str()] = per_model_obj["name"].get<std::string>().c_str();
                    if (per_model_obj["model"].get<std::string>() == model){
                        if(isSupportDoubleStream) {
                            std::string res = "multiple/double_stream/param/" + name_attr + "/" +
                                              per_model_obj["name"].get<std::string>() + "/" +
                                              DEVICE_PARAM_NAME;
                            return env->NewStringUTF(res.c_str());
                        } else {
                            std::string res = "multiple/param/" + name_attr + "/" + per_model_obj["name"].get<std::string>() + "/" + DEVICE_PARAM_NAME;
                            return env->NewStringUTF(res.c_str());
                        }

                    }
                }

                if(isSupportDoubleStream) {
                    std::string res = "multiple/double_stream/param/" + name_attr + "/default/" + DEVICE_PARAM_NAME;
                    return env->NewStringUTF(res.c_str());
                } else {
                    std::string res = "multiple/param/" + name_attr + "/default/" + DEVICE_PARAM_NAME;
                    return env->NewStringUTF(res.c_str());
                }
            }
        }
    }

    std::string default_path = "";
    if(isSupportDoubleStream)
        default_path = default_path + "multiple/double_stream/param/default/default/" + DEVICE_PARAM_NAME;
    else
        default_path = default_path + "multiple/param/default/default/" + DEVICE_PARAM_NAME;

    return env->NewStringUTF(default_path.c_str());
}

#ifdef __cplusplus
};
#endif