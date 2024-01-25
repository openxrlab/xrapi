
#ifndef STANDARD_AR_ARHANDGESTURE_C_API_H_
#define STANDARD_AR_ARHANDGESTURE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include "StandardARCommon.h"
#include "ArTrackable.h"
#include "HandGestureAlgorithmResult.h"


namespace standardar
{
    class CHandGesture : public ITrackable
    {
    public:
        int32_t m_hand_id; //id
        int32_t m_gesture_type; // gesture type
        float m_gesture_type_confidence;    //gesture type confidence
        int32_t m_side; // 左右手判断 1：左手 0：右手
        int32_t m_towards;   //  手掌朝向：0：朝内 1：朝外 2：侧面
        float m_palm_center[3]; //palm center xyz
        float m_palm_normal[3]; //palm normal xyz
        int32_t m_rect_top; //bbox rect top
        int32_t m_rect_left;    //bbox rect left
        int32_t m_rect_right;   //bbox rect right
        int32_t m_rect_bottom;  //bbox rect bottom
        float m_landmark_2D[2*20];  //landmark 2d
        float m_landmark_3D[3*21];  //landmark 3d

        int32_t m_landmark2d_count;  //number of landmark 2d
        int32_t m_landmark3d_count;  //number of landmark 3d

        int32_t m_rgb_seg_width;
        int32_t m_rgb_seg_height;
        char* m_rgb_seg_array;

//        int32_t m_depth_seg_width;
//        int32_t m_depth_seg_height;
//        char* m_rgb_depth_array;

        ARHandGestureNode_* m_Handle;


    public:
        CHandGesture(int32_t handGestureId);
        virtual ~CHandGesture();
        int32_t getHandGestureType(HandGestureAlgorithmResult* result);
        int32_t getHandSide(HandGestureAlgorithmResult* result);
        int32_t getHandToward(HandGestureAlgorithmResult* result);
        float_t getHandTypeConfidence(HandGestureAlgorithmResult* result);
        int32_t getLandMark2DCount(HandGestureAlgorithmResult* result);
        void getLandMark2DArray(HandGestureAlgorithmResult* result,float* out_landmark2d_array);


    };

}

#endif //STANDARD_AR_ARHANDGESTURE_C_API_H_
