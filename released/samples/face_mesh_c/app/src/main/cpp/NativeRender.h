
#include <glm.h>
#include <GLES3/gl3.h>
#ifndef SAMPLE_C_NATIVERENDER_H
#define SAMPLE_C_NATIVERENDER_H




class NativeRender
{
public:

    NativeRender();

    virtual ~NativeRender();

    int createGlProgram();





    int drawMesh(const float* positions,int index_count,  const int* indices,const float* texcoords,glm::mat4 mvp);


    void setFaceTexture(unsigned char *imageData, int width, int height);

  //  void drawBackground(bool isFrontCamera);

   // GLuint GetTextureId() const;





private:
    GLuint mProgram;
    GLuint mTextureName;

    unsigned int VERTEX_POS_INDX=0;

    unsigned int   VERTEX_COLOR_INDX=1;
    unsigned  int COORDS_PER_VERTEX =3;
    unsigned int TEXCOORDS_PER_VERTEX =2;


};

#endif //SAMPLE_C_NATIVERENDER_H
