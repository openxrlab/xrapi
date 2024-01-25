#ifndef COM_STANDARDAR_COMMON_UTIL_API_H
#define COM_STANDARDAR_COMMON_UTIL_API_H

#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_com_standardar_common_Util_copyImage(JNIEnv* env, jobject obj,
                                                                 jobject dst, jint order, jint pos,
                                                                 jobject src, jint rgb_size, jint depth_size);

JNIEXPORT void JNICALL Java_com_standardar_common_Util_copyBuffer(JNIEnv* env, jobject obj,
                                                                  jobject dst_buffer, jint dst_pos,
                                                                  jobject src_buffer, jint src_pos, jint length);

JNIEXPORT jstring JNICALL Java_com_standardar_common_Util_getXRServerPkg(JNIEnv* env, jobject obj);

JNIEXPORT void JNICALL Java_com_standardar_common_Util_bindCore(JNIEnv* env, jobject obj);

#ifdef __cplusplus
};
#endif

#endif
