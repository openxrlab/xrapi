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

#include "mesh_renderer.h"
#include "util.h"

namespace hello_ar {
    namespace {
        constexpr char kVertexShader[] = R"(
    precision highp float;
    precision highp int;
    attribute vec3 vertex;
    attribute vec3 normal;
    uniform mat4 mvp;
    varying mediump vec3 vertex_normal;
    varying float v_alpha;
    varying vec3 frag_pos;

    void main() {
      vertex_normal = normalize(normal);
      gl_Position = mvp * vec4(vertex.xyz, 1.0);
      frag_pos = vertex;
      v_alpha = 1.0;
    })";

        constexpr char kFragmentShader[] = R"(
    precision highp float;
    precision highp int;
    uniform vec3 light_pos;
    varying mediump vec3 vertex_normal;
    varying float v_alpha;
    varying vec3 frag_pos;
    void main() {
      vec3 light_dir = normalize(light_pos - frag_pos);
      float diffuse = max(dot(vertex_normal, light_dir), 0.0);
      gl_FragColor = vec4(diffuse, diffuse, diffuse, v_alpha);
    })";
    }  // namespace

    void MeshRenderer::InitializeGlContent(AAssetManager *asset_manager) {
        shader_program_ = util::CreateProgram(kVertexShader, kFragmentShader);

        if (!shader_program_) {
            LOGE("Could not create program.");
        }

        uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "mvp");
        uniform_light_pos_ = glGetUniformLocation(shader_program_, "light_pos");
        attri_vertices_ = glGetAttribLocation(shader_program_, "vertex");
        attri_normals_ = glGetAttribLocation(shader_program_, "normal");

        util::CheckGlError("mesh_renderer::InitializeGlContent()");
    }

    void MeshRenderer::Draw(const glm::mat4 &projection_mat, const glm::mat4 &view_mat, const ARSession* session, const ARFrame* frame) {
        if (!shader_program_) {
            LOGE("shader_program is null.");
            return;
        }

        glm::mat4 model_mat;
        glm::vec3 color;
        color.r = 1.0f;
        color.g = 0.0f;
        color.b = 0.0f;

        ARDenseMesh* denseMesh = nullptr;
        arFrameAcquireDenseMesh(session, frame, &denseMesh);

        int32_t vertex_count = 0;
        arDenseMeshGetVertexCount(session, denseMesh, &vertex_count);
        if(vertex_count>0)
        {
            vertices_.resize(vertex_count*3);
            arDenseMeshGetVertex(session, denseMesh, vertices_.data());

            normals_.resize(vertex_count*3);
            arDenseMeshGetNormal(session, denseMesh, normals_.data());
        }

        int32_t index_count =0;
        arDenseMeshGetIndexCount(session, denseMesh, &index_count);
        if(index_count>0)
        {
            triangles_.resize(index_count);
            arDenseMeshGetIndex(session, denseMesh, triangles_.data());
        }

        arDenseMeshRelease(denseMesh);

        if(vertex_count==0||index_count==0) {
            return;
        }

        glUseProgram(shader_program_);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Compose final mvp matrix for this plane renderer.
        glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(projection_mat * view_mat * model_mat_));

        glm::mat4 view_inv = glm::inverse(view_mat);
        glUniform3f(uniform_light_pos_, view_inv[3][0], view_inv[3][1], view_inv[3][2]);

        glEnableVertexAttribArray(attri_vertices_);
        glVertexAttribPointer(attri_vertices_, 3, GL_FLOAT, GL_FALSE, 0, vertices_.data());

        glEnableVertexAttribArray(attri_normals_);
        glVertexAttribPointer(attri_normals_, 3, GL_FLOAT, GL_FALSE, 0, normals_.data());

        glDrawElements(GL_TRIANGLES, (GLsizei)triangles_.size(), GL_UNSIGNED_SHORT, (GLvoid*)triangles_.data());

        glUseProgram(0);
        glDisable(GL_BLEND);
        util::CheckGlError("mesh_renderer::Draw()");
    }

}  // namespace hello_ar
