#ifndef STANDARD_AR_OFFSCREENRENDERTARGET_C_API_H_
#define STANDARD_AR_OFFSCREENRENDERTARGET_C_API_H_

#include "FrameBufferObject.h"
#include "RenderBuffer.h"

namespace standardar
{

    enum CONTEXT_TYPE
    {
        /** rgb */
                kCONTEXT_COLOR = 0x01,

        /** depth context*/
                kCONTEXT_DEPTH = 0x02,

        /** stencil ( not implemented )*/
                kCONTEXT_STENCIL = 0x04,
    };

    class COffscreenRenderTarget
    {
    public :

        /** constructor create an offscreen render target
         */
        COffscreenRenderTarget();

        /** destructor*/
        ~COffscreenRenderTarget();

        bool InitializeWithExt(GLuint texture_id, uint32_t context_type = kCONTEXT_COLOR | kCONTEXT_DEPTH,
                               uint32_t context_width = 512, uint32_t context_height = 512);

        bool IsViewportSizeChanged(int width, int height);

        /** return the attached color buffer*/
        uint32_t GetColorTexId() const;

        /** activate context ( all drawing commands will be executed into this context*/
        void Enable();

        /** disable context and switch back to default context ( the default context is one bound before Enable was called, this is retrieved via glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_uCurrentFbo); for better performance, it is better to remove the glGet from the rendering loop ( the case when u know the fbo ID )*/
        void Disable();
    private:

        /** not permitted*/
        COffscreenRenderTarget(const COffscreenRenderTarget& sm);
        COffscreenRenderTarget& operator=(const COffscreenRenderTarget& sm);

        void CleanUp();

    private:

        uint32_t m_iContextWidth, m_iContextHeight;

        CFrameBufferObject* m_pFbo;
        CRenderBuffer* m_pDepth;

        GLuint m_pColor;

        /** current bound fbo */
        int32_t m_uCurrentFbo;

        /** default viewport*/
        int32_t m_vViewport[4];
    };

}

#endif
