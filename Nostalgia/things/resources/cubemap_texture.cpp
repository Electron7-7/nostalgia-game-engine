#include "./cubemap_texture.hpp"
#include "filesystem/image_handler.hpp"

using namespace TheatreFile;

void CubemapTexture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    m_pFileData = nullptr;
    for(uint i{0}; i < 6; ++i)
    {
        if(std::string path{}; data.get_variable(path, "Image" + std::to_string(i)) == OK)
        {
            m_pImages[i] = MakeShared<FileData>();
            print_error_enum(m_pImages[i]->LoadFile(path));
        }
    }
}

Shared<ThingData> CubemapTexture::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    uint i{0};
    for(FAUTO image : m_pImages)
    {
        if(!image) { continue; }
        else if(image->HasPath())
            { data->set_variable(image->Path(), "Image" + std::to_string(i)); }
    }
    return data;
}

Error CubemapTexture::Import()
{
    mStatus = FAILED;
    mTextureBuffer = TextureBuffer::Create();

    for(int i{0}; i < 6; ++i)
    {
        if(UID::GetReservedType(mUID()) == UID::ReservedType::Image)
            { VariableRegistry::try_GetResourceData(mUID() + i, m_pImages[i]); }

        if(m_pImages[i])
        {
            auto image_data{ImageHandler::Load(m_pImages[i], mFormat, mStatus)};

            if(print_error_enum(mStatus))
            {
                print_error("Failed to load image data for Texture ['{}', {}]",
                    mName,
                    mUID());
            }
            mStatus = mTextureBuffer->Load(image_data, mFormat);

            ImageHandler::Free(image_data);
        }
    }

    if(!print_error_enum(mStatus))
        { print_error("Failed to create Texture ['{}', {}]", mName, mUID()); }
    else
    {
        mTextureBuffer->GenerateMipMaps();
        mTextureBuffer->SetSamplerState(mSampler);
    }

    return mStatus;
}
