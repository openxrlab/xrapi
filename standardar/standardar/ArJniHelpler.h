#ifndef STANDARD_AR_ARJNIHELPLER_C_API_H
#define STANDARD_AR_ARJNIHELPLER_C_API_H

#include "jni.h"
#include "log.h"

#define CLIENT_CLASS "com/standardar/sdkclient/Client"
#define CAMERASOURCE_CLASS "com/standardar/sensor/camera/CameraSource"
#define IMUREADER_CLASS "com/standardar/sensor/imu/IMUReader"
#define UTIL_CLASS "com/standardar/common/Util"
#define CAMERA_UTIL_CLASS "com/standardar/sensor/camera/CameraUtils"
#define POSE_CLASS "com/standardar/common/Pose"
#define CLIENTPROXY_CLASS "com/standardar/sdkclient/ClientProxy"

namespace standardar
{

    class JNIEnvProxy {
    public:
        JNIEnvProxy(JavaVM* jvm): m_Env(nullptr),
                                  m_jvm(jvm),
                                  m_AttachedHere(false)
        {
            if (m_jvm)
            {
                m_jvm->GetEnv((void**)&m_Env, JNI_VERSION_1_6);
                if (m_Env == nullptr) {
                    m_jvm->AttachCurrentThread(&m_Env, nullptr);
                    m_AttachedHere = true;
                }
            }
        }
        ~JNIEnvProxy()
        {
            if (m_jvm && m_Env && m_AttachedHere)
                m_jvm->DetachCurrentThread();
            m_jvm = nullptr;
            m_Env = nullptr;
        }

        JNIEnv* operator->() const
        {
            if (!m_Env)
                LOGE("env is null!!");
            return m_Env;
        }

        bool operator==(void* ptr)
        {
            return m_Env == ptr;
        }

        bool operator!=(void* ptr)
        {
            return m_Env != ptr;
        }

        JNIEnv* get()
        {
            return m_Env;
        }

        JNIEnvProxy(JNIEnvProxy&& proxy)
        {
            m_Env = proxy.m_Env;
            m_jvm = proxy.m_jvm;
            proxy.m_Env = nullptr;
            proxy.m_jvm = nullptr;
            proxy.m_AttachedHere = false;
        }
    private:
        JNIEnv* m_Env;
        JavaVM* m_jvm;
        bool m_AttachedHere;
    };

    class ArJniHelpler
    {
    public:
        static void init(JavaVM* vm, jobject, jmethodID findclass_id);
        static jclass findClass(const char* class_name);
        static JNIEnvProxy getJavaEnv();
        static jobject getApplicationContextGlobalRef();
        static jclass getClientClass() {return s_ClientClz;}
        static jclass getCameraSourceClass() { return  s_CameraSourceClz;}
        static jclass getImuReaderClass() {return s_ImuReaderClz;}
        static jclass getUtilClass() {return s_UtilClz;}
        static jclass getCameraUtilClass() {return s_CameraUtilClz;}
        static jclass getPoseClass() {return s_PoseClz;}

    private:
        static jobject s_ClassLoader;
        static jmethodID s_FindClassId;
        static JavaVM* s_Jvm;
        static jclass s_ApplicationClz;
        static jclass s_ClientClz;
        static jclass s_CameraSourceClz;
        static jclass s_ImuReaderClz;
        static jclass s_UtilClz;
        static jclass s_CameraUtilClz;
        static jclass s_PoseClz;
    };



}

#endif
