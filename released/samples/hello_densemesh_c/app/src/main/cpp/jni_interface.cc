/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include "hello_ar_application.h"
#include "StandardAR.h"
#define JNI_METHOD(return_type, method_name) \
  JNIEXPORT return_type JNICALL              \
      Java_com_standardar_examples_c_densemesh_JniInterface_##method_name

extern "C" {

namespace {
// maintain a reference to the JVM so we can use it later.
    static JavaVM *g_vm = nullptr;

    inline jlong jptr(hello_ar::HelloArApplication *native_hello_ar_application) {
        return reinterpret_cast<intptr_t>(native_hello_ar_application);
    }

    inline hello_ar::HelloArApplication *native(jlong ptr) {
        return reinterpret_cast<hello_ar::HelloArApplication *>(ptr);
    }

}  // namespace

jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    LOGI("Hello C JNI_OnLoad called");
    arJavaEnvOnLoad(vm, nullptr);
    return JNI_VERSION_1_6;
}

JNI_METHOD(jlong, createNativeApplication)
(JNIEnv *env, jclass, jobject j_asset_manager, jobject context, jstring j_package_name) {
    AAssetManager *asset_manager = AAssetManager_fromJava(env, j_asset_manager);
    jboolean flag = JNI_FALSE;
    char *package_name = (char *) env->GetStringUTFChars(j_package_name, &flag);
    return jptr(new hello_ar::HelloArApplication(asset_manager, env, context, package_name));
}

JNI_METHOD(void, destroyNativeApplication)
(JNIEnv *, jclass, jlong native_application) {
    delete native(native_application);
}

JNI_METHOD(void, onDebugCommandInt)
(JNIEnv *, jclass, jlong native_application, jint type, jint value) {
    native(native_application)->OnDebugCommandInt(type, value);
}

JNI_METHOD(void, onPause)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnPause();
}

JNI_METHOD(void, onResume)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnResume();
}

JNI_METHOD(void, onStartAlgorithm)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnStartAlgorithm();
}

JNI_METHOD(void, onStopAlgorithm)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnStopAlgorithm();
}

JNI_METHOD(void, onGlSurfaceCreated)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnSurfaceCreated();
}

JNI_METHOD(void, onDisplayGeometryChanged)
(JNIEnv *, jclass, jlong native_application, int display_rotation, int width,
 int height) {
    native(native_application)
            ->OnDisplayGeometryChanged(display_rotation, width, height);
}

JNI_METHOD(void, onGlSurfaceDrawFrame)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnDrawFrame();
}

JNI_METHOD(void, onTouched)
(JNIEnv *, jclass, jlong native_application, jfloat x, jfloat y) {
    native(native_application)->OnTouched(x, y);
}

JNI_METHOD(void, deleteModel)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->DeleteModel();
}

JNI_METHOD(jboolean, hasDetectedPlanes)
(JNIEnv *, jclass, jlong native_application) {
    return static_cast<jboolean>(
            native(native_application)->HasDetectedPlanes() ? JNI_TRUE : JNI_FALSE);
}

JNI_METHOD(jstring, getSLAMInfo)
(JNIEnv *env, jclass, jlong native_application) {
    const char *tempstr = native(native_application)->getDebugInfo();
    return (jstring) (env)->NewStringUTF(tempstr);
}

JNI_METHOD(jboolean, isSLAMInitializing)
(JNIEnv *env, jclass, jlong native_application) {
    return static_cast<jboolean>(
            native(native_application)->isSLAMInitializing() ? JNI_TRUE : JNI_FALSE);
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}

jclass FindClass(const char *classname) {
    JNIEnv *env = GetJniEnv();
    return env->FindClass(classname);
}

}  // extern "C"
