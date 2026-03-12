#ifndef NOSTALGIA_PRE_COMPILED_HEADER_H
#define NOSTALGIA_PRE_COMPILED_HEADER_H

#define __n_make_string_2(x) #x
#define __n_make_string(x) __n_make_string_2(x)

#define NOSTALGIA_VERSION_MAJOR 0
#define NOSTALGIA_VERSION_MINOR 5
#define NOSTALGIA_VERSION_PATCH 1
#define NOSTALGIA_VERSION_STRING \
    __n_make_string(NOSTALGIA_VERSION_MAJOR) "." \
    __n_make_string(NOSTALGIA_VERSION_MINOR) "." \
    __n_make_string(NOSTALGIA_VERSION_PATCH)

#ifdef _WIN32
#   include <windows.h> // IWYU pragma: keep
#   include <stdio.h>   // IWYU pragma: keep
#   include <tchar.h>   // IWYU pragma: keep
#   define DIV 1048576
#   define WIDTH 7
#endif // _WIN32

#ifdef linux
#   include <unistd.h> // IWYU pragma: keep
#   include <stdlib.h> // IWYU pragma: keep
#   include <stdio.h>  // IWYU pragma: keep
#   include <string.h> // IWYU pragma: keep
#endif

// C++ STL
#include <set>              // IWYU pragma: keep
#include <map>              // IWYU pragma: keep
#include <print>            // IWYU pragma: keep
#include <string>           // IWYU pragma: keep
#include <vector>           // IWYU pragma: keep
#include <memory>           // IWYU pragma: keep
#include <array>            // IWYU pragma: keep
#include <mutex>            // IWYU pragma: keep
#include <cstdlib>          // IWYU pragma: keep
#include <format>           // IWYU pragma: keep
#include <random>           // IWYU pragma: keep
#include <chrono>           // IWYU pragma: keep
#include <concepts>         // IWYU pragma: keep
#include <typeindex>        // IWYU pragma: keep
#include <stdexcept>        // IWYU pragma: keep
#include <type_traits>      // IWYU pragma: keep
#include <unordered_map>    // IWYU pragma: keep
#include <unordered_set>    // IWYU pragma: keep
#include <source_location>  // IWYU pragma: keep
#include <initializer_list> // IWYU pragma: keep

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <Nostalgia/thirdparty/glm/glm.hpp>
#include <Nostalgia/thirdparty/glm/gtc/quaternion.hpp>
#include <Nostalgia/thirdparty/glm/ext/matrix_clip_space.hpp>

// Nostalgia
#include <Nostalgia/core/types.hpp>                           // IWYU pragma: keep
#include <Nostalgia/core/farg.hpp>                            // IWYU pragma: keep
#include <Nostalgia/core/constexpr_string_hash.hpp>           // IWYU pragma: keep
#include <Nostalgia/core/id.hpp>                              // IWYU pragma: keep
#include <Nostalgia/core/uid.hpp>                             // IWYU pragma: keep
#include <Nostalgia/core/ansi_codes.hpp>                      // IWYU pragma: keep
#include <Nostalgia/core/ansi_sequence.hpp>                   // IWYU pragma: keep
#include <Nostalgia/core/message_labels.hpp>                  // IWYU pragma: keep
#include <Nostalgia/core/error.hpp>                           // IWYU pragma: keep
#include <Nostalgia/core/log.hpp>                             // IWYU pragma: keep
#include <Nostalgia/core/time.hpp>                            // IWYU pragma: keep
#include <Nostalgia/core/mutex.hpp>                           // IWYU pragma: keep
#include <Nostalgia/core/concepts.hpp>                        // IWYU pragma: keep
#include <Nostalgia/core/vector.hpp>                          // IWYU pragma: keep
#include <Nostalgia/core/macros.hpp>                          // IWYU pragma: keep
#include <Nostalgia/core/bitmask.hpp>                         // IWYU pragma: keep
#include <Nostalgia/core/smart_pointers.hpp>                  // IWYU pragma: keep
#include <Nostalgia/core/enum_prettifier.hpp>                 // IWYU pragma: keep
#include <Nostalgia/console/console.hpp>                      // IWYU pragma: keep
#include <Nostalgia/math/conversion.hpp>                      // IWYU pragma: keep
#include <Nostalgia/math/glm_concepts.hpp>                    // IWYU pragma: keep
#include <Nostalgia/math/glm_format.hpp>                      // IWYU pragma: keep
#include <Nostalgia/filesystem/filesystem.hpp>                // IWYU pragma: keep
#include <Nostalgia/filesystem/file_data.hpp>                 // IWYU pragma: keep
#include <Nostalgia/theatre/parser/number_parser.hpp>         // IWYU pragma: keep
#include <Nostalgia/theatre/thing_type.hpp>                   // IWYU pragma: keep
#include <Nostalgia/theatre/variable_registry.hpp>            // IWYU pragma: keep
#include <Nostalgia/theatre/thing_data.hpp>                   // IWYU pragma: keep
#include <Nostalgia/theatre/things/thing.hpp>                 // IWYU pragma: keep
#include <Nostalgia/theatre/things/resources/resource.hpp>    // IWYU pragma: keep
#include <Nostalgia/theatre/things/thinkers/thinker.hpp>      // IWYU pragma: keep

#endif // NOSTALGIA_PRE_COMPILED_HEADER_H
