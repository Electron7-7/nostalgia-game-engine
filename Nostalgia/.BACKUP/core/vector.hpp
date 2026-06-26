#ifndef NOSTALGIA_VECTOR_H
#define NOSTALGIA_VECTOR_H

#define ONLY_IF(VM) requires (M == VectorMembers::VM)
#define INDEX_ACCESSOR(NAME, VM, INDEX) \
    T& NAME()       ONLY_IF(VM) { return _array[INDEX]; } \
    T  NAME() const ONLY_IF(VM) { return _array[INDEX]; }
#define NO_OVERFLOW(INDEX) \
    (Length > INDEX) ? INDEX : (Length - 1)

#ifndef WIN32
    #include <cxxabi.h>
    // https://stackoverflow.com/a/12877598
    template<typename T>
        static std::string s_DemangleTypeName()
        {
            int status;
            const char* mangled_name{typeid(T).name()};
            std::unique_ptr<char[], void (*)(void*)> result(
                abi::__cxa_demangle(mangled_name, 0, 0, &status), std::free);
            return result.get() ? result.get() : mangled_name;
        }
#else
    template<typename T>
        static std::string s_DemangleTypeName()
            { return typeid(T).name(); }
#endif // WIN32

struct __vector_base {};

template<typename T>
    concept __is_num = std::integral<T> or std::floating_point<T>;

template<typename T>
    concept __is_num_strict = __is_num<T> and not std::same_as<T, bool>;

enum class VectorMembers : ushort
{
    None,
    XYZ, WHL, RGBA,
    RGB = RGBA, WH  = WHL, XY  = XYZ,
};

