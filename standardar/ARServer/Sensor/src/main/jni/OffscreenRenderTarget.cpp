#include "OffscreenRenderTarget.h"
#include "ShaderUtils.h"

namespace standardar
{

    COffscreenRenderTarget::COffscreenRenderTarget()
            : m_iContextWidth(512)
            , m_iContextHeight(512)
            , m_pFbo(NULL)
            , m_pDepth(NULL)
            , m_pColor(0)
            , m_uCurrentFbo(0)
    {
    }

    COffscreenRenderTarget::~COffscreenRenderTarget()
    {
        CleanUp();
    }

    void COffscreenRenderTarget::CleanUp()
    {

        StandardAR_SAFE_DELETE(m_pFbo);
        StandardAR_SAFE_DELETE(m_pDepth);
    }

    bool COffscreenRenderTarget::IsViewportSizeChanged(int width, int height)
    {
        if ((int32_t )m_iContextWidth != width || (int32_t )m_iContextHeight != height) {
            return true;
        }
        else {
            return false;
        }
    }

    bool COffscreenRenderTarget::InitializeWithExt(GLuint texture_id, uint32_t context_type,
                                                   uint32_t context_width, uint32_t context_height){

        CleanUp();

        m_iContextWidth = context_width;
        m_iContextHeight = context_height;


        //create the frame buffer object and bind it !
        m_pFbo = new CFrameBufferObject();
        m_pFbo->CreateID();
        m_pFbo->Bind();

        if (context_type & kCONTEXT_COLOR)
        {
            m_pColor = texture_id;
            m_pFbo->AttachTexture(GL_TEXTURE_2D, m_pColor, GL_COLOR_ATTACHMENT0, 0);
        }

        if(context_type & kCONTEXT_DEPTH)
        {
            m_pDepth = new CRenderBuffer();
            if(!m_pDepth->SetParameters(GL_DEPTH_COMPONENT16, m_iContextWidth, m_iContextHeight))
                return false;

            m_pDepth->Bind();
            m_pFbo->AttachRenderBuffer(m_pDepth->GetID(),GL_DEPTH_ATTACHMENT);
        }

        //check fbo for completeness
        if(! CFrameBufferObject::CheckStatus())
            return false;

        //disabel offscreen context
        m_pFbo->Disable();

        return true;
    }

    void COffscreenRenderTarget::Enable()
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_uCurrentFbo);
        glGetIntegerv(GL_VIEWPORT, m_vViewport);

        m_pFbo->Bind();
        m_pFbo->AttachTexture(GL_TEXTURE_2D, m_pColor, GL_COLOR_ATTACHMENT0, 0);

        glViewport(0, 0, m_iContextWidth, m_iContextHeight);
    }

    void COffscreenRenderTarget::Disable()
    {
        m_pFbo->Disable();

        glBindFramebuffer(GL_FRAMEBUFFER, m_uCurrentFbo);
        glViewport(m_vViewport[0], m_vViewport[1], m_vViewport[2], m_vViewport[3]);
    }

    uint32_t COffscreenRenderTarget::GetColorTexId() const
    {
        return m_pColor;
    }
}