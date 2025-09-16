#ifndef SETUP_VARIABLES_H
#define SETUP_VARIABLES_H

#include "theatre/data_t.hpp"

class DoSetupVariables
{
public:
    virtual void SetupVariables(const data_t&) = 0;
};

#endif // SETUP_VARIABLES_H
