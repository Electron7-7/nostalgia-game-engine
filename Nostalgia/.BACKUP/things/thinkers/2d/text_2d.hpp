#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>

class Text2D : public Visual2D
{
public:
	THING_CLASS(Text2D, Visual2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    bool mDebugSolid{false};
    bool mDebugOutline{false};

    virtual Sarg Text() const;
    virtual void SetText(Sarg);

    virtual ID Font() const;
    virtual void SetFont(ID);

    virtual Farg<ColorRGB<float>> Color() const;
    virtual void SetColor(Farg<ColorRGB<float>>);

protected:
    ID mFontID{UID::f_Audiowide};
    std::string mText{};
    ColorRGB<float> mColor{0.0, 0.0, 0.0};
};

#endif // TEXT_2D_H
