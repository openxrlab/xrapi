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

// This modules handles drawing the passthrough camera image into the OpenGL
// scene.

#include <type_traits>

#include "background_renderer.h"

namespace hello_ar {
    namespace {

        GLfloat quadVerticesArray[] = {
                -1.0f, -1.0f, 0.0f, +1.0f, -1.0f, 0.0f,
                -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, 0.0f,
        };

        GLfloat quadTexCoordsArray[] = {
                0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        };

//        GLfloat quadTexCoordsArray[] = {
//                1.0f, 0.0f,
//                1.0f, 1.0f,
//                0.0f, 1.0f,
//                0.0f, 0.0f
//        };

        short quadIndicesArray[] = {
                0, 1, 2,
                1, 2, 3
        };


        constexpr char VERTEX_SHADER[] = R"(attribute vec4 vertexPosition;
                                   attribute vec2 vertexTexCoord;
                                   varying vec2 texCoord;
                                   void main() {
                                   gl_Position = vertexPosition;
                                   texCoord = vertexTexCoord;
                                   })";

        constexpr char FRAGMENT_SHADER[] = R"(
                                     uniform sampler2D videoFrameY;
                                     varying lowp vec2 texCoord;
                                     void main() {
                                     lowp vec3 yuv;
                                     yuv = texture2D(videoFrameY, texCoord).rgb;
                                     gl_FragColor = vec4(yuv,1.0);
                                     })";

    }  // namespace

    void BackgroundRenderer::InitializeGlContent() {

        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        cameraShaderID = util::CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
        cameraVertexHandle = glGetAttribLocation(cameraShaderID, "vertexPosition");
        cameraTexCoordHandle = glGetAttribLocation(cameraShaderID, "vertexTexCoord");
        cameraYUniformHandle = glGetUniformLocation(cameraShaderID, "videoFrameY");
    }

    GLuint BackgroundRenderer::GetTextureId() const { return texture_id_; }


    void BackgroundRenderer::Draw(const ARSession* session, const ARFrame* frame) {
        // If display rotation changed (also includes view size change), we need to
        // re-query the uv coordinates for the on-screen portion of the camera image.
        int32_t geometry_changed = 0;
        arFrameGetDisplayGeometryChanged(session, frame, &geometry_changed);
        if (geometry_changed != 0 || !uvs_initialized_) {
            arFrameTransformDisplayUvCoords(session, frame, kNumVertices * 2, quadTexCoordsArray,
                                             transformed_uvs_);
            uvs_initialized_ = true;
        }

        glDisable(GL_DEPTH_TEST);

        glUseProgram(cameraShaderID);

        glVertexAttribPointer(cameraVertexHandle, 3, GL_FLOAT, false, 0, quadVerticesArray);
        //glVertexAttribPointer(cameraTexCoordHandle, 2, GL_FLOAT, false, 0, quadTexCoordsArray);
        glVertexAttribPointer(cameraTexCoordHandle, 2, GL_FLOAT, false, 0, transformed_uvs_);

        glEnableVertexAttribArray(cameraVertexHandle);
        glEnableVertexAttribArray(cameraTexCoordHandle);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glUniform1i(cameraYUniformHandle, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadIndicesArray);

        glDisable(GL_BLEND);

        glDisableVertexAttribArray(cameraVertexHandle);
        glDisableVertexAttribArray(cameraTexCoordHandle);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }


}  // namespace hello_ar
