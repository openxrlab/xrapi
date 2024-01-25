#ifndef STANDARD_AR_FRAMEBUFFEROBJECT_C_API_H_
#define STANDARD_AR_FRAMEBUFFEROBJECT_C_API_H_


#include <GLES2/gl2.h>

namespace standardar
{

    /** opengl frame buffer object, this is generally used to perfrom offscreen rendering (shadow mapping, reflections, post processing)
     create frame buffer object, attach a render target to the frame buffer. all drawing commands are performed on the frame buffer attachment point.
     */
    class CFrameBufferObject
    {
        
    public:
        /** default constructor*/
        CFrameBufferObject();
        
        /** destructor*/
        ~CFrameBufferObject();
        
        /** attach a render buffer to the frame buffer object*/
        void AttachRenderBuffer(GLuint buffID,GLenum attachment);
        
        protected :
        /** generate a unique GL ID for the frame buffer,
         @return GL ID
         */
        GLuint GenerateId();
        
    public:
        
        /** call this to create the buffer ID */
        bool CreateID();
        
        /** activate frame buffer object */
        void Bind();
        
        /** disable frame buffer object*/
        void Disable();
        
        /** Attach texture as an offscreen render target to the frame buffer,
         * @param texture the texture to attach
         * @param attachment attachment point ( can be color or depth attachment), by default frame buffer doesnt have a depth buffer attached, in order to get depth test u should attach a depth texture or depth render buffer. attaching a buffer as render target is generally fast, but if we want to process the output of the frame buffer later by a shader its better to attach texture, plz note that on iOS devices u cant attach depth textures.
         * @param mipLevel
         */
        void AttachTexture(GLenum target, GLuint textureid, GLenum attachment = GL_COLOR_ATTACHMENT0, int mipLevel = 0);
        
        /** test frame buffer completeness, call this to verify that ur frame buffer is setup correctly and that all attachment points are attached correctly. return true if the frame buffer status is complete. use this outside of the rendering loop
         */
        static bool CheckStatus();
        
    private:
        
        /** not allowed*/
        CFrameBufferObject(const CFrameBufferObject &p);
        
        /** not allowed*/
        CFrameBufferObject& operator=(CFrameBufferObject const& p);
        
        /** prevent the frame buffer from being binded twice*/
        bool _GuardedBind();
        
        /** prevent unbind twice*/
        bool _GuardedUnbind();

    private:

        /** frame buffer ID returned by generate ID*/
        GLuint m_uID;

        /** this is used for guarded bind ( we dont want to bind the same buffer twice)*/
        GLint m_uSaveID;
    };
}

#endif
