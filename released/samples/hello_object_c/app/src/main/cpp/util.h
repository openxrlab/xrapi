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

#ifndef C_ARCORE_HELLOE_AR_UTIL_H_
#define C_ARCORE_HELLOE_AR_UTIL_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <errno.h>
#include <jni.h>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>

#include "StandardAR.h"
#include "glm.h"

#ifndef LOGI
#define LOGI(...) \
  __android_log_print(ANDROID_LOG_INFO, "standard ar", __VA_ARGS__)
#endif  // LOGI

#ifndef LOGE
#define LOGE(...) \
  __android_log_print(ANDROID_LOG_ERROR, "standard ar", __VA_ARGS__)
#endif  // LOGE

#ifndef CHECK
#define CHECK(condition)                                                   \
  if (!(condition)) {                                                      \
    LOGE("*** standard ar CHECK FAILED at %s:%d: %s", __FILE__, __LINE__, #condition); \
  }
#endif  // CHECK

namespace hello_ar {

// Utilities for C hello AR project.
namespace util {

// Provides a scoped allocated instance of Anchor.
// Can be treated as an ArAnchor*.
class ScopedArPose {
 public:
  explicit ScopedArPose(const ARSession* ar_session) {
    arPoseCreate(nullptr, &pose_);
  }
  ~ScopedArPose() { arPoseDestroy(pose_); }
  ARPose* GetArPose() { return pose_; }
  // Delete copy constructors.
  ScopedArPose(const ScopedArPose&) = delete;
  void operator=(const ScopedArPose&) = delete;

 private:
  ARPose* pose_;
};

// Looks up Java class IDs and Method IDs and cache them.
void InitializeJavaMethodIDs();

// Clear the Java class IDs and Method IDs.
void ReleaseJavaMethodIDs();

// Check GL error, and abort if an error is encountered.
//
// @param operation, the name of the GL function call.
void CheckGlError(const char* operation);

// Create a shader program ID.
//
// @param vertex_source, the vertex shader source.
// @param fragment_source, the fragment shader source.
// @return
GLuint CreateProgram(const char* vertex_source, const char* fragment_source);

// Loads a file from asset.
//
// @param mgr, the pointer to AAssetManager
// @param file_name, the filename to load
// @param out_file_buffer, a pointer to a std::string object, it will
// be resized to the file content size, and populated with file content.
// @return true if file is loaded successfully, otherwise false.
bool LoadEntireAssetFile(AAssetManager* mgr, const std::string& file_name,
                             std::string* out_file_buffer);

bool extractAssetFile(AAssetManager* mgr, const std::string& file_name, const std::string& apk_internal_path,
                          char** out_file_buffer, int* file_buffer_length);

bool LoadFile(const std::string& path, char** out_file_buffer, int* file_buffer_length);

// Load png file from assets folder and then assign it to the OpenGL target.
// This method must be called from the renderer thread since it will result in
// OpenGL calls to assign the image to the texture target.
//
// @param target, openGL texture target to load the image into.
// @param path, path to the file, relative to the assets folder.
// @return true if png is loaded correctly, otherwise false.
bool LoadPngFromAssetManager(int target, const std::string& path);

// Hides the fit-to-scan image.
//
// @param activity a jobject value, pointing to the ObjectTrackingActivity
// instance.
// @return true if function is successful, otherwise false
bool HideFitToScanImage(void* activity);

// Load obj file from assets folder from the app.
//
// @param mgr, AAssetManager pointer.
// @param file_name, name of the obj file.
// @param out_vertices, output vertices.
// @param out_normals, output normals.
// @param out_uv, output texture UV coordinates.
// @param out_indices, output triangle indices.
// @return true if obj is loaded correctly, otherwise false.
bool LoadObjFile(AAssetManager* mgr, const std::string& file_name,
                 std::vector<GLfloat>* out_vertices,
                 std::vector<GLfloat>* out_normals,
                 std::vector<GLfloat>* out_uv,
                 std::vector<GLushort>* out_indices);

// Format and output the matrix to logcat file.
// Note that this function output matrix in row major.
void Log4x4Matrix(float raw_matrix[16]);

// Get transformation matrix from ArAnchor.
void GetTransformMatrixFromAnchor(ARSession* ar_session,
                                  const ARAnchor* ar_anchor,
                                  glm::mat4* out_model_mat);

void SaveObjectPointCloud(std::string outputPath, uint8_t *raw_bytes, int64_t raw_bytes_size);
}  // namespace util
}  // namespace hello_ar

#endif  // C_ARCORE_HELLOE_AR_UTIL_H_