template<ushort Length, class T, VectorMembers M = VectorMembers::None>
        requires (Length > 1) && std::is_arithmetic_v<T>
    struct vector : __vector_base
    {
    private:
        T _array[Length]{};

    public:
        constexpr vector() noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { _array[i] = static_cast<T>(0); }
        }

        template<typename U> requires std::derived_from<U, __vector_base>
            constexpr vector(Farg<U> inVector) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                {
                    if(i < inVector.count())
                        { _array[i] = static_cast<T>(inVector[i]); }
                    else
                        { _array[i] = static_cast<T>(0); }
                }
            }

        constexpr vector(T (&inVector)[Length]) noexcept
        { for(ushort i{0}; i < Length; ++i) { _array[i] = inVector[i]; } }

        template<__is_num... Args>
            requires std::constructible_from<T[Length], Args...> && (sizeof...(Args) == Length)
            constexpr vector(Args... inArgs) noexcept:
                _array{static_cast<T>(inArgs)...} {}

        template<__is_num... Args>
            requires std::constructible_from<T[Length], Args...> && (sizeof...(Args) < Length)
            constexpr vector(Args... inArgs) noexcept
            {
                std::vector<T> _args{static_cast<T>(inArgs)...};
                for(ushort i{0}; i < Length; ++i)
                {
                    if(i < sizeof...(Args))
                        { _array[i] = _args[i]; }
                    else
                        { _array[i] = static_cast<T>(0); }
                }
            }

        template<ushort Length2, __is_num T2, VectorMembers M2 = VectorMembers::None>
                requires (Length2 > 1)
            constexpr vector(const vector<Length2,T2,M2>& inVec) noexcept
                {
                    ushort max_size{(Length < Length2) ? Length : Length2};
                    for(ushort i{0}; i < max_size; ++i)
                        { _array[i] = inVec[i]; }
                }

        constexpr vector(const glm::vec2& inVec2) noexcept requires (Length == 2):
            _array{static_cast<T>(inVec2[0]), static_cast<T>(inVec2[1])} {}

        constexpr vector(const glm::vec3& inVec3) noexcept requires (Length == 3):
            _array{static_cast<T>(inVec3[0]), static_cast<T>(inVec3[1]), static_cast<T>(inVec3[2])} {}

        constexpr vector(const glm::vec4& inVec4) noexcept requires (Length == 4):
            _array{static_cast<T>(inVec4[0]),
                static_cast<T>(inVec4[1]),
                static_cast<T>(inVec4[2]),
                static_cast<T>(inVec4[3])} {}

        constexpr glm::vec2 glm() const noexcept requires (Length == 2)
        {
            return {static_cast<float>(_array[0]),
                static_cast<float>(_array[1])};
        }

        constexpr glm::vec3 glm() const noexcept requires (Length == 3)
        {
            return {static_cast<float>(_array[0]),
                static_cast<float>(_array[1]),
                static_cast<float>(_array[2])};
        }

        constexpr glm::vec4 glm() const noexcept requires (Length == 4)
        {
            return {static_cast<float>(_array[0]),
                static_cast<float>(_array[1]),
                static_cast<float>(_array[2]),
                static_cast<float>(_array[3])};
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
        { return std::format("{}", _array); }

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
                { if(_array[i]) { status = false; } }
            return status;
        }

        bool is_zero_approx() const noexcept
        {
            bool status{true};
            for(uint i{0}; i < Length; ++i)
            {
                if constexpr(!std::integral<T>)
                    { if(_array[i] > 0.001) { status = false; } }
                else
                    { if(_array[i]) { status = false; } }
            }
            return status;
        }

        double AspectRatio() const noexcept ONLY_IF(WHL or M == VectorMembers::XYZ)
        { return static_cast<double>(_array[0]) / _array[1]; }

        template<glm::length_t L, typename U>
            constexpr operator glm::vec<L,U>() const noexcept requires (L <= Length)
            {
                glm::vec<L,U> output{};
                ushort limit{(Length > L) ? L : Length};
                for(uint i{0}; i < limit; ++i)
                    { output[i] = static_cast<float>(_array[i]); }
                return output;
            }

        constexpr ushort count() const noexcept { return Length; }

        T& operator[](ushort Index) noexcept
            { return _array[(Index < Length) ? Index : (Length - 1)]; }

        const T& operator[](ushort Index) const noexcept
            { return _array[(Index < Length) ? Index : (Length - 1)]; }

        constexpr T* data() noexcept
        { return _array; }

        constexpr const T* cdata() const noexcept
        { return _array; }

        template<__is_num N>
            constexpr vector& operator+=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { _array[i] += inScalar; }
                return *this;
            }

        template<__is_num N> constexpr vector operator+(N inScalar) const noexcept
            { return vector{*this} += inScalar; }

        template<__is_num N>
            constexpr vector& operator-=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { _array[i] -= inScalar; }
                return *this;
            }

        template<__is_num N> constexpr vector operator-(N inScalar) const noexcept
            { return vector{*this} -= inScalar; }

        template<__is_num N>
            constexpr vector& operator*=(N inScalar) noexcept
            {
                for(ushort i{0}; i < Length; ++i)
                    { _array[i] *= inScalar; }
                return *this;
            }

        template<__is_num N> constexpr vector operator*(N inScalar) const noexcept
            { return vector{*this} *= inScalar; }

        constexpr bool operator==(const vector& other) const noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { if(_array[i] != other._array[i]) { return false; } }
            return true;
        }

        constexpr bool operator!=(const vector& other) const noexcept
        { return !(*this == other); }

        constexpr const vector& operator-=(const vector& other) noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { _array[i] -= other._array[i]; }
            return *this;
        }

        constexpr vector operator-(const vector& other) const noexcept
        { return vector{*this} -= other; }

        constexpr const vector& operator+=(const vector& other) noexcept
        {
            for(ushort i{0}; i < Length; ++i)
                { _array[i] += other._array[i]; }
            return *this;
        }

        constexpr vector operator+(const vector& other) const noexcept
        { return vector{*this} += other; }
    };

template<typename T>
    concept Vector_t = std::derived_from<T, __vector_base>;

template<typename T>
    concept VectorElementType = std::is_arithmetic_v<T>;

template<VectorElementType T>
    using Position2D = vector<2,T,VectorMembers::XY>;
template<VectorElementType T>
    using Motion2D = vector<2,T,VectorMembers::XY>;
template<VectorElementType T>
    using Size2D = vector<2,T,VectorMembers::WH>;
template<VectorElementType T>
    using Position3D = vector<3,T,VectorMembers::XYZ>;
template<VectorElementType T>
    using Size3D = vector<3,T,VectorMembers::WHL>;
template<VectorElementType T>
    using ColorRGB = vector<3,T,VectorMembers::RGB>;
template<VectorElementType T>
    using ColorRGBA = vector<4,T,VectorMembers::RGBA>;

#undef ONLY_IF
#undef INDEX_ACCESSOR
#undef NO_OVERFLOW
#endif // NOSTALGIA_VECTOR_H
