#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include <memory>

struct FileData; // Forward Declaration

struct ResourceData
{
public:
    static bool Exists(const std::string& Name);
    static bool try_AddData(const std::string& Name, std::shared_ptr<const FileData> Data);
    static bool GetData(std::shared_ptr<const FileData> Output, const std::string& Name);
    static std::shared_ptr<const FileData> GetData(const std::string& Name);
};

namespace Fonts
{
    namespace Raw
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
    }

    extern constinit const FileData Audiowide_Regular;
    extern constinit const FileData DejaVuSansMono;
    extern constinit const FileData Verdana;
}

namespace Images
{
    namespace Raw
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
    }

    extern constinit const FileData COMP04_5;
    extern constinit const FileData Missing;
    extern constinit const FileData LolBit;
}

namespace Models
{
    namespace Raw
    {
        constexpr unsigned char Error[] =
        {
            #embed "models/Error.obj"
        };
        constexpr unsigned char Ramiel[] =
        {
            #embed "models/Ramiel.obj"
        };
    }

    extern constinit const FileData Error;
    extern constinit const FileData Ramiel;
}

namespace GLSL
{
    constexpr char BlinnPhong_Frag[] =
    {
        #embed "shaders/GLSL/BlinnPhong.frag" \
            suffix(,)
            0
    };
    constexpr char BlinnPhong_Vert[] =
    {
        #embed "shaders/GLSL/BlinnPhong.vert" \
            suffix(,)
            0
    };
    constexpr char SafetyShader_Frag[] =
    {
        #embed "shaders/GLSL/SafetyShader.frag" \
            suffix(,)
            0
    };
    constexpr char SafetyShader_Vert[] =
    {
        #embed "shaders/GLSL/SafetyShader.vert" \
            suffix(,)
            0
    };
}
#endif // RESOURCE_DATA_H
