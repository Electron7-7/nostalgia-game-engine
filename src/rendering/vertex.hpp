#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <array>
#include <vector>

// IMPORTANT: Vertex data will be packed in this order: Position, Normal, Color, UV

struct Vertex
{
public:
    Vertex(glm::vec3 Position, glm::vec3 Color, glm::vec3 Normal, glm::vec2 UV): _position(Position), _color(Color), _normal(Normal), _uv(UV) {}
    Vertex(glm::vec3 Position): Vertex(Position, glm::vec3(1.0f, 0.1f, 0.9f), glm::vec3(0.0f), glm::vec2(0.0f)) {}
    Vertex(): Vertex(glm::vec3(0.0f)) {}

    glm::vec3 Position() const { return _position; }
    glm::vec3 Normal()   const { return _normal; }
    glm::vec3 Color()    const { return _color; }
    glm::vec2 UV()       const { return _uv; }

    void SetPosition(glm::vec3 Position) { _position = Position; }
    void SetColor(glm::vec3 Color)       { _color = Color; }
    void SetNormal(glm::vec3 Normal)     { _normal = Normal; }
    void SetUV(glm::vec2 UV)             { _uv = UV; }

    static constexpr unsigned int Stride = 11;

    // FIXME: This representation of vertex attributes might be specific to OpenGL. If so, find an agnostic form of a generic "data" function, or give that responsibility to the backend.
    constexpr std::array<float, Stride> VertexData() const
    {
        return
        {
            _position.x, _position.y, _position.z,
            _color.x,    _color.y,    _color.z,
            _normal.x,   _normal.y,   _normal.z,
            _uv.x,       _uv.y
        };
    }

    constexpr void GetVertexData(std::vector<float>& Output) const
    {
        Output.push_back(_position.x);
        Output.push_back(_position.y);
        Output.push_back(_position.z);

        Output.push_back(_color.x);
        Output.push_back(_color.y);
        Output.push_back(_color.z);

        Output.push_back(_normal.x);
        Output.push_back(_normal.y);
        Output.push_back(_normal.z);

        Output.push_back(_uv.x);
        Output.push_back(_uv.y);
    }

    const bool operator==(Vertex other) const { return (this->VertexData() == other.VertexData()); }
    const bool operator!=(Vertex other) const { return !(*this == other); }

private:
    glm::vec3 _position = glm::vec3();
    glm::vec3 _color    = glm::vec3();
    glm::vec3 _normal   = glm::vec3();
    glm::vec2 _uv       = glm::vec2();
};

#endif // VERTEX_H
