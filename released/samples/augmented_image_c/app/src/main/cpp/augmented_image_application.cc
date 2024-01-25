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

#include "augmented_image_application.h"

#include <android/asset_manager.h>
#include <array>
#include <cstdint>
#include <utility>

#include "obj_renderer.h"
#include "util.h"

namespace augmented_image {
    namespace {
        constexpr int32_t kTintColorRgbaSize = 16;

        constexpr std::array<uint32_t, kTintColorRgbaSize> kTintColorRgba = {
                {0x000000FF, 0xF44336FF, 0xE91E63FF, 0x9C27B0FF, 0x673AB7FF, 0x3F51B5FF,
                        0x2196F3FF, 0x03A9F4FF, 0x00BCD4FF, 0x009688FF, 0x4CAF50FF, 0x8BC34AFF,
                        0xCDDC39FF, 0xFFEB3BFF, 0xFFC107FF, 0xFF9800FF}};

// AugmentedImage configuration and rendering.
// Load a single image (true) or a pre-generated image database (false).
        enum AugmentedImageType{
            Image
        };
        constexpr AugmentedImageType augmentedImageType = Image;

        inline glm::vec3 GetRandomPlaneColor() {
            const int32_t colorRgba = kTintColorRgba[std::rand() % kTintColorRgbaSize];
            return glm::vec3(((colorRgba >> 24) & 0xff) / 255.0f,
                             ((colorRgba >> 16) & 0xff) / 255.0f,
                             ((colorRgba >> 8) & 0xff) / 255.0f);
        }

    }  // namespace

    AugmentedImageApplication::AugmentedImageApplication(
            AAssetManager *asset_manager, void *env,
            void *context, char *package_name, char* apkInternalPath)
            : asset_manager_(asset_manager) {
        LOGI("OnCreate()");

        CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
        CHECK(ar_session_);

        arSessionAcquireWorldMap(ar_session_, &ar_map_);

        arFrameCreate(ar_session_, &ar_frame_);
        CHECK(ar_frame_);

        m_apkInternalPath = apkInternalPath;
    }

    AugmentedImageApplication::~AugmentedImageApplication() {
        if (ar_session_ != nullptr) {
            arFrameDestroy(ar_frame_);
            arSessionDestroy(ar_session_);
            arWorldMapRelease(ar_map_);
        }
    }

    void AugmentedImageApplication::OnPause() {
        LOGI("OnPause()");
        if (ar_session_ != nullptr) {
            arSessionPause(ar_session_);
        }
        marker_set_.clear();
    }

    void AugmentedImageApplication::OnResume(void *env, void *context,
                                             void *activity) {
        LOGI("OnResume()");
        ARConfig *ar_config = nullptr;
        arConfigCreate(&ar_config);
        CHECK(ar_config);

        arConfigSetAlgorithmMode(ar_config, ARALGORITHM_TYPE_IMAGE_TRACKING, ARALGORITHM_MODE_ENABLE);
        ARReferenceImageDatabase* ar_image_database = CreateAugmentedImageDatabase();
        arConfigSetReferenceImageDatabase(ar_session_, ar_config, ar_image_database);
        arReferenceImageDatabaseDestroy(ar_image_database);
        CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

        arConfigDestroy(ar_config);

        const ARResult status = arSessionResume(ar_session_);
        CHECK(status == ARRESULT_SUCCESS);
    }

    ARReferenceImageDatabase* AugmentedImageApplication::CreateAugmentedImageDatabase() const {
        ARReferenceImageDatabase *ar_augmented_image_database = nullptr;
        arReferenceImageDatabaseCreate(ar_session_, &ar_augmented_image_database);
        // There are two ways to configure a ArAugmentedImageDatabase:
        // 1. Add Bitmap to DB directly
        // 2. Load a pre-built AugmentedImageDatabase
        // Option 2) has
        // * shorter setup time
        // * doesn't require images to be packaged in apk.
        switch (augmentedImageType) {
            case Image: {
                int32_t width, height, stride, index;
                std::string kSampleImageName[1] = {"1.jpg"};
                bool load_image_result;
                uint8_t *grayscale_buffer;
                for (int i = 0; i < 1; i++) {
                    LOGE("add target image %d", i);
                    load_image_result = util::LoadImageFromAssetManager(
                            kSampleImageName[i], &width, &height, &stride, &grayscale_buffer);
                    CHECK(load_image_result);

                    ARResult status = arReferenceImageDatabaseAddImage(
                            ar_session_, ar_augmented_image_database, kSampleImageName[i].c_str(),
                            grayscale_buffer, width, height, stride, &index);
                    CHECK(status == ARRESULT_SUCCESS);
                    delete[] grayscale_buffer;
                }
                break;
            }
            default:
                break;
        }

        return ar_augmented_image_database;
    }

