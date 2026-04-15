#ifndef NUMBER_PARSER_H
#define NUMBER_PARSER_H

enum class NumberType : int
{
    NIL=0, FLOAT, INT, VEC2, VEC3, VEC4
};

template<GLMContainer T>
    consteval ushort gGlmSize()
    {
        if constexpr(GLM_Vec2<T>)
            { return 2; }
        else if constexpr(GLM_Vec3<T>)
            { return 3; }
        else if constexpr(GLM_Vec4<T> || GLM_Quat<T>)
            { return 4; }
        return 1;
    }

template<NumberOrBool T>
    std::string NumToString(T inNum)
    { return std::format("{}", inNum); }

// NOTE: Wait, can't I just use `std::format` for all of these? Unless I want to define my own format, of course
template<GLMContainer T, uint size = gGlmSize<T>()>
    std::string NumToString(const T& inVec) noexcept
    {
        std::string buffer{};
        for(uint i{0}; i < size; ++i)
            { buffer += std::to_string(inVec[i]) + ","; }
        buffer.pop_back();
        return buffer;
    }

template<NumberOrGLM T>
    bool StringToNum(T& output, const std::string& string)
    {
        T number{static_cast<T>(-1)}; // Because this is used a lot on IDs, and '-1' == `ID::Invalid`
        if constexpr(not std::is_integral_v<T>)
        {
            try { number = std::stof(string); }
            catch(std::invalid_argument const& e) { return false; }
        }
        else
        {
            int _base{10};
            if(string.starts_with("0b"))
                { _base = 2; }
            else if(string.starts_with("0x"))
                { _base = 16; }
            try { number = std::stoi(string, nullptr, _base); }
            catch(std::invalid_argument const& e) { return false; }
        }
        output = number;
        return true;
    }

template<uint size = 0, GLMContainer T>
    bool InterpretGLM(T& variable, const std::string& inString)
    {
        std::vector<std::string> numbers{};
        std::string buffer{};
        size_t string_size{inString.size()};
        for(size_t i{0}; i < string_size; ++i)
        {
            char character{inString.at(i)};
            if(std::isdigit(character) or character == '-' or character == '.')
                { buffer += character; }
            if(not buffer.empty() and
                (character == ' ' or character == ',' or i+1 == string_size))
            {
                numbers.push_back(buffer);
                buffer.clear();
            }
        }
        auto numbers_size{numbers.size()};
        if(numbers_size > size)
            { return false; }
        for(uint index{0}; index < numbers_size; ++index)
        {
            float temp_num{variable[index]};
            if(not StringToNum<float>(temp_num, numbers.at(index)))
                { return false; }
            variable[index] = temp_num;
        }
        return true;
    }

template<>
inline bool StringToNum(BitMask& output, const std::string& string)
{
    int _num{0};
    int _base{(string.starts_with("0b")) ? 2 : 10};
    if(string.starts_with("0x"))
        { _base = 16; }
    try { _num = std::stoi(string, nullptr, _base); }
    catch(std::invalid_argument const& e) { return false; }
    output.set(_num);
    return true;
}

template<>
inline bool StringToNum(glm::vec2& output, const std::string& string)
{ return InterpretGLM<2>(output, string); }

template<>
inline bool StringToNum(glm::vec3& output, const std::string& string)
{ return InterpretGLM<3>(output, string); }

template<>
inline bool StringToNum(glm::vec4& output, const std::string& string)
{ return InterpretGLM<4>(output, string); }

template<>
inline bool StringToNum(glm::quat& output, const std::string& string)
{ return InterpretGLM<4>(output, string); }

inline NumberType GetNumberType(Sarg inNumber)
{
    int _int_test;
    float _float_test;
    glm::vec2 _vec2_test;
    glm::vec3 _vec3_test;
    glm::vec4 _vec4_test;

    if(inNumber.contains(','))
    {
        if(StringToNum(_vec2_test, inNumber))
            { return NumberType::VEC2; }
        else if(StringToNum(_vec3_test, inNumber))
            { return NumberType::VEC3; }
        else if(StringToNum(_vec4_test, inNumber))
            { return NumberType::VEC4; }
    }
    else if(inNumber.contains('.'))
    {
        if(StringToNum(_float_test, inNumber))
            { return NumberType::FLOAT; }
    }
    else if(StringToNum(_int_test, inNumber))
        { return NumberType::INT; }
    return NumberType::NIL;
}

#endif // NUMBER_PARSER_H
