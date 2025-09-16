#ifndef RESOURCE_DATA_H
#define RESOURCE_DATA_H

#include "filesystem/file_data.hpp"

#include <memory>

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
            #embed <fonts/Audiowide-Regular.ttf>
        };
        constexpr unsigned char DejaVuSansMono[] =
        {
            #embed <fonts/DejaVuSansMono.ttf>
        };
        constexpr unsigned char Verdana[] =
        {
            #embed <fonts/Verdana.ttf>
        };
    }

    constexpr FileData Audiowide_Regular(FileType::font_TTF, Raw::Audiowide_Regular);
    constexpr FileData DejaVuSansMono(FileType::font_TTF, Raw::DejaVuSansMono);
    constexpr FileData Verdana(FileType::font_TTF, Raw::Verdana);
}

namespace Images
{
    namespace Raw
    {
        constexpr unsigned char COMP04_5[] =
        {
            #embed <images/COMP04_5.png>
        };
        constexpr unsigned char Missing[] =
        {
            #embed <images/Missing.jpg>
        };
    }

    constexpr FileData COMP04_5(FileType::image_PNG, Raw::COMP04_5);
    constexpr FileData Missing(FileType::image_JPG, Raw::Missing);
}

namespace Models
{
    namespace Raw
    {
        constexpr unsigned char Error[] =
        {
            #embed <models/Error.obj>
        };
        constexpr unsigned char Ramiel[] =
        {
            #embed <models/Ramiel.obj>
        };
    }

    constexpr FileData Error(FileType::model_OBJ, Raw::Error);
    constexpr FileData Ramiel(FileType::model_OBJ, Raw::Ramiel);
}

namespace GLSL
{
    constexpr char BlinnPhong_Frag[] =
    {
        #embed <shaders/GLSL/BlinnPhong.frag> \
            suffix(,)
            0
    };
    constexpr char BlinnPhong_Vert[] =
    {
        #embed <shaders/GLSL/BlinnPhong.vert> \
            suffix(,)
            0
    };
    constexpr char SafetyShader_Frag[] =
    {
        #embed <shaders/GLSL/SafetyShader.frag> \
            suffix(,)
            0
    };
    constexpr char SafetyShader_Vert[] =
    {
        #embed <shaders/GLSL/SafetyShader.vert> \
            suffix(,)
            0
    };
}
#endif // RESOURCE_DATA_H
