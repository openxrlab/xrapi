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
 * 1. rename attribute_vertices_ parameter  to cameraVertexHandle
 * 2. rename attribute_uvs_  parameter  to uniform_texture_
 * 3. rename uniform_texture_ parameter  to cameraYUniformHandle
 * 4. add cameraShaderID parameter
 */
#ifndef C_ARCORE_HELLOE_AR_BACKGROUND_RENDERER_H_
#define C_ARCORE_HELLOE_AR_BACKGROUND_RENDERER_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>

#include "util.h"



// This class renders the passthrough camera image into the OpenGL frame.
class BackgroundRenderer {
 public:
  BackgroundRenderer() = default;
  ~BackgroundRenderer() = default;

  // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
  // other methods below.
  void InitializeGlContent();

  // Draws the background image.  This methods must be called for every ArFrame
  // returned by ArSession_update() to catch display geometry change events.
  void Draw(const ARSession * session, const ARFrame* frame,uint32_t cameraType);

  // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
  GLuint GetTextureId() const;

 private:
    static constexpr int kNumVertices = 4;
    GLuint texture_id_;

    GLuint cameraShaderID = 0;
    GLuint cameraVertexHandle = 0;
    GLuint cameraTexCoordHandle = 0;
    GLuint cameraYUniformHandle = 0;

    float transformed_uvs_[kNumVertices * 2];
    bool uvs_initialized_ = false;
};

#endif  // TANGO_GL_VIDEO_OVERLAY_H_
