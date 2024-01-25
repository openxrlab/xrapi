# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../sdk/)
set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/)


add_library(bodydetect_native SHARED

        src/main/cpp/hello_ar_application.cc
        src/main/cpp/jni_interface.cc
        src/main/cpp/background_renderer.cc
        src/main/cpp/body_render.cpp
        src/main/cpp/util.cc)


add_library(standardar SHARED IMPORTED)
set_target_properties(standardar
        PROPERTIES IMPORTED_LOCATION
        ${LIBDIR}/libs/${ANDROID_ABI}/libstandardar.so)


target_include_directories(bodydetect_native PRIVATE
        src/main/cpp
        ${SRCDIR}/../../../sdk/includes
        ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

find_library(log-lib log)


target_link_libraries(
        bodydetect_native
        standardar
        android
        log
        GLESv3
        jnigraphics
)