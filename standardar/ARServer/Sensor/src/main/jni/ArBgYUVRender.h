#ifndef STANDARD_AR_ARBGYUVRENDER_C_API_H_
#define STANDARD_AR_ARBGYUVRENDER_C_API_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>
#include "CommonConstant.h"

namespace standardar
{
    class CBgYUVRender
    {
    public:
        CBgYUVRender(SImageFormat format);
        ~CBgYUVRender();

        // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
        // other methods below.
        void InitializeGlContent();

        void DrawYUV();

        void DrawFullScreen(GLuint bgtexid);

        GLuint GetyuvYTextureId() const { return yuvTextureYID; }
        GLuint GetyuvUTextureId() const { return yuvTextureUID; }
        GLuint GetyuvVTextureId() const { return yuvTextureVID; }

    private:
        SImageFormat m_ImageFormat;

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

        GLuint vbo;
    };
}


#endif  // TANGO_GL_VIDEO_OVERLAY_H_
