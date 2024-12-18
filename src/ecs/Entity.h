#pragma once

#include <string>
#include <tuple>
#include "Components.h"

class EntityManager;

using ComponentTuple = std::tuple<TransformComponent,
                                  SpriteComponent,
                                  AnimationComponent,
                                  DrawComponent,
                                  RectComponent,
                                  InputComponent,
                                  MoveComponent,
                                  SplineMoveComponent,
                                  StateComponent,
                                  LifespanComponent,
                                  BoxCollisionComponent,
                                  EventComponent>;

class Entity
{
    friend class EntityManager;

public:
    Entity(const size_t& id, const std::string& tag);

    void Destroy();
    size_t GetId() const;
    bool IsActive() const;
    const std::string& GetTag() const;

    template<typename T>
    bool HasComponent() const
    {
        return GetComponent<T>().has;
    }

    template<typename T, typename... TArgs>
    T& AddComponent(TArgs&&... args)
    {
        auto& component = GetComponent<T>();
        component       = T(std::forward<TArgs>(args)...);
        component.has   = true;
        return component;
    }

    template<typename T>
    T& GetComponent()
    {
        return std::get<T>(components);
    }

    template<typename T>
    const T& GetComponent() const
    {
        return std::get<T>(components);
    }

    template<typename T>
    void RemoveComponent()
    {
        GetComponent<T>() = T();
    }

private:
    bool active     = true;
    std::string tag = "default";
    size_t id       = 0;
    ComponentTuple components;
};
