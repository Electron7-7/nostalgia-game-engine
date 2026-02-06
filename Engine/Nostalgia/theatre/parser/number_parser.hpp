#ifndef NUMBER_PARSER_H
#define NUMBER_PARSER_H

#ifdef NOSTALGIA_GCC_PCH
#   include <Nostalgia/Nostalgia.hpp>
#endif

#include <stdexcept>

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

template<typename T>
    bool StringToNum(T& output, const std::string& string)
    {
        T number{static_cast<T>(-1)}; // Because this is used a lot on IDs, and '-1' == `ID::Invalid`
        if constexpr(!std::is_integral_v<T>)
        {
            try { number = std::stold(string); }
            catch(std::invalid_argument const& e) { return false; }
        }
        else
        {
            try { number = std::stoll(string); }
            catch(std::invalid_argument const& e) { return false; }
        }
        output = number;
        return true;
    }

template<GLMContainer T, uint size = gGlmSize<T>()>
    bool InterpretGLM(T& variable, const std::string& string)
    {
        T output{};
        std::string buffer{};
        size_t index{0};
        float temp_num{0.0f};
        for(const char& character : string)
        {
            switch(character)
            {
            case ',':
                if(index < (size - 1))
                {
                    temp_num = output[index];
                    if(!StringToNum<float>(temp_num, buffer))
                        { return false; }
                    output[index++] = temp_num;
                }
                buffer.clear();
                [[fallthrough]];
            case ' ':
                continue;
            default:
                buffer += character;
                continue;
            }
        }
        temp_num = output[size - 1];
        if(!StringToNum<float>(temp_num, buffer))
            { return false; }
        output[size - 1] = temp_num;
        variable = output;
        return true;
    }

template<>
inline bool StringToNum(glm::vec2& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::vec3& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::vec4& output, const std::string& string)
{ return InterpretGLM(output, string); }

template<>
inline bool StringToNum(glm::quat& output, const std::string& string)
{ return InterpretGLM(output, string); }

#endif // NUMBER_PARSER_H
