#include "settings.hpp"

#pragma message("Replace with horizontal & vertical FOVs?")
float Settings::Player::FOV = 45.0f;
float Settings::Player::ViewCutoffNear = 0.01f;
float Settings::Player::ViewCutoffFar  = 1000.0f;
bool  Settings::Player::RawMouseMotion = true;
float Settings::Player::MouseSensitivity = 1.0f;
float Settings::Player::MouseSensitivityScale = 0.1f;
float Settings::Player::MovementSpeed = 1.0f;
