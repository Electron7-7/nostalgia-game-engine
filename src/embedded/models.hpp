#ifndef MODELS_H
#define MODELS_H


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

}

#endif // MODELS_H
