#ifndef RESOURCES_H
#define RESOURCES_H

#include "resource.hpp"

#include <memory>

std::shared_ptr<Resource>(*g_MakeResource(size_t Type))();

#endif // RESOURCES_H
