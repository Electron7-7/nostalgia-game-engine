#include "./camera_2d.hpp"
#include "../viewport.hpp"
#include "application/application.hpp"
#include "things/thing_data.hpp"
#include "settings/graphics.hpp"
#include "theatre/theatre.hpp"
#include "settings/engine.hpp"
#include "things/resources/texture.hpp"
#include "things/resources/font.hpp"
#include "rendering/renderer_api.hpp"
#include "things/resources/array_mesh.hpp"
#include "rendering/shader.hpp"
#include "things/resource_database.hpp"
#include "things/thinkers/2d/sprite_2d.hpp"
#include "things/thinkers/2d/text_2d.hpp"

using namespace TheatreFile;

void Camera2D::Ready()
{
    Super::Ready();
    auto parent{Theatre::Current()->GetParent(uid())};

    if(mViewportID.invalid())
    {
        if(not parent.invalid())
        {
            if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                { mViewportID = parent; }
            else
            {
                auto ancestors{Theatre::Current()->GetAllParents(uid())};
                for(ID parent : ancestors)
                {
                    if(Theatre::Current()->DerivedFrom(parent, ThingType::Viewport))
                        { mViewportID = parent; break; }
                    else if(Theatre::Current()->DerivedFrom(parent, ThingType::NostalgiaPlayer))
                        { mInitCurrent = true; }
                }
            }
        }
    }

    if(mInitCurrent)
        { Theatre::Current()->SetCurrentCamera(uid(), mViewportID); }
}

void Camera2D::SetVariables(Farg<ThingData> data)
{
    Super::SetVariables(data);

    data.get_variable(mLayersMask, "RenderLayersMask", "LayersMask", "RenderLayers");
    if(glm::vec2 _zoom{mZoom}; data.get_variable(_zoom, "Zoom") == OK)
        { SetZoom(_zoom); } // Make sure mZoom never contains a 0
    data.get_variable(mInitCurrent, "Current", "CurrentCamera", "IsCurrent");

    mVisible = mVisible and Settings::Engine::IsEditorHint;
}

Shared<ThingData> Camera2D::GetVariables() const
{
    Shared<ThingData> data{Super::GetVariables()};

    data->set_variable(mLayersMask, "RenderLayersMask");
    data->set_variable(mZoom, "Zoom");
    data->set_variable(mInitCurrent, "Current");

    return data;
}

