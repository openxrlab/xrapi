
#ifndef STANDARD_AR_ARIMAGENODE_C_API_H_
#define STANDARD_AR_ARIMAGENODE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>

#include "ArPose.h"
#include "ArTrackable.h"

namespace standardar
{
    enum ImageNodeType{
        UnKnown,
        Patt,
        Image
    };
    class MarkerAlgorithmResult;
    class CImageNode: public ITrackable
    {
    public:
        CImageNode(int id, const char* name, ImageNodeType type);
        void copyImageNode(CImageNode* imageNode);
        void getCenterPose(MarkerAlgorithmResult* result, int screen_rotate, float* viewArray, CPose& out_pose);
        int32_t getIndex();
        float getExtentX(MarkerAlgorithmResult* result);
        float getExtentY(MarkerAlgorithmResult* result);
        float getExtentZ(MarkerAlgorithmResult* result);
        void  get2DCorners(MarkerAlgorithmResult* result, float* corners);
        ~CImageNode();

    public:
        ImageNodeType m_InputType;
        int32_t     m_MarkerId;
        std::string m_Name;
        std::string m_GlobalId;
        int         m_Score;
        int         m_width;
        int         m_height;
        int         m_stride;
        int         m_bufferSize;
        unsigned char* m_buffer;
        float       m_physicalSize;
    };

}


#endif  // STANDARD_AR_ARIMAGENODE_C_API_H_
