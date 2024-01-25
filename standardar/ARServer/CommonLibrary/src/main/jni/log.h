
#ifndef ARSERVER_LOG_H
#define ARSERVER_LOG_H
#define TAG "arserver"
#include <android/log.h>

static bool enableLog = true;

#define LOGV(...) if (enableLog) __android_log_print(ANDROID_LOG_VERBOSE,TAG ,__VA_ARGS__)
#define LOGD(...) if (enableLog) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) if (enableLog) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGW(...) if (enableLog) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define LOGE(...) if (enableLog) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define ASSERT(cond, fmt, ...)                                \
  if (!(cond)) {                                              \
    __android_log_assert(#cond, TAG, fmt, ##__VA_ARGS__); \
  }
#endif //ARSERVER_LOG_H
