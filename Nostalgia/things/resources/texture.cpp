#include "./texture.hpp"
#include "theatre/thing_data.hpp"
#include "things/thing_factory.hpp"
#include "rendering/texture_buffer.hpp"
// #include "filesystem/image_handler.hpp"

using namespace TheatreFile;

Shared<Texture> Texture::CreateFromMemory(const uchar* inData, size_t inSize)
{
    Shared<Texture> output{DCast<Texture>(ThingFactory::MakeResource(ThingType::Texture, "Untitled_Texture"))};
    output->m_pImage = MakeShared<FileData>(inData, inSize);
    output->Import();
    return output;
}

void Texture::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    data.get_variable(m_pImage, "Image", "File", "Data", "Path");

    data.get_variable(mFormat.type, "Type");
    data.get_variable(mFormat.data_format, "Format");
    data.get_variable(mFormat.width, "Width");
    data.get_variable(mFormat.height, "Height");
    data.get_variable(mFormat.depth, "Depth");
    data.get_variable(mFormat.array_layers, "ArrayLayers");
    data.get_variable(mFormat.mipmaps, "MipMaps");
    data.get_variable(mSampler.min_filter, "SamplerMinFilter", "MinFilter", "Min");
    data.get_variable(mSampler.mip_filter_min, "SamplerMipFilterMin", "MipFilterMin", "MipMin");
    data.get_variable(mSampler.mag_filter, "SamplerMagFilter", "MagFilter", "Mag");
    data.get_variable(mSampler.repeat_u, "SamplerRepeatU", "SamplerRepeatX", "RepeatU", "RepeatX");
    data.get_variable(mSampler.repeat_v, "SamplerRepeatV", "SamplerRepeatY", "RepeatV", "RepeatY");
    data.get_variable(mSampler.repeat_w, "SamplerRepeatW", "SamplerRepeatZ", "RepeatW", "RepeatZ");
    data.get_variable(mSampler.use_anisotropy, "UseAnisotropy", "AnisotropyEnabled");
    data.get_variable(mSampler.anisotropy_max, "AnisotropyMax", "Anisotropy");
    // data.get_variable(mBoundToFramebuffer, "Bound to Framebuffer");

    if(m_pImage)
        { Import(); }
}

Shared<ThingData> Texture::GetVariables() const
{
    auto data{Super::GetVariables()};
    data->set_variable(m_pImage, "Image");
    data->set_variable(mFormat.type, "Type");
    data->set_variable(mFormat.data_format, "Format");
    data->set_variable(mFormat.width, "Width");
    data->set_variable(mFormat.height, "Height");
    data->set_variable(mFormat.depth, "Depth");
    data->set_variable(mFormat.array_layers, "ArrayLayers");
    data->set_variable(mFormat.mipmaps, "MipMaps");
    data->set_variable(mSampler.min_filter, "SamplerMinFilter");
    data->set_variable(mSampler.mip_filter_min, "SamplerMipFilterMin");
    data->set_variable(mSampler.mag_filter, "SamplerMagFilter");
    data->set_variable(mSampler.repeat_u, "SamplerRepeatU");
    data->set_variable(mSampler.repeat_v, "SamplerRepeatV");
    data->set_variable(mSampler.repeat_w, "SamplerRepeatW");
    data->set_variable(mSampler.use_anisotropy, "UseAnisotropy");
    data->set_variable(mSampler.anisotropy_max, "AnisotropyMax");
    // data->set_variable(mBoundToFramebuffer, "Bound to Framebuffer");
    return data;
}

Error Texture::Import()
{
    mTextureBuffer = TextureBuffer::Create();
    return UNIMPLEMENTED;
    /*if(not m_pImage)
    {
        print_error("No image data found/loaded");
        return ERR_EMPTY;
    }

    Error status{};
    auto image_data{ImageHandler::Load(m_pImage, mFormat, status)};

    if(print_error_enum(status))
    {
        print_error("Failed to load image data for Texture ['{}', {}]",
            mName,
            uid()());
    }
    mTextureBuffer->Load(image_data, mFormat);

    if(!print_error_enum(status))
        { print_error("Failed to create Texture ['{}', {}]", mName, uid()()); }
    else
    {
        mTextureBuffer->GenerateMipMaps();
        mTextureBuffer->SetSamplerState(mSampler);
    }

    ImageHandler::Free(image_data);
    return status;*/
}

Farg<TextureFormat> Texture::Format() const
{ return mFormat; }

void Texture::SetFormat(Farg<TextureFormat> inFormat)
{ mFormat = inFormat; }

Farg<SamplerState> Texture::Sampler() const
{ return mSampler; }

void Texture::SetSampler(Farg<SamplerState> inSampler)
{ mSampler = inSampler; }

Shared<TextureBuffer> Texture::GetBuffer() const
{ return mTextureBuffer; }

Error Texture::SetBuffer(Shared<TextureBuffer> inBuffer)
{
    if(Error status{print_error_enum(inBuffer->Status())}; not status)
    {
        print_warning("Failed to replace the TextureBuffer in Texture ['{}', {}]",
            mName,
            uid()());
        return status;
    }
    mTextureBuffer = inBuffer;
    return OK;
}