    void AugmentedImageApplication::OnSurfaceCreated() {
        LOGI("OnSurfaceCreated()");

        background_renderer_.InitializeGlContent();
        andy_renderer_.InitializeGlContent(asset_manager_, "andy.obj", "andy.png");
        arSessionSetCameraTextureName(ar_session_,
                                    background_renderer_.GetTextureId());
        plane_renderer_3d_.InitializeGlContent(asset_manager_);
    }

    void AugmentedImageApplication::OnDisplayGeometryChanged(int display_rotation,
                                                             int width,
                                                             int height) {
        LOGI("OnSurfaceChanged(%d, %d)", width, height);

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

    void AugmentedImageApplication::OnDrawFrame(void *activity) {
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
            LOGE("AugmentedImageApplication::OnDrawFrame ArSession_update error");
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

        float color_correction[4] = {1.f, 1.f, 1.f, 1.f};
        bool found_ar_image = DrawAugmentedImage(view_mat, projection_mat, color_correction);

//   Once we found the first image, hide the scan animation
        if (found_ar_image) {
            util::HideFitToScanImage(activity);
        }
    }

    bool AugmentedImageApplication::DrawAugmentedImage(
            const glm::mat4 &view_mat, const glm::mat4 &projection_mat,
            const float *color_correction) {
        bool found_ar_image = false;

        ARNodeList *updated_image_list = nullptr;
        arNodeListCreate(ar_session_, &updated_image_list);
        CHECK(updated_image_list != nullptr);
        arWorldMapGetAllNodes(ar_session_, ar_map_, ARNODE_TYPE_IMAGE, updated_image_list);

        int32_t image_list_size;
        arNodeListGetSize(ar_session_, updated_image_list, &image_list_size);
        float light_intensity = 0.8f;
        // Find newly detected image, add it to map
        for (int i = 0; i < image_list_size; ++i) {
            ARNode *ar_node = nullptr;
            arNodeListAcquireItem(ar_session_, updated_image_list, i,
                                  &ar_node);
            ARImageNode *image = ARNodeAsARImageNode(ar_node);

            ARTrackingState tracking_state;
            arNodeGetTrackingState(ar_session_, ar_node, &tracking_state);

            int image_index;
            arImageNodeGetIndex(ar_session_, image, &image_index);
            switch (tracking_state) {
                case ARTRACKING_STATE_STOPPED:
                    break;
                case ARTRACKING_STATE_SUCCESS: {
                    found_ar_image = true;

                    LOGI("Detected Image %d", image_index);
                    // Record the image and its marker.
                    util::ScopedArPose scopedArPose(ar_session_);
                    arImageNodeGetCenterPose(ar_session_, image,
                                             scopedArPose.GetArPose());
                    ARPose *pose = scopedArPose.GetArPose();

                    // Render Andy objects.
                    glm::mat4 model_mat(1.0f);
                    glm::vec3 scale(0.6);
                    arPoseGetMatrix(pose, glm::value_ptr(model_mat));
                    model_mat = glm::scale(model_mat, scale);
                    andy_renderer_.Draw(projection_mat, view_mat, model_mat, &light_intensity);
                    break;
                }
                default:
                    break;
            } // End of switch (tracking_state)
        }    // End of for (int i = 0; i < image_list_size; ++i) {

        arNodeListDestroy(updated_image_list);
        updated_image_list = nullptr;

        return found_ar_image;
    }

}  // namespace augmented_image
