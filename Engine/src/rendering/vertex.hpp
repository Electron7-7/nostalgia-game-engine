#ifdef FWD_DCL
    struct Vertex;
#elif !defined VERTEX_H
#define VERTEX_H

#include "core/farg.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
public:
    Vertex() noexcept = default;

    Farg<glm::vec3> Position() const { return position_; }
    Farg<glm::vec3> Normal()   const { return normal_; }
    Farg<glm::vec3> Color()    const { return color_; }
    Farg<glm::vec2> UV()       const { return uv_; }

    void SetPosition(Farg<glm::vec3> Position) { position_ = Position; }
    void SetColor(Farg<glm::vec3> Color)       { color_ = Color; }
    void SetNormal(Farg<glm::vec3> Normal)     { normal_ = Normal; }
    void SetUV(Farg<glm::vec2> UV)             { uv_ = UV; }

    void GetVertexData(std::vector<float>& Output) const
    {
        Output.emplace_back(position_.x);
        Output.emplace_back(position_.y);
        Output.emplace_back(position_.z);

        Output.emplace_back(color_.x);
        Output.emplace_back(color_.y);
        Output.emplace_back(color_.z);

        Output.emplace_back(normal_.x);
        Output.emplace_back(normal_.y);
        Output.emplace_back(normal_.z);

        Output.emplace_back(uv_.x);
        Output.emplace_back(uv_.y);
    }

private:
    glm::vec3 position_ {0.0f};
    glm::vec3 color_    {1.0f};
    glm::vec3 normal_   {0.0f};
    glm::vec2 uv_       {0.0f};
};

#endif // VERTEX_H
