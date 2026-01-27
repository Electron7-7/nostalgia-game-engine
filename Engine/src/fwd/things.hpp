#ifndef THINGS_FWD_H
#define THINGS_FWD_H

// class Thing; // Already in the pch
    // class Resource; // Already in the pch
        class Material;
        class Mesh;
        class Texture;
    // class Thinker; // Already in the pch
    class Viewport;
        class Actor2D;
            class Camera2D;
            class Visual2D;
        class Actor3D;
            class Camera3D;
            class Collider3D;
            class Visual3D;
                class Light3D;
                class PointLight3D;
                class SpotLight3D;
                class DirectionalLight3D;
                class NostalgiaPlayer3D;
                class MeshInstance3D;

#endif // THINGS_FWD_H
