
#ifndef STANDARD_AR_AROBJECTNODE_C_API_H_
#define STANDARD_AR_AROBJECTNODE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>
#include <ArMath.h>

#include "ArPose.h"
#include "ArTrackable.h"

namespace standardar
{
    class ObjectTrackingAlgorithmResult;
    class CObjectNode: public ITrackable
    {
    public:
        CObjectNode(int id, const char* name);
        CObjectNode(CObjectNode* object);
        void getCenterPose(ObjectTrackingAlgorithmResult* result, CPose& out_pose);
        int32_t getIndex();
        float getExtentX(ObjectTrackingAlgorithmResult* result);
        float getExtentY(ObjectTrackingAlgorithmResult* result);
        float getExtentZ(ObjectTrackingAlgorithmResult* result);
        std::string& getName();
        void getBoundingBox(ObjectTrackingAlgorithmResult* result, float* boundingBox);
        ~CObjectNode();
        friend class CReferenceObjectDatabase;

    private:
        int32_t     m_ObjectId;
        std::string m_Name;
        int         m_bufferSize;
        unsigned char* m_buffer;
    };

}


#endif  // STANDARD_AR_AROBJECTNODE_C_API_H_
