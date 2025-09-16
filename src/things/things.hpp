#ifndef THINGS_H
#define THINGS_H

#include "thing.hpp"
#include "fwd.hpp"

#include <memory>

std::shared_ptr<Thing>(*g_MakeThing(size_t Type))();

template<typename T>
std::shared_ptr<T> g_GetThing(id_t ID);

#endif // THINGS_H
