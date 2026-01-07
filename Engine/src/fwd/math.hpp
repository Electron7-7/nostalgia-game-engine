#ifndef MATH_FWD_H
#define MATH_FWD_H

#include <type_traits>

using vector_members_t = unsigned short;

namespace VectorMembers
{
    constexpr unsigned short None {0},
        XYZ  {1},
        WHL  {2},
        RGBA {3},
        RGB  {RGBA},
        WH   {WHL},
        XY   {XYZ};
}

template<unsigned short Length, class T, vector_members_t M>
    requires (Length > 1) and std::is_arithmetic_v<T>
        struct vector;

// Note: I'm using numbers instead of the variables in `VectorMembers` because if I
// did, I'd have to make them `constexpr` and defining them in the "forward declarations"
// header doesn't sit right with me.
typedef vector<2,double,1>  Position2D;
typedef vector<2,double,1>  Motion2D;
typedef vector<2,int,2>     Size2D;
typedef vector<2,int,2>     Scale2D;
typedef vector<3,double,1>  Position3D;
typedef vector<3,double,1>  Motion3D;
typedef vector<3,int,2>     Size3D;
typedef vector<3,int,2>     Scale3D;
typedef vector<3,double,3>  ColorRGB;
typedef vector<4,double,3>  ColorRGBA;

#endif // MATH_FWD_H
