# Copyright (C) 2017 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##

# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../sdk/)
set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(hello_ar_native SHARED
           src/main/cpp/background_renderer.cc
        src/main/cpp/hello_ar_application.cc
           src/main/cpp/jni_interface.cc
           src/main/cpp/mesh_renderer.cc
           src/main/cpp/obj_renderer.cc
           src/main/cpp/point_cloud_renderer.cc
           src/main/cpp/util.cc)

add_library(standardar SHARED IMPORTED)
set_target_properties( standardar
                        PROPERTIES IMPORTED_LOCATION
                        ${LIBDIR}/libs/${ANDROID_ABI}/libstandardar.so )

target_include_directories(hello_ar_native PRIVATE
           src/main/cpp
           ${SRCDIR}/../../../sdk/includes
            ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)



target_link_libraries(hello_ar_native
                      standardar
                      android
                      log
                      GLESv2)