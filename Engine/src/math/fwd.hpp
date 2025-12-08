#ifndef MATH_FWD_H
#define MATH_FWD_H

#include "concepts.hpp"

#include <concepts>

struct __vector_members_t {};
namespace VectorMembers
{
    struct None : __vector_members_t {};
    struct XYZ  : __vector_members_t {};
    struct WHL  : __vector_members_t {};
    struct RGBA : __vector_members_t {};
    using RGB = RGBA;
    using WH  = WHL;
    using XY  = XYZ;
}

template<unsigned short Length, Number T, class M = VectorMembers::None>
        requires (Length > 1) && std::derived_from<M,__vector_members_t> && (!std::is_same_v<M,__vector_members_t>)
    struct vector;

typedef vector<2,double,VectorMembers::XYZ>  Position2D;
typedef vector<2,double,VectorMembers::XYZ>  Motion2D;
typedef vector<2,int,VectorMembers::WHL>     Scale2D;
typedef vector<3,double,VectorMembers::XYZ>  Position3D;
typedef vector<3,double,VectorMembers::XYZ>  Motion3D;
typedef vector<3,int,VectorMembers::WHL>     Scale3D;

#endif // MATH_FWD_H
