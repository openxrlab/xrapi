
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>
#include <sys/syscall.h>
#include "jni_impl.h"
#include "log.h"
#include "commonlib.h"

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_PriorityUtil_setPriority(JNIEnv* env, jobject obj,
                                                                                        jint priority)
{
    int which = PRIO_PROCESS;
    int ret = setpriority(which, gettid(), priority);
    if (ret) {
        LOGE("setpriority failed:%d", errno);
    }
}

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_DirectByteBufferNativeUtil_startCollect(JNIEnv* env, jobject obj,
                                                                                                       jobject buffer, jint order,
                                                                                                       jintArray tags, jlongArray callbackHandler)
{
    collectResult(env, buffer, order, tags, callbackHandler);
}

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_DirectByteBufferNativeUtil_copyBuffer(JNIEnv* env, jobject obj,
                                                                                                     jobject dst_buffer, jint dst_pos,
                                                                                                     jobject src_buffer, jint src_pos, jint length)
{

    jbyte* dst_ptr = (jbyte*)env->GetDirectBufferAddress(dst_buffer);
    jbyte* src_ptr = (jbyte*)env->GetDirectBufferAddress(src_buffer);
    memcpy(dst_ptr + dst_pos, src_ptr + src_pos, length);
}

JNIEXPORT void JNICALL Java_com_standardar_service_common_util_PriorityUtil_bindCore(JNIEnv* env, jobject obj)
{
    int64_t standard = 1;
    int num = sysconf(_SC_NPROCESSORS_CONF);
    for(int i=1; i<num; ++i){
        standard = (standard << 1) | 1;
    }

    int64_t mask;
    syscall(__NR_sched_getaffinity, 0, sizeof(mask), &mask);

    if(standard != mask){
        syscall(__NR_sched_setaffinity, 0, sizeof(standard), &standard);
    }
}