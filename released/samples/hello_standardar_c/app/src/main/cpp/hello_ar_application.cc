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

#include <unistd.h>

namespace hello_ar {
#define US_PER_SECOND 1000000
#define MS_PER_US     1000
#define ARPARAMETER_DEBUG_SHOW_LANDMARK 0
#define ARPARAMETER_DEBUG_SHOW_PLANE 1
    namespace {
        constexpr int32_t kPlaneColorRgbaSize = 16;
        constexpr std::array<uint32_t, kPlaneColorRgbaSize> kPlaneColorRgba = {
                0xFFFFFFFF, 0xF44336FF, 0xE91E63FF, 0x9C27B0FF, 0x673AB7FF, 0x3F51B5FF,
                0x2196F3FF, 0x03A9F4FF, 0x00BCD4FF, 0x009688FF, 0x4CAF50FF, 0x8BC34AFF,
                0xCDDC39FF, 0xFFEB3BFF, 0xFFC107FF, 0xFF9800FF};

        inline glm::vec3 GetRandomPlaneColor() {
            const int32_t colorRgba = kPlaneColorRgba[std::rand() % kPlaneColorRgbaSize];
            return glm::vec3(((colorRgba >> 24) & 0xff) / 255.0f,
                             ((colorRgba >> 16) & 0xff) / 255.0f,
                             ((colorRgba >> 8) & 0xff) / 255.0f);
        }
    }  // namespace

