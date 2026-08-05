#ifndef NOSTALGIA_ENGINE_H
#define NOSTALGIA_ENGINE_H

// Versioning
#define NOSTALGIA_VERSION_MAJOR 0
#define NOSTALGIA_VERSION_MINOR 20
#define NOSTALGIA_VERSION_PATCH 0
#define NOSTALGIA_VERSION_STRING "0.20.0"

// Platform-specific includes & defines
#ifdef _WIN32
#   ifdef NOSTALGIA_DLL_EXPORT // Windows DLL export/import macro
#       define NGE __declspec(dllexport)
#   else
#       define NGE __declspec(dllimport)
#   endif
#   include <tchar.h>   // IWYU pragma: keep
#   include <windows.h> // IWYU pragma: keep
#   define DIV 1048576
#   define WIDTH 7
#elifdef linux
#   define NGE
#   include <unistd.h> // IWYU pragma: keep
#   include <stdlib.h> // IWYU pragma: keep
#   include <string.h> // IWYU pragma: keep
#endif // _WIN32 / linux

// C++ STL
#include <stdio.h>          // IWYU pragma: keep
#include <set>              // IWYU pragma: keep
#include <map>              // IWYU pragma: keep
#include <print>            // IWYU pragma: keep
#include <array>            // IWYU pragma: keep
#include <mutex>            // IWYU pragma: keep
#include <string>           // IWYU pragma: keep
#include <vector>           // IWYU pragma: keep
#include <memory>           // IWYU pragma: keep
#include <format>           // IWYU pragma: keep
#include <random>           // IWYU pragma: keep
#include <ranges>           // IWYU pragma: keep
#include <chrono>           // IWYU pragma: keep
#include <cstdlib>          // IWYU pragma: keep
#include <concepts>         // IWYU pragma: keep
#include <typeindex>        // IWYU pragma: keep
#include <stdexcept>        // IWYU pragma: keep
#include <type_traits>      // IWYU pragma: keep
#include <unordered_map>    // IWYU pragma: keep
#include <unordered_set>    // IWYU pragma: keep
#include <source_location>  // IWYU pragma: keep
#include <initializer_list> // IWYU pragma: keep

// Nostalgia Core
#include <Nostalgia/core/typedefs.hpp> // IWYU pragma: keep

NGE void say_hello_world();

#endif // NOSTALGIA_ENGINE_H
