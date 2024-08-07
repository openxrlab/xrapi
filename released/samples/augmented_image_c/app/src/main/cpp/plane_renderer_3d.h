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
 * 1. add plane_renderer_3d class to renderer 3D planes
 */
#ifndef C_ARCORE_HELLOE_AR_PLANE_RENDERER_3D_H_
#define C_ARCORE_HELLOE_AR_PLANE_RENDERER_3D_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "StandardAR.h"
#include "glm.h"

namespace augmented_image {

// PlaneRenderer3D renders ARCore plane type.
class PlaneRenderer3D {
 public:
  PlaneRenderer3D() = default;
  ~PlaneRenderer3D() = default;

  // Sets up OpenGL state used by the plane renderer.  Must be called on the
  // OpenGL thread.
  void InitializeGlContent(AAssetManager* asset_manager);

  // Draws the provided plane.
  void Draw(const glm::mat4& projection_mat, const glm::mat4& view_mat,
            const ARSession* ar_session, const ARPlaneNode* ar_plane,
            const glm::vec3& color);

 private:
  void UpdateForPlane(const  ARSession* ar_session, const ARPlaneNode* ar_plane);

  std::vector<glm::vec4> vertices_;
  std::vector<GLushort> triangles_;
  glm::mat4 model_mat_ = glm::mat4(1.0f);
  glm::vec3 normal_vec_ = glm::vec3(0.0f);

  GLuint texture_id_;

  GLuint shader_program_;
  GLuint attri_vertices_;
  GLuint uniform_mvp_mat_;
  GLuint uniform_texture_;
  GLuint uniform_texture_mat_;
  GLuint uniform_normal_vec_;
  GLuint uniform_color_;

};
}  // namespace hello_ar

#endif  // C_ARCORE_HELLOE_AR_PLANE_RENDERER_H_
