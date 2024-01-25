#include "ArCamera.h"
namespace standardar
{
    CCameraConfig::CCameraConfig()
    {
        m_Width = 1280;
        m_Height = 720;
    }


    CCameraConfig::CCameraConfig(int32_t width, int32_t height)
    {
        m_Width = width;
        m_Height = height;
    }


    CCameraConfig::~CCameraConfig()
    {

    }

    CCameraConfigList::CCameraConfigList()
    {
    }


    CCameraConfigList::~CCameraConfigList()
    {

    }

    CCameraIntrinsics::CCameraIntrinsics()
    {
        m_FocalLengthX = 0.0f;
        m_FocalLengthY = 0.0f;
        m_PrincipalPTX = 0.0f;
        m_PrincipalPTY = 0.0f;
        m_ImageWidth = 0;
        m_ImageHeight = 0;
        m_Distortion = nullptr;

        m_FocalLengthSlvX = 0.0f;
        m_FocalLengthSlvY = 0.0f;
        m_PrincipalPTSlvX = 0.0f;
        m_PrincipalPTSlvY = 0.0f;
        m_ImageWidthSlv = 0;
        m_ImageHeightSlv = 0;
        m_Distortion = nullptr;
    }


    CCameraIntrinsics::~CCameraIntrinsics()
    {

    }

}
