
#include "ArBgYUVRender.h"
#include "ArCommon.h"
#include <string.h>

namespace standardar
{

    GLfloat quadVerticesArray[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    GLfloat quadTexCoordsArray[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };


    GLfloat flipVerticesArray[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    GLfloat flipTexCoordsArray[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    short quadIndicesArray[] = {
        0, 1, 2,
        2, 3, 0
    };

    constexpr char VERTEX_SHADER[] = R"(attribute vec4 vertexPosition;
                                   attribute vec2 vertexTexCoord;
                                   varying vec2 texCoord;
                                   void main() {
                                   gl_Position = vertexPosition;
                                   texCoord = vertexTexCoord;
                                   })";

    constexpr char YUV_NV21_FRAGMENT_SHADER[] = R"(uniform sampler2D videoFrameY;
                                     uniform sampler2D videoFrameUV;
                                     varying lowp vec2 texCoord;
                                     const lowp mat3 M = mat3( 1, 1, 1, 0, -.18732, 1.8556, 1.57481, -.46813, 0 );
                                     void main() {
                                     lowp vec3 yuv;
                                     lowp vec3 rgb;
                                     yuv.x = texture2D(videoFrameY, texCoord).r;
                                     yuv.yz = texture2D(videoFrameUV, texCoord).ar - vec2(0.5, 0.5);
                                     rgb = M * yuv;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";

    constexpr char YUV_NV12_FRAGMENT_SHADER[] = R"(uniform sampler2D videoFrameY;
                                     uniform sampler2D videoFrameUV;
                                     varying lowp vec2 texCoord;
                                     const lowp mat3 M = mat3( 1, 1, 1, 0, -.18732, 1.8556, 1.57481, -.46813, 0 );
                                     void main() {
                                     lowp vec3 yuv;
                                     lowp vec3 rgb;
                                     yuv.x = texture2D(videoFrameY, texCoord).r;
                                     yuv.yz = texture2D(videoFrameUV, texCoord).ra - vec2(0.5, 0.5);
                                     rgb = M * yuv;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";

    constexpr char YUV_I420_FRAGMENT_SHADER[] = R"(uniform sampler2D videoFrameY;
                                     uniform sampler2D videoFrameU;
                                     uniform sampler2D videoFrameV;
                                     varying lowp vec2 texCoord;
                                     const lowp mat3 M = mat3( 1, 1, 1, 0, -.18732, 1.8556, 1.57481, -.46813, 0 );
                                     void main() {
                                     lowp vec3 yuv;
                                     lowp vec3 rgb;
                                     yuv.x = texture2D(videoFrameY, texCoord).r;
                                     yuv.y = texture2D(videoFrameU, texCoord).r - 0.5;
                                     yuv.z = texture2D(videoFrameV, texCoord).r - 0.5;
                                     rgb = M * yuv;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";

    constexpr char YUV_YV12_FRAGMENT_SHADER[] = R"(uniform sampler2D videoFrameY;
                                     uniform sampler2D videoFrameU;
                                     uniform sampler2D videoFrameV;
                                     varying lowp vec2 texCoord;
                                     const lowp mat3 M = mat3( 1, 1, 1, 0, -.18732, 1.8556, 1.57481, -.46813, 0 );
                                     void main() {
                                     lowp vec3 yuv;
                                     lowp vec3 rgb;
                                     yuv.x = texture2D(videoFrameY, texCoord).r;
                                     yuv.y = texture2D(videoFrameV, texCoord).r - 0.5;
                                     yuv.z = texture2D(videoFrameU, texCoord).r - 0.5;
                                     rgb = M * yuv;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";




    constexpr char vs_fullscreen_shader[] = R"(attribute vec4 vertexPosition;
                                   attribute vec2 vertexTexCoord;
                                   varying vec2 texCoord;
                                   void main() {
                                   gl_Position = vertexPosition;
                                   texCoord = vertexTexCoord;
                                   })";

    constexpr char ps_fullscreen_shader[] = R"(uniform sampler2D rgbTex;
                                     varying lowp vec2 texCoord;
                                     void main() {
                                     lowp vec3 rgb;
                                     rgb = texture2D(rgbTex, texCoord).rgb;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";

    CBgYUVRender::CBgYUVRender(ARPixelFormat format):m_ImageFormat(format)
    {
        yuvShaderID = 0;
        yuvVertexHandle = 0;
        yuvTexCoordHandle = 0;
        yuvYUniformHandle = 0;
        yuvUUniformHandle = 0;
        yuvVUniformHandle = 0;

        yuvTextureYID = 0;
        yuvTextureUID = 0;
        yuvTextureVID = 0;

        fullscreenShaderID = 0;
        fullscreenVertexHandle = 0;
        fullscreenTexCoordHandle = 0;
        fullscreenRGBTexHandle = 0;
    }

    CBgYUVRender::~CBgYUVRender()
    {
        SAFE_DELETE_PROGRAM(yuvShaderID);
        SAFE_DELETE_PROGRAM(fullscreenShaderID);

        SAFE_DELETE_TEXTURE(yuvTextureYID);
        SAFE_DELETE_TEXTURE(yuvTextureUID);
        SAFE_DELETE_TEXTURE(yuvTextureVID);
    }

    void CBgYUVRender::InitializeGlContent() {

        glGenTextures(1, &yuvTextureYID);
        glGenTextures(1, &yuvTextureUID);
        glGenTextures(1, &yuvTextureVID);

        if (m_ImageFormat == ARPIXEL_FORMAT_YUV_I420) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER, YUV_I420_FRAGMENT_SHADER);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameU");
            yuvVUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_YV12) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER, YUV_YV12_FRAGMENT_SHADER);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameU");
            yuvVUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_NV12) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER, YUV_NV12_FRAGMENT_SHADER);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameUV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_NV21) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER, YUV_NV21_FRAGMENT_SHADER);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameUV");
        }

        yuvVertexHandle = glGetAttribLocation(yuvShaderID, "vertexPosition");
        yuvTexCoordHandle = glGetAttribLocation(yuvShaderID, "vertexTexCoord");
        yuvYUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameY");

        fullscreenShaderID = StandardAR_CreateProgram(vs_fullscreen_shader, ps_fullscreen_shader);
        fullscreenVertexHandle = glGetAttribLocation(fullscreenShaderID, "vertexPosition");
        fullscreenTexCoordHandle = glGetAttribLocation(fullscreenShaderID, "vertexTexCoord");
        fullscreenRGBTexHandle = glGetUniformLocation(fullscreenShaderID, "rgbTex");
    }

    void CBgYUVRender::DrawYUV() {
        glDepthFunc(GL_LEQUAL);

        glUseProgram(yuvShaderID);

        glVertexAttribPointer(yuvVertexHandle, 3, GL_FLOAT, false, 0, quadVerticesArray);
        glVertexAttribPointer(yuvTexCoordHandle, 2, GL_FLOAT, false, 0, quadTexCoordsArray);

        glEnableVertexAttribArray(yuvVertexHandle);
        glEnableVertexAttribArray(yuvTexCoordHandle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, yuvTextureYID);
        glUniform1i(yuvYUniformHandle, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, yuvTextureUID);
        glUniform1i(yuvUUniformHandle, 1);

        if(m_ImageFormat == ARPIXEL_FORMAT_YUV_I420 || m_ImageFormat == ARPIXEL_FORMAT_YUV_YV12) {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, yuvTextureVID);
            glUniform1i(yuvVUniformHandle, 2);
        }


        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadIndicesArray);

        glDisableVertexAttribArray(yuvVertexHandle);
        glDisableVertexAttribArray(yuvTexCoordHandle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    void CBgYUVRender::DrawFullScreen(GLuint bgtexid)
    {
        glDepthFunc(GL_LEQUAL);

        glUseProgram(fullscreenShaderID);

        glVertexAttribPointer(fullscreenVertexHandle, 3, GL_FLOAT, false, 0, quadVerticesArray);
        glVertexAttribPointer(fullscreenTexCoordHandle, 2, GL_FLOAT, false, 0, quadTexCoordsArray);

        glEnableVertexAttribArray(fullscreenVertexHandle);
        glEnableVertexAttribArray(fullscreenTexCoordHandle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgtexid);
        glUniform1i(fullscreenRGBTexHandle, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, quadIndicesArray);

        glDisableVertexAttribArray(fullscreenVertexHandle);
        glDisableVertexAttribArray(fullscreenTexCoordHandle);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

}
