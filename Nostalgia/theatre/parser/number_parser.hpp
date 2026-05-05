#ifndef NUMBER_PARSER_H
#define NUMBER_PARSER_H

inline bool IsFloat(Sarg inString)
{
    char* _ptr{};
    strtof(inString.data(), &_ptr);
    return (*_ptr) == '\0';
}

inline bool IsInt(Sarg inString)
{
    char* _ptr{};
    strtol(inString.data(), &_ptr, 10);
    return (*_ptr) == '\0';
}

inline std::string GetLowercase(Sarg inString)
{
    std::string _val{inString};
    // https://stackoverflow.com/a/313990
    std::transform(_val.begin(), _val.end(), _val.begin(),
        [](unsigned char character){ return std::tolower(character); });
    return _val;
}

inline bool IsBool(Sarg inString)
{
    std::string _val{GetLowercase(inString)};
    return _val == "false" or _val == "true";
}

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
            { buffer += std::format("{:f}", inVec[i]) + ","; }
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

#endif // NUMBER_PARSER_H
