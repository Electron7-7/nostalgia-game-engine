#include "./text_2d.hpp"
#include "things/thing_data.hpp"

using namespace TheatreFile;

void Text2D::InitVariables()
{
    Super::InitVariables();
}

void Text2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mDebugSolid, "Solid");
    data.get_variable(mDebugOutline, "Outline");
    data.get_variable(mFontID, "Font");
    data.get_variable(mText, "Text");
    if(glm::vec3 color{}; data.get_variable(color, "Color") == OK)
        { mColor = color; }
}

Shared<ThingData> Text2D::GetVariables() const
{
    auto data{Super::GetVariables()};

    data->set_variable(mDebugSolid, "Solid");
    data->set_variable(mDebugOutline, "Outline");
    data->set_variable(mFontID, "Font");
    data->set_variable(mText, "Text");
    data->set_variable(mColor.glm(), "Color");

    return data;
}

Sarg Text2D::Text() const
{ return mText; }

void Text2D::SetText(Sarg inText)
{ mText = inText; }

ID Text2D::Font() const
{ return mFontID; }

void Text2D::SetFont(ID inFontID)
{ mFontID = inFontID; }

Farg<ColorRGB> Text2D::Color() const
{ return mColor; }

void Text2D::SetColor(Farg<ColorRGB> inColor)
{ mColor = inColor; }
