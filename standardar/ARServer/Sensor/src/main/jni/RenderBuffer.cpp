#include "RenderBuffer.h"
#include "log.h"

namespace standardar
{

    CRenderBuffer::CRenderBuffer()
    :m_uID(GenerateId()),m_iSaveID(0)
    {
    }

    CRenderBuffer::~CRenderBuffer()
    {
        if (glIsRenderbuffer(m_uID)) {
            glDeleteRenderbuffers(1,&m_uID);
        }
    }

    void CRenderBuffer::Bind()
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_uID);
    }

    void CRenderBuffer::UnBind()
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    bool CRenderBuffer::SetParameters(GLenum internalFormat, int32_t  width, int32_t height)
    {
        int maxSize = CRenderBuffer::GetMaximumSize();
        if (width > maxSize || height > maxSize )
        {
#ifdef DEBUG
            LOGE("Renderbuffer::SetParameters() ERROR:Size too big %i, %i", width, height);
#endif
            
            return false;
        }
        _GuardedBind();
        // Allocate memory for renderBuffer
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height );
        _GuardedUnbind();
        
        return true;
        
    }

    bool CRenderBuffer::_GuardedBind()
    {
        // Only binds if m_render buffer id  is different than the currently bound to render buffer
        glGetIntegerv( GL_RENDERBUFFER_BINDING, &m_iSaveID );
        if (m_uID != (GLuint)m_iSaveID) {
            Bind();
            return true;
        } else return false;
    }
    
    bool CRenderBuffer::_GuardedUnbind()
    {
        
        if (m_uID != (GLuint)m_iSaveID) {
            glBindRenderbuffer(GL_RENDERBUFFER, (GLuint)m_iSaveID);
            return true;
        } return false;
    }

    int32_t CRenderBuffer::GetMaximumSize()
    {
        GLint maxAttach = 0;
        glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &maxAttach );
        return maxAttach;
    }

    GLuint CRenderBuffer::GenerateId()
    {
        GLuint id = 0;
        glGenRenderbuffers(1, &id);
        return id;
    }

}