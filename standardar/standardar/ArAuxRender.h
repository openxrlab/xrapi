#ifndef STANDARD_AR_ARAUXRENDER_C_API_H_
#define STANDARD_AR_ARAUXRENDER_C_API_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>
#include <vector>


namespace standardar
{
    class CAuxRender
    {
    public:
        CAuxRender();
        ~CAuxRender();

        // Initialize the GL content, needs to be called on GL thread.
        void InitializeGlContent();

        // Render the AR point cloud.
        //
        // @param mvp_matrix, the model view projection matrix of point cloud.
        // @param ar_session, the session that is used to query point cloud points
        //     from ar_point_cloud.
        // @param ar_point_cloud, point cloud data to for rendering.
        void DrawFeatures(float aspect, const float *featurepoint, int feature_count) const;

        void DrawAxis(float *mvp) const;

        void DrawLandMark(const float *pt_data, int pt_count, float *mvp)const;

    protected:


    private:
        GLuint pt_shader_program_;
        GLuint pt_attribute_vertices_;
        GLuint pt_uniform_mvp_mat_;
        GLuint pt_uniform_ptcolor_;
        GLuint pt_uniform_ptsize_;

        GLuint axis_shader_program_;
        GLuint axis_attribute_position_;
        GLuint axis_attribute_color_;
        GLuint axis_uniform_mvp_mat_;
    };
}
#endif
