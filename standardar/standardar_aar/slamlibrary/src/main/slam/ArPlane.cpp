#include "ArPlane.h"
#include "ArPose.h"
#include "StandardAR.h"

namespace standardar
{
    CPlane::CPlane(int32_t planeid):ITrackable(ARNODE_TYPE_PLANE)
    {
        m_PlaneID = planeid;
    }

    CPlane::~CPlane()
    {

    }

    ARPlaneType CPlane::getPlaneType(standardar::SLAMAlgorithmResult *result, ArAxisUpMode upMode)
    {
        return result->getPlaneType(m_PlaneID);
    }

    void CPlane::getPlaneNormal(standardar::SLAMAlgorithmResult *result, float *out_plane_normal)
    {
        if (result == nullptr)
            return;

        result->getPlaneNormal(out_plane_normal, m_PlaneID);
    }

    void CPlane::getCenterPose(standardar::SLAMAlgorithmResult *result, standardar::CPose &out_pose)
    {
        if (result == nullptr)
            return;
        vec3f pose = result->getCenterPose(m_PlaneID);
        out_pose.tx = pose.x;
        out_pose.ty = pose.y;
        out_pose.tz = pose.z;
    }

    int32_t CPlane::getPolygonSize(standardar::SLAMAlgorithmResult *result)
    {
        if (result == nullptr)
            return 0;

        return result->getPolygonSize(m_PlaneID);
    }

    void CPlane::getPolygon(standardar::SLAMAlgorithmResult *result, float *out_data)
    {
        if (result == nullptr)
            return;
        result->getPolygon(out_data, m_PlaneID);
    }

    float CPlane::getExtentX(standardar::SLAMAlgorithmResult *result)
    {
        if (result == nullptr)
            return 0;
        return result->getExtent(AXIS_X, m_PlaneID);
    }

    float CPlane::getExtentY(standardar::SLAMAlgorithmResult *result)
    {
        if (result == nullptr)
            return 0.0f;
        return result->getExtent(AXIS_Y, m_PlaneID);
    }

    float CPlane::getExtentZ(standardar::SLAMAlgorithmResult *result)
    {
        if (result == nullptr)
            return 0.0f;
        return result->getExtent(AXIS_Z, m_PlaneID);
    }

    int32_t CPlane::getPolygon3DSize(standardar::SLAMAlgorithmResult *result)
    {
        if (result == nullptr)
            return 0;

        return result->getPolygon3DSize(m_PlaneID);
    }

    void CPlane::getPolygon3D(standardar::SLAMAlgorithmResult *result, float *out_data)
    {
        if (result == nullptr)
            return;
        result->getPolygon3D(out_data, m_PlaneID);

    }

    int32_t CPlane::isPoseInPolygon(standardar::SLAMAlgorithmResult *result,const standardar::CPose &pose)
    {
        if(m_PlaneID == SPECIAL_PLANE_ID_HORIZONTAL_PLANE)
        {
            return 1;
        }

        if (result == nullptr) {
            return 0;
        }
        vec3f p;
        p.x = pose.tx; p.y = pose.ty; p.z = pose.tz;
        return result->isPoseInPolygon(p, m_PlaneID);
    }

    int32_t CPlane::isPoseInExtents(standardar::SLAMAlgorithmResult *result, const standardar::CPose &pose)
    {
        if (result == nullptr)
            return 0;

        vec3f p;
        p.x = pose.tx; p.y = pose.ty; p.z = pose.tz;
        return result->isPoseInExtents(p, m_PlaneID);
    }

    int32_t CPlane::getPlaneIndex(standardar::SLAMAlgorithmResult *result)
    {
        return result->getSLAMPlaneIndex(m_PlaneID);
    }

    void CPlane::getPlaneOriginPoint(standardar::SLAMAlgorithmResult *result,
                                        standardar::CPose &originPoint,
                                        ArAxisUpMode upMode)
    {
        vec3f ori_point = result->getPlaneOriginPoint(m_PlaneID);
        originPoint.tx = ori_point.x;
        originPoint.ty = ori_point.y;
        originPoint.tz = ori_point.z;
    }
}
