#ifndef BODY_DETECT_C_SRC_ARBODYDETCTNODE_H
#define BODY_DETECT_C_SRC_ARBODYDETCTNODE_H

#include "ArTrackable.h"
#include "stdlib.h"

namespace standardar
{
    class  BodyDetectAlgorithmResult;
    class CBodyDetectNode: public ITrackable{
    public:
        CBodyDetectNode(int32_t id);

        ~CBodyDetectNode();
        int32_t getBodyId();
        int32_t getSkeletonPoint2dCount(BodyDetectAlgorithmResult* result);
        int32_t getSkeletonPoint3dCount();

        void setSkeletonPointCount(int32_t pointCount);
        void updateJoints2dArray(float* joints2d_array);
        void updateJoints2dConfidence(float* joints2d_confidence);


        void updateJoints3dArray(float* joints3d_array);
        void updateJoints3dConfidence(float* joints3d_confidence);



        void getSkeletonPoint2d(BodyDetectAlgorithmResult* result,float *out_point2D);
        void getSkeletonPoint2dConfidence(BodyDetectAlgorithmResult* result,float *out_point2d_confidence);

        void getSkeletonPoint3d(float *out_point3D);

        void getSkeletonPoint3dConfidence(float *out_point3d_confidence);





    private:
        int32_t     m_BodyId;
        int32_t     m_joints2d_count;
        float *     m_joints2d_array;
        float *     m_joints2d_confidence;


        int32_t     m_joints3d_count;
        float *     m_joints3d_array;
        float *     m_joints3d_confidence;


    };




}



#endif //BODY_DETECT_C_SRC_ARBODYDETCTNODE_H
