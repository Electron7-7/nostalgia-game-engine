#include "./cubemap.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"

using namespace TheatreFile;

Shared<Cubemap> Cubemap::CreateFromImages(std::initializer_list<Shared<Image>> inImages)
{
    auto output{DCast<Cubemap>(ThingFactory::MakeThing(ThingType::Cubemap, "Untitled_Cubemap"))};
    output->mFormat.type = TEXTURE_TYPE_CUBE;
    FAUTO first{inImages.begin()->get()};
    output->mFormat.data_format = first->Format();
    output->mFormat.width = first->Width();
    output->mFormat.height = first->Height();
    output->mFormat.channels = first->Channels();
    int i{0};
    for(FAUTO image : inImages)
    {
        if(output->mFormat.width != image->Width()
            or output->mFormat.height != image->Height()
            or output->mFormat.data_format != image->Format()
            or output->mFormat.channels != image->Channels())
        {
            print_error("Images must all have the same width, height, data format, and channels");
            break;
        }
        else if(i == 5)
            { output->mInitialized = true; }
        output->UpdateLayer(image, i++);
    }
    return output;
}

void Cubemap::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    mFormat.type = TEXTURE_TYPE_CUBE;
    ID image_uid{};
    std::string filepath{};
    Shared<Image> image{nullptr};
    for(uint i{0}; i < 6; ++i)
    {
        std::string variable_name{"Image" + std::to_string(i)};
        if(data.get_variable(image_uid, variable_name) == OK
            and Theatre::Current()->DerivedFrom(image_uid, ThingType::Image))
        {
            image = Theatre::Current()->GetResource<Image>(image_uid);
        }
        else if(data.get_variable(filepath, variable_name) == OK)
        {
            image = Image::CreateEmpty(0, 0);
            image->LoadFile(filepath);
        }
        else
            { continue; }
        if(i == 0)
        {
            mFormat.data_format = image->Format();
            mFormat.width = image->Width();
            mFormat.height = image->Height();
        }
        UpdateLayer(image, i);
        mInitialImageIDs[i] = image_uid;
    }
}

Shared<ThingData> Cubemap::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    for(uint i{0}; i < 6; ++i)
        { data->set_variable(mInitialImageIDs[i], "Image" + std::to_string(i)); }
    return data;
}

Error Cubemap::UpdateLayer(Shared<Image> inImage, int inLayer)
{
    if(not mTextureBuffer)
        { mTextureBuffer = TextureBuffer::Create(); }

    if(inLayer > 5 or inLayer < 0)
    {
        print_error("Invalid layer: {}", inLayer);
        return ERR_INVALID;
    }
    else if(mFormat.width != inImage->Width()
        or mFormat.height != inImage->Height()
        or mFormat.data_format != inImage->Format()
        or mFormat.channels != inImage->Channels())
    {
        print_error("Image parameters do not match ImageTexture's parameters");
        print_debug("Image Params        - Width: {}, Height: {}, Channels: {}, DataFormat: {}",
            inImage->Width(),inImage->Height(),inImage->Channels(),EnumRegistry::GetEnumName(inImage->Format()));
        print_debug("ImageTexture Params - Width: {}, Height: {}, Channels: {}, DataFormat: {}",
            mFormat.width, mFormat.height, mFormat.channels, EnumRegistry::GetEnumName(mFormat.data_format));
        return ERR_INVALID;
    }

    if(not print_error_enum(mTextureBuffer->Load(inImage->raw_data(), mFormat, inLayer)))
    {
        print_error("Failed to create Cubemap ['{}', {}]", name(), uid()());
        return ERR_DATA_LOAD;
    }
    else if(mInitialized)
    {
        if(not mFormat.mipmaps)
        {
            mSampler.mip_filter_min = SAMPLER_FILTER_NONE;
            mTextureBuffer->SetSamplerState(mSampler);
        }
        else
        {
            mTextureBuffer->SetSamplerState(mSampler);
            mTextureBuffer->GenerateMipMaps();
        }
    }
    return OK;
}
