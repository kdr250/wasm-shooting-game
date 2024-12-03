#pragma once

#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

class Component
{
public:
    bool has = false;
};

class TransformComponent : public Component
{
public:
    glm::vec2 position = {0.0f, 0.0f};
    glm::vec2 velocity = {0.0f, 0.0f};
    float scale        = 1.0f;
    float rotation     = 0.0f;

    TransformComponent() {}
    TransformComponent(const glm::vec2& p, const float s) : position(p), scale(s) {}
    TransformComponent(const glm::vec2& p, const glm::vec2& v, const float s) :
        position(p), velocity(v), scale(s)
    {
    }
};

class InputComponent : public Component
{
public:
    bool up    = false;
    bool left  = false;
    bool right = false;
    bool down  = false;

    InputComponent() {}
};

enum State
{
    Normal,
    Paused,
    Dead
};

class StateComponent : public Component
{
public:
    State state = State::Normal;

    StateComponent() {}
    StateComponent(const State& s) : state(s) {}
};
