#ifndef THINGS_H
#define THINGS_H

#include "thing.hpp"

#include <memory>

std::shared_ptr<Thing>(*g_MakeThing(size_t Type))();

#endif // THINGS_H