void Camera2D::Draw(Shared<Visual2D> inVisual2D) const
{
    FAUTO renderer_api{RendererAPI::Get()};

    auto missing_texture{ResourceDatabase::GetResource<Texture>(UID::i_Missing)};
    auto quad_mesh{ResourceDatabase::GetResource<ArrayMesh>(UID::m_Quad)};

    auto shader{renderer_api->GetShader(Shaders::Fast2D)};
    if(not inVisual2D->Visible()
        or not LayersMask().contains(inVisual2D->Layers()))
            { return; }
    else if(inVisual2D->DerivedFrom(ThingType::Sprite2D))
    {
        auto sprite{DCast<Sprite2D>(inVisual2D)};
        auto texture{sprite->GetTexture()};

        glm::vec2 texture_size{texture->Format().width, texture->Format().height};

        if(texture and not texture->GetBuffer())
            { texture = missing_texture; }

        auto size{sprite->GlobalScale() * texture_size};

        auto model_matrix{glm::translate(glm::mat4{1.0f}, glm::vec3{sprite->GlobalPosition(), 0.0f})};
        model_matrix = glm::rotate(model_matrix, sprite->GlobalRotation(), glm::vec3{0.0f, 0.0f, -1.0f});
        model_matrix = glm::scale(model_matrix, glm::vec3{size, 1.0f});

        bool use_texture {renderer_api->BindTexture(texture, 0)};

        renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or sprite->Wireframe());
        renderer_api->SetFramebufferSRGB(use_texture);

        shader->SetUniform("model_matrix", model_matrix);
        shader->SetUniform("projection_matrix", ProjectionMatrix());
        shader->SetUniform("view_matrix", ViewMatrix());
        shader->SetUniform("view_position", GlobalPosition());
        shader->SetUniform("sprite_texture",  0);
        shader->SetUniform("use_texture",  use_texture);
        shader->SetUniform("sprite_color", glm::vec3{1.0f});
        shader->SetUniform("debug_highlight", sprite->DebugHighlight());

        shader->Bind();
        for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
            { renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }
        renderer_api->SetFramebufferSRGB(false);
    }
    else if(inVisual2D->DerivedFrom(ThingType::Text2D))
    {
        auto text2d{DCast<Text2D>(inVisual2D)};
        auto font{ResourceDatabase::GetResource<Font>(text2d->Font())};
        auto shader{renderer_api->GetShader(Shaders::Fonts)};

        glm::mat4 default_model{1.0f};
        default_model = glm::translate(default_model, {text2d->GlobalPosition(), 0.0f});
        default_model = glm::rotate(default_model, text2d->GlobalRotation(), {0.0f, 0.0f, -1.0f});
        default_model = glm::scale(default_model, {text2d->GlobalScale(), 0.0f});

        shader->SetUniform("debug_highlight", text2d->DebugHighlight());
        shader->SetUniform("projection_matrix", ProjectionMatrix());
        shader->SetUniform("model_matrix", default_model);
        shader->SetUniform("view_matrix", ViewMatrix());
        shader->SetUniform("view_position", GlobalPosition());
        shader->SetUniform("glyph", 0);
        shader->Bind();

        if(not Console::GetVariable("Theatre.draw_text_new")->int_value)
        {
            if(text2d->mDebugOutline)
            {
                shader->SetUniform("debug_solid", 1);
                shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f});
                renderer_api->SetWireframe(true);
                renderer_api->DrawText(text2d->Text(), font, glm::vec2{0.0f}, glm::vec2{1.0f});
            }

            shader->SetUniform("debug_solid", text2d->mDebugSolid);
            shader->SetUniform("text_color", text2d->Color().glm());

            renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or text2d->Wireframe());
            renderer_api->DrawText(text2d->Text(), font, glm::vec2{0.0f}, glm::vec2{1.0f});
        }
        else
        {
            FAUTO text{text2d->Text()};
            glm::vec2 position{0.0f, 0.0f};
            auto glyph_top{font->GetGlyph('H').bitmap_top};
            for(auto c{text.cbegin()}; c != text.cend(); ++c)
            {
                FAUTO glyph{font->GetGlyph(*c)};
                if(!glyph.texture) { continue; }

                glm::vec2 pos{
                    (c != text.cbegin()) * position.x + glyph.bitmap_left,
                    (c != text.cbegin()) * position.y - (glyph_top - glyph.bitmap_top),
                },
                scale{
                    glyph.bitmap_width,
                    glyph.bitmap_height,
                };

                glm::mat4 model_matrix{1.0f};
                model_matrix = glm::translate(model_matrix, {text2d->GlobalPosition(), 0.0f});
                model_matrix = glm::rotate(model_matrix, text2d->GlobalRotation(), {0.0f, 0.0f, -1.0f});
                model_matrix = glm::scale(model_matrix, {text2d->GlobalScale(), 0.0f});

                model_matrix = glm::translate(model_matrix, {pos, 0.0f});
                model_matrix = glm::scale(model_matrix, {scale, 0.0f});

                shader->SetUniform("debug_solid", 0);
                shader->SetUniform("text_color", text2d->Color().glm());
                shader->SetUniform("model_matrix", model_matrix);

                renderer_api->BindTexture(glyph.texture, 0);
                renderer_api->SetWireframe(false);

                for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
                    { renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }

                if(text2d->mDebugSolid)
                {
                    shader->SetUniform("debug_solid", 1);
                    if(text2d->Wireframe() and text2d->mDebugSolid)
                        { shader->SetUniform("text_color", glm::vec3{1.0f, 0.4f, 1.0f}); }
                    renderer_api->SetWireframe(Settings::Graphics::GlobalWireframe or text2d->Wireframe());
                    for(int i{0}; i < quad_mesh->SurfaceCount(); ++i)
                        { renderer_api->DrawIndexed(quad_mesh->SurfaceGetVertexArray(i)); }
                }

                position.x += (glyph.advance_x >> 6);
                position.y -= (glyph.advance_y >> 6);
            }
        }
    }
}

