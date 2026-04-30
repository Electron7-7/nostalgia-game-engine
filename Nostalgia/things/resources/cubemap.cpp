#include "./cubemap.hpp"
#include "things/thing_data.hpp"
#include "theatre/theatre.hpp"
#include "things/thing_factory.hpp"

using namespace TheatreFile;

Shared<Cubemap> Cubemap::CreateFromImages(std::initializer_list<Shared<Image>> inImages)
{
    auto output{DCast<Cubemap>(ThingFactory::MakeThing(ThingType::Cubemap, "Untitled_Cubemap"))};
    FAUTO first{inImages.begin()->get()};
    output->mBuffer =
        TextureBuffer::Create({TEXTURE_TYPE_CUBE, first->Width(), first->Height(), first->Format()});
    int i{0};
    for(FAUTO image : inImages)
    {
        if(image->Width() != first->Width()
            or image->Height() != first->Height()
            or image->Format() != first->Format()
            or image->Channels() != first->Channels())
        {
            print_error("Images must all have the same width, height, data format, and channels");
            break;
        }
        output->UpdateLayer(image, i++);
    }
    return output;
}

void Cubemap::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);
    Shared<Image> image{nullptr};
    for(uint i{0}; i < 6; ++i)
    {
        std::string variable_name{"Image" + std::to_string(i)};
        if(data.get_variable(mInitialImageUIDs->at(i), variable_name) == OK
            and Theatre::Current()->DerivedFrom(mInitialImageUIDs->at(i), ThingType::Image))
        {
            image = Theatre::Current()->GetResource<Image>(mInitialImageUIDs->at(i));
        }
        else if(data.get_variable(mInitialImagePaths->at(i), variable_name) == OK)
        {
            image = Image::CreateEmpty(0, 0);
            image->LoadFile(mInitialImagePaths->at(i));
        }
        else
            { continue; }
        if(i == 0)
        {
            mBuffer =
                TextureBuffer::Create({TEXTURE_TYPE_CUBE, image->Width(), image->Height(), image->Format()});
        }
        UpdateLayer(image, i);
    }
}

Shared<ThingData> Cubemap::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};
    for(uint i{0}; i < 6; ++i)
    {
        std::string _name{"Image" + std::to_string(i)};
        if(not data->set_variable(mInitialImageUIDs->at(i), _name))
            { data->set_variable(mInitialImagePaths->at(i), _name); }
    }
    return data;
}

Error Cubemap::UpdateLayer(Shared<Image> inImage, int inLayer)
{
    auto _format{mBuffer->GetFormat()};
    if(inLayer > 5 or inLayer < 0)
    {
        print_error("Invalid layer: {}", inLayer);
        return ERR_INVALID;
    }
    else if(inImage->Width() != _format.width
        or inImage->Height() != _format.height
        or inImage->Format() != _format.data_format)
    {
        print_error("Image parameters do not match Cubemap's parameters");
        print_debug("Image Params   - Width: {}, Height: {}, DataFormat: {}",
            inImage->Width(), inImage->Height(), EnumRegistry::GetEnumName(inImage->Format()));
        print_debug("Cubemap Params - Width: {}, Height: {}, DataFormat: {}",
            _format.width, _format.height, EnumRegistry::GetEnumName(_format.data_format));
        return ERR_INVALID;
    }

    mBuffer->SetData({inImage->data(), inLayer});
    return OK;
}
