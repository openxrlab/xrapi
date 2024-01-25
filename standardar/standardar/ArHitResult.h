
#ifndef STANDARD_AR_ARHITRESULT_C_API_H_
#define STANDARD_AR_ARHITRESULT_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include "ArPose.h"

namespace standardar
{
    class ITrackable;
    class CHitResult
    {
    public:
        CHitResult();
        virtual ~CHitResult();

        const CPose& getHitPose(){return m_HitPoint;}
        ITrackable* getTrackable(){return m_pTrackable;}

    public:
        CPose          m_HitPoint;
        ITrackable*    m_pTrackable;
    };

    class CHitResultList
    {
    public:
        std::vector<CHitResult*> m_List;
    };
}

#endif  // ARHITRESULT_C_API_H_
