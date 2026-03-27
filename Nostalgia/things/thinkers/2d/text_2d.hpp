#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <Nostalgia/things/thinkers/2d/visual_2d.hpp>

class Text2D : public Visual2D
{
public:
	SET_SUPER(Visual2D)
	SET_TYPEID(ThingType::Text2D)
    SET_VARIABLES_OVERRIDE
    GET_VARIABLES_OVERRIDE

    bool mDebugSolid{false};
    bool mDebugOutline{false};

    virtual Sarg Text() const;
    virtual void SetText(Sarg);

    virtual ID Font() const;
    virtual void SetFont(ID);

    virtual Farg<ColorRGB> Color() const;
    virtual void SetColor(Farg<ColorRGB>);

protected:
    ID mFontID{UID::f_Audiowide};
    std::string mText{};
    ColorRGB mColor{0.0, 0.0, 0.0};
};

#endif // TEXT_2D_H
