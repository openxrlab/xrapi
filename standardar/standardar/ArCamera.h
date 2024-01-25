
#ifndef STANDARD_AR_ARCAMERA_C_API_H_
#define STANDARD_AR_ARCAMERA_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>


namespace standardar
{
    class CCameraConfig
    {
    public:
        CCameraConfig();
        CCameraConfig(int32_t width, int32_t height);
        virtual  ~CCameraConfig();

    public:

        int32_t m_Width;
        int32_t m_Height;
    };

    class CCameraConfigList
    {
    public:
        CCameraConfigList();
        virtual  ~CCameraConfigList();
    public:
        std::vector<CCameraConfig*> m_List;
    };

    class CCameraIntrinsics
    {
    public:
        CCameraIntrinsics();
        virtual  ~CCameraIntrinsics();

    public:
        float m_FocalLengthX;
        float m_FocalLengthY;
        float m_PrincipalPTX;
        float m_PrincipalPTY;
        int32_t m_ImageWidth;
        int32_t m_ImageHeight;
        float *m_Distortion;
        float m_FocalLengthSlvX;
        float m_FocalLengthSlvY;
        float m_PrincipalPTSlvX;
        float m_PrincipalPTSlvY;
        int32_t m_ImageWidthSlv;
        int32_t m_ImageHeightSlv;
        float *m_DistortionSlv;
    };
}


#endif  // STANDARD_AR_ARCAMERA_C_API_H_
