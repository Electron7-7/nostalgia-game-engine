#ifndef THING_TYPE_H
#define THING_TYPE_H

#define TYPE_ID(TYPE) inline static const PID TYPE{#TYPE};

struct ThingType
{
private:
    PID _type_id{};
    PID _base_type_id{};
    std::set<PID> _all_base_types{};

    friend class ThingFactory;
    ThingType(FPID) noexcept;

public:
    using typeids_t = std::set<PID>;

    ThingType() noexcept;

    Farg<typeids_t> base_types() const noexcept;
    PID base_type() const noexcept;
    PID type() const noexcept;
    PID operator()() const noexcept;
    bool is_derived_from(FPID inTypeID) const noexcept;
    std::string log() const noexcept;

    bool operator<(Farg<ThingType> inOther) const noexcept
    { return _type_id < inOther._type_id; }

    bool operator==(Farg<ThingType> inOther) const noexcept
    { return _type_id == inOther._type_id; }

    inline static const PID Invalid{ID::Invalid, "Invalid"};

    TYPE_ID(Thing)
        TYPE_ID(Resource)
            TYPE_ID(Font)
            TYPE_ID(Mesh)
            TYPE_ID(Texture)
            TYPE_ID(CubemapTexture)
            TYPE_ID(ViewportTexture)
            TYPE_ID(Material)
        TYPE_ID(Thinker)
            TYPE_ID(Viewport)
            TYPE_ID(Actor3D)
                TYPE_ID(NostalgiaPlayer3D)
                TYPE_ID(Camera3D)
                TYPE_ID(Collider3D)
                TYPE_ID(Visual3D)
                    TYPE_ID(MeshInstance3D)
                    TYPE_ID(Sprite3D)
                    TYPE_ID(Light3D)
                        TYPE_ID(PointLight3D)
                        TYPE_ID(SpotLight3D)
                        TYPE_ID(DirectionalLight3D)
            TYPE_ID(Actor2D)
                TYPE_ID(NostalgiaPlayer2D)
                TYPE_ID(Camera2D)
                TYPE_ID(Visual2D)
                    TYPE_ID(Sprite2D)
                    TYPE_ID(Text2D)
};

#undef TYPE_ID

#endif // THING_TYPE_H