    HelloArApplication::HelloArApplication(AAssetManager *asset_manager, void *env,
                                           void *context, char* package_name)
            : asset_manager_(asset_manager)
    {
        LOGI("OnCreate()");

        m_bNeedClear = false;
        m_bShowPlane = true;
        m_bShowLANDMARK = true;

        ARAvailability availability;
        arCapabilityCheckAvailability(env, context, &availability);
        CHECK(availability == ARAVAILABILITY_SUPPORTED);
        CHECK(arCapabilityCheckAlgorithm(env, context, ARALGORITHM_TYPE_SLAM, ARSTREAM_MODE_BACK_RGB) == ARRESULT_SUCCESS);

        // === ATTENTION!  ATTENTION!  ATTENTION! ===
        // This method can and will fail in user-facing situations.  Your application
        // must handle these cases at least somewhat gracefully.  See HelloAR Java
        // sample code for reasonable behavior.
        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);

        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_SLAM, ARALGORITHM_MODE_ENABLE);
        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_PLANE_DETECTION, ARALGORITHM_MODE_ENABLE);
        //arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_ILLUMINATION_ESTIMATION, ARALGORITHM_MODE_ENABLE);
        CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

        arConfigDestroy(ar_config);

        arFrameCreate(ar_session_, &ar_frame_);
        CHECK(ar_frame_);

        frame_index =0.0f;
        gettimeofday(&current_time, nullptr);
        elapse_time =0;
        fps =0.0f;

        slam_info_size = 1024;
        slam_info = new char[slam_info_size];
    }

    HelloArApplication::~HelloArApplication() {
        arFrameDestroy(ar_frame_);
        arSessionDestroy(ar_session_);
        arWorldMapRelease(ar_map_);

        if(slam_info != nullptr) {
            delete[] slam_info;
            slam_info = nullptr;
        }
    }

    void HelloArApplication::OnDebugCommandInt(int type, int value) {
        LOGI("OnDebugCommandInt()");
        arSessionSetIntValue(ar_session_, (ARParameterEnum )type, &value, 1);

        switch (type){
            case ARPARAMETER_DEBUG_SHOW_LANDMARK:
                m_bShowLANDMARK = value!=0;
                break;
            case ARPARAMETER_DEBUG_SHOW_PLANE:
                m_bShowPlane = value!=0;
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

        m_bNeedClear = true;
    }

    void HelloArApplication::OnSurfaceCreated() {
        LOGI("OnSurfaceCreated()");

        background_renderer_.InitializeGlContent();
        arSessionSetCameraTextureName(ar_session_,
                                       background_renderer_.GetTextureId());

        point_cloud_renderer_.InitializeGlContent();
        andy_renderer_.InitializeGlContent(asset_manager_, "andy.obj", "andy.png");
        plane_renderer_3d_.InitializeGlContent(asset_manager_);

        arFrameSetPointQueryMode(ar_session_, ar_frame_, ARQUERY_MODE_POLYGON_PERSISTENCE);

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
        float ratio = (float)w / h;

        float debugRenderAspect = 0.0;
        float originScreenAspect = (float) width / height;
        if (m_displayRotation == 0 || m_displayRotation == 2)
        {
            debugRenderAspect = 1.0 / ratio;
        }
        else if (m_displayRotation == 1 || m_displayRotation == 3)
        {
            debugRenderAspect = ratio;
        }

        m_ViewportWidth = originScreenAspect > debugRenderAspect ? width : (height * debugRenderAspect);
        m_ViewportHeight = m_ViewportWidth / debugRenderAspect;
        glViewport((width - m_ViewportWidth) / 2 , (height - m_ViewportHeight) / 2, m_ViewportWidth, m_ViewportHeight);

        m_ScreenAspect = ratio;

        arSessionSetDisplayGeometry(ar_session_, display_rotation, m_ViewportWidth, m_ViewportHeight);
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
            plane_color_map_.clear();
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
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        timeval time02;
        gettimeofday(&time02, nullptr);
        float clear_update = (float) difftime(time01, time02) / MS_PER_US;

        // Update session to get current frame and render camera background.
        if (arSessionUpdate(ar_session_, ar_frame_) != ARRESULT_SUCCESS) {
            LOGE("HelloArApplication::OnDrawFrame ArSession_update error");
        }

        timeval time03;
        gettimeofday(&time03, nullptr);
        float session_update = (float) difftime(time02, time03) / MS_PER_US;

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


        {
            float RenderAspect = 0.0;
            float originScreenAspect = (float) m_ScreenWidth / m_ScreenHeight;
            if (m_displayRotation == 0 || m_displayRotation == 2) {
                RenderAspect = 1.0 / m_ScreenAspect;
            } else if (m_displayRotation == 1 || m_displayRotation == 3) {
                RenderAspect = m_ScreenAspect;
            }

            m_ViewportWidth = originScreenAspect > RenderAspect ? m_ScreenWidth : m_ScreenHeight * RenderAspect;
            m_ViewportHeight = m_ViewportWidth / RenderAspect;

            glViewport((m_ScreenWidth - m_ViewportWidth) / 2,
                       (m_ScreenHeight - m_ViewportHeight) / 2, m_ViewportWidth, m_ViewportHeight);

            arSessionSetDisplayGeometry(ar_session_, m_displayRotation, m_ViewportWidth, m_ViewportHeight);
        }

        arCameraRelease(ar_camera);

        background_renderer_.Draw(ar_session_, ar_frame_);

        if (trackState != ARTRACKING_STATE_SUCCESS)
            return;

        timeval time04;
        gettimeofday(&time04, nullptr);
        float draw_bg = (float) difftime(time03, time04) / MS_PER_US;

        // Get light estimation value.
        ARIlluminationEstimate *ar_light_estimate;
        ARIlluminationEstimateState ar_light_estimate_state;
        arIlluminationEstimateCreate(ar_session_, &ar_light_estimate);

        arFrameGetIlluminationEstimate(ar_session_, ar_frame_, ar_light_estimate);
        arIlluminationEstimateGetState(ar_session_, ar_light_estimate,
                                       &ar_light_estimate_state);

        // Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
        float light_intensity = 0.8f;
        if (ar_light_estimate_state == ARILLUMINATION_ESTIMATE_STATE_VALID) {
            arIlluminationEstimateGetPixelIntensity(ar_session_, ar_light_estimate,
                                                    &light_intensity);
        }

        arIlluminationEstimateDestroy(ar_light_estimate);
        ar_light_estimate = nullptr;
        light_intensity /= 15.0f;
        if (light_intensity <= 0.3f)
            light_intensity = 0.3f;
        if (light_intensity >= 1.0f)
            light_intensity = 1.0f;

        // Render Andy objects.
        glm::mat4 model_mat(1.0f);
        glm::vec3 scale(1);
        for (const auto &obj_iter : tracked_obj_set_) {
            ARTrackingState tracking_state = ARTRACKING_STATE_STOPPED;
            arAnchorGetTrackingState(ar_session_, obj_iter, &tracking_state);
            if (tracking_state == ARTRACKING_STATE_SUCCESS) {
                // Render object only if the tracking state is AR_TRACKING_STATE_TRACKING.
                util::GetTransformMatrixFromAnchor(ar_session_, obj_iter, &model_mat);
                model_mat = glm::scale(model_mat, scale);
                andy_renderer_.Draw(projection_mat, view_mat, model_mat, light_intensity);
            }
        }

        timeval time05;
        gettimeofday(&time05, nullptr);
        float draw_andy_model = (float) difftime(time04, time05) / MS_PER_US;

        if (m_bShowPlane) {
            // Update and render planes.
            ARNodeList *plane_list = nullptr;
            arNodeListCreate(ar_session_, &plane_list);
            CHECK(plane_list != nullptr);

            ARNodeType plane_tracked_type = ARNODE_TYPE_PLANE;

            arWorldMapGetAllNodes(ar_session_, ar_map_, plane_tracked_type, plane_list);

            int32_t plane_list_size = 0;
            arNodeListGetSize(ar_session_, plane_list, &plane_list_size);

            for (int i = 0; i < plane_list_size; ++i) {
                ARNode *ar_node = nullptr;
                arNodeListAcquireItem(ar_session_, plane_list, i, &ar_node);
                ARPlaneNode *ar_plane = ARNodeAsARPlaneNode(ar_node);

                const auto iter = plane_color_map_.find(i);
                glm::vec3 color;
                if (iter != plane_color_map_.end()) {
                    color = iter->second;
                } else {
                    color = GetRandomPlaneColor();
                    plane_color_map_.insert({i, color});
                }

                //plane_renderer_.Draw(projection_mat, view_mat, ar_session_, ar_plane, color);
                plane_renderer_3d_.Draw(projection_mat, view_mat, ar_session_, ar_plane, color);

                arNodeRelease(ar_node);
            }
            arNodeListDestroy(plane_list);
            plane_list = nullptr;
        }

        timeval time06;
        gettimeofday(&time06, nullptr);
        float draw_planes = (float) difftime(time05, time06) / MS_PER_US;

        if (m_bShowLANDMARK) {
            // Update and render point cloud.
            ARPointCloud *ar_point_cloud = nullptr;
            ARResult point_cloud_status = arFrameAcquirePointCloud(ar_session_, ar_frame_,
                                                                   &ar_point_cloud);
            if (point_cloud_status == ARRESULT_SUCCESS) {
                point_cloud_renderer_.Draw(projection_mat * view_mat, ar_session_, ar_point_cloud);
                arPointCloudRelease(ar_point_cloud);
            }
        }

        timeval time07;
        gettimeofday(&time07, nullptr);
        float draw_point_cloud = (float) difftime(time06, time07) / MS_PER_US;
        float total_time = (float) difftime(time01, time07) / MS_PER_US;

        char tempbuffer[2048];
        memset(tempbuffer, 0, 2048);
        sprintf(tempbuffer,
                "fps:%f, \nclear_update:%f, \nArSession_update:%f, \nDrawYUV:%f, \ndraw_andy_model:%f, \ndraw planes:%f, \ndraw_point_cloud%f, \ntotal time:%f",
                fps, clear_update, session_update, draw_bg, draw_andy_model, draw_planes,
                draw_point_cloud, total_time);
    }

    void HelloArApplication::OnTouched(float x, float y) {

        ARTrackingState world_state = ARTRACKING_STATE_STOPPED;
        arSessionGetTrackingState(ar_session_, &world_state);
        if(world_state != ARTRACKING_STATE_SUCCESS)
            return;

        if (ar_frame_ != nullptr && ar_session_ != nullptr) {
            ARQueryResultList *query_result_list = nullptr;
            arQueryResultListCreate(ar_session_, &query_result_list);
            CHECK(query_result_list);
            x -= (float)(m_ScreenWidth - m_ViewportWidth) / 2;
            y -= (float)(m_ScreenHeight - m_ViewportHeight) / 2;
            arFramePointQuery(ar_session_, ar_frame_, x, y, query_result_list);

            int32_t query_result_list_size = 0;
            arQueryResultListGetSize(ar_session_, query_result_list,
                                    &query_result_list_size);

            // The hitTest method sorts the resulting list by distance from the camera,
            // increasing.  The first hit result will usually be the most relevant when
            // responding to user input
            for (int32_t i = 0; i < query_result_list_size; ++i) {
                ARQueryResult *ar_query_result = nullptr;
                arQueryResultCreate(ar_session_, &ar_query_result);
                arQueryResultListGetItem(ar_session_, query_result_list, i, ar_query_result);

                if (ar_query_result == nullptr) {
                    LOGE("HelloArApplication::OnTouched ArHitResultList_getItem error");
                    return;
                }

                // Only consider planes for this sample app.
                ARNode *ar_node = nullptr;
                arQueryResultAcquireNode(ar_session_, ar_query_result, &ar_node);
                ARNodeType ar_trackable_type = ARNODE_TYPE_UNKNOWN;
                arNodeGetType(ar_session_, ar_node, &ar_trackable_type);
                if (ar_trackable_type != ARNODE_TYPE_PLANE) {
                    arNodeRelease(ar_node);
                    continue;
                }

                ARPose *ar_pose = nullptr;
                arPoseCreate(nullptr, &ar_pose);
                arQueryResultGetHitPose(ar_session_, ar_query_result, ar_pose);
                int32_t in_polygon = 0;
                ARPlaneNode *ar_plane = ARNodeAsARPlaneNode(ar_node);
                arPlaneNodeIsPoseInPolygon(ar_session_, ar_plane, ar_pose, &in_polygon);
                arNodeRelease(ar_node);
                arPoseDestroy(ar_pose);

                // Note that the application is responsible for releasing the anchor
                // pointer after using it. Call ArAnchor_release(anchor) to release.
                ARAnchor *anchor = nullptr;
                if (arQueryResultAcquireNewAnchor(ar_session_, ar_query_result, &anchor) !=
                    ARRESULT_SUCCESS) {
                    LOGE("HelloArApplication::OnTouched ArHitResult_acquireNewAnchor error");
                    return;
                }

                ARTrackingState tracking_state = ARTRACKING_STATE_STOPPED;
                arAnchorGetTrackingState(ar_session_, anchor, &tracking_state);
                if (tracking_state != ARTRACKING_STATE_SUCCESS) {
                    arAnchorRelease(anchor);
                    continue;
                }

                tracked_obj_set_.push_back(anchor);
                arQueryResultDestroy(ar_query_result);
                ar_query_result = nullptr;
            }

            arQueryResultListDestroy(query_result_list);
            query_result_list = nullptr;
        }
    }

    void HelloArApplication::DeleteModel() {
        if(!tracked_obj_set_.empty()){
            tracked_obj_set_.pop_back();
        }
    }

    const char* HelloArApplication::getDebugInfo(){
        int filled_size = 0;
        arSessionGetStringValue(ar_session_, ARPARAMETER_ALGORITHM_SLAM_INFO, slam_info, &filled_size, slam_info_size);
        return slam_info;
    }

    bool HelloArApplication::isSLAMInitializing() {

        ARTrackingState world_state = ARTRACKING_STATE_STOPPED;
        arSessionGetTrackingState(ar_session_, &world_state);
        if(world_state == ARTRACKING_STATE_INITIALIZING)
            return true;

        return false;
    }
}  // namespace hello_ar
