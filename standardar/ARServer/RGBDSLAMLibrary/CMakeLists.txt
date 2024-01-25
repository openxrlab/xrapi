# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

# OpenCV is mandatory for undistortion step in StereoSLAM
# download OpenCV libs from ftp server and update OpenCV_DIR in this file
# ftp://10.6.4.23
cmake_minimum_required(VERSION 3.4.1)

set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/)
set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/)
# set(OpenCV_DIR "C:/OpenCV/opencv_3.4.2_clang_no_thread_fix_load_201809061044/sdk/native/jni/abi-armeabi-v7a")

# find_package(OpenCV REQUIRED)

include_directories( ${SRCDIR}/implibs/include/ )
include_directories( ${SRCDIR}/../libs/sdk_common/include/ )
include_directories( ${SRCDIR}/../CommonLibrary/src/main/jni )
# include_directories( ${OpenCV_DIR}/../include/ )

add_library( rgbd_slam
             SHARED
             IMPORTED )

set_target_properties( # Specifies the target library.
                       rgbd_slam

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${LIBDIR}/implibs/libs/${ANDROID_ABI}/libxrslam.so )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             rgbd_slamjni

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${SRCDIR}/../CommonLibrary/src/main/jni/Utils.cpp
             ${SRCDIR}/../CommonLibrary/src/main/jni/ArMath.cpp
             src/main/jni/com_standardar_service_slam_algorithm_SLAM.cpp
             src/main/jni/SLAMBase.cpp
             src/main/jni/RGBSLAM.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )


target_include_directories(rgbd_slamjni PRIVATE
                           ${ANDROID_NDK}/sources/third_party/vulkan/src/libs/glm)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        rgbd_slamjni
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        rgbd_slam
                        android
#                         ${OpenCV_LIBS}
                        )