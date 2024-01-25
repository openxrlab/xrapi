#ifndef ARSERVER_SHADER_UTILS_H
#define ARSERVER_SHADER_UTILS_H
#include <GLES2/gl2.h>

#define StandardAR_SAFE_DELETE(ptr) \
if(ptr != NULL) \
{delete ptr; ptr = NULL;}

#define SAFE_DELETE_TEXTURE(texid) \
if(texid && glIsTexture(texid)) \
{glDeleteTextures(1, &texid); texid = 0;}

#define SAFE_DELETE_PROGRAM(shaderid) \
if(shaderid && glIsProgram(shaderid)) \
{glDeleteProgram(shaderid); shaderid = 0;}

void StandardAR_CheckGlError(const char* operation);

GLuint StandardAR_CreateProgram(const char* vertex_source, const char* fragment_source);



#endif //ARSERVER_FILEUTILS_H
