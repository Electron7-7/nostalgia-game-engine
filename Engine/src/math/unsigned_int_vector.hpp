#ifndef UNSIGNED_INT_VECTOR_H
#define UNSIGNED_INT_VECTOR_H

#include <glm/glm.hpp>

struct uintvec3
{
public:
    uintvec3(unsigned int X, unsigned int Y, unsigned int Z): _xyz(X, Y, Z) {}
    uintvec3(unsigned int XYZ): uintvec3(XYZ, XYZ, XYZ) {}
    uintvec3(const glm::vec3& XYZ): uintvec3((unsigned int)std::abs(XYZ.x), (unsigned int)std::abs(XYZ.y), (unsigned int)std::abs(XYZ.z)) {}

    unsigned int x() const { return _xyz[0]; }
    unsigned int y() const { return _xyz[1]; }
    unsigned int z() const { return _xyz[2]; }
    unsigned int* xyz()    { return _xyz;    }

    unsigned int  operator[](unsigned int i) const { assert(i < 4); return (_xyz[i]); }
    unsigned int& operator[](unsigned int i)       { assert(i < 4); return (_xyz[i]); }

private:
    unsigned int _xyz[3] = {0, 0, 0};
};

#endif // UNSIGNED_INT_VECTOR_H
