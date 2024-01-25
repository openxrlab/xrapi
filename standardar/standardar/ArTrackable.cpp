#include "ArTrackable.h"

namespace standardar
{
    ITrackable::ITrackable(ARNodeType type):m_Type(type),
                                           m_Handle(NULL),
                                           m_isUpdated(false),
                                           m_State(ARTRACKING_STATE_SUCCESS)
    {

    }

    ITrackable::~ITrackable()
    {

    }

    ARNodeType ITrackable::getType()
    {
        return m_Type;
    }

}
