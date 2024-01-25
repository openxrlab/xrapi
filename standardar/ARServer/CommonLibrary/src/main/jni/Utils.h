#ifndef ARSERVER_FILEUTILS_H
#define ARSERVER_FILEUTILS_H
#include <string>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <time.h>
#include <pthread.h>
#include "ArMath.h"

#define IMU_ACC_TAG  0
#define IMU_GYRO_TAG  1
#define IMU_GRAVITY_TAG  2
#define IMU_RV_TAG  3

#define IMU_ACC_LENGTH      6
#define IMU_GYRO_LENGTH     6
#define IMU_RV_LENGTH        5
#define IMU_GRAVITY_LENGTH  3

#define NS_PER_SECOND  1000000000.0f

static jfloatArray vec3fToFloatArray(JNIEnv* env, standardar::vec3f vec) {
    jfloatArray float_array = env->NewFloatArray(3);
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_array, 0, 3, float_array_data);
    float_array_data[0] = vec.x; float_array_data[1] = vec.y; float_array_data[2] = vec.z;
    env->SetFloatArrayRegion(float_array, 0, 3, float_array_data);
    return float_array;
}

static standardar::vec3f floatArrayToVec3f(JNIEnv* env, jfloatArray float_array) {
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

static void copyVec3fToFloatArray(JNIEnv* env, standardar::vec3f& vec, jfloatArray float_array) {
    if (float_array == nullptr)
        return;
    jfloat float_array_data[3];
    env->GetFloatArrayRegion(float_array, 0, 3, float_array_data);
    float_array_data[0] = vec.x; float_array_data[1] = vec.y; float_array_data[2] = vec.z;
    env->SetFloatArrayRegion(float_array, 0, 3, float_array_data);
}

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

struct GlassConfig
{
    int     m_eyeWidth;
    int     m_eyeHeight;
    int     m_refreshRate;

    float   m_leftFx;
    float   m_leftFy;
    float   m_leftCx;
    float   m_leftCy;
    float   m_leftRotX;
    float   m_leftRotY;
    float   m_leftRotZ;
    float   m_leftPosX;
    float   m_leftPosY;
    float   m_leftPosZ;

    float   m_rightFx;
    float   m_rightFy;
    float   m_rightCx;
    float   m_rightCy;
    float   m_rightRotX;
    float   m_rightRotY;
    float   m_rightRotZ;
    float   m_rightPosX;
    float   m_rightPosY;
    float   m_rightPosZ;
};

class Utils {
public:
    static std::string getPhoneModel();
    static std::string getPhoneBrand();
    static std::string getProperty(std::string property);
    static double nanosecond2second(jlong ns) {
        return (double)ns / NS_PER_SECOND;
    }

    static int64_t getMonoToBoottimeOffset() {
        // try three times to get the clock offset, choose the one
        // with the minimum gap in measurements.
        const int tries = 3;
        int64_t bestGap, measured;
        for (int i = 0; i < tries; ++i) {
            struct timespec tmono, tbase, tmono2;
            clock_gettime(CLOCK_MONOTONIC, &tmono);
            clock_gettime(CLOCK_BOOTTIME ,&tbase);
            clock_gettime(CLOCK_MONOTONIC, &tmono2);

            const int64_t monotime = tmono.tv_sec * 1.0e9 + tmono.tv_nsec;
            const int64_t basetime = tbase.tv_sec * 1.0e9 + tbase.tv_nsec;
            const int64_t  monotime2 = tmono2.tv_sec * 1.0e9 + tmono2.tv_nsec;
            const int64_t gap = monotime2 - monotime;
            if (i == 0 || gap < bestGap) {
                bestGap = gap;
                measured = basetime - ((monotime + monotime2) >> 1);
            }
        }
        return measured;
    }

    static bool extractAsset(AAssetManager* apkAssetManager,
                             std::string assetName,
                             std::string outputPath);

    static bool readFile(std::string filepath, char** file_buf,
                         int* file_length);

    static bool readFromAsset(AAssetManager *apkAssetManager, std::string assetName,
                               char** file_buf, int* file_length);

    static bool readGlassConfig(const char *filePath, GlassConfig *glassConfig);

    static bool isFileExist(std::string filepath);
};



#endif //ARSERVER_FILEUTILS_H
