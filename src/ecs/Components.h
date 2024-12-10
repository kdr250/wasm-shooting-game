#pragma once

#include <functional>
#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/glm.hpp>

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
    float speed        = 0.0f;

    TransformComponent() {}
    TransformComponent(const glm::vec2& p) : position(p) {}
    TransformComponent(const glm::vec2& p, const glm::vec2& v) : position(p), velocity(v) {}
    TransformComponent(const glm::vec2& p, const float sc) : position(p), scale(sc) {}
    TransformComponent(const glm::vec2& p, const float sc, const float sp) :
        position(p), scale(sc), speed(sp)
    {
    }
    TransformComponent(const glm::vec2& p, const glm::vec2& v, const float sc) :
        position(p), velocity(v), scale(sc)
    {
    }
    TransformComponent(const glm::vec2& p, const glm::vec2& v, const float sc, const float sp) :
        position(p), velocity(v), scale(sc), speed(sp)
    {
    }

    void ResetVelocity()
    {
        velocity.x = 0.0f;
        velocity.y = 0.0f;
    }
};

class SpriteComponent : public Component
{
public:
    std::string shaderName;
    std::string textureName;

    SpriteComponent() {}
    SpriteComponent(const std::string& shaName, const std::string& texName) :
        shaderName(shaName), textureName(texName)
    {
    }
};

class DrawComponent : public Component
{
public:
    std::string shaderName;
    glm::vec3 color = {0.0f, 0.0f, 0.0f};

    DrawComponent() {}
    DrawComponent(const std::string& shaName, const glm::vec3& drawColor) :
        shaderName(shaName), color(drawColor)
    {
    }
};

class RectComponent : public Component
{
public:
    float edge = 0.0f;

    RectComponent() {}
    RectComponent(const float e) : edge(e) {}
};

class InputComponent : public Component
{
public:
    bool up    = false;
    bool left  = false;
    bool right = false;
    bool down  = false;
    bool shoot = false;

    float maxShootInterval = 0.1f;
    float shootInterval    = 0.0f;

    bool pause = false;

    InputComponent() {}
    InputComponent(const float maxInterval) :
        maxShootInterval(maxInterval), shootInterval(maxInterval)
    {
    }

    void Reset()
    {
        up            = false;
        left          = false;
        right         = false;
        down          = false;
        shoot         = false;
        shootInterval = maxShootInterval;
    }

    void ResetShootInterval()
    {
        shootInterval = maxShootInterval;
    }
};

class AIMoveComponent : public Component
{
public:
    std::vector<glm::vec2> points;
    int current = 0;
    float speed = 0.0f;
    float t     = 0.0f;

    AIMoveComponent() {}
    AIMoveComponent(const std::vector<glm::vec2>& movePoints, const float sp) : speed(sp)
    {
        points = movePoints;
    }

    glm::vec2 Lerp(float deltaTime)
    {
        int next                = (current + 1) % points.size();
        glm::vec2 currentToNext = points[next] - points[current];
        t += speed * deltaTime / glm::length(currentToNext);

        if (t >= 1.0f)
        {
            t       = 0.0f;
            current = next;
            return points[next];
        }
        glm::vec2 result = points[current] + t * currentToNext;
        return result;
    }

    const glm::vec2& CurrentPoint()
    {
        return points[current];
    }

    const glm::vec2& NextPoint()
    {
        int next = (current + 1) % points.size();
        return points[next];
    }
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

class LifespanComponent : public Component
{
public:
    float maxLifespan = 1.0f;
    float lifespan    = maxLifespan;

    LifespanComponent() {}
    LifespanComponent(const float maxLife) : maxLifespan(maxLife), lifespan(maxLife) {}
};

class BoxCollisionComponent : public Component
{
public:
    glm::vec2 size;
    glm::vec2 halfSize;
    std::vector<std::string> excludeTags;

    BoxCollisionComponent() {}
    BoxCollisionComponent(const glm::vec2& s) : size(s), halfSize(s.x / 2.0f, s.y / 2.0f) {}
    BoxCollisionComponent(const glm::vec2& s, const std::vector<std::string> excludes) :
        size(s), halfSize(s.x / 2.0f, s.y / 2.0f)
    {
        excludeTags = excludes;
    }
};

class EventComponent : public Component
{
public:
    std::vector<std::function<bool(long, int)>> events;
    std::vector<long> previousTimes;
    std::vector<int> executionCounts;
    long elapsedTimeMilli = 0;

    EventComponent() {}
    EventComponent(const std::vector<std::function<bool(long, int)>>& ev) :
        previousTimes(ev.size(), 0), executionCounts(ev.size(), 0)
    {
        events = ev;
    }

    void Add(const std::function<bool(long, int)>& event)
    {
        events.push_back(event);
        previousTimes.push_back(0);
        executionCounts.push_back(0);
    }

    void Execute(float deltaTime)
    {
        elapsedTimeMilli += deltaTime * 1000;

        int index = 0;
        auto iter = events.begin();
        while (iter != events.end())
        {
            auto event         = *iter;
            long previousTime  = previousTimes[index];
            long previous      = elapsedTimeMilli - previousTime;
            int executionCount = executionCounts[index];
            bool isExecuted    = event(previous, executionCount);
            if (isExecuted)
            {
                previousTimes[index] = elapsedTimeMilli;
                executionCounts[index]++;
            }
            ++iter;
            ++index;
        }
    }
};
