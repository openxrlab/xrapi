#ifndef HELLO_OBJECT_C_SRC_OBJECT_TRACKING_APPLICATION_H
#define HELLO_OBJECT_C_SRC_OBJECT_TRACKING_APPLICATION_H

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
#include "scanner_renderer.h"
#include "obj_renderer.h"

namespace hello_ar {

// ObjectTrackingApplication handles all application logics.
    class ObjectTrackingApplication {
    public:
        // Constructor and deconstructor.
        ObjectTrackingApplication() = default;
        ObjectTrackingApplication(AAssetManager* asset_manager, void *env, void *context, char* package_name, char* apkInternalPath);
        ~ObjectTrackingApplication();

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

        void OnStopAlgorithm();

        const char* getDebugInfo();

        bool isSLAMInitializing();

    private:
        ARReferenceObjectDatabase* CreateObjectTrackingDatabase() const;

        // Draws frame on an ObjectTracking.
        // @return true if there is an ObjectTracking, false otherwise.
        bool DrawObjectTracking(const glm::mat4& view_mat,
                                const glm::mat4& projection_mat,
                                const float* color_correction);

        ARSession *ar_session_ = nullptr;
        ARFrame *ar_frame_ = nullptr;
        ARWorldMap   *ar_map_   = nullptr;

        AAssetManager* const asset_manager_;

        BackgroundRenderer background_renderer_;
        ScannerRenderer scanner_renderer_;
        ObjRenderer andy_renderer_;

        std::unordered_map<int, glm::vec3> plane_color_map_;
        PlaneRenderer3D plane_renderer_3d_;
        std::map<int, glm::mat4>     object_set_;

        std::string object_tracking_info;
    };
}  // namespace hello_ar

#endif //HELLO_OBJECT_C_SRC_OBJECT_TRACKING_APPLICATION_H
