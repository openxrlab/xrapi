#include "ArBodyDetctNode.h"
#include "BodyDetectAlgorithmResult.h"
namespace standardar
{



    CBodyDetectNode::CBodyDetectNode(int32_t id)  :ITrackable(ARNODE_TYPE_BODY_DETECT),m_BodyId(id){
    }


    CBodyDetectNode::~CBodyDetectNode() {

        if(m_joints2d_array!= nullptr){
            delete[] m_joints2d_array;
            m_joints2d_array = nullptr;
        }

        if(m_joints2d_confidence!= nullptr){
            delete[] m_joints2d_confidence;
            m_joints2d_confidence = nullptr;
        }

        if(m_joints3d_array!= nullptr){
            delete[] m_joints3d_array;
            m_joints3d_array = nullptr;
        }

        if(m_joints3d_confidence!= nullptr){
            delete[] m_joints3d_confidence;
            m_joints3d_confidence = nullptr;
        }
    }


    int32_t CBodyDetectNode::getBodyId() {
        return m_BodyId;
    }


    void CBodyDetectNode::updateJoints2dArray( float *joints2d_array) {
        memcpy(m_joints2d_array,joints2d_array, sizeof(float)*m_joints2d_count*2);
    }

    void CBodyDetectNode::updateJoints2dConfidence(float *joints2d_confidence) {
        memcpy(m_joints2d_confidence,joints2d_confidence, sizeof(float)*m_joints2d_count);

    }

    void CBodyDetectNode::updateJoints3dArray(float *joints3d_array) {
        memcpy(m_joints3d_array,joints3d_array, sizeof(float)*m_joints3d_count*3);

    }

    void CBodyDetectNode::updateJoints3dConfidence(float *joints3d_confidence) {
        memcpy(m_joints3d_confidence,joints3d_confidence, sizeof(float)*m_joints3d_count);

    }

    void CBodyDetectNode::getSkeletonPoint2d(standardar::BodyDetectAlgorithmResult* result,float *out_point2d) {
        result->getSkeletonPoint2d(m_BodyId,out_point2d);
    }

    void CBodyDetectNode::getSkeletonPoint2dConfidence(
            standardar::BodyDetectAlgorithmResult *result, float *out_point2d_confidence) {
        result->getSkeletonPoint2dConfidence(m_BodyId,out_point2d_confidence);

    }

    void CBodyDetectNode::getSkeletonPoint3d(float *out_point3d) {
        memcpy(out_point3d,m_joints3d_array, sizeof(float)*m_joints3d_count*3);
    }


    void CBodyDetectNode::getSkeletonPoint3dConfidence(float *out_point3d_confidence) {
        memcpy(out_point3d_confidence,m_joints3d_confidence, sizeof(float)*m_joints3d_count);

    }

    void CBodyDetectNode::setSkeletonPointCount(int32_t pointCount) {
        m_joints2d_count = pointCount;
    }

    /*int32_t CBodyDetectNode::getSkeletonPointCount() {

        return m_joints2d_count;

    }*/

    int32_t CBodyDetectNode::getSkeletonPoint2dCount(standardar::BodyDetectAlgorithmResult* result) {
        return result->getSkeletonPoint2dCount(m_BodyId);
    }

    int32_t CBodyDetectNode::getSkeletonPoint3dCount() {
        return m_joints3d_count;
    }



}