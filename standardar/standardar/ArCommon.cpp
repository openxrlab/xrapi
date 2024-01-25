#include "ArCommon.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <sys/system_properties.h>
#include <unistd.h>

#include "ArMath.h"
#include "ArJniHelpler.h"
#include "StandardAR.h"
#include "picojson.h"
using namespace standardar;

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

double  getCurrentSecond()
{
    static double appstart = getHPCounter();
    double hpcounter = getHPCounter();
    double hpfrequency = getHPFrequency();
    return (double)((hpcounter-appstart)/hpfrequency);
}


//
//void dumpStandardARSLAMCorner(const std::string& prefix, const StandardARSLAMCorner& corner)
//{
//    LOGI("%s %f, %f", prefix.c_str(), corner.x, corner.y);
//}
//
//void dumpStandardARSLAMMarker(const std::string& prefix, const StandardARSLAMMarker& marker)
//{
//    std::string tempstr=prefix+"    ";
//    LOGI("StandardARSLAMMarker:");
//    for (int i = 0; i < 4; ++i) {
//        dumpStandardARSLAMCorner(tempstr, marker.corners[i]);
//    }
//}
//
//void dumpStandardARSLAMCamera(const std::string& prefix, const StandardARSLAMCamera& camera)
//{
//
//}
//
//void dumpStandardARSLAMLandmark(const std::string& prefix, const StandardARSLAMLandmark& landmark)
//{
//    LOGI("%s %f, %f, %f, %f", prefix.c_str(), landmark.X, landmark.Y, landmark.Z, landmark.W);
//}
//
//void dumpStandardARSLAMPlane(const std::string& prefix, const StandardARSLAMPlane* pPlane)
//{
//    LOGI("%s StandardARSLAMPlane id:%i", prefix.c_str(), pPlane->plane_id);
//    LOGI("%s original:%f, %f, %f", prefix.c_str(), pPlane->plane_origin_point[0], pPlane->plane_origin_point[1], pPlane->plane_origin_point[2]);
//    //LOGI("%s vertex index", prefix.c_str(), pPlane->plane_origin_point[0], pPlane->plane_origin_point[1], pPlane->plane_origin_point[2]);
//}
//


void getPhoneBrand(std::string& phone_model)
{
    char brand[PROP_VALUE_MAX];
    __system_property_get("ro.product.brand", brand);
    phone_model = std::string(brand);
}

void getPhoneModel(std::string& phone_brand)
{
    char model[PROP_VALUE_MAX];
    __system_property_get("ro.product.model", model);
    phone_brand = std::string(model);
}

std::string getProperty(std::string property)
{
    char value[PROP_VALUE_MAX];
    __system_property_get(property.c_str(), value);
    return std::string(value);
}
void StandardAR_CheckGlError(const char* operation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", operation, error);
    }
}

// Convenience function used in CreateProgram below.
static GLuint StandardAR_LoadShader(GLenum shader_type, const char* shader_source) {
    GLuint shader = glCreateShader(shader_type);
    if (!shader) {
        return shader;
    }

    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint info_len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (!info_len) {
            return shader;
        }

        char* buf = reinterpret_cast<char*>(malloc(info_len));
        if (!buf) {
            return shader;
        }

        glGetShaderInfoLog(shader, info_len, nullptr, buf);
        LOGI("Could not compile shader %d:\n%s\n", shader_type, buf);
        free(buf);
        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}

