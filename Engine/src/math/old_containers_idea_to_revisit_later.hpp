#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "concepts.hpp"
#include "thirdparty/DearImGui/imgui.h"

#include <concepts>
#include <sys/types.h>
#include <glm/fwd.hpp>

#ifdef _MSC_VER
#   define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else // !_MSC_VER
#   define NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif // _MSC_VER

#define DEFINE_IF(VECTOR_TYPE) \
    NO_UNIQUE_ADDRESS std::conditional_t<std::is_same_v<M,VECTOR_TYPE>,T&,__empty>

namespace { struct __empty { template<Number T> constexpr __empty(T) noexcept {} }; }

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

#define Derived std::derived_from
#define Same std::is_same_v

template<ushort Length, Number T, class M>
        requires (Length > 1) && Derived<M,__vector_members_t> && (!Same<M,__vector_members_t>)
    struct vector
    {
    private:
        T array_[Length]{};

    public:
        constexpr vector() noexcept = default;

        constexpr vector(T (&inVector)[Length]) noexcept
        { for(ushort i{0}; i < Length; ++i) { array_[i] = inVector[i]; } }

        template<Number... Args>
            requires std::constructible_from<T[Length], Args...> && (sizeof...(Args) <= Length)
            constexpr vector(Args... inArgs) noexcept:
                array_{static_cast<T>(inArgs)...} {}

        template<ushort Length2, Number T2, class M2 = VectorMembers::None>
                requires (Length2 > 1) && Derived<M2,__vector_members_t> && (!Same<M2,__vector_members_t>)
            constexpr vector(const vector<Length2,T2,M2>& inVec) noexcept
                {
                    ushort max_size{(Length < Length2) ? Length : Length2};
                    for(ushort i{0}; i < max_size; ++i)
                        { array_[i] = inVec[i]; }
                }

        constexpr vector(const ImVec2& inImVec2) noexcept:
            array_{static_cast<T>(inImVec2[0]), static_cast<T>(inImVec2[1])} {}

        constexpr vector& operator=(const vector& inVec) noexcept
        {
            ushort max_size{(Length < inVec.count()) ? Length : inVec.count()};
                for(ushort i{0}; i < max_size; ++i)
                    { array_[i] = inVec[i]; }
            return *this;
        }

        constexpr vector& operator=(ImVec2& inVec2) noexcept
        { array_[0] = inVec2[0]; array_[1] = inVec2[1]; return *this; }

        constexpr operator ImVec2() const noexcept
        { return ImVec2{static_cast<float>(array_[0]), static_cast<float>(array_[1])}; }

        DEFINE_IF(VectorMembers::XYZ) x{array_[0]};
        DEFINE_IF(VectorMembers::XYZ) y{array_[1]};
        DEFINE_IF(VectorMembers::XYZ) z{array_[(Length > 2) ? 2 : 0]};

        DEFINE_IF(VectorMembers::WHL) w{array_[0]};
        DEFINE_IF(VectorMembers::WHL) h{array_[1]};
        DEFINE_IF(VectorMembers::WHL) l{array_[(Length > 2) ? 2 : 0]};
        DEFINE_IF(VectorMembers::WHL) width{array_[0]};
        DEFINE_IF(VectorMembers::WHL) height{array_[1]};
        DEFINE_IF(VectorMembers::WHL) length{array_[(Length > 2) ? 2 : 0]};

        DEFINE_IF(VectorMembers::RGBA) r{array_[0]};
        DEFINE_IF(VectorMembers::RGBA) g{array_[1]};
        DEFINE_IF(VectorMembers::RGBA) b{array_[(Length > 2) ? 2 : 0]};
        DEFINE_IF(VectorMembers::RGBA) a{array_[(Length > 3) ? 3 : 0]};

        template<glm::length_t L, typename U>
            constexpr operator glm::vec<L,U>() noexcept
            {
                glm::vec<L,U> output{};
                ushort limit{(Length <= L) ? Length : L};
                for(uint i{0}; i < limit; ++i)
                    { output[i] = static_cast<float>(array_[i]); }
                return output;
            }

        consteval ushort count() const noexcept { return Length; }

        T& operator[](ushort Index) noexcept
            { return array_[(Index < Length) ? Index : (Length - 1)]; }

        const T& operator[](ushort Index) const noexcept
            { return array_[(Index < Length) ? Index : (Length - 1)]; }

        constexpr T* data() noexcept
        { return array_; }

        constexpr const T* cdata() const noexcept
        { return array_; }

        template<Number N>
            constexpr vector& operator+=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] += inScalar; }
                return *this;
            }

        template<Number N> constexpr vector operator+(N inScalar) const noexcept
            { return vector{*this} += inScalar; }

        template<Number N>
            constexpr vector& operator-=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] -= inScalar; }
                return *this;
            }

        template<Number N> constexpr vector operator-(N inScalar) const noexcept
            { return vector{*this} -= inScalar; }

        template<Number N>
            constexpr vector& operator*=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] *= inScalar; }
                return *this;
            }

        template<Number N> constexpr vector operator*(N inScalar) const noexcept
            { return vector{*this} *= inScalar; }

        constexpr bool operator==(const vector& other) const noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { if(array_[i] != other.array_[i]) { return false; } }
            return true;
        }

        constexpr bool operator!=(const vector& other) const noexcept
        { return !(*this == other); }

        constexpr const vector& operator-=(const vector& other) noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { array_[i] -= other.array_[i]; }
            return *this;
        }

        constexpr vector operator-(const vector& other) const noexcept
        { return vector{*this} -= other; }

        constexpr const vector& operator+=(const vector& other) noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { array_[i] += other.array_[i]; }
            return *this;
        }

        constexpr vector operator+(const vector& other) const noexcept
        { return vector{*this} += other; }
    };

struct Position2D : public vector<2,double,VectorMembers::XYZ> { using vector::vector; };
struct Motion2D   : public vector<2,double,VectorMembers::XYZ> { using vector::vector; };
struct Scale2D    : public vector<2,int,VectorMembers::WHL>    { using vector::vector; };

struct Position3D : public vector<3,double,VectorMembers::XYZ> { using vector::vector; };
struct Motion3D   : public vector<3,double,VectorMembers::XYZ> { using vector::vector; };
struct Scale3D    : public vector<3,int,VectorMembers::WHL>    { using vector::vector; };

#undef DEFINE_IF
#undef Derived
#undef Same
#endif // CONTAINERS_H
