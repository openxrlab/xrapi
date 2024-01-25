/*
 * Copyright 2018 Google Inc. All Rights Reserved.
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

#include "object_tracking_application.h"
#include "util.h"

#define JNI_METHOD(return_type, method) \
  JNIEXPORT return_type JNICALL         \
      Java_com_standardar_examples_c_object_ObjectTrackingJniInterface_##method

extern "C" {

namespace {
// maintain a reference to the JVM so we can use it later.

    inline jlong jptr(hello_ar::ObjectTrackingApplication
                      *native_object_tracking_application) {
        return reinterpret_cast<intptr_t>(native_object_tracking_application);
    }

    inline hello_ar::ObjectTrackingApplication *native(jlong ptr) {
        return reinterpret_cast<hello_ar::ObjectTrackingApplication *>(ptr);
    }

}  // namespace

JNI_METHOD(jlong, createNativeApplication)
(JNIEnv *env, jclass, jobject j_asset_manager, jobject context, jstring j_package_name, jstring j_apk_internal_path) {
    AAssetManager *asset_manager = AAssetManager_fromJava(env, j_asset_manager);
    jboolean flag = JNI_FALSE;
    char *package_name = (char *) env->GetStringUTFChars(j_package_name, &flag);
    char *apk_internal_path = (char *)env->GetStringUTFChars(j_apk_internal_path, &flag);
    return jptr(new hello_ar::ObjectTrackingApplication(asset_manager, env, context, package_name, apk_internal_path));
}

JNI_METHOD(void, destroyNativeApplication)
(JNIEnv *, jclass, jlong native_application) {
    delete native(native_application);
}

JNI_METHOD(void, onPause)
(JNIEnv *, jclass, jlong native_application) {
    native(native_application)->OnPause();
}

JNI_METHOD(void, onResume)
(JNIEnv *env, jclass, jlong native_application, jobject context,
 jobject activity) {
    native(native_application)->OnResume(env, context, activity);
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
(JNIEnv *, jclass, jlong native_application, jobject activity) {
    native(native_application)->OnDrawFrame(activity);
}

JNI_METHOD(jstring, getObjectTrackingInfo)
(JNIEnv *env, jclass, jlong native_application) {
    const char *tempstr = native(native_application)->getDebugInfo();
    return (jstring) (env)->NewStringUTF(tempstr);
}

JNI_METHOD(void, onStopAlgorithm)
(JNIEnv *env, jclass, jlong native_application) {
    native(native_application)->OnStopAlgorithm();
}

JNI_METHOD(jboolean, isSLAMInitializing)
(JNIEnv *env, jclass, jlong native_application) {
    return static_cast<jboolean>(
            native(native_application)->isSLAMInitializing() ? JNI_TRUE : JNI_FALSE);
}

}  // extern "C"

