#include "StandardARInternal.h"
#include "ArJniHelpler.h"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGD("Hello JAVA JNI_OnLoad called");
    javaEnvOnLoad(vm, NULL);
    return JNI_VERSION_1_6;
}