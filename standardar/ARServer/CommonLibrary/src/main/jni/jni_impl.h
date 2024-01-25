
#ifndef ARSERVER_JNI_IMPL_H
#define ARSERVER_JNI_IMPL_H

#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_com_standardar_service_common_util_PriorityUtil_setPriority(JNIEnv* env, jobject obj,
                                                                                                       jint priority);

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_DirectByteBufferNativeUtil_startCollect(JNIEnv* env, jobject obj,
                                                                                        jobject buffer, jint order,
                                                                                        jintArray tags, jlongArray callbackHandler);

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_DirectByteBufferNativeUtil_copyBuffer(JNIEnv* env, jobject obj,
                                                                                                     jobject dst_buffer, jint dst_pos,
                                                                                                     jobject src_buffer, jint src_pos, jint length);


JNIEXPORT void JNICALL Java_com_standardar_service_common_util_PriorityUtil_bindCore(JNIEnv* env, jobject obj);

#ifdef __cplusplus
}
#endif


#endif //ARSERVER_JNI_IMPL_H
