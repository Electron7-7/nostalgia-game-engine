#include "player.hpp"

// TODO: Replace `Settings::Player::FOV` with horizontal and/or vertical FOVs? (currently it's only vertical)
bool  Settings::Player::RawMouseMotion{true};
float Settings::Player::MouseSensitivity{0.15f};
float Settings::Player::MouseSensitivityScale{0.5f};
float Settings::Player::MovementSpeed{0.4f};
float Settings::Player::Mass{10.0f};
