
#include "ArBgYUVRenderVAO.h"
#include "ArCommon.h"
#include <string.h>

namespace standardar
{

    GLfloat quadVerticesArrayVAO[] = {
        // vertex            // texture
        -1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        +1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, +1.0f, 0.0f,  0.0f, 0.0f,
        +1.0f, +1.0f, 0.0f,  1.0f, 0.0f,
    };

    short quadIndicesArrayVAO[] = {
            0, 1, 2,
            1, 2, 3
    };

    constexpr char VERTEX_SHADER_VAO[] = R"(attribute vec4 vertexPosition;
                                   attribute vec2 vertexTexCoord;
                                   varying vec2 texCoord;
                                   void main() {
                                   gl_Position = vertexPosition;
                                   texCoord = vertexTexCoord;
                                   })";

    constexpr char YUV_NV21_FRAGMENT_SHADER_VAO[] = R"(uniform sampler2D videoFrameY;
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

    constexpr char YUV_NV12_FRAGMENT_SHADER_VAO[] = R"(uniform sampler2D videoFrameY;
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

    constexpr char YUV_I420_FRAGMENT_SHADER_VAO[] = R"(uniform sampler2D videoFrameY;
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

    constexpr char YUV_YV12_FRAGMENT_SHADER_VAO[] = R"(uniform sampler2D videoFrameY;
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




    constexpr char vs_fullscreen_shader_VAO[] = R"(attribute vec4 vertexPosition;
                                   attribute vec2 vertexTexCoord;
                                   varying vec2 texCoord;
                                   void main() {
                                   gl_Position = vertexPosition;
                                   texCoord = vertexTexCoord;
                                   })";

    constexpr char ps_fullscreen_shader_VAO[] = R"(uniform sampler2D rgbTex;
                                     varying lowp vec2 texCoord;
                                     void main() {
                                     lowp vec3 rgb;
                                     rgb = texture2D(rgbTex, texCoord).rgb;
                                     gl_FragColor = vec4(rgb,1.0);
                                     })";

    CBgYUVRenderVAO::CBgYUVRenderVAO(ARPixelFormat format):m_ImageFormat(format)
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

    CBgYUVRenderVAO::~CBgYUVRenderVAO()
    {
        SAFE_DELETE_PROGRAM(yuvShaderID);
        SAFE_DELETE_PROGRAM(fullscreenShaderID);

        SAFE_DELETE_TEXTURE(yuvTextureYID);
        SAFE_DELETE_TEXTURE(yuvTextureUID);
        SAFE_DELETE_TEXTURE(yuvTextureVID);
    }

    void CBgYUVRenderVAO::InitializeGlContent() {

        glGenTextures(1, &yuvTextureYID);
        glGenTextures(1, &yuvTextureUID);
        glGenTextures(1, &yuvTextureVID);

        if (m_ImageFormat == ARPIXEL_FORMAT_YUV_I420) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER_VAO, YUV_I420_FRAGMENT_SHADER_VAO);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameU");
            yuvVUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_YV12) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER_VAO, YUV_YV12_FRAGMENT_SHADER_VAO);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameU");
            yuvVUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_NV12) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER_VAO, YUV_NV12_FRAGMENT_SHADER_VAO);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameUV");
        }
        else if (m_ImageFormat == ARPIXEL_FORMAT_YUV_NV21) {
            yuvShaderID = StandardAR_CreateProgram(VERTEX_SHADER_VAO, YUV_NV21_FRAGMENT_SHADER_VAO);
            yuvUUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameUV");
        }

        yuvVertexHandle = glGetAttribLocation(yuvShaderID, "vertexPosition");
        yuvTexCoordHandle = glGetAttribLocation(yuvShaderID, "vertexTexCoord");
        yuvYUniformHandle = glGetUniformLocation(yuvShaderID, "videoFrameY");

        fullscreenShaderID = StandardAR_CreateProgram(vs_fullscreen_shader_VAO, ps_fullscreen_shader_VAO);
        fullscreenVertexHandle = glGetAttribLocation(fullscreenShaderID, "vertexPosition");
        fullscreenTexCoordHandle = glGetAttribLocation(fullscreenShaderID, "vertexTexCoord");
        fullscreenRGBTexHandle = glGetUniformLocation(fullscreenShaderID, "rgbTex");

        //// vao_yuv init
        glGenVertexArrays(1, &vao_yuv);
        glGenBuffers(1, &vbo_yuv);
        glGenBuffers(1, &ebo_yuv);
        glBindVertexArray(vao_yuv);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_yuv);
        glBufferData(GL_ARRAY_BUFFER, 5 * 4 * sizeof(GLfloat), &quadVerticesArrayVAO[0], GL_STATIC_DRAW);
        glVertexAttribPointer(yuvVertexHandle, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(yuvTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(yuvVertexHandle);
        glEnableVertexAttribArray(yuvTexCoordHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_yuv);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLushort), &quadIndicesArrayVAO[0], GL_STATIC_DRAW);
        glBindVertexArray(0);

        //// vao_fullscreen init
        glGenVertexArrays(1, &vao_fullscreen);
        glGenBuffers(1, &vbo_fullscreen);
        glGenBuffers(1, &ebo_fullscreen);
        glBindVertexArray(vao_fullscreen);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_fullscreen);
        glBufferData(GL_ARRAY_BUFFER, 5 * 4 * sizeof(GLfloat), &quadVerticesArrayVAO[0], GL_STATIC_DRAW);
        glVertexAttribPointer(fullscreenVertexHandle, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(fullscreenTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(fullscreenVertexHandle);
        glEnableVertexAttribArray(fullscreenTexCoordHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_fullscreen);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLushort), &quadIndicesArrayVAO[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    void CBgYUVRenderVAO::DrawYUV() {
        glDepthFunc(GL_LEQUAL);

        glUseProgram(yuvShaderID);

        glBindVertexArray(vao_yuv);

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

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)(0));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void CBgYUVRenderVAO::DrawFullScreen(GLuint bgtexid)
    {
        glDepthFunc(GL_LEQUAL);

        glUseProgram(fullscreenShaderID);

        glBindVertexArray(vao_fullscreen);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgtexid);
        glUniform1i(fullscreenRGBTexHandle, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

}
