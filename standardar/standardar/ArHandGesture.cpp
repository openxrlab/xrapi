#include "ArHandGesture.h"
#include "StandardAR.h"



namespace standardar
{
    CHandGesture::CHandGesture(int32_t handGestureId):ITrackable(ARNODE_TYPE_HAND_GESTURE),
                                 m_hand_id(handGestureId),
                                 m_gesture_type(-1),
                                 m_side(-1),
                                 m_towards(-1),
                                 m_palm_center{0},
                                 m_palm_normal{0},
                                 m_rect_top(0),
                                 m_rect_left(0),
                                 m_rect_right(0),
                                 m_rect_bottom(0),
                                 m_landmark_2D{},
                                 m_landmark_3D{},
                                 m_landmark2d_count(0),
                                 m_landmark3d_count(0),
                                 m_Handle(NULL)
    {

    }

    CHandGesture::~CHandGesture() {

    }

    int32_t CHandGesture::getHandSide(standardar::HandGestureAlgorithmResult *result) {

        return result->getHandSide(m_hand_id);
    }


    int32_t CHandGesture::getHandGestureType(
            standardar::HandGestureAlgorithmResult * result) {
        return result->getHandGestureType(m_hand_id);
    }

    int32_t CHandGesture::getHandToward(standardar::HandGestureAlgorithmResult *result) {
        return result->getHandToward(m_hand_id);
    }



    float_t CHandGesture::getHandTypeConfidence(standardar::HandGestureAlgorithmResult *result) {

        return result->getHandTypeConfidence(m_hand_id);

    }

    int32_t CHandGesture::getLandMark2DCount(standardar::HandGestureAlgorithmResult *result) {
        return result->getLandMark2DCount(m_hand_id);

    }

    void CHandGesture::getLandMark2DArray(standardar::HandGestureAlgorithmResult *result,
                                          float *out_landmark2d_array) {
        result->getLandMark2DArray(m_hand_id,out_landmark2d_array);

    }
}

