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
#include "background_renderer.h"
#include "glm.h"
#include "obj_renderer.h"
#include "plane_renderer_3d.h"
#include "point_cloud_renderer.h"
#include "util.h"
#include "scanner_renderer.h"

namespace hello_ar {

// HelloArApplication handles all application logics.
    class HelloArApplication {
    public:
        // Constructor and deconstructor.
        HelloArApplication() = default;

        HelloArApplication(AAssetManager *asset_manager, void *env, void *context,
                           char *package_name);

        ~HelloArApplication();

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

        // OnTouched is called on the OpenGL thread after the user touches the screen.
        // @param x: x position on the screen (pixels).
        // @param y: y position on the screen (pixels).
        void OnTouched(float x, float y);

        void OnDoubleTouched(float x, float y);

        void OnTouchedZoom(float scale);

        void OnTouchedMove(float x, float y);

        void ResetTouched();

        bool IsScanStart();

        void OnStartScan();

        void OnStopScan();

        void OnStartScanGuidance();

        void OnStopScanGuidance();

        void ResetAlgorithm();

        void getScanResult();

        int getScanProgress();

        bool isNearPlane();

        // Returns true if any planes have been detected.  Used for hiding the
        // "searching for planes" snackbar.
        bool HasDetectedPlanes() const { return true; }

        const char *getDebugInfo();

        bool isSLAMInitializing();

    private:

        ARSession *ar_session_ = nullptr;
        ARFrame *ar_frame_ = nullptr;
        ARWorldMap   *ar_map_   = nullptr;
        AAssetManager *const asset_manager_;

        // Stores the randomly-selected color each plane is drawn with
        std::unordered_map<int, glm::vec3> plane_color_map_;

        PointCloudRenderer point_cloud_renderer_;
        BackgroundRenderer background_renderer_;
        PlaneRenderer3D plane_renderer_3d_;
        ScannerRenderer scanner_renderer_;

        int     frame_index;
        struct timeval current_time;
        double elapse_time;
        float   fps;

        int slam_info_size;
        char* slam_info;

        bool m_bNeedClear;
        bool m_bShowPlane;

        double difftime(timeval& start, timeval& end);

        glm::mat4 m_BoxMat;
        glm::mat4 m_ScaleMat;

        int     m_ScreenWidth;
        int     m_ScreenHeight;
        int32_t m_displayRotation;
        int     m_ViewportWidth;
        int     m_ViewportHeight;

        bool m_EnableScan;
        bool m_bFinishScanning;
        bool m_bFirstPlace;
    };
}  // namespace hello_ar

#endif  // C_ARCORE_HELLOE_AR_HELLO_AR_APPLICATION_H_
