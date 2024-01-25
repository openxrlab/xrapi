
#ifndef STANDARD_AR_ARPOSE_C_API_H_
#define STANDARD_AR_ARPOSE_C_API_H_

#include <stddef.h>
#include <stdint.h>

namespace standardar
{
    class CPose
    {
    public:
        CPose()
        {
            qx = qy = qz = qw = 0.0f;
            tx = ty = tz = 0.0f;
        }

    public:
        float   qx, qy, qz, qw, tx, ty, tz;
    };
}


#endif  // ARPOSE_C_API_H_
