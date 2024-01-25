
#ifndef STANDARD_AR_ARPLANE_C_API_H_
#define STANDARD_AR_ARPLANE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include "ArCommon.h"
#include "ArPose.h"
#include "ArTrackable.h"
#include "SLAMAlgorithmResult.h"
#include "ArMath.h"
namespace standardar{

    enum SPECIAL_PLANE_ID
    {
        SPECIAL_PLANE_ID_HORIZONTAL_PLANE = 0x0000ffff,
    };

    enum COORDINATE_AXIS {
        AXIS_X,
        AXIS_Y,
        AXIS_Z
    };

    class CPlane: public ITrackable
    {
    public:
        CPlane(int32_t planeid);
        virtual ~CPlane();

        int32_t getPlaneID(){return m_PlaneID;}


        ARPlaneType getPlaneType(SLAMAlgorithmResult* result, ArAxisUpMode upMode);
        void getPlaneNormal(SLAMAlgorithmResult* result, float* out_plane_normal);
        void getCenterPose(SLAMAlgorithmResult* result, CPose& out_pose);
        int32_t getPolygonSize(SLAMAlgorithmResult* result);
        void getPolygon(SLAMAlgorithmResult* result, float* out_data);
        float getExtentX(SLAMAlgorithmResult* result);
        float getExtentY(SLAMAlgorithmResult* result);
        float getExtentZ(SLAMAlgorithmResult* result);
        int32_t getPolygon3DSize(SLAMAlgorithmResult* result);
        void getPolygon3D(SLAMAlgorithmResult* result, float* out_data);
        int32_t isPoseInPolygon(SLAMAlgorithmResult* result, const CPose& pose);
        int32_t isPoseInExtents(SLAMAlgorithmResult* result, const CPose& pose);
        int32_t getPlaneIndex(SLAMAlgorithmResult* result);
        void getPlaneOriginPoint(SLAMAlgorithmResult* result, CPose& originPoint, ArAxisUpMode upMode);


    public:
        int32_t m_PlaneID;
    };

}


#endif  // ARPLANE_C_API_H_
