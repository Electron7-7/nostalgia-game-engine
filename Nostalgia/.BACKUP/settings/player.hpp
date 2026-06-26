#ifndef PLAYER_SETTINGS_H
#define PLAYER_SETTINGS_H

namespace Settings
{
    namespace Player
    {
        inline bool  RawMouseMotion{true};
        inline bool  EnableGravity{true};
        inline float MouseSensitivity{0.15f};
        inline float MouseSensitivityScale{0.5f};
        inline float MovementSpeed{3.0f};
        inline float MovementAcceleration{20.0f};
        inline float Mass{10.0f};
    }
}

#endif // PLAYER_SETTINGS_H
