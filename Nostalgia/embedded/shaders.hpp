#ifndef SHADERS_H
#define SHADERS_H

#pragma clang diagostic push
#pragma clang diagnostic ignored "-Wc23-extensions"

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

constexpr const char GLSL_Fast2D_Vert[]{
    #embed "shaders/GLSL/Fast2D.vert" \
        suffix(,)
        0
};

constexpr const char GLSL_Fast2D_Frag[]{
    #embed "shaders/GLSL/Fast2D.frag" \
        suffix(,)
        0
};

constexpr const char GLSL_Font_Vert[]{
    #embed "shaders/GLSL/Font.vert" \
        suffix(,)
        0
};

constexpr const char GLSL_Font_Frag[]{
    #embed "shaders/GLSL/Font.frag" \
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

#pragma clang diagostic pop
#endif // SHADERS_H
