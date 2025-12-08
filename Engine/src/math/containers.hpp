#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "fwd.hpp"

#include "concepts.hpp"
#include "thirdparty/DearImGui/imgui.h"

#include <concepts>
#include <sys/types.h>
#include <glm/fwd.hpp>

#define Derived std::derived_from
#define Same std::is_same_v
#define __INDEX_ACCESSOR(NAME, VM, INDEX, IS_CONST...) \
    IS_CONST T& NAME() IS_CONST requires Same<M,VectorMembers::VM> { return array_[INDEX]; }
#define INDEX_ACCESSOR(NAME, VM, INDEX) \
    __INDEX_ACCESSOR(NAME, VM, INDEX) \
    __INDEX_ACCESSOR(NAME, VM, INDEX, const)
#define NO_OVERFLOW(INDEX) (Length > INDEX) ? INDEX : (Length - 1)

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

        constexpr vector& operator=(ImVec2& inVec2) noexcept
        { array_[0] = inVec2[0]; array_[1] = inVec2[1]; return *this; }

        constexpr operator ImVec2() const noexcept
        { return ImVec2{static_cast<float>(array_[0]), static_cast<float>(array_[1])}; }

        INDEX_ACCESSOR(x, XYZ, 0)
        INDEX_ACCESSOR(y, XYZ, 1)
        INDEX_ACCESSOR(z, XYZ, NO_OVERFLOW(2))

        INDEX_ACCESSOR(w, WHL, 0)
        INDEX_ACCESSOR(h, WHL, 1)
        INDEX_ACCESSOR(l, WHL, NO_OVERFLOW(2))
        INDEX_ACCESSOR(width, WHL, 0)
        INDEX_ACCESSOR(height, WHL, 1)
        INDEX_ACCESSOR(length, WHL, NO_OVERFLOW(2))

        INDEX_ACCESSOR(r, RGBA, 0)
        INDEX_ACCESSOR(g, RGBA, 1)
        INDEX_ACCESSOR(b, RGBA, NO_OVERFLOW(2))
        INDEX_ACCESSOR(a, RGBA, NO_OVERFLOW(3))

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

        template<NumberOrBool N>
            constexpr vector& operator*=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] *= inScalar; }
                return *this;
            }

        template<NumberOrBool N> constexpr vector operator*(N inScalar) const noexcept
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

typedef vector<2,double,VectorMembers::XYZ>  Position2D;
typedef vector<2,double,VectorMembers::XYZ>  Motion2D;
typedef vector<2,int,VectorMembers::WHL>     Scale2D;
typedef vector<3,double,VectorMembers::XYZ>  Position3D;
typedef vector<3,double,VectorMembers::XYZ>  Motion3D;
typedef vector<3,int,VectorMembers::WHL>     Scale3D;

#undef Derived
#undef Same
#undef __INDEX_ACCESSOR
#undef INDEX_ACCESSOR
#undef NO_OVERFLOW
#endif // CONTAINERS_H
