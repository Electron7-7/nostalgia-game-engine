#ifndef STRING_TO_NUM_H
#define STRING_TO_NUM_H

#include <string>
#include "printing.hpp"

#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>

#define INTERPRET_NUMBER_MACRO(STD_FUNCTION)                                    \
T try_out = 0;                                                                  \
try                                                                             \
    { try_out = std::STD_FUNCTION(string); }                                    \
catch(std::invalid_argument const& exception)                                   \
    { PRINT_WARNING("A variable tried to interpret '{}' as a number", string) } \
return try_out;

template<typename T>
concept SignedNumber = std::is_integral_v<std::decay_t<T>> && std::is_signed_v<T>;

template<typename T>
concept UnsignedNumber = std::is_unsigned_v<std::decay_t<T>>;

template<typename T>
concept RealNumber = std::is_floating_point_v<std::decay_t<T>>;

template<SignedNumber T>
T InterpretNumber(const std::string& string)
{
    INTERPRET_NUMBER_MACRO(stol)
}

template<UnsignedNumber T>
T InterpretNumber(const std::string& string)
{
    INTERPRET_NUMBER_MACRO(stoul)
}

template<RealNumber T>
T InterpretNumber(const std::string& string)
{
    INTERPRET_NUMBER_MACRO(stod)
}

template<typename T>
T StringToNum(const std::string& string)
{
    return InterpretNumber<T>(string);
}

// Fuck you, I'm tired...
#define INTERPRET_GLM_MACRO(GLM, SIZE)                              \
GLM StringToNum(const std::string& string)                          \
{                                                                   \
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
                { output[index] = InterpretNumber<float>(buffer); } \
            buffer.clear();                                         \
            [[fallthrough]];                                        \
        case ' ':                                                   \
            continue;                                               \
        default:                                                    \
            buffer += character;                                    \
            continue;                                               \
        }                                                           \
    }                                                               \
    output[SIZE - 1] = InterpretNumber<float>(buffer);              \
    return output;                                                  \
}

template<>
INTERPRET_GLM_MACRO(glm::vec2, 2)

template<>
INTERPRET_GLM_MACRO(glm::vec3, 3)

template<>
INTERPRET_GLM_MACRO(glm::quat, 4)


#endif // STRING_TO_NUM_H
