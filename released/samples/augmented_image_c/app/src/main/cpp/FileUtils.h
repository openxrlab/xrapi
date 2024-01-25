#ifndef ARSERVER_FILEUTILS_H
#define ARSERVER_FILEUTILS_H
#include <string>
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <time.h>

#define IMU_ACC_TAG  0
#define IMU_GYRO_TAG  1
#define IMU_GRAVITY_TAG  2
#define IMU_RV_TAG  3

#define IMU_ACC_LENGTH      6
#define IMU_GYRO_LENGTH     6
#define IMU_RV_LENGTH        5
#define IMU_GRAVITY_LENGTH  3

#define NS_PER_SECOND  1000000000.0f

class FileUtils {
public:
    static std::string getPhoneModel();
    static std::string getPhoneBrand();
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

};


#endif //ARSERVER_FILEUTILS_H
