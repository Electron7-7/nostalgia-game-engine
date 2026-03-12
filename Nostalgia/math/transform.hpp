#ifndef TRANSFORM_H
#define TRANSFORM_H

struct Transform2D
{
    glm::vec2 position{0.0f},
        scale{1.0f};
    float rotation_radians{0.0f},
        rotation_degrees{0.0f};
};

struct Transform3D
{
    glm::vec3 position{0.0f},
        euler_rotation_radians{0.0f},
        euler_rotation_degrees{0.0f},
        scale{1.0f};
    glm::quat quaternion{euler_rotation_radians};
};

#endif // TRANSFORM_H
