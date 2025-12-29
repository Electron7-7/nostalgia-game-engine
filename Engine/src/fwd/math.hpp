#ifndef MATH_FWD_H
#define MATH_FWD_H

#include <type_traits>

using vector_members_t = unsigned short;

namespace VectorMembers
{
    extern constinit const vector_members_t None,
        XYZ, WHL, RGBA,
        XY,  WH,  RGB;
}

template<unsigned short Length, class T, vector_members_t M>
    requires (Length > 1) and std::is_arithmetic_v<T>
        struct vector;

// Note: I'm using numbers instead of the variables in `VectorMembers` because if I
// did, I'd have to make them `constexpr` and defining them in the "forward declarations"
// header doesn't sit right with me.
typedef vector<2,double,1>  Position2D;
typedef vector<2,double,1>  Motion2D;
typedef vector<2,int,2>     Scale2D;
typedef vector<3,double,1>  Position3D;
typedef vector<3,double,1>  Motion3D;
typedef vector<3,int,2>     Scale3D;

#endif // MATH_FWD_H
