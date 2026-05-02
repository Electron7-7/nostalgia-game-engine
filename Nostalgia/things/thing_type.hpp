#ifndef THING_TYPE_H
#define THING_TYPE_H

#define TYPE_ID(CLASS) CLASS{#CLASS}

namespace ThingType
{
    inline const PID Invalid{ID::Invalid, "Invalid"},
        TYPE_ID(Thing),
            TYPE_ID(Resource),
                TYPE_ID(Font),
                TYPE_ID(Mesh),
                    TYPE_ID(ArrayMesh),
                TYPE_ID(Texture),
                    TYPE_ID(Cubemap),
                    TYPE_ID(ViewportTexture),
                    TYPE_ID(ImageTexture),
                TYPE_ID(Material),
                TYPE_ID(Image),
            TYPE_ID(Thinker),
                TYPE_ID(NostalgiaPlayer),
                TYPE_ID(Viewport),
                TYPE_ID(Actor3D),
                    TYPE_ID(Camera3D),
                    TYPE_ID(Collider3D),
                    TYPE_ID(Visual3D),
                        TYPE_ID(MeshInstance3D),
                        TYPE_ID(Sprite3D),
                        TYPE_ID(Light3D),
                            TYPE_ID(PointLight3D),
                            TYPE_ID(SpotLight3D),
                            TYPE_ID(DirectionalLight3D),
                TYPE_ID(Actor2D),
                    TYPE_ID(Camera2D),
                    TYPE_ID(Visual2D),
                        TYPE_ID(Sprite2D),
                        TYPE_ID(Text2D);
}

#undef TYPE_ID
#endif // THING_TYPE_H
