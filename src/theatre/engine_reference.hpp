#ifndef ENGINE_REFERENCE_H
#define ENGINE_REFERENCE_H

#include "safe_return.hpp"

#include <string>
#include <any>

bool IsEngineReference(const std::string& Reference);
bool try_AddEngineReference(const std::string& Reference, std::any Value);
SafeReturn<std::any> try_GetEngineReference(const std::string& Reference);

#endif // ENGINE_REFERENCE_H
