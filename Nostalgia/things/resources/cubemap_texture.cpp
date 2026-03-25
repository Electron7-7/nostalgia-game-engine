#include "./cubemap_texture.hpp"
#include "things/thing_factory.hpp"
#include "filesystem/image_handler.hpp"

using namespace TheatreFile;

Shared<CubemapTexture> CubemapTexture::CreateFromMemory(std::initializer_list<FileData> inImages,
    ID inUID, Sarg inName)
{
    auto output{DCast<CubemapTexture>(ThingFactory::MakeResource(ThingType::CubemapTexture, inName, inUID))};
    output->mFormat.type = TEXTURE_TYPE_CUBE;
    int i{0};
    for(FAUTO image : inImages)
        { output->m_pImages[i++] = MakeShared<FileData>(image); }
    output->Import();
    return output;
}

void CubemapTexture::SetVariables(Farg<ThingData> data)
{
    m_pImage = nullptr;
    Super::SetVariables(data);
    for(uint i{0}; i < 6; ++i)
    {
        if(not m_pImages[i])
            { m_pImages[i] = MakeShared<FileData>(); }
        data.get_variable(m_pImages[i], "Image" + std::to_string(i));
    }

    Import();
}

Shared<ThingData> CubemapTexture::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    for(uint i{0}; i < 6; ++i)
        { data->set_variable(m_pImages[i], "Image" + std::to_string(i)); }
    return data;
}

Error CubemapTexture::Import()
{
    Error status{};

    mTextureBuffer = TextureBuffer::Create();

    for(int i{0}; i < 6; ++i)
    {
        auto image_data{ImageHandler::Load(m_pImages[i], mFormat, status)};

        if(print_error_enum(status))
        {
            print_error("Failed to load image data for CubemapTexture ['{}', {}]",
                mName,
                mUID());
        }
        mTextureBuffer->Load(image_data, mFormat);
        ImageHandler::Free(image_data);
    }

    if(!print_error_enum(status))
        { print_error("Failed to create CubemapTexture ['{}', {}]", mName, mUID()); }
    else
    {
        mTextureBuffer->GenerateMipMaps();
        mTextureBuffer->SetSamplerState(mSampler);
    }

    return status;
}
