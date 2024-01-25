
#ifndef STANDARD_AR_ARTRACKABLE_C_API_H_
#define STANDARD_AR_ARTRACKABLE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include "StandardARCommon.h"
#include "StandardAR.h"

namespace standardar
{
    class ITrackable
    {
    public:
        ITrackable(ARNodeType type);
        virtual ~ITrackable();

        ARNodeType getType();

    public:
        ARNodeType      m_Type;
        ARNode_*        m_Handle;
        bool            m_isUpdated;
        ARTrackingState m_State;
    };

    class CTrackableList
    {
    public:
        std::vector<ITrackable*> m_List;
    };
}


#endif  // ARTRACKABLE_C_API_H_
