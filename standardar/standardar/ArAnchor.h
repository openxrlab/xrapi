
#ifndef STANDARD_AR_ARANCHOR_C_API_H_
#define STANDARD_AR_ARANCHOR_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "ArPose.h"
#include "StandardAR.h"

namespace standardar
{
    class ITrackable;

    class CAnchor
    {
    public:
        CAnchor();
        virtual ~CAnchor();
        virtual ARCloudAnchorState getCloudAnchorState();
        virtual void getCloudAnchorId(std::string& str);

    public:
        ARTrackingState m_State;

        bool            m_isUpdated;

        CPose           m_Pose;
        CPose           m_LocalPose;    // pose related to trackable;
        ITrackable*     m_Trackable;
        ARAnchor_*      m_Handle;
    };

    class CAnchorList
    {
    public:
        std::vector<CAnchor*> m_List;
    };

    class CCloudAnchor:public CAnchor
    {
    public:
        CCloudAnchor();
        CCloudAnchor(CAnchor anchor);
        ~CCloudAnchor();

        void getCloudAnchorId(std::string& str);
        void setCloudAnchorId(char* anchorID);
        ARCloudAnchorState getCloudAnchorState();
        void setCloudAnchorState(ARCloudAnchorState state);

    private:
        ARCloudAnchorState  m_CloudStatus;
        std::string         m_AnchorId;
    };



}


#endif  // ARANCHOR_C_API_H_
