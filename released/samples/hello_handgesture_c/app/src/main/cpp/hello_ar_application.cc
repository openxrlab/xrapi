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
#include "jni_interface.h"
#include <string>

namespace hello_ar {
#define US_PER_SECOND 1000000
#define ARPARAMETER_DEBUG_SHOW_LANDMARK 0
#define ARPARAMETER_DEBUG_SHOW_PLANE 1
    std::string handGestureTypes[] = {"ok", "剪刀", "点赞", "布", "手枪", "拳头", "比心", "指尖", "666", "三根手指",
                                      "四根手指", "我爱你", "rock", "小拇指", "未知"};
    char gestureInfo[200];
    float points2dBuf[20*2];

    HelloArApplication::HelloArApplication(AAssetManager *asset_manager, void *env,
                                           void *context, char *package_name) {
        LOGI("OnCreate()");

        m_bNeedClear = false;
        m_bfullScreen = true;


        // === ATTENTION!  ATTENTION!  ATTENTION! ===
        // This method can and will fail in user-facing situations.  Your application
        // must handle these cases at least somewhat gracefully.  See HelloAR Java
        // sample code for reasonable behavior.
        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);
        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_HAND_GESTURE,
                                  ARALGORITHM_MODE_ENABLE);

        CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

        arConfigDestroy(ar_config);

        arFrameCreate(ar_session_, &ar_frame_);
        CHECK(ar_frame_);


        frame_index = 0.0f;
        gettimeofday(&current_time, nullptr);
        elapse_time = 0;
        fps = 0.0f;

        m_DebugCameraRadius = 5.0;
        m_AlphaAngle = 0.0;
        m_ThetaAngle = 45.0;
        m_BeginAlphaAngle = 0.0;
        m_BeginThetaAngle = 0.0;
        m_PreDistance = 0.0f;


    }

    HelloArApplication::~HelloArApplication() {
        arFrameDestroy(ar_frame_);
        arWorldMapRelease(ar_map_);
        arSessionDestroy(ar_session_);
    }

    void HelloArApplication::OnDebugCommandInt(int type, int value) {
        LOGI("OnDebugCommandInt()");
        arSessionSetIntValue(ar_session_, (ARParameterEnum )type, &value, 1);

        switch (type){
            case ARPARAMETER_DEBUG_SHOW_LANDMARK:
                break;
            case ARPARAMETER_DEBUG_SHOW_PLANE:
                break;
            default:
                break;
        }
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
        arSessionStopAlgorithm(ar_session_, ARALGORITHM_TYPE_ALL, 0);
    }

    void HelloArApplication::OnSurfaceCreated() {
        LOGI("OnSurfaceCreated()");


        background_renderer_.InitializeGlContent();
        arSessionSetCameraTextureName(ar_session_,
                                    background_renderer_.GetTextureId());




    }

    void HelloArApplication::OnDisplayGeometryChanged(int display_rotation,
                                                      int width, int height) {
        m_ScreenWidth = width;
        m_ScreenHeight = height;
        m_displayRotation = display_rotation;
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
        m_previewWidth = w;
        m_previewHeight = h;

        float debugRenderAspect = 0.0;
        if (m_displayRotation == 0 || m_displayRotation == 2) {
            debugRenderAspect = 1.0 / ratio;
            m_DebugViewportWidth = width;
            m_DebugViewPortHeight = width / debugRenderAspect;

        } else if (m_displayRotation == 1 || m_displayRotation == 3) {
            debugRenderAspect = ratio;
            m_DebugViewPortHeight = height;
            m_DebugViewportWidth = height * debugRenderAspect;
        }


        m_ViewportHeight = m_ScreenWidth * ratio;
        m_ViewportWidth = m_ScreenWidth;
        glViewport(0, (height - m_ViewportHeight) / 2, m_ViewportWidth, m_ViewportHeight);

        m_ScreenAspect = ratio;


        arSessionSetDisplayGeometry(ar_session_, display_rotation, m_ViewportWidth, m_ViewportHeight);

        const float angletorad = 3.1415926 / 180.0;

        float verticalFOV = 50.0f;
        arSessionGetFloatValue(ar_session_, ARPARAMETER_VIDEO_VERTICAL_FOV, &verticalFOV, 1);
        verticalFOV = verticalFOV * angletorad;


    }

    /**
     * return elpased time (us)
     * @param start
     * @param end
     * @return
     */
    double HelloArApplication::difftime(timeval &start, timeval &end) {
        return US_PER_SECOND * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    }

    void HelloArApplication::OnDrawFrame() {

        if (m_bNeedClear) {
            tracked_obj_set_.clear();
            m_bNeedClear = false;
        }

        struct timeval time01;
        gettimeofday(&time01, nullptr);

        elapse_time += difftime(current_time, time01);
        current_time = time01;
        frame_index++;

        if (frame_index > 100) {
            fps = (float) ((double) frame_index * US_PER_SECOND / (double) elapse_time);
            frame_index = 0;
            elapse_time = 0.0f;
        }

        // Render the scene.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        timeval time02;
        gettimeofday(&time02, nullptr);



        // Update session to get current frame and render camera background.
        if (arSessionUpdate(ar_session_, ar_frame_) != ARRESULT_SUCCESS) {
            LOGE("HelloArApplication::OnDrawFrame ArSession_update error");
        }

        timeval time03;
        gettimeofday(&time03, nullptr);
        //   float session_update = (float) difftime(time02, time03) / MS_PER_US;



        ARCamera *ar_camera;
        arFrameAcquireCamera(ar_session_, ar_frame_, &ar_camera);





        float znear = 0.1f;
        float zfar = 100.0f;
        glm::mat4 view_mat;
        glm::mat4 projection_mat;
        arCameraGetViewMatrix(ar_session_, ar_camera, glm::value_ptr(view_mat));
        arCameraGetProjectionMatrix(ar_session_, ar_camera, znear, zfar,
                                    glm::value_ptr(projection_mat));


        ARTrackingState trackState;
        arCameraGetTrackingState(ar_session_, ar_camera, &trackState);


        background_renderer_.Draw(ar_session_, ar_frame_);

        /*if(trackState != ARTRACKING_STATE_SUCCESS)
            return;*/



        //HandGestue


        ARNodeList *hand_gesture_list = nullptr;
        arNodeListCreate(ar_session_, &hand_gesture_list);
        CHECK(hand_gesture_list != nullptr);

        ARNodeType handgesture_tracked_type = ARNODE_TYPE_HAND_GESTURE;

        arWorldMapGetAllNodes(ar_session_, ar_map_, handgesture_tracked_type, hand_gesture_list);

        int32_t hand_gesture_list_size = 0;
        arNodeListGetSize(ar_session_, hand_gesture_list, &hand_gesture_list_size);

        if (hand_gesture_list_size > 0) {
            for (int i = 0; i < hand_gesture_list_size; i++) {
                ARNode *ar_node = nullptr;
                arNodeListAcquireItem(ar_session_, hand_gesture_list, i, &ar_node);
                ARHandGestureNode *arHandGesture = ARNodeAsARHandGestureNode(ar_node);

                char side[10];
                ARHandType arhandside;

                arHandGestureNodeGetHandType(ar_session_,arHandGesture,&arhandside);


                if (arhandside == ARHAND_SIDE_LEFT_HAND) {
                    strcpy(side, "左手");
                } else {
                    strcpy(side, "右手");
                }

                char towards[10];
                ARHandTowards arhandtowards;
                arHandGestureNodeGetHandTowards(ar_session_, arHandGesture, &arhandtowards);

                if (arhandtowards == ARHAND_TOWARDS_PALM) {
                    strcpy(towards, "手心");

                } else if (arhandtowards == ARHAND_TOWARDS_THE_BACK_OF_HAND) {
                    strcpy(towards, "手背");

                } else if (arhandtowards == ARHAND_TOWARDS_SIDE_HAND) {
                    strcpy(towards, "侧手");

                }

                ARHandGestureType handGestureTye;
                arHandGestureNodeGetHandGestureType(ar_session_, arHandGesture, &handGestureTye);

                float handGestureTypeConfidence;
                arHandGestureNodeGetGestureTypeConfidence(ar_session_, arHandGesture,
                                                      &handGestureTypeConfidence);

                sprintf(gestureInfo, "%s \n %s \n %s \n 置信度：%f", side, towards,
                        handGestureTypes[handGestureTye].c_str(),
                        handGestureTypeConfidence);


                int points2dcount = 0;

                arHandGestureNodeGetLandMark2DCount(ar_session_,arHandGesture,&points2dcount);


                JNIEnv *jniEnv = GetJniEnv();

                jfloatArray posints2dFloArray = jniEnv->NewFloatArray(points2dcount * 2);



                arHandGestureNodeGetLandMark2DArray(ar_session_,arHandGesture,points2dBuf);
                jniEnv->SetFloatArrayRegion(posints2dFloArray, 0, points2dcount * 2,
                                            points2dBuf);


                jniEnv->CallVoidMethod(getHelloArActivityObj(), getUpdateHandGestureInfoMid(),
                                       jniEnv->NewStringUTF(gestureInfo), posints2dFloArray,
                                       points2dcount, m_previewWidth, m_previewHeight,
                                       m_ViewportWidth, m_ViewportHeight, m_ScreenHeight);
            }


        } else {
            JNIEnv *jniEnv = GetJniEnv();
            jniEnv->CallVoidMethod(getHelloArActivityObj(), getUpdateHandGestureInfoMid(),
                                   nullptr, nullptr,
                                   0, 0, 0, 0, 0, 0);
        }
    }











}  // namespace hello_ar
