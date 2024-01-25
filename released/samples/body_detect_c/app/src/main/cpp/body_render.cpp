#include "body_render.h"

namespace {

constexpr char VERTEX_SHADER[] =R"(
#version 300 es
in vec3 pos;
in vec2 tc;
uniform mat4 mvp;

out vec2 v_tc;

void main() {
    v_tc = tc;
    gl_Position = mvp * vec4(pos, 1.0f);
    gl_PointSize = 10.0f;
}
)";

constexpr char FRAGMENT_SHADER[] = R"(
#version 300 es
precision highp float;
uniform vec4 u_color;
out vec4 out_color;
void main() {
     out_color = u_color;
})";

}  // namespace



void BodyRenderer::InitializeGlContent() {

    mPointProgram = hello_ar::util::CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
}


void BodyRenderer::DrawPoint(float *landmarks, int count, float* color) {


    glDepthMask(GL_FALSE);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 2, landmarks, GL_STATIC_DRAW);

    GLuint posLoc = static_cast<GLuint>(glGetAttribLocation(mPointProgram, "pos"));
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posLoc);

    glBindVertexArray(0);

    glUseProgram(mPointProgram);

    GLint mvpLoc = glGetUniformLocation(mPointProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    GLint colorLoc = glGetUniformLocation(mPointProgram, "u_color");
    glUniform4fv(colorLoc,1,color);

    glBindVertexArray(vao);

    glDrawArrays(GL_POINTS, 0, count);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glDepthMask(GL_TRUE);

   // CheckGlError("drawPoints");

}

void BodyRenderer::DrawBody(float *points2d) {
    float middleLine[]={points2d[10*2],points2d[10*2+1] ,points2d[9*2],points2d[9*2+1],
                        points2d[9*2],points2d[9*2+1],points2d[8*2],points2d[8*2+1],
                        points2d[11*2],points2d[11*2+1],points2d[8*2],points2d[8*2+1],
                        points2d[14*2],points2d[14*2+1],points2d[8*2],points2d[8*2+1],
                        points2d[8*2],points2d[8*2+1],points2d[7*2],points2d[7*2+1],
                        points2d[7*2],points2d[7*2+1],points2d[0],points2d[1],
                        points2d[4*2],points2d[4*2+1],points2d[0],points2d[1],
                        points2d[1*2],points2d[1*2+1],points2d[0],points2d[1],

    };


    float leftLine[]={points2d[11*2],points2d[11*2+1] ,points2d[12*2],points2d[12*2+1],
                      points2d[12*2],points2d[12*2+1], points2d[13*2],points2d[13*2+1],
                      points2d[4*2],points2d[4*2+1], points2d[5*2],points2d[5*2+1],
                      points2d[5*2],points2d[5*2+1],points2d[6*2],points2d[6*2+1],

    };

    float rightLine[]={points2d[14*2],points2d[14*2+1] ,points2d[15*2],points2d[15*2+1],
                       points2d[15*2],points2d[15*2+1],points2d[16*2],points2d[16*2+1],
                       points2d[2],points2d[2+1],points2d[2*2],points2d[2*2+1],
                       points2d[2*2],points2d[2*2+1],points2d[3*2],points2d[3*2+1],


    };

    float redClor[] ={1.0f,0.0f,0.0f,1.0f};
    float blueClor[] ={0.0f,0.0f,1.0f,1.0f};

    float greenClor[] ={0.0f,1.0f,0.0f,1.0f};

    float yellowClor[] ={1.0f,1.0f,0.0f,1.0f};




    DrawLine(middleLine,16,redClor);

    DrawLine(leftLine,8,blueClor);
    DrawLine(rightLine,8,greenClor);


   // DrawPoint(points2d,17,yellowClor);

}


void BodyRenderer::DrawLine(float *landmarks, int count, float * color) {
    glDepthMask(GL_FALSE);
    glLineWidth(15);
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * count * 2, landmarks, GL_STATIC_DRAW);

    GLuint posLoc = static_cast<GLuint>(glGetAttribLocation(mPointProgram, "pos"));
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posLoc);

    glBindVertexArray(0);

    glUseProgram(mPointProgram);

    GLint mvpLoc = glGetUniformLocation(mPointProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

    GLint colorLoc = glGetUniformLocation(mPointProgram, "u_color");
    glUniform4fv(colorLoc,1,color);

    glBindVertexArray(vao);

    glDrawArrays(GL_LINES, 0, count);

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glDepthMask(GL_TRUE);
}