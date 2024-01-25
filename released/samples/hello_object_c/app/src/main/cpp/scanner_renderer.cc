#include "scanner_renderer.h"
#include <string.h>
#include "util.h"
#include <vector>
#include <time.h>


namespace hello_ar {

    constexpr char vs_scanner_shader[] =
            R"(attribute vec3 position;
                uniform mat4 mvp;
                void main(void)
                {
                    gl_Position = mvp * vec4(position,1);
                })";

    constexpr char ps_scanner_shader[] =
            R"(precision mediump float;
                uniform float alpha;
                void main(void)
                {
                    gl_FragColor = vec4(1.0, 1.0, 0.0, alpha);
                })";

    const GLfloat cube_edge_vertex[] =
            {

                    -1, 0, -1,
                    -1, 0, 1,
                    1, 0, 1,
                    1, 0, -1,
                    -1, 2, -1,
                    -1, 2, 1,
                    1, 2, 1,
                    1, 2, -1,
            };

    const GLfloat extent_cube_edge_vertex_offset[] =
            {
                    0.0f,  0.0f, 0.0f,
                    -0.05f,  0.0f, 0.0f,
                    0.0f, -0.05f, 0.0f,
                    0.0f,  0.0f, -0.05f,

                    0.0f,  0.0f, 0.0f,
                    -0.05f,  0.0f, 0.0f,
                    0.0f, -0.05f, 0.0f,
                    0.0f,  0.0f, 0.05f,

                    0.0f,  0.0f, 0.0f,
                    0.05f,  0.0f, 0.0f,
                    0.0f, -0.05f, 0.0f,
                    0.0f,  0.0f, 0.05f,

                    0.0f,  0.0f, 0.0f,
                    0.05f,  0.0f, 0.0f,
                    0.0f, -0.05f, 0.0f,
                    0.0f,  0.0f, -0.05f,

                    0.0f,  0.0f,  0.0f,
                    -0.05f,  0.0f,  0.0f,
                    0.0f,  0.05f,  0.0f,
                    0.0f,  0.0f, -0.05f,

                    0.0f,  0.0f, 0.0f,
                    -0.05f,  0.0f, 0.0f,
                    0.0f, 0.05f, 0.0f,
                    0.0f,  0.0f, 0.05f,

                    0.0f,  0.0f, 0.0f,
                    0.05f,  0.0f, 0.0f,
                    0.0f,  0.05f, 0.0f,
                    0.0f,  0.0f, 0.05f,

                    0.0f,  0.0f, 0.0f,
                    0.05f,  0.0f, 0.0f,
                    0.0f,  0.05f, 0.0f,
                    0.0f,  0.0f, -0.05f
            };

    GLfloat extent_cube_edge_vertex[8][12];

    short extent_translate_cube_edge_index[6][16] =
            {
                    {0, 1, 0, 2, 16, 17, 18, 16, 29, 28, 30, 28, 12, 13, 12, 14},
                    {7, 4, 4, 6, 23, 20, 20, 22, 16, 19, 18, 16, 0, 3, 0, 2},
                    {4, 5, 4, 6, 20, 21, 22, 20, 25, 24, 26, 24, 9, 8, 8, 10},
                    {11, 8, 8, 10, 27, 24, 26, 24, 28, 31, 30, 28, 12, 15, 12, 14},
                    {20, 21, 23, 20, 16, 17, 16, 19, 29, 28, 28, 31, 25, 24, 27, 24},
                    {4, 5, 7, 4, 0, 1, 0, 3, 13, 12, 12, 15, 9, 8, 11, 8}
            };

    short extent_scale_cube_edge_index[6][8] =
            {
                    {0, 3, 16, 19, 28, 31, 12, 15},
                    {4, 5, 20, 21, 16, 17, 0, 1},
                    {4, 7, 20, 23, 24, 27, 8, 11},
                    {8, 9, 24, 25, 28, 29, 12, 13},
                    {16, 18, 20, 22, 24, 26, 28, 30},
                    {0, 2, 4, 6, 8, 10, 12, 14},
            };

    float vertex[6][48];
    //按照由下至上，由左往右顺序确定顶点，每个小方块用"\"方向切割为两块小三角形，由直角边开始瞬时针依次建立索引
    const short index[108] = {
            0, 4, 4, 1, 1, 0,
            5, 1, 1, 4, 4, 5,
            1, 5, 5, 2, 2, 1,
            6, 2, 2, 5, 5, 6,
            2, 6, 6, 3, 3, 2,
            7, 3, 3, 6, 6, 7,
            4, 8, 8, 5, 5, 4,
            9, 5, 5, 8, 8, 9,
            5, 9, 9, 6, 6, 5,
            10, 6, 6, 9, 9, 10,
            6, 10, 10, 7, 7, 6,
            11, 7, 7, 10, 10, 11,
            8, 12, 12, 9, 9, 8,
            13, 9, 9, 12, 12, 13,
            14, 10, 10, 13, 13, 14,
            9, 13, 13, 10, 10, 9,
            10, 14, 14, 11, 11, 10,
            15, 11, 11, 14, 14, 15
    };

    const short plane_index[6] = {
            0, 12, 3,
            15, 3, 12
    };

    //按照顺序 前右后左上下 创造六个面的顶点
    void create_ghost_cube_vertex(float xStart, float xEnd, float yStart, float yEnd, float zStart,
                                  float zEnd) {
        float deltaX = xEnd - xStart;
        float deltaY = yEnd - yStart;
        float deltaZ = zEnd - zStart;
        float xStep = deltaX / 3;
        float yStep = deltaY / 3;
        float zStep = deltaZ / 3;
        //第一面
        for (int i = 0; i < 16; ++i) {
            vertex[0][i * 3] = xStart + i % 4 * xStep;
            vertex[0][i * 3 + 1] = yStart + i / 4 * yStep;
            vertex[0][i * 3 + 2] = zStart;
        }
        //第二面
        for (int i = 0; i < 16; ++i) {
            vertex[1][i * 3] = xStart;
            vertex[1][i * 3 + 1] = yStart + i / 4 * yStep;
            vertex[1][i * 3 + 2] = zStart + i % 4 * zStep;
        }
        //第三面
        for (int i = 0; i < 16; ++i) {
            vertex[2][i * 3] = xStart + i % 4 * xStep;
            vertex[2][i * 3 + 1] = yStart + i / 4 * yStep;
            vertex[2][i * 3 + 2] = zEnd;
        }
        //第四面
        for (int i = 0; i < 16; ++i) {
            vertex[3][i * 3] = xEnd;
            vertex[3][i * 3 + 1] = yStart + i / 4 * yStep;
            vertex[3][i * 3 + 2] = zStart + i % 4 * zStep;
        }
        //第五面
        for (int i = 0; i < 16; ++i) {
            vertex[4][i * 3] = xStart + i % 4 * xStep;
            vertex[4][i * 3 + 1] = yEnd;
            vertex[4][i * 3 + 2] = zStart + i / 4 * zStep;
        }
        //第六面
        for (int i = 0; i < 16; ++i) {
            vertex[5][i * 3] = xStart + i % 4 * xStep;
            vertex[5][i * 3 + 1] = yStart;
            vertex[5][i * 3 + 2] = zStart + i / 4 * zStep;
        }

    }

    const short cube_edge_index[] =
            {
                    0, 1,
                    1, 2,
                    2, 3,
                    3, 0,
                    4, 5,
                    5, 6,
                    6, 7,
                    7, 4,
                    0, 4,
                    1, 5,
                    2, 6,
                    3, 7
            };

    GLfloat cube_thick_edge_vertex[8][24];
    const GLfloat cube_thick_edge_offset[] =
            {
                    -0.001f, -0.001f, -0.001f,
                    -0.001f, -0.001f,  0.001f,
                    0.001f, -0.001f,  0.001f,
                    0.001f, -0.001f, -0.001f,
                    -0.001f,  0.001f, -0.001f,
                    -0.001f,  0.001f,  0.001f,
                    0.001f,  0.001f,  0.001f,
                    0.001f,  0.001f, -0.001f,
            };


    short cube_thick_edge_index[12][8];
    short cube_thick_edge_draw_index[432];

    void create_ghost_cube_thick_line()
    {
        for(int i=0; i<2; i++) {
            //0-1/4-5
            cube_thick_edge_index[i*4][0] = (short)((i*4) * 8 + 0);
            cube_thick_edge_index[i*4][1] = (short)((i*4) * 8 + 4);
            cube_thick_edge_index[i*4][2] = (short)((i*4) * 8 + 7);
            cube_thick_edge_index[i*4][3] = (short)((i*4) * 8 + 3);
            cube_thick_edge_index[i*4][4] = (short)((i*4+1) * 8 + 1);
            cube_thick_edge_index[i*4][5] = (short)((i*4+1) * 8 + 5);
            cube_thick_edge_index[i*4][6] = (short)((i*4+1) * 8 + 6);
            cube_thick_edge_index[i*4][7] = (short)((i*4+1) * 8 + 2);

            //1-2/5-6
            cube_thick_edge_index[i*4 + 1][0] = (short)((i*4+1) * 8 + 1);
            cube_thick_edge_index[i*4 + 1][1] = (short)((i*4+1) * 8 + 5);
            cube_thick_edge_index[i*4 + 1][2] = (short)((i*4+1) * 8 + 4);
            cube_thick_edge_index[i*4 + 1][3] = (short)((i*4+1) * 8 + 0);
            cube_thick_edge_index[i*4 + 1][4] = (short)((i*4+2) * 8 + 2);
            cube_thick_edge_index[i*4 + 1][5] = (short)((i*4+2) * 8 + 6);
            cube_thick_edge_index[i*4 + 1][6] = (short)((i*4+2) * 8 + 7);
            cube_thick_edge_index[i*4 + 1][7] = (short)((i*4+2) * 8 + 3);

            //2-3/6-7
            cube_thick_edge_index[i*4 + 2][0] = (short)((i*4+2) * 8 + 2);
            cube_thick_edge_index[i*4 + 2][1] = (short)((i*4+2) * 8 + 6);
            cube_thick_edge_index[i*4 + 2][2] = (short)((i*4+2) * 8 + 5);
            cube_thick_edge_index[i*4 + 2][3] = (short)((i*4+2) * 8 + 1);
            cube_thick_edge_index[i*4 + 2][4] = (short)((i*4+3) * 8 + 3);
            cube_thick_edge_index[i*4 + 2][5] = (short)((i*4+3) * 8 + 7);
            cube_thick_edge_index[i*4 + 2][6] = (short)((i*4+3) * 8 + 4);
            cube_thick_edge_index[i*4 + 2][7] = (short)((i*4+3) * 8 + 0);

            //3-0/7-4
            cube_thick_edge_index[i*4 + 3][0] = (short)((i*4+3) * 8 + 3);
            cube_thick_edge_index[i*4 + 3][1] = (short)((i*4+3) * 8 + 7);
            cube_thick_edge_index[i*4 + 3][2] = (short)((i*4+3) * 8 + 6);
            cube_thick_edge_index[i*4 + 3][3] = (short)((i*4+3) * 8 + 2);
            cube_thick_edge_index[i*4 + 3][4] = (short)((i*4) * 8 + 0);
            cube_thick_edge_index[i*4 + 3][5] = (short)((i*4) * 8 + 4);
            cube_thick_edge_index[i*4 + 3][6] = (short)((i*4) * 8 + 5);
            cube_thick_edge_index[i*4 + 3][7] = (short)((i*4) * 8 + 1);
        }

        //0-4/1-5/2-6/3-7
        for(int i=0; i<4; ++i)
        {
            cube_thick_edge_index[8+i][0] = (short)(i * 8 + 4);
            cube_thick_edge_index[8+i][1] = (short)(i * 8 + 5);
            cube_thick_edge_index[8+i][2] = (short)(i * 8 + 6);
            cube_thick_edge_index[8+i][3] = (short)(i * 8 + 7);
            cube_thick_edge_index[8+i][4] = (short)((i+4) * 8 + 0);
            cube_thick_edge_index[8+i][5] = (short)((i+4) * 8 + 1);
            cube_thick_edge_index[8+i][6] = (short)((i+4) * 8 + 2);
            cube_thick_edge_index[8+i][7] = (short)((i+4) * 8 + 3);
        }


        for(int i=0; i<12; ++i){
            cube_thick_edge_draw_index[i*36] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 1] = cube_thick_edge_index[i][1];
            cube_thick_edge_draw_index[i*36 + 2] = cube_thick_edge_index[i][2];
            cube_thick_edge_draw_index[i*36 + 3] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 4] = cube_thick_edge_index[i][2];
            cube_thick_edge_draw_index[i*36 + 5] = cube_thick_edge_index[i][3];

            cube_thick_edge_draw_index[i*36 + 6] = cube_thick_edge_index[i][4];
            cube_thick_edge_draw_index[i*36 + 7] = cube_thick_edge_index[i][5];
            cube_thick_edge_draw_index[i*36 + 8] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 9] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 10] = cube_thick_edge_index[i][5];
            cube_thick_edge_draw_index[i*36 + 11] = cube_thick_edge_index[i][1];

            cube_thick_edge_draw_index[i*36 + 12] = cube_thick_edge_index[i][4];
            cube_thick_edge_draw_index[i*36 + 13] = cube_thick_edge_index[i][5];
            cube_thick_edge_draw_index[i*36 + 14] = cube_thick_edge_index[i][7];
            cube_thick_edge_draw_index[i*36 + 15] = cube_thick_edge_index[i][7];
            cube_thick_edge_draw_index[i*36 + 16] = cube_thick_edge_index[i][5];
            cube_thick_edge_draw_index[i*36 + 17] = cube_thick_edge_index[i][6];

            cube_thick_edge_draw_index[i*36 + 18] = cube_thick_edge_index[i][7];
            cube_thick_edge_draw_index[i*36 + 19] = cube_thick_edge_index[i][6];
            cube_thick_edge_draw_index[i*36 + 20] = cube_thick_edge_index[i][3];
            cube_thick_edge_draw_index[i*36 + 21] = cube_thick_edge_index[i][3];
            cube_thick_edge_draw_index[i*36 + 22] = cube_thick_edge_index[i][6];
            cube_thick_edge_draw_index[i*36 + 23] = cube_thick_edge_index[i][2];

            cube_thick_edge_draw_index[i*36 + 24] = cube_thick_edge_index[i][5];
            cube_thick_edge_draw_index[i*36 + 25] = cube_thick_edge_index[i][1];
            cube_thick_edge_draw_index[i*36 + 26] = cube_thick_edge_index[i][6];
            cube_thick_edge_draw_index[i*36 + 27] = cube_thick_edge_index[i][6];
            cube_thick_edge_draw_index[i*36 + 28] = cube_thick_edge_index[i][1];
            cube_thick_edge_draw_index[i*36 + 29] = cube_thick_edge_index[i][2];

            cube_thick_edge_draw_index[i*36 + 30] = cube_thick_edge_index[i][4];
            cube_thick_edge_draw_index[i*36 + 31] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 32] = cube_thick_edge_index[i][7];
            cube_thick_edge_draw_index[i*36 + 33] = cube_thick_edge_index[i][7];
            cube_thick_edge_draw_index[i*36 + 34] = cube_thick_edge_index[i][0];
            cube_thick_edge_draw_index[i*36 + 35] = cube_thick_edge_index[i][3];
        }
    }

    float M_FLOAT_MAX = 10000000000;

    bool scanned_surface[6][9];

    void ScannerRenderer::InitializeGlContent() {
        shader_program_ = util::CreateProgram(vs_scanner_shader, ps_scanner_shader);
        attribute_position_ = glGetAttribLocation(shader_program_, "position");
        uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "mvp");
        uniform_alpha_float_ = glGetUniformLocation(shader_program_, "alpha");

        util::CheckGlError("DebugViewRenderer::InitializeGlContent()");
        create_ghost_cube_vertex(-1, 1, 0, 2, -1, 1);
        create_ghost_cube_thick_line();
        enable_scan_guidence_ = false;
        m_touch_plane_index = -1;
        m_bTranslate = false;
        m_bScale = false;
        m_isNearToPlane = false;
        m_isAlreadyNearToPlane = true;
    }

    glm::vec3 calTriangleIntersectPoint(glm::vec4 &vert0, glm::vec4 &vert1, glm::vec4 &vert2,
                                        glm::vec4 &lineVector, glm::vec4 &linePoint,
                                        bool &infinit) {

        float det, inv_det, u, v, t;
        glm::vec3 edge1, edge2, pvec, tvec, qvec, returnResult;
        edge1.x = vert1.x - vert0.x;
        edge1.y = vert1.y - vert0.y;
        edge1.z = vert1.z - vert0.z;
        edge2.x = vert2.x - vert0.x;
        edge2.y = vert2.y - vert0.y;
        edge2.z = vert2.z - vert0.z;


        pvec.x = lineVector.y * edge2.z - lineVector.z * edge2.y;
        pvec.y = lineVector.z * edge2.x - lineVector.x * edge2.z;
        pvec.z = lineVector.x * edge2.y - lineVector.y * edge2.x;

        det = pvec.x * edge1.x + pvec.y * edge1.y + pvec.z * edge1.z;

        if (det < 0.000001 && det > -0.000001) {
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        inv_det = 1.0 / det;

        tvec.x = linePoint.x - vert0.x;
        tvec.y = linePoint.y - vert0.y;
        tvec.z = linePoint.z - vert0.z;

        u = tvec.x * pvec.x + tvec.y * pvec.y + tvec.z * pvec.z;
        u *= inv_det;
        if (u < 0.0 || u > 1.0) {
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        qvec.x = tvec.y * edge1.z - tvec.z * edge1.y;
        qvec.y = tvec.z * edge1.x - tvec.x * edge1.z;
        qvec.z = tvec.x * edge1.y - tvec.y * edge1.x;

        v = lineVector.x * qvec.x + lineVector.y * qvec.y + lineVector.z * qvec.z;
        v *= inv_det;
        if (v < 0.0 || u + v > 1.0) {
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;

            return returnResult;
        }

        t = edge2.x * qvec.x + edge2.y * qvec.y + edge2.z * qvec.z;
        t *= inv_det;

        returnResult.x = linePoint.x + lineVector.x * t;
        returnResult.y = linePoint.y + lineVector.y * t;
        returnResult.z = linePoint.z + lineVector.z * t;

        infinit = true;
        return returnResult;
    }

    glm::vec3 CalPlaneLineIntersectPoint(glm::vec3 planeNormal, glm::vec3 planePoint, glm::vec4 lineVector, glm::vec4 linePoint, bool& infinit)
    {
        glm::vec3 returnResult;
        float vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t,vpt;

        vp1 = planeNormal.x;
        vp2 = planeNormal.y;
        vp3 = planeNormal.z;

        n1 = planePoint.x;
        n2 = planePoint.y;
        n3 = planePoint.z;

        v1 = lineVector.x;
        v2 = lineVector.y;
        v3 = lineVector.z;

        m1 = linePoint.x;
        m2 = linePoint.y;
        m3 = linePoint.z;

        vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;

        infinit = true;
        //首先判断直线是否与平面平行
        if (vpt == 0)
        {
            infinit = false;
            returnResult.x = M_FLOAT_MAX;
            returnResult.y = M_FLOAT_MAX;
            returnResult.z = M_FLOAT_MAX;
        }
        else
        {
            t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
            returnResult.x = m1 + v1 * t;
            returnResult.y = m2 + v2 * t;
            returnResult.z = m3 + v3 * t;
        }

        return returnResult;
    }

    glm::vec3 CalTriangleNormal(glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2)
    {
        glm::vec3 edge1, edge2;
        edge1.x = vert1.x - vert0.x;
        edge1.y = vert1.y - vert0.y;
        edge1.z = vert1.z - vert0.z;
        edge2.x = vert2.x - vert0.x;
        edge2.y = vert2.y - vert0.y;
        edge2.z = vert2.z - vert0.z;

        return glm::cross(edge2, edge1);
    }

    glm::vec3 GetPlaneNormal(short index)
    {
        switch (index){
            case 0: return glm::vec3(0,0,-1);
            case 1: return glm::vec3(-1,0,0);
            case 2: return glm::vec3(0,0,1);
            case 3: return glm::vec3(1,0,0);
            case 4: return glm::vec3(0,1,0);
            case 5: return glm::vec3(0,-1,0);
            default: return glm::vec3(0,0,0);
        }
    }

    float CalAngleBetweenVectors(glm::vec3 vert0, glm::vec3 vert1)
    {
        float length0 = glm::length(vert0);
        float length1 = glm::length(vert1);
        return acos(glm::dot(vert0, vert1) / length0 / length1) * 180.0f / 3.1415926f;
    }

    void ScannerRenderer::DrawTest(const glm::mat4 &projection_mat,
                                   const glm::mat4 &view_mat, const glm::mat4 &model_mat,
                                   float light_intensity) {
        if (!shader_program_) {
            LOGE("shader_program is null.");
            return;
        }

        glUseProgram(shader_program_);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glm::mat4 mat(1.0f);
        glm::vec3 scale(0.0885692f);
        mat = glm::scale(mat, scale);

        glm::mat4 mvp_mat = projection_mat * view_mat * model_mat * mat;
        glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));
        glEnableVertexAttribArray(attribute_position_);
        glLineWidth(3);

        //画出正方体框架
        glUniform1f(uniform_alpha_float_, 1.0f);
        glVertexAttribPointer(attribute_position_, 3, GL_FLOAT, GL_FALSE, 0, cube_edge_vertex);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, cube_edge_index);

        glDisableVertexAttribArray(attribute_position_);
        glUseProgram(0);
        glDisable(GL_BLEND);
        util::CheckGlError("obj_renderer::Draw()");
    }


    void ScannerRenderer::Draw(const glm::mat4 &projection_mat,
                               const glm::mat4 &view_mat, const glm::mat4 &model_mat,
                               float light_intensity) {
        if (!shader_program_) {
            LOGE("shader_program is null.");
            return;
        }
        m_model_mat = model_mat;
        m_view_mat = view_mat;

        glUseProgram(shader_program_);
        glEnable(GL_BLEND);
        glm::mat4 mvp_mat = projection_mat * view_mat;
        glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));
        glEnableVertexAttribArray(attribute_position_);

        //画出正方体框架
        for(int i=0; i<8; ++i){
            glm::vec4 local_vertex = glm::vec4(cube_edge_vertex[i * 3], cube_edge_vertex[i * 3 + 1], cube_edge_vertex[i * 3 + 2], 1.0f);
            glm::vec4 world_vertex = model_mat * local_vertex;
            for(int j=0; j<8; ++j) {
                cube_thick_edge_vertex[i][j * 3] = world_vertex.x + cube_thick_edge_offset[j * 3];
                cube_thick_edge_vertex[i][j * 3 + 1] = world_vertex.y + cube_thick_edge_offset[j * 3 + 1];
                cube_thick_edge_vertex[i][j * 3 + 2] = world_vertex.z + cube_thick_edge_offset[j * 3 + 2];
            }
        }

        glUniform1f(uniform_alpha_float_, 1.0f);
        glVertexAttribPointer(attribute_position_, 3, GL_FLOAT, GL_FALSE, 0, cube_thick_edge_vertex);
        glDrawElements(GL_TRIANGLES, 432, GL_UNSIGNED_SHORT, cube_thick_edge_draw_index);

        if(m_touch_plane_index >= 0) {
            //画出提示边
            for(int i=0; i<8; ++i){
                glm::vec4 local_vertex = glm::vec4(cube_edge_vertex[i * 3], cube_edge_vertex[i * 3 + 1], cube_edge_vertex[i * 3 + 2], 1.0f);
                glm::vec4 world_vertex = model_mat * local_vertex;
                for(int j=0; j<4; ++j) {
                    glm::vec4 offset_world_vertex =  glm::vec4(world_vertex.x + extent_cube_edge_vertex_offset[i*12+j*3], world_vertex.y+ extent_cube_edge_vertex_offset[i*12+j*3+1], world_vertex.z+ extent_cube_edge_vertex_offset[i*12+j*3+2], 1.0f);
                    extent_cube_edge_vertex[i][j * 3] = offset_world_vertex.x;
                    extent_cube_edge_vertex[i][j * 3 + 1] = offset_world_vertex.y;
                    extent_cube_edge_vertex[i][j * 3 + 2] = offset_world_vertex.z;
                }
            }

            glLineWidth(10);
            if(m_bTranslate) {
                glVertexAttribPointer(attribute_position_, 3, GL_FLOAT, GL_FALSE, 0, extent_cube_edge_vertex);
                glDrawElements(GL_LINES, 16, GL_UNSIGNED_SHORT, extent_translate_cube_edge_index[m_touch_plane_index]);
            }

            if(m_bScale) {
                glVertexAttribPointer(attribute_position_, 3, GL_FLOAT, GL_FALSE, 0, extent_cube_edge_vertex);
                glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, extent_scale_cube_edge_index[m_touch_plane_index]);
            }
        }

        mvp_mat = projection_mat * view_mat * model_mat;
        glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

        short now_location[2];
        if (enable_scan_guidence_) {
            //camera_vec_local参数第三位可以控制扫描距离阀值
            glm::vec4 camera_vec_local = {0, 0, -1, 1};
            glm::vec4 camera_point_local = {0, 0, 0, 1};
            glm::vec3 result_0, result_1;
            bool isFirst = true;
            bool hasFound = false;
            short location[2][2];
            for (int i = 0; i < 6; ++i) {
                for (int j = 0; j < 18; ++j) {
                    glm::vec3 vert0, vert1, vert2;
                    //找个每个小三角形的顶点index
                    int index_0 = index[j * 6 + 0];
                    int index_1 = index[j * 6 + 1];
                    int index_2 = index[j * 6 + 3];
                    //获得坐标
                    vert0.x = vertex[i][index_0 * 3 + 0];
                    vert0.y = vertex[i][index_0 * 3 + 1];
                    vert0.z = vertex[i][index_0 * 3 + 2];

                    vert1.x = vertex[i][index_1 * 3 + 0];
                    vert1.y = vertex[i][index_1 * 3 + 1];
                    vert1.z = vertex[i][index_1 * 3 + 2];

                    vert2.x = vertex[i][index_2 * 3 + 0];
                    vert2.y = vertex[i][index_2 * 3 + 1];
                    vert2.z = vertex[i][index_2 * 3 + 2];
                    //转换为世界坐标
                    glm::vec4 vert0_world = view_mat * model_mat * glm::vec4(vert0, 1);
                    glm::vec4 vert1_world = view_mat * model_mat * glm::vec4(vert1, 1);
                    glm::vec4 vert2_world = view_mat * model_mat * glm::vec4(vert2, 1);

                    //计算视角和平面是否存在交点
                    bool infinit;
                    glm::vec3 result = calTriangleIntersectPoint(vert0_world, vert1_world,
                                                                 vert2_world,
                                                                 camera_vec_local,
                                                                 camera_point_local,
                                                                 infinit);
                    if (infinit) {
                        //必定会穿过两个平面，记录穿过的两个面
                        if (isFirst) {
                            result_0 = result;
                            location[0][0] = i;
                            location[0][1] = j / 2;
                        } else {
                            hasFound = true;
                            result_1 = result;
                            location[1][0] = i;
                            location[1][1] = j / 2;
                        }
                        isFirst = !isFirst;
                    }
                }
            }

            if (hasFound) {
                //取最近的面为扫描位置
                glm::vec3 camera_point_local = {0, 0, 0};
                float distance_1 = glm::distance(result_0, camera_point_local);
                float distance_2 = glm::distance(result_1, camera_point_local);
                glm::vec4 camera_vec_local_vec4 = {0, 0, -1, 1};
                glm::vec4 camera_point_local_vec4 = {0, 0, 0, 1};
                glm::mat4 invViewMat = glm::inverse(view_mat);
                glm::vec4 camera_vec_world_vec4 = invViewMat * camera_vec_local_vec4;
                glm::vec4 camera_point_world_vec4 = invViewMat * camera_point_local_vec4;
                if (distance_1 < distance_2) {
                    glm::vec3 planeNormal = GetPlaneNormal(location[0][0]);
                    glm::vec3 dir = glm::vec3(camera_point_world_vec4.x - camera_vec_world_vec4.x, camera_point_world_vec4.y - camera_vec_world_vec4.y, camera_point_world_vec4.z - camera_vec_world_vec4.z);
                    float angle = CalAngleBetweenVectors(planeNormal, dir);
                    if((angle >=0 && angle < 60) || (angle >= 120 && angle <= 180)) {
                        if (!scanned_surface[location[0][0]][location[0][1]]) {
                            scanned_surface[location[0][0]][location[0][1]] = true;
                            m_progress++;
                        }
                    }
                    now_location[0] = location[0][0];
                    now_location[1] = location[0][1];
                } else {
                    glm::vec3 planeNormal = GetPlaneNormal(location[1][0]);;
                    glm::vec3 dir = glm::vec3(camera_point_world_vec4.x - camera_vec_world_vec4.x, camera_point_world_vec4.y - camera_vec_world_vec4.y, camera_point_world_vec4.z - camera_vec_world_vec4.z);
                    float angle = CalAngleBetweenVectors(planeNormal, dir);

                    if((angle >=0 && angle < 60) || (angle >= 120 && angle <= 180)) {
                        if (!scanned_surface[location[1][0]][location[1][1]]) {
                            scanned_surface[location[1][0]][location[1][1]] = true;
                            m_progress++;
                        }
                    }
                    now_location[0] = location[1][0];
                    now_location[1] = location[1][1];
                }
            }

            //画出每个面
            //第6个面为底面 不画
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (scanned_surface[i][j] || (now_location[0] == i && now_location[1] == j && hasFound)) {
                        if (now_location[0] == i && now_location[1] == j)
                            glUniform1f(uniform_alpha_float_, 0.75f);
                        else
                            glUniform1f(uniform_alpha_float_, 0.4f);

                        short rect[12];
                        for (int k = 0; k < 12; ++k) {
                            rect[k] = index[12 * j + k];
                        }
                        short triangle[] = {rect[0], rect[1], rect[3], rect[7], rect[9], rect[6]};
                        glVertexAttribPointer(attribute_position_, 3, GL_FLOAT, GL_FALSE, 0,
                                              vertex[i]);
                        glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, triangle);
                    }
                }
            }
        }

        glDisableVertexAttribArray(attribute_position_);
        glUseProgram(0);
        glDisable(GL_BLEND);
        util::CheckGlError("obj_renderer::Draw()");
    }

    void ScannerRenderer::Clear() {
        memset(scanned_surface,false, sizeof(bool)*54);
    }

    void ScannerRenderer::StartScanGuidance() {
        enable_scan_guidence_ = true;
        m_progress = 0;
    }

    void ScannerRenderer::StopScanGuidance() {
        enable_scan_guidence_ = false;
        m_progress = 0;
    }

    int ScannerRenderer::GetScanProgress() {
        return m_progress;
    }

    void ScannerRenderer::GetBoundingBox(float* vertex)
    {
        memcpy(vertex, cube_edge_vertex, sizeof(float) * 24);
    }

    void ScannerRenderer::OnTouched(float x, float y)
    {
        GLfloat zNear = 0.1f;
        float foyrad = m_fov * 3.1415926f / 180.0f;
        GLfloat half_tan_fov = std::tan(foyrad/2.0f);
        GLfloat halffrustumHeight = zNear * half_tan_fov;
        GLfloat halffrustumWidth = halffrustumHeight * m_aspect;

        float ratiox = 2.0f*x - 1.0f;
        float ratioy = 2.0f*y - 1.0f;

        glm::vec4 tmppt;
        tmppt.x = ratiox*halffrustumWidth;
        tmppt.y = -ratioy*halffrustumHeight;
        tmppt.z = -zNear;
        tmppt.w = 1.0f;

        //LOGI("camera fovy:%f, aspect:%f\n", m_FovY, m_Aspect);
        //LOGI("frustum:%f, %f, %f\n", half_tan_fov, halffrustumWidth, halffrustumHeight);
        //LOGI("near plane pt:%f, %f, %f\n", tmppt.x, tmppt.y, tmppt.z);

        glm::mat4 invViewMat = glm::inverse(m_view_mat);
        glm::vec4 tmppt2 = invViewMat*tmppt;

        glm::vec4 viewpt;
        viewpt.x = invViewMat[3][0];
        viewpt.y = invViewMat[3][1];
        viewpt.z = invViewMat[3][2];
        viewpt.w = 1.0f;

        glm::vec4 dirvec;
        dirvec.x = tmppt2.x-viewpt.x;
        dirvec.y = tmppt2.y-viewpt.y;
        dirvec.z = tmppt2.z-viewpt.z;
        dirvec.w = 1.0f;
        dirvec = glm::normalize(dirvec);

        glm::vec3 result_0, result_1;
        bool isFirst = true;
        bool hasFound = false;
        short location[2];
        for (int i = 0; i < 6; ++i) {
            for (int j = 0; j < 2; ++j) {
                glm::vec3 vert0, vert1, vert2;
                //找个每个小三角形的顶点index
                int index_0 = plane_index[j * 3 + 0];
                int index_1 = plane_index[j * 3 + 1];
                int index_2 = plane_index[j * 3 + 2];
                //获得坐标
                vert0.x = vertex[i][index_0 * 3 + 0];
                vert0.y = vertex[i][index_0 * 3 + 1];
                vert0.z = vertex[i][index_0 * 3 + 2];

                vert1.x = vertex[i][index_1 * 3 + 0];
                vert1.y = vertex[i][index_1 * 3 + 1];
                vert1.z = vertex[i][index_1 * 3 + 2];

                vert2.x = vertex[i][index_2 * 3 + 0];
                vert2.y = vertex[i][index_2 * 3 + 1];
                vert2.z = vertex[i][index_2 * 3 + 2];
                //转换为世界坐标
                glm::vec4 vert0_world = m_model_mat * glm::vec4(vert0, 1);
                glm::vec4 vert1_world = m_model_mat * glm::vec4(vert1, 1);
                glm::vec4 vert2_world = m_model_mat * glm::vec4(vert2, 1);

                //计算视角和平面是否存在交点
                bool infinit;
                glm::vec3 result = calTriangleIntersectPoint(vert0_world, vert1_world,
                                                             vert2_world,
                                                             dirvec,
                                                             viewpt,
                                                             infinit);
                if (infinit) {
                    //必定会穿过两个平面，记录穿过的两个面
                    if (isFirst) {
                        result_0 = result;
                        location[0] = i;
                    } else {
                        hasFound = true;
                        result_1 = result;
                        location[1] = i;
                    }
                    isFirst = !isFirst;
                }
            }
        }

        if (hasFound) {
            //取最近的面为扫描位置
            glm::vec3 camera_point_local = {viewpt.x, viewpt.y, viewpt.z};
            float distance_1 = glm::distance(result_0, camera_point_local);
            float distance_2 = glm::distance(result_1, camera_point_local);
            if (distance_1 < distance_2) {
                m_touch_plane_index = location[0];
            } else {
                m_touch_plane_index = location[1];
            }
        }
    }

    void ScannerRenderer::OnSingleTouched(float x, float y)
    {
        OnTouched(x, y);
        m_bScale = false;
        m_bTranslate = true;

        glm::vec3 box_center;
        if(m_touch_plane_index == 4 || m_touch_plane_index == 5)
            box_center = glm::vec3(0, 0, 0);
        else
            box_center = glm::vec3(0, 1, 0);
        glm::vec4 box_center_world = m_model_mat * glm::vec4(box_center, 1);
        m_PreBoxCenter = glm::vec3(box_center_world.x, box_center_world.y, box_center_world.z);
        m_PreTouch = glm::vec3(MAXFLOAT, MAXFLOAT, MAXFLOAT);
        m_NearPlaneY = MAXFLOAT;
    }

    void ScannerRenderer::OnDoubleTouched(float x, float y)
    {
        OnTouched(x, y);
        m_bScale = true;
        m_bTranslate = false;
        m_PreScalePos = glm::vec3(MAXFLOAT, MAXFLOAT, MAXFLOAT);
        m_PreTouch = glm::vec3(MAXFLOAT, MAXFLOAT, MAXFLOAT);
    }

    static float GetWithinMinAndMax(float x, float min, float max)
    {
        if(x < min)
            return min;
        else if(x>=min && x<max)
            return x;
        else
            return max;
    }

    short ScannerRenderer::GetFrontBoxPlaneIndex() {
        glm::mat4 invViewMat = glm::inverse(m_view_mat);

        glm::vec4 viewpt;
        viewpt.x = invViewMat[3][0];
        viewpt.y = invViewMat[3][1];
        viewpt.z = invViewMat[3][2];
        viewpt.w = 1.0f;

        glm::vec3 vert[4];
        glm::vec4 vert_world[4];
        int index[4];
        for(int i=0; i<4; ++i){
            index[i] = plane_index[i];
            vert[i].x = vertex[4][index[i] * 3 + 0];
            vert[i].y = vertex[4][index[i] * 3 + 1];
            vert[i].z = vertex[4][index[i] * 3 + 2];

            vert_world[i] = m_model_mat * glm::vec4(vert[i], 1);
        }

        float offset = 0.02f;
        if(viewpt.z > (vert_world[0].z - offset) && viewpt.z < (vert_world[1].z + offset))
        {
            return 1;
        }

        return 0;

    }

    void ScannerRenderer::OnTouchedMove(const ARSession* ar_session, const ARWorldMap* ar_map, float x, float y, glm::mat4& out_scale_mat, glm::mat4& out_mat)
    {
        GLfloat zNear = 0.1f;
        float foyrad = m_fov * 3.1415926f / 180.0f;
        GLfloat half_tan_fov = std::tan(foyrad/2.0f);
        GLfloat halffrustumHeight = zNear * half_tan_fov;
        GLfloat halffrustumWidth = halffrustumHeight * m_aspect;

        float ratiox = 2.0f*x - 1.0f;
        float ratioy = 2.0f*y - 1.0f;

        glm::vec4 tmppt;
        tmppt.x = ratiox*halffrustumWidth;
        tmppt.y = -ratioy*halffrustumHeight;
        tmppt.z = -zNear;
        tmppt.w = 1.0f;

        glm::mat4 invViewMat = glm::inverse(m_view_mat);
        glm::vec4 tmppt2 = invViewMat*tmppt;

        glm::vec4 viewpt;
        viewpt.x = invViewMat[3][0];
        viewpt.y = invViewMat[3][1];
        viewpt.z = invViewMat[3][2];
        viewpt.w = 1.0f;

        glm::vec4 dirvec;
        dirvec.x = tmppt2.x-viewpt.x;
        dirvec.y = tmppt2.y-viewpt.y;
        dirvec.z = tmppt2.z-viewpt.z;
        dirvec.w = 1.0f;
        dirvec = glm::normalize(dirvec);

        if(m_bTranslate) {
            glm::vec3 vert0, vert1, vert2;
            int index_0 = plane_index[0];
            int index_1 = plane_index[1];
            int index_2 = plane_index[2];
            //获得坐标
            vert0.x = vertex[m_touch_plane_index][index_0 * 3 + 0];
            vert0.y = vertex[m_touch_plane_index][index_0 * 3 + 1];
            vert0.z = vertex[m_touch_plane_index][index_0 * 3 + 2];

            vert1.x = vertex[m_touch_plane_index][index_1 * 3 + 0];
            vert1.y = vertex[m_touch_plane_index][index_1 * 3 + 1];
            vert1.z = vertex[m_touch_plane_index][index_1 * 3 + 2];

            vert2.x = vertex[m_touch_plane_index][index_2 * 3 + 0];
            vert2.y = vertex[m_touch_plane_index][index_2 * 3 + 1];
            vert2.z = vertex[m_touch_plane_index][index_2 * 3 + 2];

            glm::vec3 planeNormal = CalTriangleNormal(vert0, vert1, vert2);
            planeNormal = glm::normalize(planeNormal);
            bool infinit;
            glm::vec3 result = CalPlaneLineIntersectPoint(planeNormal, m_PreBoxCenter, dirvec, viewpt, infinit);

            if(m_PreTouch.x != MAXFLOAT && m_PreTouch.y != MAXFLOAT &&  m_PreTouch.z != MAXFLOAT) {
                out_mat = glm::translate(out_mat, result - m_PreTouch);
            }

            //判断是否靠近平面
            ARNodeList *plane_list = nullptr;
            arNodeListCreate(ar_session, &plane_list);
            CHECK(plane_list != nullptr);

            ARNodeType plane_tracked_type = ARNODE_TYPE_PLANE;

            arWorldMapGetAllNodes(ar_session, ar_map, plane_tracked_type, plane_list);

            int32_t plane_list_size = 0;
            arNodeListGetSize(ar_session, plane_list, &plane_list_size);

            bool isNearPlane = false;
            for (int i = 0; i < plane_list_size; ++i) {
                ARNode *ar_node = nullptr;
                arNodeListAcquireItem(ar_session, plane_list, i, &ar_node);
                ARPlaneNode *ar_plane = ARNodeAsARPlaneNode(ar_node);

                glm::mat4 plane_mat;
                util::ScopedArPose scopedArPose(ar_session);
                arPlaneNodeGetCenterPose(ar_session, ar_plane, scopedArPose.GetArPose());
                arPoseGetMatrix(scopedArPose.GetArPose(),
                                glm::value_ptr(plane_mat));
                float plane_y = plane_mat[3][1];
                if(abs(plane_y - out_mat[3][1]) <= 0.05) {
                    m_NearPlaneY = plane_y;
                    isNearPlane = true;
                    if(m_PreTouch.x == MAXFLOAT && m_PreTouch.y == MAXFLOAT &&  m_PreTouch.z == MAXFLOAT) {
                        m_isAlreadyNearToPlane = true;
                        break;
                    }

                    if(m_isNearToPlane)
                        m_isAlreadyNearToPlane = true;
                    m_isNearToPlane = true;
                    break;
                }
                arNodeRelease(ar_node);
            }
            arNodeListDestroy(plane_list);
            plane_list = nullptr;

            if(!isNearPlane){
                m_isNearToPlane = false;
                m_isAlreadyNearToPlane = false;
                m_NearPlaneY = MAXFLOAT;
            }

            m_PreTouch = result;
        }

        if(m_bScale){
            short next_plane_index= -1;
            glm::vec3 scale_dir(0, 0, 0);
            glm::vec3 trans_dir(0, 0, 0);
            switch (m_touch_plane_index){
                case 0:
                    next_plane_index = 5;
                    scale_dir = glm::vec3(0, 0, -1);
                    trans_dir = glm::vec3(0, 0, 1);
                    break;
                case 1:
                    next_plane_index = 5;
                    scale_dir = glm::vec3(-1, 0, 0);
                    trans_dir = glm::vec3(1, 0, 0);
                    break;
                case 2:
                    next_plane_index = 5;
                    scale_dir = glm::vec3(0, 0, 1);
                    trans_dir = glm::vec3(0, 0, 1);
                    break;
                case 3:
                    next_plane_index = 5;
                    scale_dir = glm::vec3(1, 0, 0);
                    trans_dir = glm::vec3(1, 0, 0);
                    break;
                case 4:
                    next_plane_index = GetFrontBoxPlaneIndex();
                    scale_dir = glm::vec3(0, 1, 0);
                    break;
                case 5:
                    next_plane_index = GetFrontBoxPlaneIndex();
                    scale_dir = glm::vec3(0, -1, 0);
                    break;
                default:
                    break;
            }

            glm::vec3 vert0, vert1, vert2;
            int index_0 = plane_index[0];
            int index_1 = plane_index[1];
            int index_2 = plane_index[2];
            //获得坐标
            vert0.x = vertex[next_plane_index][index_0 * 3 + 0];
            vert0.y = vertex[next_plane_index][index_0 * 3 + 1];
            vert0.z = vertex[next_plane_index][index_0 * 3 + 2];

            vert1.x = vertex[next_plane_index][index_1 * 3 + 0];
            vert1.y = vertex[next_plane_index][index_1 * 3 + 1];
            vert1.z = vertex[next_plane_index][index_1 * 3 + 2];

            vert2.x = vertex[next_plane_index][index_2 * 3 + 0];
            vert2.y = vertex[next_plane_index][index_2 * 3 + 1];
            vert2.z = vertex[next_plane_index][index_2 * 3 + 2];

            glm::vec3 planeNormal = CalTriangleNormal(vert0, vert1, vert2);
            planeNormal = glm::normalize(planeNormal);
            bool infinit;

            glm::vec4 vert0_world = m_model_mat * glm::vec4(vert0, 1);
            glm::vec3 result = CalPlaneLineIntersectPoint(planeNormal, glm::vec3(vert0_world.x, vert0_world.y, vert0_world.z), dirvec, viewpt, infinit);

            if(m_PreScalePos.x == MAXFLOAT && m_PreScalePos.y == MAXFLOAT && m_PreScalePos.z == MAXFLOAT){
                m_PreScalePos = result;
                return;
            }

            glm::vec3 scale, origin;
            origin = glm::vec3(out_scale_mat[0][0], out_scale_mat[1][1], out_scale_mat[2][2]);
            scale.x = out_scale_mat[0][0] + scale_dir.x * (result.x - m_PreScalePos.x) / 2.0f;
            scale.y = out_scale_mat[1][1] + scale_dir.y * (result.y - m_PreScalePos.y) / 2.0f;
            scale.z = out_scale_mat[2][2] + scale_dir.z * (result.z - m_PreScalePos.z) / 2.0f;

            out_scale_mat[0][0] = GetWithinMinAndMax(scale.x, 0.01f, 1.0f);
            out_scale_mat[1][1] = GetWithinMinAndMax(scale.y, 0.01f, 1.0f);
            out_scale_mat[2][2] = GetWithinMinAndMax(scale.z, 0.01f, 1.0f);

            if(m_touch_plane_index !=4 && m_touch_plane_index !=5) {
                glm::vec3 trans;
                trans.x = scale_dir.x == 0 ? 0:(out_scale_mat[0][0] - origin.x) / scale_dir.x *trans_dir.x;
                trans.y = scale_dir.y == 0 ? 0:(out_scale_mat[1][1] - origin.y) / scale_dir.y *trans_dir.y;
                trans.z = scale_dir.z == 0 ? 0:(out_scale_mat[2][2] - origin.z) / scale_dir.z *trans_dir.z;
                out_mat = glm::translate(out_mat, trans);
            }

            if(out_scale_mat[0][0] != scale.x || out_scale_mat[1][1] != scale.y || out_scale_mat[2][2] != scale.z)
                return;
            m_PreScalePos = result;
        }

    }

    void ScannerRenderer::ResetTouched(glm::mat4& out_mat)
    {
        if(m_bTranslate && m_NearPlaneY != MAXFLOAT){
            out_mat[3][1] = m_NearPlaneY;
        }

        m_bScale = false;
        m_bTranslate = false;
        m_PreTouch = glm::vec3(MAXFLOAT, MAXFLOAT, MAXFLOAT);
        m_isNearToPlane = false;
        m_isAlreadyNearToPlane = false;
        m_NearPlaneY = MAXFLOAT;
    }

    void ScannerRenderer::SetFov(float fov, float screen_aspect)
    {
        m_fov = fov;
        m_aspect = screen_aspect;
    }

    bool ScannerRenderer::IsNearPlane()
    {
        return m_isNearToPlane & !m_isAlreadyNearToPlane;
    }

}