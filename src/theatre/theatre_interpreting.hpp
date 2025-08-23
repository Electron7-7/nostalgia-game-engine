#ifndef THEATRE_INTERPRETING_H
#define THEATRE_INTERPRETING_H

#include "safe_return.hpp"

#include <string>

struct Theatre; // Forward Declaration

SafeStatus try_LoadTheatreFile(const std::string& TheatreFile);
Theatre    InterpretTheatreFile();

#endif // THEATRE_INTERPRETING_H
