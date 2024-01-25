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

/* modification list:
 * 1. HelloArApplication add package_name parameter
 * 2. add OnStartAlgorithm
 * 3. add OnStopAlgorithm
 * 4. add OnDebugCommandInt
 * 5. add readPixel
 * 6. add DeleteModel
 * 7. add getDebugInfo
 * 8. add isSLAMInitializing
 * 9. add m_bNeedClear, m_bShowPlane, m_bshowLANDMARK members
 * 10. add plane_renderer_3d, mesh_renderer members
 */

#ifndef C_ARCORE_HELLOE_AR_HELLO_AR_APPLICATION_H_
#define C_ARCORE_HELLOE_AR_HELLO_AR_APPLICATION_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <jni.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <time.h>
#include "StandardAR.h"
#include "glm.h"
#include "util.h"
#include "NativeRender.h"
#include "background_renderer.h"

namespace hello_ar {

// HelloArApplication handles all application logics.
    class HelloArApplication {
    public:
        // Constructor and deconstructor.
        HelloArApplication() = default;

        HelloArApplication(AAssetManager *asset_manager, void *env, void *context,
                           char *package_name);

        ~HelloArApplication();

        void OnDebugCommandInt(int type, int value);

        // OnPause is called on the UI thread from the Activity's onPause method.
        void OnPause();

        // OnResume is called on the UI thread from the Activity's onResume method.
        void OnResume();

        void OnStartAlgorithm();
        void OnStopAlgorithm();

        // OnSurfaceCreated is called on the OpenGL thread when GLSurfaceView
        // is created.
        void OnSurfaceCreated();

        // OnDisplayGeometryChanged is called on the OpenGL thread when the
        // render surface size or display rotation changes.
        //
        // @param display_rotation: current display rotation.
        // @param width: width of the changed surface view.
        // @param height: height of the changed surface view.
        void OnDisplayGeometryChanged(int display_rotation, int width, int height);

        // OnDrawFrame is called on the OpenGL thread to render the next frame.
        void OnDrawFrame();

        void  setTexture(JNIEnv * env,jobject bitmap);

        void switchCamera();


    public:

        static ARStreamMode sCurrentStreamMode;

    private:

        ARSession *ar_session_ = nullptr;
        ARFrame *ar_frame_ = nullptr;
        ARWorldMap   *ar_map_   = nullptr;

        int     m_ScreenWidth;
        int     m_ScreenHeight;
        int32_t m_displayRotation;
        int     m_ViewportWidth;
        int     m_ViewportHeight;
        bool    mIsSwitchingCamera ;
        NativeRender * m_NativeRender;
        BackgroundRenderer background_renderer_;



    };
}  // namespace hello_ar

#endif  // C_ARCORE_HELLOE_AR_HELLO_AR_APPLICATION_H_
