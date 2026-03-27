#ifndef THING_TYPE_H
#define THING_TYPE_H

// These PIDs only exist to avoid passing string literals to functions like `ThingFactory::IsDerivedFrom`
namespace ThingType
{
    inline const PID Thing{"Thing"},
        Resource{"Resource"},
            Font{"Font"},
            Mesh{"Mesh"},
            Texture{"Texture"},
            CubemapTexture{"CubemapTexture"},
            ViewportTexture{"ViewportTexture"},
            Material{"Material"},
        Thinker{"Thinker"},
            Viewport{"Viewport"},
            Actor3D{"Actor3D"},
                NostalgiaPlayer3D{"NostalgiaPlayer3D"},
                Camera3D{"Camera3D"},
                Collider3D{"Collider3D"},
                Visual3D{"Visual3D"},
                    MeshInstance3D{"MeshInstance3D"},
                    Sprite3D{"Sprite3D"},
                    Light3D{"Light3D"},
                        PointLight3D{"PointLight3D"},
                        SpotLight3D{"SpotLight3D"},
                        DirectionalLight3D{"DirectionalLight3D"},
            Actor2D{"Actor2D"},
                NostalgiaPlayer2D{"NostalgiaPlayer2D"},
                Camera2D{"Camera2D"},
                Visual2D{"Visual2D"},
                    Sprite2D{"Sprite2D"},
                    Text2D{"Text2D"},
        Invalid{ID::Invalid, "Invalid"};
}

struct ThingType_t
{
private:
    PID _type_id{};
    PID _base_type_id{};
    std::set<PID> _all_base_types{};

    friend class ThingFactory;
    ThingType_t(FPID) noexcept;

public:
    using typeids_t = std::set<PID>;

    ThingType_t() noexcept;

    Farg<typeids_t> base_types() const noexcept;
    PID base_type() const noexcept;
    PID type() const noexcept;
    PID operator()() const noexcept;
    bool is_derived_from(FPID inTypeID) const noexcept;
    std::string log() const noexcept;

    bool operator<(Farg<ThingType_t> inOther) const noexcept
    { return _type_id < inOther._type_id; }

    bool operator==(Farg<ThingType_t> inOther) const noexcept
    { return _type_id == inOther._type_id; }
};

#endif // THING_TYPE_H
