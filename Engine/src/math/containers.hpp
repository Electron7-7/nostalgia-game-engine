#ifdef FWD_DCL
#   ifndef MATH_FWD_DCL
#   define MATH_FWD_DCL
#   include "common/concepts.hpp"

enum class VectorMembers : unsigned short
{
    None, XYZ, WHL, RGBA,
    RGB = RGBA, WH = WHL, XY = XYZ,
};

static constexpr std::string s_GetVectorMemberString(VectorMembers inVM) noexcept
{
    switch(inVM)
    {
    case VectorMembers::None:
        return "VectorMembers::None";
    case VectorMembers::XYZ:
        return "VectorMembers::XYZ (or XY)";
    case VectorMembers::WHL:
        return "VectorMembers::WHL (or WH)";
    case VectorMembers::RGBA:
        return "VectorMembers::RGBA (or RGB)";
    }
}

template<unsigned short Length, Number T, VectorMembers M = VectorMembers::None>
        requires (Length > 1)
    struct vector;

typedef vector<2,double,VectorMembers::XYZ>  Position2D;
typedef vector<2,double,VectorMembers::XYZ>  Motion2D;
typedef vector<2,int,VectorMembers::WHL>     Scale2D;
typedef vector<3,double,VectorMembers::XYZ>  Position3D;
typedef vector<3,double,VectorMembers::XYZ>  Motion3D;
typedef vector<3,int,VectorMembers::WHL>     Scale3D;

#   endif // MATH_FWD_DCL
#elif !defined CONTAINERS_H
#define CONTAINERS_H

#define FWD_DCL
#   include "containers.hpp"
#undef  FWD_DCL

#include "common/concepts.hpp"
#include "thirdparty/DearImGui/imgui.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <cxxabi.h>
#include <string>
#include <memory>
#include <cstdlib>

#define Derived std::derived_from
#define Same std::is_same_v
#define ONLY_IF(VECTOR_MEMBER) requires (M == VectorMembers::VECTOR_MEMBER)
#define __INDEX_ACCESSOR(NAME, VM, INDEX, IS_CONST...) \
    IS_CONST T& NAME() IS_CONST ONLY_IF(VM) { return array_[INDEX]; }
#define INDEX_ACCESSOR(NAME, VM, INDEX) \
    __INDEX_ACCESSOR(NAME, VM, INDEX) \
    __INDEX_ACCESSOR(NAME, VM, INDEX, const)
#define NO_OVERFLOW(INDEX) (Length > INDEX) ? INDEX : (Length - 1)

// https://stackoverflow.com/a/12877598
template<typename T>
    static constexpr std::string s_DemangleTypeName()
    {
        int status;
        std::unique_ptr<char[], void (*)(void*)> result(
            abi::__cxa_demangle(typeid(T).name(), 0, 0, &status), std::free);
        return result.get() ? std::string(result.get()) : "N/A";
    }

struct __vector_base {};

template<ushort Length, Number T, VectorMembers M>
        requires (Length > 1)
    struct vector : __vector_base
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

        template<ushort Length2, Number T2, VectorMembers M2 = VectorMembers::None>
                requires (Length2 > 1)
            constexpr vector(const vector<Length2,T2,M2>& inVec) noexcept
                {
                    ushort max_size{(Length < Length2) ? Length : Length2};
                    for(ushort i{0}; i < max_size; ++i)
                        { array_[i] = inVec[i]; }
                }

        constexpr vector(const ImVec2& inImVec2) noexcept:
            array_{static_cast<T>(inImVec2[0]), static_cast<T>(inImVec2[1])} {}

        explicit constexpr operator ImVec2() const noexcept
        { return ImVec2{static_cast<float>(array_[0]), static_cast<float>(array_[1])}; }

        constexpr vector(const glm::vec2& inVec2) noexcept:
            array_{static_cast<T>(inVec2[0]), static_cast<T>(inVec2[1])} {}

        explicit constexpr operator glm::vec2() const noexcept
        { return glm::vec2{static_cast<float>(array_[0]), static_cast<float>(array_[1])}; }

        constexpr vector(const glm::vec3& inVec3) noexcept:
            array_{static_cast<T>(inVec3[0]), static_cast<T>(inVec3[1]), static_cast<T>(inVec3[2])} {}

        constexpr operator glm::vec3() const noexcept
        {
            return glm::vec3{static_cast<float>(array_[0]),
                static_cast<float>(array_[1]),
                static_cast<float>(array_[2])};
        }

        constexpr std::string type_log() const noexcept
        {
            return std::format("vector<{}, {}, {}>",
                Length,
                s_DemangleTypeName<T>(),
                s_GetVectorMemberString(M));
        }

        std::string data_log() const noexcept
        { return std::format("{}", array_); }

        std::string full_log() const noexcept
        { return std::format("{}{}", type_log(), data_log()); }

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

        double AspectRatio() const noexcept ONLY_IF(WHL)
        { return static_cast<double>(w()) / h(); }

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

        template<NumberOrBool N>
            constexpr vector& operator+=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] += inScalar; }
                return *this;
            }

        template<NumberOrBool N> constexpr vector operator+(N inScalar) const noexcept
            { return vector{*this} += inScalar; }

        template<NumberOrBool N>
            constexpr vector& operator-=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { array_[i] -= inScalar; }
                return *this;
            }

        template<NumberOrBool N> constexpr vector operator-(N inScalar) const noexcept
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

template<typename T>
    concept IsVector = std::derived_from<T, __vector_base>;

typedef vector<2,double,VectorMembers::XYZ>  Position2D;
typedef vector<2,double,VectorMembers::XYZ>  Motion2D;
typedef vector<2,int,VectorMembers::WHL>     Scale2D;
typedef vector<3,double,VectorMembers::XYZ>  Position3D;
typedef vector<3,double,VectorMembers::XYZ>  Motion3D;
typedef vector<3,int,VectorMembers::WHL>     Scale3D;

#undef Derived
#undef Same
#undef ONLY_IF
#undef __INDEX_ACCESSOR
#undef INDEX_ACCESSOR
#undef NO_OVERFLOW
#endif // CONTAINERS_H
