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
        constexpr const char* Verdana = "VerdanaFont";
        constexpr const char* DejaVuSansMono = "DejaVuSansMonoFont";
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
    constexpr unsigned char Empty[] =
    {
        #embed "images/Empty.png"
    };

    namespace Name
    {
        constexpr const char* Missing = "MissingTexture";
        constexpr const char* COMP04_5 = "DoomTexture";
        constexpr const char* LolBit = "LolBitTexture";
        constexpr const char* Empty = "EmptyTexture";
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

    namespace Name
    {
        constexpr const char* Error = "ErrorModel";
        constexpr const char* Ramiel = "RamielModel";
    }
}

#endif // RESOURCE_DATA_H
