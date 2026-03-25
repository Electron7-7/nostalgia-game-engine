#include "./cubemap_texture.hpp"
#include "filesystem/image_handler.hpp"

using namespace TheatreFile;

void CubemapTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    for(uint i{0}; i < 6; ++i)
        { data.get_variable(m_pImages[i], "Image" + std::to_string(i)); }
}

Shared<ThingData> CubemapTexture::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    for(uint i{0}; i < 6; ++i)
        { data->set_variable(m_pImages[i], "Image" + std::to_string(i)); }
    return data;
}

void CubemapTexture::Init()
{
    for(uint i{0}; i < 6; ++i)
        { m_pImages[i] = MakeShared<FileData>(); }
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
            print_error("Failed to load image data for Texture ['{}', {}]",
                mName,
                mUID());
        }
        mTextureBuffer->Load(image_data, mFormat);
        ImageHandler::Free(image_data);
    }

    if(!print_error_enum(status))
        { print_error("Failed to create Texture ['{}', {}]", mName, mUID()); }
    else
    {
        mTextureBuffer->GenerateMipMaps();
        mTextureBuffer->SetSamplerState(mSampler);
    }

    return status;
}
