
#ifndef STANDARD_AR_ARCOMMON_C_API_H_
#define STANDARD_AR_ARCOMMON_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <string>
#include <cstring>
#include <vector>
#include <assert.h>
#include <jni.h>
#include "log.h"
#include "ArMath.h"

#ifdef __ANDROID_API__
# include <time.h> // clock, clock_gettime

#else
# include <sys/time.h> // gettimeofday
#endif


#define StandardAR_SAFE_DELETE(ptr) \
if(ptr != NULL) \
{delete ptr; ptr = NULL;}


#define SAFE_DELETE_TEXTURE(texid) \
if(texid && glIsTexture(texid)) \
{glDeleteTextures(1, &texid); texid = 0;}

#define SAFE_DELETE_PROGRAM(shaderid) \
if(shaderid && glIsProgram(shaderid)) \
{glDeleteProgram(shaderid); shaderid = 0;}

#define SAFE_DELETE_BUFFER(bufferid) \
if(bufferid && glIsBuffer(bufferid)) \
{glDeleteBuffers(1, &bufferid); bufferid = 0;}


#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct Rect
    {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    } Rect;

    typedef struct PointF
    {
        float x;
        float y;
    } PointF;
    typedef struct Point3F
    {
        float x;
        float y;
        float z;
    } Point3F;
    typedef struct QuaternionF
    {
        float x;
        float y;
        float z;
        float w;
    } QuaternionF;

    typedef struct CvHandTrackingResult {
        int32_t id;//不可用
        int32_t handType;//手势类型 0：OK  1：剪刀  2：点赞  3：布  4：手枪  5：爱心  6： 托举   7： 拳头   8： 作揖
        //9：比心 10：指尖 11.666 12：双手合十  13：三根手指 14：四根手指 15：我爱你 16：rock 17:未知

        Rect handRect;//手势框
        float score;//手势置信度，范围0-1
        int32_t handSide;//手心:0、手背:1
        int32_t handToward;//手势朝向
        Point3F palmNormal;//手掌朝向法向量
        Point3F palmCenter;//手掌中心/重心法向量
        int32_t point2DCount;//2D手指关键点数量
        std::vector<Point3F> points2D;
        std::vector<Point3F> points3D;
        int32_t point3DCount;//2D手指关键点数量

        std::vector<float> jointsNorm;//四元数
        int32_t jointsNormCount;//四元数数量


        int32_t rgbSegWidth;//rgb 图像宽度
        int32_t rgbSegHeight;//rgb 图像高度
        char* rgbSegData;
        int32_t rgbSegDataSize;

        int32_t depthSegWidth;//depth 图像宽度
        int32_t depthSegHeight;//depth 图像高度
        char* depthSegData;
        int32_t depthSegDataSize;
    } CvHandTrackingResult;


#ifdef __cplusplus
}
#endif

double  getCurrentSecond();


// matrix
typedef enum MatrixConvertFlag
{
    NO_CONVERT,
    LANDSCAPERIGHT_TO_PORTRAIT,
    LANDSCAPERIGHT_TO_LANDSCAPELEFT,
    LANDSCAPERIGHT_TO_REVERSEPORTAIT
} MatrixConvertFlag;

extern void getPhoneModel(std::string& phone_model);
extern void getPhoneBrand(std::string& phone_brand);
extern std::string getProperty(std::string property);
// Check GL error, and abort if an error is encountered.
//
// @param operation, the name of the GL function call.
void StandardAR_CheckGlError(const char* operation);

// Create a shader program ID.
//
// @param vertex_source, the vertex shader source.
// @param fragment_source, the fragment shader source.
// @return
GLuint StandardAR_CreateProgram(const char* vertex_source, const char* fragment_source);


#if defined(__LP64__)
#define __AR_RESERVED_INITIALIZER , 0
#else
#define __AR_RESERVED_INITIALIZER
#endif

#define  AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER   {0x4000 __AR_RESERVED_INITIALIZER}

jfloatArray vec3fToFloatArray(JNIEnv* env, standardar::vec3f vec);
standardar::vec3f floatArrayToVec3f(JNIEnv* env, jfloatArray float_array);
void copyVec3fToFloatArray(JNIEnv* env, standardar::vec3f& vec, jfloatArray float_array);
void copyFloatArrayToVec3f(JNIEnv* env, jfloatArray float_arr, standardar::vec3f& vec);

class ScopeMutex
{
public:
    ScopeMutex(pthread_mutex_t* mutex)
    {
        m_Mutex = mutex;
        pthread_mutex_lock(mutex);
    }
    ~ScopeMutex()
    {
        pthread_mutex_unlock(m_Mutex);
    }

private:
    pthread_mutex_t* m_Mutex;
};

