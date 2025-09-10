#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include <stdexcept>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

// Fuck you, I'm tired...
#define INTERPRET_GLM_MACRO(GLM, SIZE)                              \
    GLM output = GLM();                                             \
    std::vector<float> numbers = {};                                \
    std::string buffer = "";                                        \
    numbers.reserve(SIZE);                                          \
    size_t index = 0;                                               \
    for(const char& character : string)                             \
    {                                                               \
        switch(character)                                           \
        {                                                           \
        case ',':                                                   \
            if(++index < (SIZE - 1))                                \
            {                                                       \
                if(!StringToNum<float>(output[index], buffer))      \
                    { return false; }                               \
            }                                                       \
            buffer.clear();                                         \
            [[fallthrough]];                                        \
        case ' ':                                                   \
            continue;                                               \
        default:                                                    \
            buffer += character;                                    \
            continue;                                               \
        }                                                           \
    }                                                               \
    if(!StringToNum<float>(output[SIZE - 1], buffer))               \
        { return false; }                                           \
    variable = output;                                              \
    return true;

#define INTERPRET_NUMBER_MACRO(STD_FUNCTION, VARIABLE, STRING) \
T new_value = 0;                                               \
try                                                            \
    { new_value = std::STD_FUNCTION(STRING); }                 \
catch(std::invalid_argument const& exception)                  \
    { return false; }                                          \
VARIABLE = new_value;                                          \
return true;

template<typename T>
concept UnsignedNumber = requires { std::is_unsigned_v<std::decay_t<T>>; };

template<typename T>
concept RealNumber = requires { std::is_floating_point_v<std::decay_t<T>>; };

template<typename T>
inline bool StringToNum(T& number, const std::string& string)
{
    if constexpr(UnsignedNumber<T>)
        { INTERPRET_NUMBER_MACRO(stoul, number, string) }
    else if constexpr(RealNumber<T>)
        { INTERPRET_NUMBER_MACRO(stod, number, string) }
    else
        { INTERPRET_NUMBER_MACRO(stol, number, string) }
}

template<>
inline bool StringToNum(glm::vec2& variable, const std::string& string)
{ INTERPRET_GLM_MACRO(glm::vec2, 2) }

template<>
inline bool StringToNum(glm::vec3& variable, const std::string& string)
{ INTERPRET_GLM_MACRO(glm::vec3, 3) }

template<>
inline bool StringToNum(glm::quat& variable, const std::string& string)
{ INTERPRET_GLM_MACRO(glm::quat, 4) }

#endif // STRING_TO_NUM_H