GLuint StandardAR_CreateProgram(const char* vertex_source, const char* fragment_source) {
    GLuint vertexShader = StandardAR_LoadShader(GL_VERTEX_SHADER, vertex_source);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragment_shader = StandardAR_LoadShader(GL_FRAGMENT_SHADER, fragment_source);
    if (!fragment_shader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        StandardAR_CheckGlError("standardar::glAttachShader");
        glAttachShader(program, fragment_shader);
        StandardAR_CheckGlError("standardar::glAttachShader");
        glLinkProgram(program);
        GLint link_status = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);
        if (link_status != GL_TRUE) {
            GLint buf_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_length);
            if (buf_length) {
                char* buf = reinterpret_cast<char*>(malloc(buf_length));
                if (buf) {
                    glGetProgramInfoLog(program, buf_length, nullptr, buf);
                    LOGI("standardar::Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    } else{
        LOGI("glCreateProgram failed...");
    }
    return program;
}


int isArServiceExist(void *env, void *application_context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return ARAVAILABILITY_UNKNOWN_ERROR;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "checkPackageInstalled", "(Landroid/content/Context;Ljava/lang/String;)I");
    jint valid = jniEnv->CallStaticIntMethod(utilClass, checkId, application_context, jniEnv->NewStringUTF("com.standardar.service.xrservice"));
    return valid;
}

bool isDeviceSupportRGBD(void *env, void *application_context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    std::string brand, model;
    getPhoneBrand(brand);
    getPhoneModel(model);
    LOGI("RGBD is not supported on this device");
    return false;
}

bool isArServiceSupportRGBD(void *env, void *application_context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isArServiceSupportRGBD", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, application_context);
    if (valid)
        return true;

    LOGI("RGBD is not supported on this arserver");
    return false;
}

bool isArServiceSupportHandGesture(void *env, void *application_context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isArServiceSupportHandGesture", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, application_context);
    if (valid)
        return true;

    LOGI("Hand Gesture is not supported on this arserver");
    return false;
}

bool isArServiceSupportCloudAnchor(void *env, void *application_context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGW("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isArServiceSupportCloudAnchor", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, application_context);
    if (valid)
        return true;

    LOGI("Cloud Anchor is not supported on this arserver");
    return false;
}

bool isArServiceSingleAlgorithm(void *context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isArServiceSingleAlgorithm", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, context);
    if (valid)
        return true;

    return false;
}

bool isArServiceSupportDoubleStream(void *context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isArServiceDoubleStream", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, context);
    if (valid)
        return true;
    return false;
}

bool isArServiceVersionNewerThan1_9()
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    jobject context = ArJniHelpler::getApplicationContextGlobalRef();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::findClass(UTIL_CLASS);
    jmethodID checkId = jniEnv->GetStaticMethodID(utilClass, "isVersionNewerThan1_9", "(Landroid/content/Context;)Z");
    jboolean valid = jniEnv->CallStaticBooleanMethod(utilClass, checkId, context);
    jniEnv->DeleteGlobalRef(context);
    return (bool)valid;
}


bool checkSLAMDeviceSupport(void* context)
{
    JNIEnvProxy jniEnv = ArJniHelpler::getJavaEnv();
    if (jniEnv == nullptr) {
        LOGI("jni env is null, call ArJavaEnv_onLoad or ArJavaEnv_setClassLoader first");
        return false;
    }
    jclass utilClass = ArJniHelpler::getUtilClass();
    jmethodID checkSLAMId = jniEnv->GetStaticMethodID(utilClass, "getXRServerSupport", "(Landroid/content/Context;)Ljava/lang/String;");
    if (checkSLAMId == nullptr)
    {
        return false;
    }
    jstring str = (jstring)jniEnv->CallStaticObjectMethod(utilClass, checkSLAMId, context);
    jboolean isCopy;
    const char* str_str = jniEnv->GetStringUTFChars(str, &isCopy);
    std::string jsonStr(str_str);
    picojson::value v;
    std::string err = picojson::parse(v, jsonStr);
    if (!err.empty()) {
        LOGI("parser support file error %s", err.c_str());
        return false;
    }

    const auto& array = v.get<picojson::array>();
    std::string brand, model;
    getPhoneBrand(brand);
    getPhoneModel(model);
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
                    if (per_model_obj["model"].get<std::string>() == model)
                        return true;
                }
            }
        }
    }
    return false;
}

jfloatArray vec3fToFloatArray(JNIEnv* env, standardar::vec3f vec) {
    jfloatArray float_array = env->NewFloatArray(3);
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_array, 0, 3, float_array_data);
    float_array_data[0] = vec.x; float_array_data[1] = vec.y; float_array_data[2] = vec.z;
    env->SetFloatArrayRegion(float_array, 0, 3, float_array_data);
    return float_array;
}

standardar::vec3f floatArrayToVec3f(JNIEnv* env, jfloatArray float_array) {
    standardar::vec3f vec;
    if (float_array == nullptr)
        return vec;
    int length = env->GetArrayLength(float_array);
    if (length != 3)
        return vec;
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_array, 0, 3, float_array_data);
    vec.x = float_array_data[0];
    vec.y = float_array_data[1];
    vec.z = float_array_data[2];
    return vec;
}

void copyVec3fToFloatArray(JNIEnv* env, standardar::vec3f& vec, jfloatArray float_array) {
    if (float_array == nullptr)
        return;
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_array, 0, 3, float_array_data);
    float_array_data[0] = vec.x; float_array_data[1] = vec.y; float_array_data[2] = vec.z;
    env->SetFloatArrayRegion(float_array, 0, 3, float_array_data);
}

void copyFloatArrayToVec3f(JNIEnv* env, jfloatArray float_arr, standardar::vec3f& vec) {
    if (float_arr == nullptr)
        return;
    int length = env->GetArrayLength(float_arr);
    if (length != 3)
        return;
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_arr, 0, 3, float_array_data);
    vec.x = float_array_data[0]; vec.y = float_array_data[1]; vec.z = float_array_data[2];
}



