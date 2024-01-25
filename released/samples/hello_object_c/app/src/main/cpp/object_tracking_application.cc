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

#include "object_tracking_application.h"

#include <android/asset_manager.h>
#include <array>
#include <cstdint>
#include <utility>
#include <unistd.h>

#include "util.h"
#include "FileUtils.h"

namespace hello_ar {
    namespace {
        constexpr int32_t kTintColorRgbaSize = 16;

        enum ReferenceObjectType{
            Object,
            Database
        };
        constexpr ReferenceObjectType referenceObjectType = Object;

        constexpr std::array<uint32_t, kTintColorRgbaSize> kTintColorRgba = {
                {0x000000FF, 0xF44336FF, 0xE91E63FF, 0x9C27B0FF, 0x673AB7FF, 0x3F51B5FF,
                        0x2196F3FF, 0x03A9F4FF, 0x00BCD4FF, 0x009688FF, 0x4CAF50FF, 0x8BC34AFF,
                        0xCDDC39FF, 0xFFEB3BFF, 0xFFC107FF, 0xFF9800FF}};

        inline glm::vec3 GetRandomPlaneColor() {
            const int32_t colorRgba = kTintColorRgba[std::rand() % kTintColorRgbaSize];
            return glm::vec3(((colorRgba >> 24) & 0xff) / 255.0f,
                             ((colorRgba >> 16) & 0xff) / 255.0f,
                             ((colorRgba >> 8) & 0xff) / 255.0f);
        }

    }  // namespace

