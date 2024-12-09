#pragma once

#include <cmath>
#include <memory>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>
#include "Entity.h"

namespace Physics
{
    inline bool IsOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
    {
        if (!a->IsActive() || !b->IsActive())
        {
            return false;
        }

        if (!a->HasComponent<TransformComponent>() || !a->HasComponent<BoxCollisionComponent>()
            || !b->HasComponent<TransformComponent>() || !b->HasComponent<BoxCollisionComponent>())
        {
            return false;
        }

        auto& transformA = a->GetComponent<TransformComponent>();
        auto& boxA       = a->GetComponent<BoxCollisionComponent>();
        auto& transformB = b->GetComponent<TransformComponent>();
        auto& boxB       = b->GetComponent<BoxCollisionComponent>();

        for (auto& tag : boxA.excludeTags)
        {
            if (tag == b->GetTag())
            {
                return false;
            }
        }

        for (auto& tag : boxB.excludeTags)
        {
            if (tag == a->GetTag())
            {
                return false;
            }
        }

        float distanceX = std::abs(transformA.position.x - transformB.position.x);
        float distanceY = std::abs(transformA.position.y - transformB.position.y);

        float x = boxA.halfSize.x + boxB.halfSize.x - distanceX;
        float y = boxA.halfSize.y + boxB.halfSize.y - distanceY;

        return x > 0.0f && y > 0.0f;
    }
}  // namespace Physics
