#ifndef STANDARD_AR_SCANNERRENDERER_C_API_H_
#define STANDARD_AR_SCANNERRENDERER_C_API_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>
#include <vector>
#include <list>
#include "glm.h"
#include "math.h"
#include "StandardAR.h"

namespace hello_ar
{

    class ScannerRenderer
    {
    public:
        ScannerRenderer() = default;

        ~ScannerRenderer() = default;

        // Sets up OpenGL state.  Must be called on the OpenGL
        // thread and before any other methods below.
        void InitializeGlContent();
        void DrawTest(const glm::mat4 &projection_mat,
                                       const glm::mat4 &view_mat, const glm::mat4 &model_mat,
                                       float light_intensity);
        void Draw(const glm::mat4& projection_mat, const glm::mat4& view_mat, const glm::mat4 &model_mat,
                  float light_intensity);
        void Clear();
        void StartScanGuidance();
        void StopScanGuidance();

        int GetScanProgress();
        void GetBoundingBox(float* vertex);
        void OnTouched(float x, float y);
        void OnSingleTouched(float x, float y);
        void OnDoubleTouched(float x, float y);
        short GetFrontBoxPlaneIndex();
        void OnTouchedMove(const ARSession* ar_session, const ARWorldMap* ar_map, float x, float y, glm::mat4& out_scale_mat, glm::mat4& out_mat);
        void ResetTouched(glm::mat4& out_mat);
        void SetFov(float fov, float screen_aspect);
        bool IsNearPlane();

    private:
        GLuint shader_program_;
        GLuint attribute_position_;
        GLuint uniform_mvp_mat_;
        GLuint uniform_alpha_float_;
		bool enable_scan_guidence_;
        int m_progress;
        glm::mat4 m_model_mat;
        glm::mat4 m_view_mat;
        float m_fov;
        float m_aspect;
        short m_touch_plane_index;
        bool m_bScale;
        bool m_bTranslate;
        glm::vec3 m_PreBoxCenter;
        glm::vec3 m_PreScalePos;
        glm::vec3 m_PreTouch;
        bool m_isNearToPlane;
        bool m_isAlreadyNearToPlane;
        float m_NearPlaneY;
    };
}


#endif  // TANGO_GL_VIDEO_OVERLAY_H_
