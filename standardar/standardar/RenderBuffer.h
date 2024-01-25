#ifndef STANDARD_AR_RENDERBUFFER_C_API_H_
#define STANDARD_AR_RENDERBUFFER_C_API_H_

#include <GLES2/gl2.h>

namespace standardar
{

    class CRenderBuffer
    {
    public :
        
        /** constructor*/
        CRenderBuffer();
        
        /** destructor*/
        ~CRenderBuffer();
        
        /** bind and activate render buffer*/
        void Bind();
        
        /** unbind and desactivate render buffer*/
        void UnBind();
        
        /** set render buffer format, widht and height
         * return true if success
         */
        bool SetParameters(GLenum InternalFormat,int32_t width,int32_t height);
        
        /** get gpu buffer ID*/
        inline GLuint GetID() { return m_uID;}
        
        /** static function to determine the maximum available size for the current opengl implementation*/
        static int32_t GetMaximumSize();
        
        private :
        /** not allowed*/
        CRenderBuffer(const CRenderBuffer& sm);
        CRenderBuffer& operator=(const CRenderBuffer& sm);
        
        /** generate GL id for the buffe*/
        GLuint GenerateId();
        
        /** guarded bind/unding to avoid unecessary call to opengl API*/
        bool _GuardedBind();
        bool _GuardedUnbind();


    public:
        /**the buffer gpu ID*/
        GLuint m_uID;
        
        /**used for the guarded bind*/
        GLint m_iSaveID;
        
    };

}

#endif

