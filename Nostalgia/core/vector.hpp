#ifndef NOSTALGIA_VECTOR_H
#define NOSTALGIA_VECTOR_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

#define ONLY_IF(VM) requires (M == VectorMembers::VM)
#define INDEX_ACCESSOR(NAME, VM, INDEX) \
    T& NAME()       ONLY_IF(VM) { return array_[INDEX]; } \
    T  NAME() const ONLY_IF(VM) { return array_[INDEX]; }
#define NO_OVERFLOW(INDEX) \
    (Length > INDEX) ? INDEX : (Length - 1)

#ifndef WIN32
    #include <cxxabi.h>
    // https://stackoverflow.com/a/12877598
    template<typename T>
        static constexpr const char* s_DemangleTypeName()
        {
            int status;
            const char* mangled_name{typeid(T).name()};
            std::unique_ptr<char[], void (*)(void*)> result(
                abi::__cxa_demangle(mangled_name, 0, 0, &status), std::free);
            return result.get() ? result.get() : mangled_name;
        }
#else
    template<typename T>
        static constexpr const char* s_DemangleTypeName()
            { return typeid(T).name(); }
#endif // WIN32

struct __vector_base {};

enum class VectorMembers : ushort
{
    None,
    XYZ, WHL, RGBA,
    RGB = RGBA, WH  = WHL, XY  = XYZ,
};

template<ushort Length, class T, VectorMembers M = VectorMembers::None>
        requires (Length > 1) and std::is_arithmetic_v<T>
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

        constexpr vector(const glm::vec2& inVec2) noexcept:
            array_{static_cast<T>(inVec2[0]), static_cast<T>(inVec2[1])} {}

        explicit constexpr operator glm::vec2() const noexcept
        { return glm::vec2{static_cast<float>(array_[0]), static_cast<float>(array_[1])}; }

        constexpr vector(const glm::vec3& inVec3) noexcept:
            array_{static_cast<T>(inVec3[0]), static_cast<T>(inVec3[1]), static_cast<T>(inVec3[2])} {}

        constexpr glm::vec2 glm() const noexcept requires (Length == 2)
        {
            return {static_cast<float>(array_[0]),
                static_cast<float>(array_[1])};
        }

        constexpr glm::vec3 glm() const noexcept requires (Length == 3)
        {
            return {static_cast<float>(array_[0]),
                static_cast<float>(array_[1]),
                static_cast<float>(array_[2])};
        }

        constexpr glm::vec4 glm() const noexcept requires (Length == 4)
        {
            return {static_cast<float>(array_[0]),
                static_cast<float>(array_[1]),
                static_cast<float>(array_[2]),
                static_cast<float>(array_[3])};
        }

        std::string type_log() const noexcept
        {
            std::string vector_member_type{};
            switch(M)
            {
            case VectorMembers::None:
                vector_member_type = "None"; break;
            case VectorMembers::XYZ:
                vector_member_type = "XYZ (or XY)"; break;
            case VectorMembers::WHL:
                vector_member_type = "WHL (or WH)"; break;
            case VectorMembers::RGBA:
                vector_member_type = "RGBA (or RGB)"; break;
            }
            return std::format("vector<{}, {}, {}>",
                Length,
                s_DemangleTypeName<T>(),
                vector_member_type);
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

        bool is_zero() const noexcept
        {
            bool status{true};
            for(uint i{0}; i < Length; ++i)
                { if(array_[i]) { status = false; } }
            return status;
        }

        bool is_zero_approx() const noexcept
        {
            bool status{true};
            for(uint i{0}; i < Length; ++i)
            {
                if constexpr(!std::integral<T>)
                    { if(array_[i] > 0.001) { status = false; } }
                else
                    { if(array_[i]) { status = false; } }
            }
            return status;
        }

        double AspectRatio() const noexcept ONLY_IF(WHL or M == VectorMembers::XYZ)
        { return static_cast<double>(array_[0]) / array_[1]; }

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

typedef vector<2,double,VectorMembers::XY>   Position2D;
typedef vector<2,double,VectorMembers::XY>   Motion2D;
typedef vector<2,int,VectorMembers::WH>      Size2D;
typedef vector<2,float,VectorMembers::XY>    Scale2D;
typedef vector<3,double,VectorMembers::XYZ>  Position3D;
typedef vector<3,double,VectorMembers::XYZ>  Motion3D;
typedef vector<3,int,VectorMembers::WHL>     Size3D;
typedef vector<3,float,VectorMembers::XYZ>   Scale3D;
typedef vector<3,double,VectorMembers::RGB>  ColorRGB;
typedef vector<4,double,VectorMembers::RGBA> ColorRGBA;

#undef ONLY_IF
#undef INDEX_ACCESSOR
#undef NO_OVERFLOW
#endif // NOSTALGIA_VECTOR_H
