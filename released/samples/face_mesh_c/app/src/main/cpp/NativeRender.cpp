#include "NativeRender.h"

#include <util.h>

#include <string>
#include <malloc.h>



static constexpr char kVertexShader[] =
        R"(#version 320 es
in vec3 pos;
in vec2 tc;

out VS_OUT {
    vec2 uv;
} vs_out;

uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(pos, 1.0f);
    vs_out.uv = tc;
}
)";

static constexpr char kGeometryShader[] =
        R"(#version 320 es
precision highp float;
layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

in VS_OUT {
    vec2 uv;
} gs_in[];

out GS_OUT {
    vec4 position;
    vec2 uv;
    vec3 dist;
} gs_out;

uniform vec3 resolution;
out vec2 g_uv;

void main() {
    vec2 p0 = resolution.xy * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
    vec2 p1 = resolution.xy * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
    vec2 p2 = resolution.xy * gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

    vec2 v0 = p2 - p1;
    vec2 v1 = p2 - p0;
    vec2 v2 = p1 - p0;

    float area = abs(v1.x * v2.y - v1.y * v2.x);

    float dist0 = area / length(v0);
    float dist1 = area / length(v1);
    float dist2 = area / length(v2);

    gs_out.position = gl_in[0].gl_Position;
    gl_Position = gl_in[0].gl_Position;
    gs_out.uv = gs_in[0].uv;
    gs_out.dist = vec3(dist0, 0, 0);
    EmitVertex();

    gs_out.position = gl_in[1].gl_Position;
    gl_Position = gl_in[1].gl_Position;
    gs_out.uv = gs_in[1].uv;
    gs_out.dist = vec3(0, dist1, 0);
    EmitVertex();

    gs_out.position = gl_in[2].gl_Position;
    gl_Position = gl_in[2].gl_Position;
    gs_out.uv = gs_in[2].uv;
    gs_out.dist = vec3(0, 0, dist2);
    EmitVertex();

    EndPrimitive();
}
)";



static constexpr char kFragmentShader[] =
        R"(#version 320 es
precision highp float;

in GS_OUT {
    vec4 position;
    vec2 uv;
    vec3 dist;
} fs_in;

out vec4 out_color;

uniform sampler2D sampler0;

void main() {

    float value = min(fs_in.dist.x, min(fs_in.dist.y, fs_in.dist.z));
    value = exp2(-1.0f / 3.0 * value * value);
    vec4 target_color = vec4(1, 1, 1, 1) * texture(sampler0, fs_in.uv);
    vec4 transparent_color = vec4(1, 1, 1, 1) * texture(sampler0, fs_in.uv);
    transparent_color.a = 0.0f;
    out_color = value * target_color + (1.0f - value) * transparent_color;
}
)";

//constexpr char VERTEX_SHADER[] = R"(attribute vec4 vertexPosition;
//                                   attribute vec2 vertexTexCoord;
//                                   varying vec2 texCoord;
//                                   void main() {
//                                   gl_Position = vertexPosition;
//                                   texCoord = vertexTexCoord;
//                                   })";
//
//constexpr char FRAGMENT_SHADER[] = R"(uniform sampler2D videoFrameY;
//                                     varying lowp vec2 texCoord;
//                                     void main() {
//                                     lowp vec3 yuv;
//                                     yuv = texture2D(videoFrameY, texCoord).rgb;
//                                     gl_FragColor = vec4(yuv,1.0);
//                                     })";



/// Class Functions Definition

NativeRender::NativeRender() :
mProgram(0),
  mTextureName(0)
{

}


NativeRender::~NativeRender()
{
    LOGI("Destruct nativerender!");
    if (mProgram > 0) { glDeleteProgram(mProgram); }
}

int NativeRender::createGlProgram()
{
//    glGenTextures(1, &texture_id_);
//    glBindTexture(GL_TEXTURE_2D, texture_id_);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//    cameraShaderID =CreateProgram(VERTEX_SHADER,NULL, FRAGMENT_SHADER);
//    cameraVertexHandle = glGetAttribLocation(cameraShaderID, "vertexPosition");
//    cameraTexCoordHandle = glGetAttribLocation(cameraShaderID, "vertexTexCoord");
//    cameraYUniformHandle = glGetUniformLocation(cameraShaderID, "videoFrameY");


    mProgram = hello_ar::util::CreateProgram(kVertexShader,kGeometryShader, kFragmentShader);
    if (mProgram == 0)
    {
        LOGE("Create Main Program ERR");
        return -1;
    }


    hello_ar::util::CheckGlError("CreateProgram");
    return 0;
}

//GLuint NativeRender::GetTextureId() const { return texture_id_; }





int NativeRender::drawMesh(const float* positions,int index_count, const int* indices,const float* texcoords,glm::mat4 mvp)
{
    if (mProgram == 0 || indices == nullptr|| (*indices) <= 0 ||(*indices)>100000  ) {return -1;}
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float resolution[2] = { (float)viewport[2], (float)viewport[3]};


    glEnable(GL_CULL_FACE);

    glUseProgram(mProgram);
    GLint matLoc = glGetUniformLocation(mProgram, "mvp");
    GLint resolutionLoc = glGetUniformLocation(mProgram, "resolution");
    glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(resolutionLoc, 1, resolution);


    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

    glEnableVertexAttribArray ( VERTEX_POS_INDX );
    glEnableVertexAttribArray ( VERTEX_COLOR_INDX );

    glVertexAttribPointer ( VERTEX_POS_INDX, COORDS_PER_VERTEX,
                            GL_FLOAT, false, 0, positions );



    if(*texcoords!=0 || *texcoords<100){
        glVertexAttribPointer ( VERTEX_COLOR_INDX, TEXCOORDS_PER_VERTEX,
                                GL_FLOAT, false, 0, texcoords );
    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,mTextureName);

    glDrawElements ( GL_TRIANGLES,index_count ,GL_UNSIGNED_INT, indices );



    glDisableVertexAttribArray ( VERTEX_POS_INDX );
    glDisableVertexAttribArray ( VERTEX_COLOR_INDX );
    glDisable(GL_CULL_FACE);

    hello_ar::util::CheckGlError("drawMesh");

    return 0;
}




void NativeRender::setFaceTexture(unsigned char *imageData, int width, int height) {
    if (mTextureName > 0) {
        glDeleteTextures(1, &mTextureName);
    }

    glGenTextures(1, &mTextureName);
    glBindTexture(GL_TEXTURE_2D, mTextureName);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    hello_ar::util::CheckGlError("set texture");
}