ID Camera2D::ViewportID() const
{ return mViewportID; }

bool Camera2D::Current() const
{ return Theatre::Current()->GetCurrentCamera2D(mViewportID) == uid(); }

Error Camera2D::SetCurrent(bool isCurrent)
{
    if(isCurrent == Current())
        { return OK; }
    return Theatre::Current()->SetCurrentCamera(uid(), mViewportID);
}

BitMask Camera2D::LayersMask() const
{ return mLayersMask; }

void Camera2D::SetLayersMask(BitMask inLayersMask)
{ mLayersMask = inLayersMask; }

void Camera2D::OnAncestorRemoved(Relative inAncestor)
{
    Super::OnAncestorRemoved(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = UID::o_RootViewport; }
}

void Camera2D::OnAncestorAdded(Relative inAncestor)
{
    Super::OnAncestorAdded(inAncestor);
    if(Theatre::Current()->DerivedFrom(inAncestor.uid, ThingType::Viewport))
        { mViewportID = inAncestor.uid; }
}

glm::mat4 Camera2D::ViewMatrix() const
{
    Size2D viewport_size{(mViewportID.invalid())
        ? MainWindow()->GetScale()
        : Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
    viewport_size[1] *= -1.0f;

    glm::vec3 _position{GlobalPosition(), 0.0f};
    glm::vec3 _center{GlobalPosition() + (viewport_size.glm() / 2.0f), 0.0f};

    auto _view{glm::lookAt(_position,
        {GlobalPosition(), 1.0f},
        {0.0f, -1.0f, 0.0f})};

    _view = glm::translate(_view, _center);
    _view = glm::scale(_view, {mZoom, 1.0f});
    _view = glm::translate(_view, -_center);

    return _view;
}

glm::mat4 Camera2D::ProjectionMatrix() const
{
    Size2D viewport_size{(mViewportID.invalid())
        ? MainWindow()->GetScale()
        : Theatre::Current()->GetThinker<Viewport>(mViewportID)->Size()};
    float left{0.0f}, right{(float)viewport_size[0]}, up{0.0f}, down{(float)viewport_size[1]},
        _aspect_ratio{static_cast<float>(viewport_size.AspectRatio())};
    if(mViewportID == UID::o_RootViewport
        and Settings::Graphics::Stretch::Mode == Settings::Graphics::StretchMode::Viewport)
    {

        switch(Settings::Graphics::Stretch::Aspect)
        {
        case Settings::Graphics::StretchAspect::Ignore:
        default:
            break;
        case Settings::Graphics::StretchAspect::Keep:
            // https://gamedev.stackexchange.com/a/49698
            left = right = 0.0f;
            up = viewport_size[0];
            if(_aspect_ratio < 1.0f)
            {
                down /= _aspect_ratio;
                up   /= _aspect_ratio;
            }
        }
    }

    return glm::ortho(
        left,
        right,
        down,
        up,
        -1.0f,
        1.0f);
}

Farg<glm::vec2> Camera2D::Zoom() const
{ return mZoom; }

void Camera2D::SetZoom(Farg<glm::vec2> inZoom)
{
    if(not inZoom[0] or not inZoom[1])
        { print_warning("Zoom values cannot be 0!"); }

    mZoom[0] = (inZoom[0]) ? inZoom[0] : mZoom[0];
    mZoom[1] = (inZoom[1]) ? inZoom[1] : mZoom[1];
}

void Camera2D::SetZoom(float inUniformZoom)
{ SetZoom(glm::vec2{inUniformZoom}); }