enum BUFFER_ORDER {
    BUFFER_BIG_ENDIAN = 0,
    BUFFER_LITTLER_ENDIAN = 1
};

class DirectByteBufferWrapper{
public:
    DirectByteBufferWrapper(void* ptr, int cap, BUFFER_ORDER order) :
            m_Pos(0),
            m_Order(order){
        m_Buffer_Ptr = (jbyte*)ptr;
        m_Capcity = cap;
        m_Native_Order = checkNativeOrder();
    }
    template <typename T>
    T get() {
        T* tPtr = (T*)(offset());
        m_Pos += sizeof(T);
        return toNativeOrder(*tPtr);
    }
    template <typename T>
    void put(T value) {
        int size = sizeof(T);
        jbyte* iPtr = offset();
        m_Pos += sizeof(T);
        memset(iPtr, 0, sizeof(T));
        int64_t v = 0;
        memcpy(&v, &value, size);
        int offset = size * 8 - 8;
        if (m_Order == BUFFER_BIG_ENDIAN) {
            for (int i = 0; i < size; ++i) {
                *iPtr |= ((v >> (offset - i * 8)) & 0xff);
                iPtr++;
            }
        } else if (m_Order == BUFFER_LITTLER_ENDIAN) {
            for (int i = 0; i < size; ++i) {
                *iPtr |= ((v >> (i * 8)) & 0xff);
                iPtr++;
            }
        }

    }

    template <typename T>
    T toNativeOrder(T value) {
        int64_t v = 0;
        memcpy(&v, &value, sizeof(T));
        int64_t* iPtr = &v;
        if (m_Native_Order == m_Order) {
            return value;
        } else if (m_Native_Order != m_Order) {
            int size = sizeof(T);
            int offset = size * 8 - 8;
            int64_t res = 0;
            for (int i = 0; i < size; ++i) {
                res |= ((*iPtr >> (offset - i * 8)) & 0xFF) << (i * 8);
            }
            return *((T*)&res);
        }
        return 0;
    }

    void get(void* dst, size_t size) {
        memcpy(dst, offset(), size);
        m_Pos += size;
    }

    void put(void* src, size_t size) {
        memcpy(offset(), src, size);
        m_Pos += size;
    }
    void rewind() {
        m_Pos = 0;
    }
    int capacity() {
        return m_Capcity;
    }
    int remaining() {
        return m_Capcity - m_Pos;
    }
    int position() {
        return m_Pos;
    }
    void position(int pos) {
        if (pos < m_Capcity) {
            m_Pos = pos;
        }
    }
    jbyte* pointerOffset() {
        return offset();
    }
private:
    int m_Pos;
    int m_Native_Order;
    jbyte* m_Buffer_Ptr;
    BUFFER_ORDER m_Order;
    int m_Capcity;
    jbyte* offset() {
        assert(m_Pos < m_Capcity);
        return m_Buffer_Ptr + m_Pos;
    }
    BUFFER_ORDER checkNativeOrder() {
        int a = 0x12345678;
        jbyte* aPtr = (jbyte*)&a;
        if (*aPtr == 0x78)
            return BUFFER_LITTLER_ENDIAN;
        else
            return BUFFER_BIG_ENDIAN;
    }
};

#define STR_CONTEXT      "context"
#define STR_ENGINE_TYPE  "engine_type"
#define STR_SESSION      "session_ptr"
#define STR_TEXID        "texid"
#define BRAND_OPPO       "OPPO"
#define BRAND_XIAOMI     "Xiaomi"
#define BRAND_HUAWEI     "HUAWEI"
#define BRAND_VIVO       "vivo"
#define BRAND_GOOGLE     "google"
#define BRAND_TECNO      "TECNO"
#define SCREEN_ROTATE    "screen_rotate"
#define STR_POSE         "pose"
#define STR_ANCHOR_ID    "anchorid"
#define BOOL_IS_OVERRIDE "isoverride"
#define STR_MAP_ID       "mapid"
#define APP_KEY          "appkey"
#define APP_SECRET       "appsecret"
#define RECEIVE_MAGIC_WORD  0xabcd
#define SEND_MAGIC_WORD     0x3a3b
#define IMAGE_TRACKINNG_VERSION 0xff1
#define OBJECT_TRACKING_VERSION 0xff1
int isArServiceExist(void *env, void *application_context);
bool isDeviceSupportRGBD(void *env, void *application_context);
bool isArServiceSupportRGBD(void *env, void *application_context);
bool isArServiceSupportHandGesture(void *env, void *application_context);
bool isArServiceSupportCloudAnchor(void *env, void *application_context);
bool isArServiceSingleAlgorithm(void *context);
bool isArServiceSupportDoubleStream(void *context);
bool isArServiceVersionNewerThan1_9();
bool checkSLAMDeviceSupport(void* context);

#endif  // ARCONFIG_C_API_H_
