#ifndef DOUBLE_VECTORS_H
#define DOUBLE_VECTORS_H

struct DoubleVector2
{
    double x = 0.0;
    double y = 0.0;

    DoubleVector2(const double& X, const double& Y): x(X), y(Y) {}
    DoubleVector2(const double& XY): DoubleVector2(XY, XY) {}
    DoubleVector2() = default;
};

struct DoubleVector3
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    DoubleVector3(const double& X, const double& Y, const double& Z): x(X), y(Y), z(Z) {}
    DoubleVector3(const double& XYZ): DoubleVector3(XYZ, XYZ, XYZ) {}
    DoubleVector3() = default;
};

// Typedefs for your fingers
typedef DoubleVector2 dvec2;
typedef DoubleVector2 vec2d;
typedef DoubleVector3 dvec3;
typedef DoubleVector3 vec3d;

#endif // DOUBLE_VECTORS_H