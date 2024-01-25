#include <malloc.h>
#include "ShaderUtils.h"
#include "log.h"

void StandardAR_CheckGlError(const char* operation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", operation, error);
    }
}

static GLuint StandardAR_LoadShader(GLenum shader_type, const char* shader_source) {
    GLuint shader = glCreateShader(shader_type);
    if (!shader) {
        return shader;
    }

    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint info_len = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if (!info_len) {
            return shader;
        }

        char* buf = reinterpret_cast<char*>(malloc(info_len));
        if (!buf) {
            return shader;
        }

        glGetShaderInfoLog(shader, info_len, nullptr, buf);
        LOGI("Could not compile shader %d:\n%s\n", shader_type, buf);
        free(buf);
        glDeleteShader(shader);
        shader = 0;
    }

    return shader;
}

GLuint StandardAR_CreateProgram(const char* vertex_source, const char* fragment_source) {
    GLuint vertexShader = StandardAR_LoadShader(GL_VERTEX_SHADER, vertex_source);
    if (!vertexShader) {
        return 0;
    }

    GLuint fragment_shader = StandardAR_LoadShader(GL_FRAGMENT_SHADER, fragment_source);
    if (!fragment_shader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        StandardAR_CheckGlError("standardar::glAttachShader");
        glAttachShader(program, fragment_shader);
        StandardAR_CheckGlError("standardar::glAttachShader");
        glLinkProgram(program);
        GLint link_status = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &link_status);
        if (link_status != GL_TRUE) {
            GLint buf_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_length);
            if (buf_length) {
                char* buf = reinterpret_cast<char*>(malloc(buf_length));
                if (buf) {
                    glGetProgramInfoLog(program, buf_length, nullptr, buf);
                    LOGI("standardar::Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    } else{
        LOGI("glCreateProgram failed...");
    }
    return program;
}

