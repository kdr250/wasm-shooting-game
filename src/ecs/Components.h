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

class AnimationComponent : public Component
{
public:
    std::string shaderName;
    std::string animationName;
    int currentFrame = 0;
    int speed        = 0;

    AnimationComponent() {}
    AnimationComponent(const std::string& shaName, const std::string& animName, int sp) :
        shaderName(shaName), animationName(animName), speed(sp)
    {
    }

    void Update()
    {
        ++currentFrame;
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

class MoveComponent : public Component
{
public:
    std::vector<glm::vec2> points;
    int current     = 0;
    float speed     = 0.0f;
    float t         = 0.0f;
    bool isFinished = false;

    MoveComponent() {}
    MoveComponent(const std::vector<glm::vec2>& movePoints, const float sp) : speed(sp)
    {
        points = movePoints;
    }

    glm::vec2 Move(float deltaTime)
    {
        int next                = (current + 1) % points.size();
        glm::vec2 currentToNext = points[next] - points[current];
        t += speed * deltaTime / glm::length(currentToNext);
        isFinished = false;

        if (t >= 1.0f)
        {
            t          = 0.0f;
            current    = next;
            isFinished = next == 0;
            return points[next];
        }
        glm::vec2 result = points[current] + t * currentToNext;
        return result;
    }

    bool MoveToNext(float deltaTime, glm::vec2& outPosition)
    {
        int next                = (current + 1) % points.size();
        glm::vec2 currentToNext = points[next] - points[current];
        t += speed * deltaTime / glm::length(currentToNext);

        if (t >= 1.0f)
        {
            t           = 0.0f;
            current     = next;
            outPosition = points[next];
            return true;
        }

        outPosition = points[current] + t * currentToNext;
        return false;
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

    bool IsFinished()
    {
        return isFinished;
    }
};

struct Spline
{
    // Control points for spline
    // (Requires n+2 points where n is number
    // of points in segment)
    std::vector<glm::vec2> controllPoints;

    // Given spline segment where startIdx = P1,
    // compute position based on t value
    glm::vec2 Compute(size_t startIdx, float t) const
    {
        // Check if startIdx is out of bounds
        if (startIdx >= controllPoints.size())
        {
            return controllPoints.back();
        }
        else if (startIdx == 0)
        {
            return controllPoints[startIdx];
        }
        else if (startIdx + 2 >= controllPoints.size())
        {
            return controllPoints[startIdx];
        }

        // Get p0 through p3
        glm::vec2 p0 = controllPoints[startIdx - 1];
        glm::vec2 p1 = controllPoints[startIdx];
        glm::vec2 p2 = controllPoints[startIdx + 1];
        glm::vec2 p3 = controllPoints[startIdx + 2];
        // Compute position according to Catmull-Rom equation
        glm::vec2 position = 0.5f
                             * ((2.0f * p1) + (-1.0f * p0 + p2) * t
                                + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t
                                + (-1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
        return position;
    }

    // Returns number of control points
    size_t GetNumPoints() const
    {
        return controllPoints.size();
    }
};

class SplineMoveComponent : public Component
{
public:
    Spline spline;
    int index       = 0;
    float speed     = 0.0f;
    float t         = 0.0f;
    bool isFinished = false;

    SplineMoveComponent() {}
    SplineMoveComponent(const std::vector<glm::vec2>& points, const float sp) : speed(sp)
    {
        spline.controllPoints = points;
        index                 = 1;
    }

    const glm::vec2 Move(float deltaTime)
    {
        if (IsFinished())
        {
            return spline.Compute(index, t);
        }

        t += speed * deltaTime;

        // Advance to the next control point if needed.
        if (t >= 1.0f)
        {
            if (index < spline.GetNumPoints() - 3)
            {
                index++;
                t = t - 1.0f;
            }
            else
            {
                isFinished = true;
            }
        }
        return spline.Compute(index, t);
    }

    bool IsFinished()
    {
        return isFinished;
    }

    void Reset()
    {
        index      = 1;
        t          = 0.0f;
        isFinished = false;
    }

    const glm::vec2& CurrentPoint()
    {
        return spline.controllPoints[index];
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

class HealthComponent : public Component
{
public:
    int maxHealth  = 0;
    int health     = 0;
    float interval = 0.0f;
    float lastTime = 0.0f;

    HealthComponent() {}
    HealthComponent(int hp) : maxHealth(hp), health(hp) {}
    HealthComponent(int hp, float interv) : maxHealth(hp), health(hp), interval(interv) {}

    void Damaged(int damage)
    {
        health -= damage;
    }

    bool Damaged(int damage, long elapsedTimeMilli)
    {
        float elapsedTime = elapsedTimeMilli / 1000.0f;
        if (elapsedTime - lastTime < interval)
        {
            return false;
        }

        Damaged(damage);

        lastTime = elapsedTime;
        return true;
    }

    bool IsAlive()
    {
        return health > 0;
    }
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

enum Result
{
    NONE,
    COMPLETED,
    CONTINUE,
};

class EventComponent : public Component
{
public:
    std::vector<std::function<Result(long, int)>> events;
    std::vector<long> previousTimes;
    std::vector<int> executionCounts;
    long elapsedTimeMilli = 0;

    EventComponent() {}
    EventComponent(const std::vector<std::function<Result(long, int)>>& ev) :
        previousTimes(ev.size(), 0), executionCounts(ev.size(), 0)
    {
        events = ev;
    }

    void Add(const std::function<Result(long, int)>& event)
    {
        events.push_back(event);
        previousTimes.push_back(0);
        executionCounts.push_back(0);
    }

    void Add(const std::vector<std::function<Result(long, int)>>& eventsToAdd)
    {
        events.insert(events.end(), eventsToAdd.begin(), eventsToAdd.end());
    }

    void Execute(float deltaTime)
    {
        elapsedTimeMilli += deltaTime * 1000;

        if (events.empty())
        {
            return;
        }

        auto event         = events.front();
        long previousTime  = previousTimes.front();
        long previous      = elapsedTimeMilli - previousTime;
        int executionCount = executionCounts.front();
        Result result      = event(previous, executionCount);
        switch (result)
        {
            case COMPLETED:
                events.erase(events.begin());
                previousTimes.erase(previousTimes.begin());
                executionCounts.erase(executionCounts.begin());
                break;

            case CONTINUE:
                previousTimes.front() = elapsedTimeMilli;
                executionCounts.front()++;
                break;

            case NONE:
            default:
                break;
        }
    }
};
