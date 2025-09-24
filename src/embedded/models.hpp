#ifndef MODELS_H
#define MODELS_H

#include "things/id.hpp"

namespace Model
{
    constexpr unsigned char Error[] =
    {
        #embed "models/Error.obj"
    };
    constexpr unsigned char Ramiel[] =
    {
        #embed "models/Ramiel.obj"
    };
    constexpr unsigned char Cube[] =
    {
        #embed "models/Cube.obj"
    };

    namespace Name
    {
        constexpr const char* Error  = "ErrorModel";
        constexpr const char* Ramiel = "RamielModel";
        constexpr const char* Cube   = "DefaultCube";
    }

    namespace ID
    {
        constexpr id_t Error  = 0;
        constexpr id_t Cube   = 1;
        constexpr id_t Ramiel = 2;
    }
}

#endif // MODELS_H
