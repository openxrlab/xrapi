#include "FrameBufferObject.h"
#include "log.h"

namespace standardar
{

    CFrameBufferObject::CFrameBufferObject()
            : m_uID(0), m_uSaveID(0)
    {

    }

    CFrameBufferObject::~CFrameBufferObject()
    {
        if (glIsFramebuffer(m_uID)) {
            glDeleteFramebuffers(1, &m_uID);
        }
    }

    GLuint CFrameBufferObject::GenerateId()
    {
        m_uID = 0;
        glGenFramebuffers(1, &m_uID);
        return m_uID;
    }

    bool CFrameBufferObject::CreateID()
    {
        //this will always return true, call CheckStatus to get the completeness status of the framebuffer object.
        GenerateId();
        m_uSaveID = 0;
        return true;

#ifdef DEAD_CODE
        //FIXME: glIsFramebuffer will only work if the fbo is already bound.. http://www.khronos.org/opengles/sdk/docs/man/xhtml/glIsFramebuffer.xml
        //the code bellow should be replaced.

        if( !glIsFramebuffer(m_uID)  ) //glIsFrameBuffer will not return true if the frame buffer is not bound yet...
        {
            GenerateId();
            m_uSaveID = 0;

            Bind(); //glIsFrameBuffer will not return true if the frame buffer is not bound yet...
            if (glIsFramebuffer(m_uID))
            {
#ifdef DEBUG
                printf("CFrameBuffer: [SUCCESS] Fbo Created.\n");
#endif
                return true;
            }
            else
            {
#ifdef DEBUG
                 printf("CFrameBuffer:[ERROR] failed to create fbo.\n");
#endif
                return false;
            }
        }
        else
        {
#ifdef DEBUG
                 printf("CFrameBuffer: [WARNING] Trying to recreate an existind and valid Frame buffer object...\n");
#endif
            return true;

        }
#endif
    }

    void CFrameBufferObject::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_uID);
    }

    void CFrameBufferObject::Disable()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void CFrameBufferObject::AttachTexture(GLenum target, GLuint textureid, GLenum attachment, int mipLevel)
    {
        // _GuardedBind();
        glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, target, textureid, mipLevel );
        // _GuardedUnbind();
    }


    bool CFrameBufferObject::CheckStatus()
    {
        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch(status)
        {
            case GL_FRAMEBUFFER_COMPLETE:
#ifdef DEBUG
                LOGE("CFrameBuffer: [SUCCESS] framebuffer object is complete...\n");
#endif
                return true;

            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
#ifdef DEBUG
                LOGE("CFrameBuffer: [ERROR] framebuffer incomplete, incomplete attachment...\n");
#endif
                return false;

            case GL_FRAMEBUFFER_UNSUPPORTED:
#ifdef DEBUG
                LOGE("Unsupported framebuffer format...\n");
#endif
                return false;

            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
#ifdef DEBUG
                LOGE("CFrameBuffer: [ERROR] framebuffer incomplete, missing attachment...\n");
#endif
                return false;

            default:
#ifdef DEBUG
                LOGE("CFrameBuffer: [ERROR] framebuffer incomplete. unknown reason..");
#endif
                return false;
        }
        return false;
    }

    bool CFrameBufferObject::_GuardedBind()
    {
        // Only binds if id is different than the currently bound id
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &m_uSaveID );

        if (m_uID != (GLuint)m_uSaveID)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_uID);
            return true;
        }

        else
        {
            return false;
        }
    }

    bool CFrameBufferObject::_GuardedUnbind()
    {
        // Returns id binding to the previously enabled fbo
        if (m_uID != (GLuint)m_uSaveID)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)m_uSaveID);
            return true;
        }

        return false;
    }

    void CFrameBufferObject::AttachRenderBuffer(GLuint buffID,GLenum attachment)
    {
        _GuardedBind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,GL_RENDERBUFFER, buffID);
        _GuardedUnbind();
    }

}