#include <cstring>
#include "ArAnchor.h"

namespace standardar
{
    CAnchor::CAnchor()
    {
        m_Trackable = NULL;
        m_State = ARTRACKING_STATE_SUCCESS;
        m_Handle = NULL;
        m_isUpdated = false;
    }

    CAnchor::~CAnchor()
    {

    }

    void CAnchor::getCloudAnchorId(std::string& str)
    {

    }

    ARCloudAnchorState CAnchor::getCloudAnchorState()
    {
        return ARCLOUD_ANCHOR_STATE_NONE;
    }


    CCloudAnchor::CCloudAnchor()
    {
        m_Trackable = NULL;
        m_State = ARTRACKING_STATE_SUCCESS;
        m_Handle = NULL;
        m_isUpdated = false;
        m_CloudStatus = ARCLOUD_ANCHOR_STATE_TASK_IN_PROGRESS;
        m_AnchorId = "";
    }

    CCloudAnchor::CCloudAnchor(standardar::CAnchor anchor)
    {
        m_Trackable = NULL;
        m_State = ARTRACKING_STATE_LOST;
        m_Handle = NULL;
        m_isUpdated = false;
        m_Pose = anchor.m_Pose;
        m_LocalPose = anchor.m_LocalPose;
        m_CloudStatus = ARCLOUD_ANCHOR_STATE_TASK_IN_PROGRESS;
        m_AnchorId = "";
    }

    CCloudAnchor::~CCloudAnchor()
    {
    }

    void CCloudAnchor::getCloudAnchorId(std::string& str)
    {
        str = std::string(m_AnchorId.c_str());
    }

    void CCloudAnchor::setCloudAnchorId(char* anchorID)
    {
        m_AnchorId = anchorID;
    }

    ARCloudAnchorState CCloudAnchor::getCloudAnchorState()
    {
        return m_CloudStatus;
    }

    void CCloudAnchor::setCloudAnchorState(ARCloudAnchorState state)
    {
        m_CloudStatus = state;
    }

}
