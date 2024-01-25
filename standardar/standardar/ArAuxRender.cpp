#include "ArAuxRender.h"
#include "ArMath.h"
#include "ArCommon.h"

namespace standardar {


    constexpr char vs_colorpoint_shader[] = R"(
    attribute vec4 vertex;
    uniform mat4 mvp;
    uniform vec4 ptcolor;
    uniform float ptsize;
    varying mediump vec4 vertex_color;
    void main() {
      gl_PointSize = ptsize;
      vertex_color = ptcolor;
      gl_Position = mvp * vec4(vertex.xyz, 1.0);
    })";

    constexpr char ps_colorpoint_shader[] = R"(
    precision lowp float;
    varying mediump vec4 vertex_color;
    void main() {
      gl_FragColor = vertex_color;
    })";

    constexpr char vs_axis_shader[] =
            R"(attribute vec3 position;
                attribute vec4 color;
                varying mediump vec4 vertex_color;
                uniform mat4 mvp;
                void main(void)
                {
                    vertex_color = color;
                    gl_Position = mvp * vec4(position,1.0);
                })";

    constexpr char ps_axis_shader[] =
            R"(precision mediump float;
                varying mediump vec4 vertex_color;
                void main(void)
                {
                    gl_FragColor = vertex_color;
                })";

    const GLfloat axis3d_vertex[] =
            {
                    0, 0, 0,
                    1, 0, 0,

                    0, 0, 0,
                    0, 1, 0,

                    0, 0, 0,
                    0, 0, 1
            };

    const GLfloat axis3d_color[] =
            {
                    1.0f, 0.0f, 0.0f, 1.0f,
                    1.0f, 0.0f, 0.0f, 1.0f,

                    0.0f, 1.0f, 0.0f, 1.0f,
                    0.0f, 1.0f, 0.0f, 1.0f,

                    0.0f, 0.0f, 1.0f, 1.0f,
                    0.0f, 0.0f, 1.0f, 1.0f
            };

    CAuxRender::CAuxRender()
    {
        pt_shader_program_ = 0;
        pt_attribute_vertices_ = 0;
        pt_uniform_mvp_mat_ = 0;
        pt_uniform_ptcolor_ = 0;
        pt_uniform_ptsize_ = 0;

        axis_shader_program_ = 0;
        axis_attribute_position_ = 0;
        axis_attribute_color_ = 0;
        axis_uniform_mvp_mat_ = 0;
    }

    CAuxRender::~CAuxRender()
    {
        SAFE_DELETE_PROGRAM(pt_shader_program_);
        SAFE_DELETE_PROGRAM(axis_shader_program_);
    }

    void CAuxRender::InitializeGlContent() {
        pt_shader_program_ = StandardAR_CreateProgram(vs_colorpoint_shader, ps_colorpoint_shader);
        if(pt_shader_program_==0)
            LOGE("pt_shader_program_ is empty!!!");

        pt_attribute_vertices_ = glGetAttribLocation(pt_shader_program_, "vertex");
        pt_uniform_mvp_mat_ = glGetUniformLocation(pt_shader_program_, "mvp");
        pt_uniform_ptcolor_ = glGetUniformLocation(pt_shader_program_, "ptcolor");
        pt_uniform_ptsize_ = glGetUniformLocation(pt_shader_program_, "ptsize");


        axis_shader_program_ = StandardAR_CreateProgram(vs_axis_shader, ps_axis_shader);
        axis_attribute_position_ = glGetAttribLocation(axis_shader_program_, "position");
        axis_attribute_color_ = glGetAttribLocation(axis_shader_program_, "color");
        axis_uniform_mvp_mat_ = glGetUniformLocation(axis_shader_program_, "mvp");

        StandardAR_CheckGlError("point_cloud_renderer::InitializeGlContent()");
    }

    void CAuxRender::DrawFeatures(float aspect, const float *featurepoint, int pt_count) const {
        if(pt_shader_program_ == 0)
            return;

        glUseProgram(pt_shader_program_);

        if (pt_count <= 0) {
            return;
        }

        mat4f projmat = mat4f::createOrtho(-aspect, aspect, -1, 1, -100, 100);
        glUniformMatrix4fv(pt_uniform_mvp_mat_, 1, GL_FALSE, projmat.data);

        vec4f ptcolor(1.0, 1.0, 0.0, 1.0);
        glUniform4fv(pt_uniform_ptcolor_, 1, (float*)ptcolor);
        glUniform1f(pt_uniform_ptsize_, 10.0f);
        glUniformMatrix4fv(pt_uniform_mvp_mat_, 1, GL_FALSE, projmat.data);

        glEnableVertexAttribArray(pt_attribute_vertices_);
        glVertexAttribPointer(pt_attribute_vertices_, 3, GL_FLOAT, GL_FALSE, 0, featurepoint);

        glDrawArrays(GL_POINTS, 0, pt_count);

        glUseProgram(0);
        StandardAR_CheckGlError("PointCloudRenderer::DrawFeatures");
    }

    void CAuxRender::DrawAxis(float *mvp) const {
        if(axis_shader_program_==0)
            return;

        glUseProgram(axis_shader_program_);

        glUniformMatrix4fv(axis_uniform_mvp_mat_, 1, GL_FALSE, mvp);

        glEnableVertexAttribArray(axis_attribute_position_);
        glVertexAttribPointer(axis_attribute_position_, 3, GL_FLOAT, GL_FALSE, 0, axis3d_vertex);

        glEnableVertexAttribArray(axis_attribute_color_);
        glVertexAttribPointer(axis_attribute_color_, 4, GL_FLOAT, GL_FALSE, 0, axis3d_color);

        glDrawArrays(GL_LINES, 0, 6);

        glUseProgram(0);
        StandardAR_CheckGlError("PointCloudRenderer::DrawAxis");
    }

    void CAuxRender::DrawLandMark(const float *pt_data, int pt_count, float *mvp)const
    {
        if(pt_shader_program_==0)
            return;

        glUseProgram(pt_shader_program_);

        if (pt_count <= 0) {
            return;
        }

        glUniformMatrix4fv(axis_uniform_mvp_mat_, 1, GL_FALSE, mvp);

        vec4f ptcolor(1.0, 0.0, 0.0, 1.0);
        glUniform4fv(pt_uniform_ptcolor_, 1, (float*)ptcolor);
        glUniform1f(pt_uniform_ptsize_, 5.0f);

        glEnableVertexAttribArray(pt_attribute_vertices_);
        glVertexAttribPointer(pt_attribute_vertices_, 4, GL_FLOAT, GL_FALSE, 0, pt_data);

        glDrawArrays(GL_POINTS, 0, pt_count);

        glUseProgram(0);
        StandardAR_CheckGlError("PointCloudRenderer::DrawLandMark");
    }

}