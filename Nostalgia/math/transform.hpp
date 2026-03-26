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

    glm::mat4 translation_matrix() const
    { return glm::translate(glm::mat4{1.0f}, position); }

    glm::mat4 scaling_matrix() const
    { return glm::scale(glm::mat4{1.0f}, scale); }

    glm::mat4 rotation_matrix() const
    { return glm::mat4_cast(glm::normalize(quaternion)); }

    glm::mat4 model_matrix() const
    { return translation_matrix() * rotation_matrix() * scaling_matrix(); }
};

#endif // TRANSFORM_H
