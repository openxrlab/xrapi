#ifndef BODY_DETECT_C_SRC_BODY_RENDER_H
#define BODY_DETECT_C_SRC_BODY_RENDER_H


#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdlib>

#include "util.h"

class BodyRenderer {
public:
    BodyRenderer() = default;
    ~BodyRenderer() = default;

    // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
    // other methods below.
    void InitializeGlContent();

    // Draws the background image.  This methods must be called for every ArFrame
    // returned by ArSession_update() to catch display geometry change events.

    void DrawBody(float *landmarks);


private:
    void DrawLine(float *landmarks, int count, float * color);
    void DrawPoint(float *landmarks, int count,float * color);



private:
    GLuint mPointProgram = 0;

};

#endif //BODY_DETECT_C_SRC_BODY_RENDER_H
