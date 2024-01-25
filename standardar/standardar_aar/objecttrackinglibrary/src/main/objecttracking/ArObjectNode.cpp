#include "ArObjectNode.h"
#include "stdlib.h"
#include "ArMath.h"
#include "ArFrame.h"
#include "ObjectTrackingAlgorithmResult.h"

namespace standardar
{
    CObjectNode::~CObjectNode() {
        if(m_buffer) {
            free(m_buffer);
            m_buffer = nullptr;
        }
    }

    CObjectNode::CObjectNode(int id, const char* name): ITrackable(ARNODE_TYPE_OBJECT){
        m_ObjectId = id;
        m_Name = name;
        m_bufferSize = 0;
        m_buffer = nullptr;
    }

    CObjectNode::CObjectNode(CObjectNode* object): ITrackable(ARNODE_TYPE_OBJECT){
        m_ObjectId = object->m_ObjectId;
        m_Name = object->m_Name;
        m_bufferSize = object->m_bufferSize;
        if(object->m_buffer != nullptr) {
            m_buffer = (unsigned char *) malloc(m_bufferSize);
            memcpy(m_buffer, object->m_buffer, m_bufferSize);
        }
    }

    void CObjectNode::getCenterPose(ObjectTrackingAlgorithmResult* result, CPose &out_pose) {
        result->getCenterPose(m_ObjectId, out_pose);
    }

    int32_t CObjectNode::getIndex()
    {
        return m_ObjectId;
    }

    float CObjectNode::getExtentX(ObjectTrackingAlgorithmResult* result)
    {
        return result->getExtentX(m_ObjectId);
    }

    float CObjectNode::getExtentY(ObjectTrackingAlgorithmResult* result)
    {
        return result->getExtentY(m_ObjectId);
    }

    float CObjectNode::getExtentZ(ObjectTrackingAlgorithmResult* result)
    {
        return result->getExtentZ(m_ObjectId);
    }

    std::string& CObjectNode::getName()
    {
        return m_Name;
    }

    void CObjectNode::getBoundingBox(ObjectTrackingAlgorithmResult* result, float* boundingBox)
    {
        result->getBoundingBox(m_ObjectId, boundingBox);
    }
}
