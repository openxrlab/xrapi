#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <time.h>
#ifdef _MSC_VER
#include <windows.h>
#include <WinSock.h>

static int gettimeofday(struct timeval *tp, void *tzp) {
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#else
#include <sys/time.h>
#endif

#define CONFIG_ST_BURSTFUSION_TIMING 0
#ifdef CONFIG_ST_BURSTFUSION_TIMING
    #define __TIC1__(tag) timeval time_##tag##_start;gettimeofday(&time_##tag##_start, NULL)
    #ifdef __ANDROID__
       #include <android/log.h>
        #define __TOC1__(tag) timeval time_##tag##_end;gettimeofday(&time_##tag##_end, NULL);\
            __android_log_print(ANDROID_LOG_ERROR, "====== prof ======", #tag " time: %ld\n",\
                ((time_##tag##_end.tv_sec - time_##tag##_start.tv_sec)*1000000) +\
                (time_##tag##_end.tv_usec - time_##tag##_start.tv_usec))
    #else
        #define __TOC1__(tag) timeval time_##tag##_end;gettimeofday(&time_##tag##_end, NULL);\
            fprintf(stderr, #tag " time: %ld\n",\
                ((time_##tag##_end.tv_sec - time_##tag##_start.tv_sec)*1000) +\
                (time_##tag##_end.tv_usec - time_##tag##_start.tv_usec)/1000)
    #endif
#else
    #define __TIC1__(tag)
    #define __TOC1__(tag)
#endif

#endif // TIME_HELPER_H
