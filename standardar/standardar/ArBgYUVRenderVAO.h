#ifndef STANDARD_AR_ARBGYUVRENDER_VAO_C_API_H_
#define STANDARD_AR_ARBGYUVRENDER_VAO_C_API_H_

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <cstdlib>
#include "StandardAR.h"

namespace standardar
{
    class CBgYUVRenderVAO
    {
    public:
        CBgYUVRenderVAO(ARPixelFormat format);
        ~CBgYUVRenderVAO();

        // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
        // other methods below.
        void InitializeGlContent();

        void DrawYUV();

        void DrawFullScreen(GLuint bgtexid);

        GLuint GetyuvYTextureId() const { return yuvTextureYID; }
        GLuint GetyuvUTextureId() const { return yuvTextureUID; }
        GLuint GetyuvVTextureId() const { return yuvTextureVID; }

    private:
        ARPixelFormat m_ImageFormat;

        /*
     * yuv shader handles
     */
        GLuint yuvShaderID;
        GLuint yuvVertexHandle;
        GLuint yuvTexCoordHandle;
        GLuint yuvYUniformHandle;
        GLuint yuvUUniformHandle;
        GLuint yuvVUniformHandle;
        GLuint yuvTextureYID;
        GLuint yuvTextureUID;
        GLuint yuvTextureVID;

        /*
         * Video background texture ids
         */

        GLuint fullscreenShaderID;
        GLuint fullscreenVertexHandle;
        GLuint fullscreenTexCoordHandle;
        GLuint fullscreenRGBTexHandle;

        GLuint vao_yuv;
        GLuint vbo_yuv;
        GLuint ebo_yuv;
        GLuint vao_fullscreen;
        GLuint vbo_fullscreen;
        GLuint ebo_fullscreen;
    };
}


#endif  // TANGO_GL_VIDEO_OVERLAY_H_
