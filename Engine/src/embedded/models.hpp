#ifndef MODELS_H
#define MODELS_H

namespace Models
{
    constexpr unsigned char Error[]{
        #embed "models/Error.obj"
    };

    constexpr unsigned char Ramiel[]{
        #embed "models/Ramiel.obj"
    };

    constexpr unsigned char Cube[]{
        #embed "models/Cube.obj"
    };

    constexpr unsigned char Camera[]{
        #embed "models/Camera3D.obj"
    };

    constexpr unsigned char DebugAxis[]{
        #embed "models/DebugAxis.obj"
    };
}

#endif // MODELS_H
