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

#ifndef C_ARCORE_AUGMENTED_IMAGE_AUGMENTED_IMAGE_APPLICATION_H_
#define C_ARCORE_AUGMENTED_IMAGE_AUGMENTED_IMAGE_APPLICATION_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <jni.h>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <map>

#include "StandardAR.h"

#include "background_renderer.h"
#include "glm.h"
#include "plane_renderer_3d.h"
#include "util.h"
#include "obj_renderer.h"

namespace augmented_image {

// AugmentedImageApplication handles all application logics.
class AugmentedImageApplication {
 public:
  // Constructor and deconstructor.
  AugmentedImageApplication() = default;
  AugmentedImageApplication(AAssetManager* asset_manager, void *env, void *context, char* package_name, char* apkInternalPath);
  ~AugmentedImageApplication();

  // OnPause is called on the UI thread from the Activity's onPause method.
  void OnPause();

  // OnResume is called on the UI thread from the Activity's onResume method.
  void OnResume(void* env, void* context, void* activity);

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
  void OnDrawFrame(void* activity);

 private:
  ARReferenceImageDatabase* CreateAugmentedImageDatabase() const;

  // Draws frame on an AugmentedImage.
  // @return true if there is an AugmentedImage, false otherwise.
  bool DrawAugmentedImage(const glm::mat4& view_mat,
                          const glm::mat4& projection_mat,
                          const float* color_correction);

  ARSession *ar_session_ = nullptr;
  ARFrame *ar_frame_ = nullptr;
  ARWorldMap *ar_map_   = nullptr;

  AAssetManager* const asset_manager_;

  BackgroundRenderer background_renderer_;
  ObjRenderer andy_renderer_;

  std::unordered_map<int, glm::vec3> plane_color_map_;
  PlaneRenderer3D plane_renderer_3d_;
  std::map<int, glm::mat4>     marker_set_;

  std::string m_apkInternalPath;
};
}  // namespace augmented_image

#endif  // C_ARCORE_AUGMENTED_IMAGE_AUGMENTED_IMAGE_APPLICATION_H_