    ObjectTrackingApplication::ObjectTrackingApplication(
            AAssetManager *asset_manager, void *env,
            void *context, char *package_name, char* apkInternalPath)
            : asset_manager_(asset_manager) {
        LOGI("OnCreate()");

        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        arFrameCreate(ar_session_, &ar_frame_);
        CHECK(ar_frame_);

        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);
        CHECK(ar_config);

        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_OBJECT_TRACKING, ARALGORITHM_MODE_ENABLE);
        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_PLANE_DETECTION, ARALGORITHM_MODE_ENABLE);
        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_SLAM, ARALGORITHM_MODE_ENABLE);

        ARReferenceObjectDatabase* ar_object_database = CreateObjectTrackingDatabase();
        arConfigSetReferenceObjectDatabase(ar_session_, ar_config, ar_object_database);
        arReferenceObjectDatabaseDestroy(ar_object_database);
        CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

        arConfigDestroy(ar_config);
    }

    ObjectTrackingApplication::~ObjectTrackingApplication() {
        if (ar_session_ != nullptr) {
            arFrameDestroy(ar_frame_);
            arSessionDestroy(ar_session_);
            arWorldMapRelease(ar_map_);
        }
    }

    void ObjectTrackingApplication::OnPause() {
        LOGI("OnPause()");
        if (ar_session_ != nullptr) {
            arSessionPause(ar_session_);
        }
        object_set_.clear();
    }

    void ObjectTrackingApplication::OnResume(void *env, void *context,
                                             void *activity) {
        LOGI("OnResume()");
        const ARResult status = arSessionResume(ar_session_);
        CHECK(status == ARRESULT_SUCCESS);
    }

    ARReferenceObjectDatabase* ObjectTrackingApplication::CreateObjectTrackingDatabase() const {
        ARReferenceObjectDatabase *ar_object_tracking_database = nullptr;
        arReferenceObjectDatabaseCreate(ar_session_, &ar_object_tracking_database);

        switch (referenceObjectType) {
            case Object: {
                std::string sampleObjectName[1] = {"1.sot"};
                std::string file_path = "/sdcard/1.sot";
                char* object_buffer;
                int object_buffer_length;
                if(util::LoadFile(file_path, &object_buffer, &object_buffer_length)) {
                    if(object_buffer && object_buffer_length > 0) {
                        int index = 0;
                        ARResult status = arReferenceObjectDatabaseAddObject(ar_session_,
                                                                             ar_object_tracking_database,
                                                                             sampleObjectName[0].c_str(),
                                                                             (uint8_t *) object_buffer,
                                                                             object_buffer_length,
                                                                             &index);
                        CHECK(status == ARRESULT_SUCCESS);
                        delete[] object_buffer;
                    }
                }
                break;
            }
            case Database: {
                break;
            }
            default:
                break;
        }

        return ar_object_tracking_database;
    }

    void ObjectTrackingApplication::OnSurfaceCreated() {
        LOGI("OnSurfaceCreated()");

        background_renderer_.InitializeGlContent();
        arSessionSetCameraTextureName(ar_session_,
                                    background_renderer_.GetTextureId());
        plane_renderer_3d_.InitializeGlContent(asset_manager_);
        scanner_renderer_.InitializeGlContent();
        andy_renderer_.InitializeGlContent(asset_manager_, "andy.obj", "andy.png");
    }

    void ObjectTrackingApplication::OnDisplayGeometryChanged(int display_rotation,
                                                             int width,
                                                             int height) {
        LOGI("OnSurfaceChanged(%d, %d)", width, height);
        glViewport(0, 0, width, height);

        int w = 0;
        int h = 0;
        arFrameGetImageResolution(ar_session_, ar_frame_, &w, &h);
        float ratio = (float) w / h;

        float debugRenderAspect = 0.0;
        float originScreenAspect = (float) width / height;

        if (originScreenAspect < 1)
            debugRenderAspect = 1.0 / ratio;
        int viewportWidth =
                originScreenAspect > debugRenderAspect ? width : (height * debugRenderAspect);
        int viewportHeight = viewportWidth / debugRenderAspect;
        glViewport((width - viewportWidth) / 2, (height - viewportHeight) / 2, viewportWidth,
                   viewportHeight);

        if (ar_session_ != nullptr) {
            arSessionSetDisplayGeometry(ar_session_, display_rotation, viewportWidth, viewportHeight);
        }

    }

    void ObjectTrackingApplication::OnDrawFrame(void *activity) {
        // Render the scene.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        //glEnable(GL_CULL_FACE);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (ar_session_ == nullptr) return;

        // Update session to get current frame and render camera background.
        if (arSessionUpdate(ar_session_, ar_frame_) != ARRESULT_SUCCESS) {
            LOGE("ObjectTrackingApplication::OnDrawFrame ArSession_update error");
        }

        ARCamera *ar_camera;
        arFrameAcquireCamera(ar_session_, ar_frame_, &ar_camera);

        float znear = 0.1f;
        float zfar = 100.0f;
        glm::mat4 view_mat;
        glm::mat4 projection_mat;
        arCameraGetViewMatrix(ar_session_, ar_camera, glm::value_ptr(view_mat));
        arCameraGetProjectionMatrix(ar_session_, ar_camera, znear, zfar, glm::value_ptr(projection_mat));

        ARTrackingState trackState;
        arCameraGetTrackingState(ar_session_, ar_camera, &trackState);
        arCameraRelease(ar_camera);

        background_renderer_.Draw(ar_session_, ar_frame_);

        // If the camera isn't tracking don't bother rendering other objects.

        // Get light estimation value.
        ARIlluminationEstimate *ar_light_estimate;
        ARIlluminationEstimateState ar_light_estimate_state;
        arIlluminationEstimateCreate(ar_session_, &ar_light_estimate);

        arFrameGetIlluminationEstimate(ar_session_, ar_frame_, ar_light_estimate);
        arIlluminationEstimateGetState(ar_session_, ar_light_estimate,
                                       &ar_light_estimate_state);

        // Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
        // The first three components are color scaling factors.
        // The last one is the average pixel intensity in gamma space.
        float color_correction[4] = {1.f, 1.f, 1.f, 1.f};

        arIlluminationEstimateDestroy(ar_light_estimate);
        ar_light_estimate = nullptr;

        bool m_bShowPlane = true;

        if(m_bShowPlane) {
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

        bool found_ar_object;
        found_ar_object = DrawObjectTracking(view_mat, projection_mat, color_correction);

        if (found_ar_object) {
            util::HideFitToScanImage(activity);
        }
    }

    bool ObjectTrackingApplication::DrawObjectTracking(
            const glm::mat4 &view_mat, const glm::mat4 &projection_mat,
            const float *color_correction) {
        bool found_ar_object = false;

        ARNodeList *updated_object_list = nullptr;
        arNodeListCreate(ar_session_, &updated_object_list);
        CHECK(updated_object_list != nullptr);
        arWorldMapGetAllNodes(ar_session_, ar_map_, ARNODE_TYPE_OBJECT, updated_object_list);

        int32_t object_list_size;
        arNodeListGetSize(ar_session_, updated_object_list, &object_list_size);
        float light_intensity = 0.8f;

        for (int i = 0; i < object_list_size; ++i) {
            ARNode *ar_node = nullptr;
            arNodeListAcquireItem(ar_session_, updated_object_list, i,
                                  &ar_node);
            ARObjectNode *object = ARNodeAsARObjectNode(ar_node);

            ARTrackingState tracking_state;
            arNodeGetTrackingState(ar_session_, ar_node, &tracking_state);

            int object_index;
            arObjectNodeGetIndex(ar_session_, object, &object_index);
            switch (tracking_state) {
                case ARTRACKING_STATE_STOPPED:
                    break;
                case ARTRACKING_STATE_SUCCESS: {
                    found_ar_object = true;

                    LOGI("Detected Image %d", object_index);
                    // Record the image and its marker.
                    util::ScopedArPose scopedArPose(ar_session_);
                    arObjectNodeGetCenterPose(ar_session_, object,
                                              scopedArPose.GetArPose());

                    float boundingBox[24];
                    arObjecNodeGetBoundingBox(ar_session_, object, boundingBox);
                    ARPose *pose = scopedArPose.GetArPose();

                    // Render Andy objects.
                    glm::mat4 model_mat(1.0f);
                    arPoseGetMatrix(pose, glm::value_ptr(model_mat));
                    scanner_renderer_.DrawTest(projection_mat, view_mat, model_mat, light_intensity);
                    andy_renderer_.Draw(projection_mat, view_mat, model_mat, light_intensity);
                    break;
                }
                default:
                    break;
            } // End of switch (tracking_state)
        }    // End of for (int i = 0; i < image_list_size; ++i) {

        arNodeListDestroy(updated_object_list);
        updated_object_list = nullptr;

        return found_ar_object;
    }

    void ObjectTrackingApplication::OnStopAlgorithm(){
        arSessionStopAlgorithm(ar_session_, ARALGORITHM_TYPE_SLAM, 0);
    }

    const char* ObjectTrackingApplication::getDebugInfo(){
        char tempstr[1024];
        int length = 0;
        arSessionGetStringValue(ar_session_, ARPARAMETER_ALGORITHM_OBJECT_TRACKING, tempstr, &length, 1024);
        object_tracking_info = tempstr;
        return object_tracking_info.c_str();
    }

    bool ObjectTrackingApplication::isSLAMInitializing() {
        ARTrackingState world_state = ARTRACKING_STATE_STOPPED;
        arSessionGetTrackingState(ar_session_, &world_state);
        if(world_state == ARTRACKING_STATE_INITIALIZING)
            return true;

        return false;
    }
}  // namespace object_tracking
