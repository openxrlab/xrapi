
#include <map>
#include "commonlib.h"
#include "log.h"


void collectResult(JNIEnv* env, jobject buffer, int order, jintArray tags, jlongArray callbackHandler) {
    DirectByteBufferWrapper bufferWrapper(env->GetDirectBufferAddress(buffer),
                                          env->GetDirectBufferCapacity(buffer),
                                          (BUFFER_ORDER)order);
    if(tags ==NULL){
        return;
    }
    int size = env->GetArrayLength(tags);
    bufferWrapper.put<int>(0xabcd);
    bufferWrapper.put<int>(size);
    jboolean isCopy;
    jint* tagPtr = env->GetIntArrayElements(tags, &isCopy);
    jlong* funcPtr = env->GetLongArrayElements(callbackHandler, &isCopy);
    for (int i = 0; i < size; ++i) {
        bufferWrapper.put<int>(tagPtr[i]);
        ALGORITHM_RESULT_FILL_FUNC func = reinterpret_cast<ALGORITHM_RESULT_FILL_FUNC>(funcPtr[i]);
        func(&bufferWrapper);
    }
    env->ReleaseIntArrayElements(tags, tagPtr, 0);
    env->ReleaseLongArrayElements(callbackHandler, funcPtr, 0);
}
