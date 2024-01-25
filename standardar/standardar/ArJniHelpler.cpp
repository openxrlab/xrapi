#include "ArJniHelpler.h"
#include "log.h"
using namespace standardar;

jobject ArJniHelpler::s_ClassLoader = nullptr;
jmethodID ArJniHelpler::s_FindClassId = 0;
JavaVM* ArJniHelpler::s_Jvm = nullptr;
jclass ArJniHelpler::s_ApplicationClz = nullptr;
jclass ArJniHelpler::s_ClientClz = nullptr;
jclass ArJniHelpler::s_CameraSourceClz = nullptr;
jclass ArJniHelpler::s_ImuReaderClz = nullptr;
jclass ArJniHelpler::s_UtilClz = nullptr;
jclass ArJniHelpler::s_CameraUtilClz = nullptr;
jclass ArJniHelpler::s_PoseClz = nullptr;


void ArJniHelpler::init(JavaVM* jvm, jobject classloader, jmethodID findclassid) {
    if (s_Jvm != nullptr) {
        LOGW("vm is already init");
        return;
    }
    s_Jvm = jvm;
    JNIEnvProxy env = getJavaEnv();
    if (classloader == nullptr || findclassid == nullptr) {
        LOGD("init from vm");
        jclass mainClass = env->FindClass("com/standardar/common/Util");
        jclass classClass = env->FindClass("java/lang/Class");
        jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
        jmethodID getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader",
                                                          "()Ljava/lang/ClassLoader;");
        jobject classLoader = env->CallObjectMethod(mainClass, getClassLoaderMethod);
        s_ClassLoader = env->NewGlobalRef(classLoader);
        s_FindClassId = env->GetMethodID(classLoaderClass, "findClass",
                                         "(Ljava/lang/String;)Ljava/lang/Class;");

    } else {
        LOGD("init from param");
        ArJniHelpler::s_ClassLoader = classloader;
        ArJniHelpler::s_FindClassId = findclassid;
    }

    if (s_ApplicationClz == nullptr) {
        jclass clz = env->FindClass("android/app/ActivityThread");
        s_ApplicationClz = (jclass) env->NewGlobalRef(clz);
    }

//    if (s_ClientClz == nullptr)
//    {
//        //jclass clientClz = env->FindClass("com/standardar/common/Client");
//        jclass clientClz = ArJniHelpler::findClass(CLIENT_CLASS);
//        s_ClientClz = (jclass) env->NewGlobalRef(clientClz);
//    }
//
//    if (s_CameraSourceClz == nullptr) {
//        //jclass cameraSourceClz = env->FindClass("com/standardar/common/CameraSource1");
//        jclass cameraSourceClz = ArJniHelpler::findClass(CAMERASOURCE_CLASS);
//        s_CameraSourceClz = (jclass) env->NewGlobalRef(cameraSourceClz);
//    }

//    if (s_ImuReaderClz == nullptr) {
//        //jclass  imuReaderClz = env->FindClass("com/standardar/common/IMUReader");
//        jclass imuReaderClz = ArJniHelpler::findClass(IMUREADER_CLASS);
//        s_ImuReaderClz = (jclass)env->NewGlobalRef(imuReaderClz);
//    }

    if (s_UtilClz == nullptr) {
        //jclass utilClz = env->FindClass("com/standardar/common/Util");
        jclass utilClz = ArJniHelpler::findClass(UTIL_CLASS);
        s_UtilClz = (jclass) env->NewGlobalRef(utilClz);
    }

//    if (s_CameraUtilClz == nullptr) {
//        //jclass utilClz = env->FindClass("com/standardar/common/Util");
//        jclass cameraUtilClz = ArJniHelpler::findClass(CAMERA_UTIL_CLASS);
//        s_CameraUtilClz = (jclass) env->NewGlobalRef(cameraUtilClz);
//    }

    if (s_PoseClz == nullptr) {
        //jclass utilClz = env->FindClass("com/standardar/common/Pose);
        jclass poseClz = ArJniHelpler::findClass("com/standardar/common/Pose");
        s_PoseClz = (jclass) env->NewGlobalRef(poseClz);
    }

}



jclass ArJniHelpler::findClass(const char *class_name)
{
    JNIEnvProxy env = getJavaEnv();
    if (env == nullptr)
    {
        LOGI("failed to get jni");
        return nullptr;
    }
    jstring classNameObj = env->NewStringUTF(class_name);
    jclass clz = static_cast<jclass>(env->CallObjectMethod(s_ClassLoader, s_FindClassId, classNameObj));
    env->DeleteLocalRef(classNameObj);

    if (clz == nullptr)
    {
        clz = env->FindClass(class_name);
    }

    return clz;
}

JNIEnvProxy ArJniHelpler::getJavaEnv()
{
    return JNIEnvProxy(s_Jvm);
}

jobject ArJniHelpler::getApplicationContextGlobalRef()
{
    JNIEnvProxy env = ArJniHelpler::getJavaEnv();
    jmethodID currentActivityThread = env->GetStaticMethodID(s_ApplicationClz,
                                                             "currentActivityThread",
                                                             "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(s_ApplicationClz, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(s_ApplicationClz, "getApplication",
                                                "()Landroid/app/Application;");
    jobject context = env->CallObjectMethod(at, getApplication);
    return env->NewGlobalRef(context);
}
