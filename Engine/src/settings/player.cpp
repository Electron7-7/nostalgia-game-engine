#include "player.hpp"

// TODO: Replace `Settings::Player::FOV` with horizontal and/or vertical FOVs? (currently it's only vertical)
float Settings::Player::FOV{45.0f};
float Settings::Player::ViewCutoffNear{0.01f};
float Settings::Player::ViewCutoffFar{1000.0f};
bool  Settings::Player::RawMouseMotion{true};
float Settings::Player::MouseSensitivity{3.5f};
float Settings::Player::MouseSensitivityScale{0.01f};
float Settings::Player::MovementSpeed{0.4f};
