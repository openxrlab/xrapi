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

ARStreamMode hello_ar::HelloArApplication::sCurrentStreamMode = ARSTREAM_MODE_FRONT_RGB;

namespace hello_ar {


    HelloArApplication::HelloArApplication(AAssetManager *asset_manager, void *env,
                                           void *context, char *package_name){
        LOGI("OnCreate()");

        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);
        mIsSwitchingCamera = false;

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);

        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_FACE_MESH,ARALGORITHM_MODE_ENABLE);



        arConfigSetAlgorithmStreamMode(ar_config, ARALGORITHM_TYPE_FACE_MESH, sCurrentStreamMode);

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
        arSessionSetCameraTextureName(ar_session_,
                                       background_renderer_.GetTextureId());

        m_NativeRender = new NativeRender();
        m_NativeRender->createGlProgram();

      //  arWorldSetCameraTextureName(ar_session_, m_NativeRender->GetTextureId());
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
        ARCamera *ar_camera;
        arFrameAcquireCamera(ar_session_, ar_frame_, &ar_camera);
        ARCameraIntrinsics *ar_cameraIntrinsics;
        arCameraIntrinsicsCreate(ar_session_, &ar_cameraIntrinsics);
        arCameraGetTextureIntrinsics(ar_session_, ar_camera, ar_cameraIntrinsics);
        arCameraIntrinsicsGetImageDimensions(ar_session_, ar_cameraIntrinsics, &w, &h);
        arCameraIntrinsicsDestroy(ar_cameraIntrinsics);
        arCameraRelease(ar_camera);
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

    float center_pose_arr[16];
    void HelloArApplication::OnDrawFrame() {
        // Render the scene.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        if(mIsSwitchingCamera){
            return;
        }


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
        arWorldMapGetAllNodes(ar_session_, ar_map_, ARNODE_TYPE_FACE_MESH, node_list);



        int node_list_size = 0;
        arNodeListGetSize(ar_session_, node_list, &node_list_size);

        if (node_list_size > 0) {
            for (int i = 0; i < node_list_size; i++) {
                ARNode *ar_node = nullptr;
                arNodeListAcquireItem(ar_session_, node_list, i, &ar_node);
                ARFaceMeshNode * arFaceMeshNode = ARNodeAsARFaceMeshNode(ar_node);
                int vertext_count = 0;
                int normal_count = 0;
                int index_count = 0;
                int texture_count = 0;
                float *vertices = nullptr;
                float *normals = nullptr;
                int *triangleIndices = nullptr;
                float *meshTextureCoordinates = nullptr;

                util::ScopedArPose scopedArPose(ar_session_);

                arFaceMeshNodeGetCenterPose(ar_session_, arFaceMeshNode, scopedArPose.GetArPose());
                arFaceMeshNodeGetVertices(ar_session_, arFaceMeshNode, &vertices, &vertext_count);
                arFaceMeshNodeGetNormals(ar_session_, arFaceMeshNode, &normals, &normal_count);
                arFaceMeshNodeGetTriangleIndices(ar_session_, arFaceMeshNode, &triangleIndices,&index_count);
                arFaceMeshNodeGetTextureCoordinates(ar_session_, arFaceMeshNode,&meshTextureCoordinates, &texture_count);



                arPoseGetMatrix(scopedArPose.GetArPose(),center_pose_arr);

                glm::mat4  mode_mat =  glm::make_mat4(center_pose_arr);
                glm::mat4  mvp= projection_mat*view_mat*mode_mat;

                m_NativeRender->drawMesh( vertices, index_count, triangleIndices, meshTextureCoordinates, mvp);



            }
        }
        arNodeListDestroy(node_list);


    }

    void HelloArApplication::setTexture(JNIEnv *env, jobject bitmap) {
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);
        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGE("Bitmap format ERROR: %d", info.format);
            return;
        }
        unsigned char *data = new unsigned char[info.width * info.width * 4];
        AndroidBitmap_lockPixels(env, bitmap, (void **) &data);
        if (m_NativeRender == NULL) {

        }
        m_NativeRender->setFaceTexture(data, info.width, info.height);
        AndroidBitmap_unlockPixels(env, bitmap);
    }


    void HelloArApplication::switchCamera() {
        mIsSwitchingCamera = true;
        if(sCurrentStreamMode ==ARSTREAM_MODE_FRONT_RGB){
            arSessionPause(ar_session_);
            ARConfig *ar_config = nullptr;
            arConfigCreate(&ar_config);

            arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_FACE_MESH,ARALGORITHM_MODE_ENABLE);


            sCurrentStreamMode = ARSTREAM_MODE_BACK_RGB;

            arConfigSetAlgorithmStreamMode(ar_config, ARALGORITHM_TYPE_FACE_MESH, sCurrentStreamMode);

            CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

            arConfigDestroy(ar_config);
            arSessionResume(ar_session_);
        } else{

            arSessionPause(ar_session_);

            ARConfig *ar_config = nullptr;
            arConfigCreate(&ar_config);

            arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_FACE_MESH,ARALGORITHM_MODE_ENABLE);


            sCurrentStreamMode = ARSTREAM_MODE_FRONT_RGB;

            arConfigSetAlgorithmStreamMode(ar_config, ARALGORITHM_TYPE_FACE_MESH, sCurrentStreamMode);

            CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

            arConfigDestroy(ar_config);
            arSessionResume(ar_session_);

        }
        mIsSwitchingCamera = false;

    }


}  // namespace hello_ar
