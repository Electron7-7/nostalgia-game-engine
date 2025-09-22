#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include "filesystem/fwd.hpp"

#include <string>

struct ResourceData
{
public:
    static bool Exists(const std::string& Name);
    static void AddData(const std::string& Name, const FileData& Data);
    static bool GetData(FileData& Output, const std::string& Name);
};

namespace Fonts
{
    constexpr unsigned char Audiowide_Regular[] =
    {
        #embed "fonts/Audiowide-Regular.ttf"
    };
    constexpr unsigned char DejaVuSansMono[] =
    {
        #embed "fonts/DejaVuSansMono.ttf"
    };
    constexpr unsigned char Verdana[] =
    {
        #embed "fonts/Verdana.ttf"
    };

    namespace Name
    {
        constexpr const char* AudiowideRegular = "AudiowideRegularFont";
        constexpr const char* Verdana          = "VerdanaFont";
        constexpr const char* DejaVuSansMono   = "DejaVuSansMonoFont";
    }

    namespace ID
    {
        constexpr id_t Verdana          = 0;
        constexpr id_t DejaVuSansMono   = 1;
        constexpr id_t AudiowideRegular = 2;
    }
}

namespace Images
{
    constexpr unsigned char COMP04_5[] =
    {
        #embed "images/COMP04_5.png"
    };
    constexpr unsigned char Missing[] =
    {
        #embed "images/Missing.jpg"
    };
    constexpr unsigned char LolBit[] =
    {
        #embed "images/lolbit.png"
    };
    constexpr unsigned char LightDebugging[] =
    {
        #embed "images/LIGHT_DEBUGGING.jpg"
    };

    namespace Name
    {
        constexpr const char* Missing        = "MissingTexture";
        constexpr const char* COMP04_5       = "DoomTexture";
        constexpr const char* LolBit         = "LolBitTexture";
        constexpr const char* LightDebugging = "LightTexture";
    }

    namespace ID
    {
        constexpr id_t Missing        = 0;
        constexpr id_t LightDebugging = 1;
        constexpr id_t COMP04_5       = 2;
        constexpr id_t LolBit         = 3;
    }

}

namespace Models
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

#endif // RESOURCE_DATA_H
