#ifndef NOSTALGIA_PRE_COMPILED_HEADER_H
#define NOSTALGIA_PRE_COMPILED_HEADER_H

// C++ STL
#include <set>              // IWYU pragma: keep
#include <map>              // IWYU pragma: keep
#include <print>            // IWYU pragma: keep
#include <string>           // IWYU pragma: keep
#include <vector>           // IWYU pragma: keep
#include <memory>           // IWYU pragma: keep
#include <format>           // IWYU pragma: keep
#include <chrono>           // IWYU pragma: keep
#include <concepts>         // IWYU pragma: keep
#include <stdexcept>        // IWYU pragma: keep
#include <type_traits>      // IWYU pragma: keep
#include <sys/types.h>      // IWYU pragma: keep
#include <unordered_map>    // IWYU pragma: keep
#include <unordered_set>    // IWYU pragma: keep
#include <source_location>  // IWYU pragma: keep
#include <initializer_list> // IWYU pragma: keep

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// Nostalgia
#include "core/id.hpp"                    // IWYU pragma: keep
#include "core/uid.hpp"                   // IWYU pragma: keep
#include "core/log.hpp"                   // IWYU pragma: keep
#include "core/farg.hpp"                  // IWYU pragma: keep
#include "core/time.hpp"                  // IWYU pragma: keep
#include "core/error.hpp"                 // IWYU pragma: keep
#include "core/mutex.hpp"                 // IWYU pragma: keep
#include "core/macros.hpp"                // IWYU pragma: keep
#include "core/bitmask.hpp"               // IWYU pragma: keep
#include "core/concepts.hpp"              // IWYU pragma: keep
#include "core/ansi_codes.hpp"            // IWYU pragma: keep
#include "core/ansi_sequence.hpp"         // IWYU pragma: keep
#include "core/message_labels.hpp"        // IWYU pragma: keep
#include "core/smart_pointers.hpp"        // IWYU pragma: keep
#include "core/enum_prettifier.hpp"       // IWYU pragma: keep
#include "core/constexpr_string_hash.hpp" // IWYU pragma: keep

#endif // NOSTALGIA_PRE_COMPILED_HEADER_H
