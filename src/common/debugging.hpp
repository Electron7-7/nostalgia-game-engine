#ifndef DEBUGGING_H
#define DEBUGGING_H
#include <iostream>

//-------------------
// PRINTOUT FUNCTIONS
//-------------------

// Macros, for my sanity
#define _PRINT(label, content) std::cout << std::string(label) << std::string(content) << "\n";
#define _PRINTERR(content) std::cerr << std::string("[ERROR]\t") << std::string(content) << "\n";

// Normal Printout
template<typename T>
constexpr void PRINT(const T& thing)
{
    if constexpr(std::is_arithmetic_v<T>) _PRINT("", std::to_string(thing))
    if constexpr(!std::is_arithmetic_v<T>) _PRINT("", std::string(thing))
}

// Warning Printout
template<typename T>
constexpr void PRINTWARN(const T& thing)
{
    if constexpr(std::is_arithmetic_v<T>) _PRINT("[WARNING]\t", std::to_string(thing))
    if constexpr(!std::is_arithmetic_v<T>) _PRINT("[WARNING]\t", std::string(thing))
}

// Error Printout
template<typename T>
constexpr void PRINTERR(const T& thing)
{
    if constexpr(std::is_arithmetic_v<T>) _PRINTERR(std::to_string(thing))
    if constexpr(!std::is_arithmetic_v<T>) _PRINTERR(std::string(thing))
}

#endif // DEBUGGING_H