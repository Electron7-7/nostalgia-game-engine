#ifndef SHADERS_H
#define SHADERS_H

constexpr const char GLSL_BlinnPhong_Frag[]{
    #embed "shaders/GLSL/BlinnPhong.frag" \
        suffix(,)
        0
};

constexpr const char GLSL_BlinnPhong_Vert[]{
    #embed "shaders/GLSL/BlinnPhong.vert" \
        suffix(,)
        0
};

constexpr const char GLSL_FullBright_Frag[]{
    #embed "shaders/GLSL/FullBright.frag" \
        suffix(,)
        0
};

constexpr const char GLSL_SkyBox_Vert[]{
    #embed "shaders/GLSL/SkyBox.vert" \
        suffix(,)
        0
};

constexpr const char GLSL_SkyBox_Frag[]{
    #embed "shaders/GLSL/SkyBox.frag" \
        suffix(,)
        0
};

#endif // SHADERS_H
