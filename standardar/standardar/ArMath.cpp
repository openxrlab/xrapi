#include "ArMath.h"

namespace standardar
{
#ifndef STRENDER_THIN_MATH
    template class vec2<float>;
    template class vec2<double>;
    template class vec2<int>;
    template class vec3<float>;
    template class vec3<double>;
    template class vec3<int>;
    template class vec4<float>;
    template class vec4<double>;
    template class vec4<int>;
    template class mat3<float>;
    template class mat3<double>;
    template class mat3<int>;
    template class mat4<float>;
    template class mat4<double>;
    template class mat4<int>;
    template class quat<float>;
    template class quat<double>;
#else
    template class vec3<float>;
    template class vec3<double>;
    template class vec3<int>;
    template class vec4<float>;
    template class vec4<double>;
    template class vec4<int>;
    template class mat4<float>;
    template class mat4<double>;
    template class mat4<int>;
    template class quat<float>;
    template class quat<double>;
#endif

    template<>
    const vec3f vec3f::UnitX(1, 0, 0);
    template<>
    const vec3f vec3f::UnitY(0, 1, 0);
    template<>
    const vec3f vec3f::UnitZ(0, 0, 1);

    template<>
    const vec3d vec3d::UnitX(1, 0, 0);
    template<>
    const vec3d vec3d::UnitY(0, 1, 0);
    template<>
    const vec3d vec3d::UnitZ(0, 0, 1);
}
