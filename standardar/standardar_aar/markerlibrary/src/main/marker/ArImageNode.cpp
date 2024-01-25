#include "ArImageNode.h"
#include "stdlib.h"
#include "ArMath.h"
#include "ArFrame.h"
#include "MarkerAlgorithmResult.h"

namespace standardar
{
//    CImageNode::CImageNode()
//    {
//    }
//
    CImageNode::~CImageNode() {
        if(m_buffer) {
            free(m_buffer);
            m_buffer = nullptr;
        }
    }

    CImageNode::CImageNode(int id, const char* name, ImageNodeType type): ITrackable(ARNODE_TYPE_IMAGE){
        m_InputType = type;
        m_MarkerId = id;
        m_width = 0;
        m_height = 0;
        m_stride = 0;
        if (name) {
            m_Name = name;
        }
    }

    void CImageNode::copyImageNode(CImageNode *imageNode)
    {
        imageNode->m_GlobalId = m_GlobalId;
        imageNode->m_Score = m_Score;
        imageNode->m_height = m_height;
        imageNode->m_width = m_width;
        imageNode->m_stride = m_stride;
        imageNode->m_physicalSize = m_physicalSize;
        imageNode->m_bufferSize = m_bufferSize;
        if(m_buffer != nullptr) {
            imageNode->m_buffer = (unsigned char*)malloc(m_bufferSize);
            memcpy(imageNode->m_buffer, m_buffer, m_bufferSize);
        }
    }

    void CImageNode::getCenterPose(MarkerAlgorithmResult* result, int screen_rotate, float* viewArray,
                                          standardar::CPose &out_pose) {
        result->getCenterPose(m_MarkerId, screen_rotate, viewArray, out_pose);
    }

    int32_t CImageNode::getIndex()
    {
        return m_MarkerId;
    }

    float CImageNode::getExtentX(MarkerAlgorithmResult* result)
    {
        return result->getExtentX(m_MarkerId);
    }

    float CImageNode::getExtentY(MarkerAlgorithmResult* result)
    {
        return result->getExtentY(m_MarkerId);
    }

    float CImageNode::getExtentZ(MarkerAlgorithmResult* result)
    {
        return result->getExtentZ(m_MarkerId);
    }

    void CImageNode::get2DCorners(MarkerAlgorithmResult* result, float* corners)
    {
        result->getCorners2d(m_MarkerId, corners);
    }
}
