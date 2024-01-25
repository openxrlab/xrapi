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

#include "hello_ar_application.h"

#include <android/asset_manager.h>
#include <array>
#include "util.h"

#include <unistd.h>
#include <android/bitmap.h>
#include <stdint.h>
#include "jni_interface.h"
#include "StandardAR.h"


namespace hello_ar {

    float points2dBuf[17 * 2];
    float points2d[17 * 2];

//    float middlePoints[];

    HelloArApplication::HelloArApplication(AAssetManager *asset_manager, void *env,
                                           void *context, char *package_name){
        LOGI("OnCreate()");

        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);

        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_BODY_DETECT,ARALGORITHM_MODE_ENABLE);

        arConfigSetAlgorithmStreamMode(ar_config, ARALGORITHM_TYPE_BODY_DETECT, ARSTREAM_MODE_BACK_RGB);

        CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

        arConfigDestroy(ar_config);

        arFrameCreate(ar_session_, &ar_frame_);
        CHECK(ar_frame_);

    }

    HelloArApplication::~HelloArApplication() {
        arFrameDestroy(ar_frame_);
        arSessionDestroy(ar_session_);
        arWorldMapRelease(ar_map_);

    }

    void HelloArApplication::OnDebugCommandInt(int type, int value) {
        LOGI("OnDebugCommandInt()");
        arSessionSetIntValue(ar_session_, (ARParameterEnum) type, &value, 1);

    }

    void HelloArApplication::OnPause() {
        LOGI("OnPause()");
        arSessionPause(ar_session_);
    }

    void HelloArApplication::OnResume() {
        LOGI("OnResume()");

        const ARResult status = arSessionResume(ar_session_);
        CHECK(status == ARRESULT_SUCCESS);
    }

    void HelloArApplication::OnStartAlgorithm() {
        LOGI("OnStartAlgorithm()");
        arSessionStartAlgorithm(ar_session_, ARALGORITHM_TYPE_ALL, 0);
    }

    void HelloArApplication::OnStopAlgorithm() {
        LOGI("OnStopAlgorithm()");
        //const ArStatus status = ArSession_stopSLAM(ar_session_);
        arSessionStopAlgorithm(ar_session_, ARALGORITHM_TYPE_ALL, 0);
    }

    void HelloArApplication::OnSurfaceCreated() {
        LOGI("OnSurfaceCreated()");

        background_renderer_.InitializeGlContent();
        bodyRenderer_.InitializeGlContent();
        arSessionSetCameraTextureName(ar_session_,
                                       background_renderer_.GetTextureId());


        arFrameSetPointQueryMode(ar_session_, ar_frame_, ARQUERY_MODE_POLYGON_PERSISTENCE);


    }

    void HelloArApplication::OnDisplayGeometryChanged(int display_rotation, int width,
                                                      int height) {
        m_ScreenWidth = width;
        m_ScreenHeight = height;
        m_displayRotation = display_rotation;

        // get frame video image width and height
        int w = 0;
        int h = 0;
        arFrameGetImageResolution(ar_session_, ar_frame_, &w, &h);
        m_VideoWidth  = w;
        m_VideoHeight = h;
        LOGI("============arFrameGetImageResolution======w%d h%d",w,h);


        float ratio = (float) w / h;

        // Calculate the appropriate size of the window display area
        float debugRenderAspect = 0.0;
        if (m_displayRotation == 0 || m_displayRotation == 2) {
            m_ViewportWidth = width;
            m_ViewportHeight = width * ratio;
            glViewport(0, (height - m_ViewportHeight) / 2, m_ViewportWidth, m_ViewportHeight);
            debugRenderAspect = 1.0 / ratio;
        } else if (m_displayRotation == 1 || m_displayRotation == 3) {
            m_ViewportHeight = height;
            m_ViewportWidth = height * ratio;
            glViewport((width - m_ViewportWidth) / 2, 0, m_ViewportWidth, m_ViewportHeight);
            debugRenderAspect = ratio;
        }


        // set ar display size
        LOGI("OnSurfaceChanged(%d, %d)", width, height);
        LOGI("viewport size(%d, %d)", m_ScreenWidth, m_ScreenHeight);
        arSessionSetDisplayGeometry(ar_session_, display_rotation, m_ViewportWidth, m_ViewportHeight);

    }

    void HelloArApplication::OnDrawFrame() {
        // Render the scene.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);






        // Update session to get current frame and render camera background.
        if (arSessionUpdate(ar_session_, ar_frame_) != ARRESULT_SUCCESS) {
            LOGE("HelloArApplication::OnDrawFrame ArSession_update error");
        }


       uint32_t camera_type;
        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);
        arSessionGetConfig(ar_session_,ar_config);
        int mode;
        arSessionGetIntValue(ar_session_, ARPARAMETER_CAMERA_TEXTURE_TARGET, &mode, 1);
        camera_type = mode;
        arConfigDestroy(ar_config);
        background_renderer_.Draw(ar_session_, ar_frame_,camera_type);


        ARCamera *ar_camera;

        arFrameAcquireCamera(ar_session_,ar_frame_,&ar_camera);


        glm::mat4 view_mat;
        glm::mat4 projection_mat;
        arCameraGetViewMatrix(ar_session_,ar_camera,glm::value_ptr(view_mat));
        arCameraGetProjectionMatrix(ar_session_,ar_camera,/*near=*/0.1f, /*far=*/100.f,
                                    glm::value_ptr(projection_mat));

        arCameraRelease(ar_camera);

        ARNodeList *node_list = nullptr;
        arNodeListCreate(ar_session_, &node_list);
        CHECK(node_list != nullptr);
        arWorldMapGetAllNodes(ar_session_, ar_map_, ARNODE_TYPE_BODY_DETECT, node_list);



        int node_list_size = 0;
        arNodeListGetSize(ar_session_, node_list, &node_list_size);



        LOGI("====node_list_size %d===m_ViewportWidth %d == m_ViewportHeight %d",node_list_size,m_ViewportWidth,m_ViewportHeight);
        if (node_list_size > 0) {
            for (int i = 0; i < node_list_size; i++) {
                ARNode *ar_node = nullptr;
                arNodeListAcquireItem(ar_session_, node_list, i, &ar_node);
                ARBodyDetectNode * arBodyDetectNode = ARNodeAsARBodyDetecthNode(ar_node);
                int bodyId = 0;
                arBodyDetectNodeGetBodyId(ar_session_,arBodyDetectNode,&bodyId);
                int point_count =0;
                arBodyDetectNodeGetSkeletonPoint2dCount(ar_session_,arBodyDetectNode,&point_count);
                arBodyDetectNodeGetSkeletonPoint2d(ar_session_,arBodyDetectNode,points2dBuf);
                int offset =0;
                for (int i = 0; i < point_count; i++) {
                    float tmp = points2dBuf[2*i+offset];
                    points2dBuf[2*i+offset] = m_VideoHeight - points2dBuf[2*i+offset+1];
                    points2dBuf[2*i+offset+1] = tmp;
                }

                for (int i = 0; i < point_count ; ++i) {
                    points2d[2*i] =  2* points2dBuf[2*i]/m_VideoHeight -1;
                    points2d[2*i+1] = 1- 2*points2dBuf[2*i+1]/m_VideoWidth;
                }
                bodyRenderer_.DrawBody(points2d);


            }
        }


        arNodeListDestroy(node_list);



    }




}  // namespace hello_ar
