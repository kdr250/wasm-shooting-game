#include <gtest/gtest.h>
#include "../../../src/ecs/EntityManager.h"

TEST(EntityManger, AddEntityTest)
{
    EntityManager entityManger;
    auto player = entityManger.AddEntity("player");
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(glm::vec2 {10.0f, 20.0f}, 2.0f);
    player->AddComponent<InputComponent>();

    entityManger.Update();

    auto actual = entityManger.GetEntities("player")[0];

    EXPECT_EQ(player, actual);
}

TEST(EntityManger, RemoveEntityTest)
{
    EntityManager entityManger;
    auto player = entityManger.AddEntity("player");
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(glm::vec2 {10.0f, 20.0f}, 2.0f);
    player->AddComponent<InputComponent>();
    player->Destroy();

    entityManger.Update();

    auto actual = entityManger.GetEntities("player");

    EXPECT_EQ(0, actual.size());
}
