# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(STANDARDAR_SRCDIR ${CMAKE_SOURCE_DIR}/../../standardar/)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(${STANDARDAR_SRCDIR}/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/sensorlibrary/src/main/sensor/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/slamlibrary/src/main/slam/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/handgesturelibrary/src/main/hand/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/markerlibrary/src/main/marker/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/facemeshlibrary/src/main/facemesh/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/sensorlibrary/src/main/sensor/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/objecttrackinglibrary/src/main/objecttracking/)
include_directories(${CMAKE_SOURCE_DIR}/../../standardar_aar/bodydetectlibrary/src/main/bodydetect/)

add_library( # Sets the name of the library.
        standardar

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).

        ${CMAKE_SOURCE_DIR}/../../standardar/ArAnchor.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArAuxRender.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArBgYUVRender.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArCommon.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArCamera.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArConfig.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArEngineBase.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArFaceMesh.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArFrame.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArHandGesture.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArHitResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArMath.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArPose.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArSession.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArTrackable.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/FrameBufferObject.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/RenderBuffer.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/OffscreenRenderTarget.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/StandardAR.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/StandardARCommon.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/StandardARInternal.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/com_standardar_common_Util.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/com_standardar_internal.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArJniHelpler.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/ArEngineJavaClient.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/NativeClient.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/IlluminationEstimation.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/miniz.c
        ${CMAKE_SOURCE_DIR}/../../standardar/ArWorldLifeCycle.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar/DynamicLoader.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/sensorlibrary/src/main/sensor/CameraEngine.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/sensorlibrary/src/main/sensor/SensorEngine.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/slamlibrary/src/main/slam/SLAMAlgorithmResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/slamlibrary/src/main/slam/ArPlane.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/handgesturelibrary/src/main/hand/HandGestureAlgorithmResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/markerlibrary/src/main/marker/MarkerAlgorithmResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/markerlibrary/src/main/marker/ArImageNode.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/markerlibrary/src/main/marker/ArImageNodeMgr.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/facemeshlibrary/src/main/facemesh/FaceMeshAlgorithmResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/bodydetectlibrary/src/main/bodydetect/BodyDetectAlgorithmResult.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/bodydetectlibrary/src/main/bodydetect/ArBodyDetctNode.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/objecttrackinglibrary/src/main/objecttracking/ArObjectNode.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/objecttrackinglibrary/src/main/objecttracking/ArReferenceObjectDatabase.cpp
        ${CMAKE_SOURCE_DIR}/../../standardar_aar/objecttrackinglibrary/src/main/objecttracking/ObjectTrackingAlgorithmResult.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


find_library(log-lib log)
# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_include_directories(standardar PRIVATE
        ${STANDARDAR_SRCDIR}/timewarp
        ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

target_link_libraries(standardar ${log-lib} EGL GLESv2 z android)

