#include <gtest/gtest.h>
#include "../../../src/ecs/Components.h"

TEST(AIMoveComponent, LerpTest)
{
    glm::vec2 expected {100.0f, 0.0f};

    std::vector<glm::vec2> positions = {
        glm::vec2 {0.0f, 0.0f},
        glm::vec2 {1000.0f, 0.0f},
        glm::vec2 {1000.0f, 1000.0f},
        glm::vec2 {0.0f, 1000.0f},
    };
    AIMoveComponent aiMove(positions, 200.0f);

    glm::vec2 actual = aiMove.Lerp(0.5);

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}

TEST(AIMoveComponent, LerpRotateTest)
{
    glm::vec2 expected {100.0f, 0.0f};

    std::vector<glm::vec2> positions = {
        glm::vec2 {0.0f, 0.0f},
        glm::vec2 {200.0f, 0.0f},
        glm::vec2 {200.0f, 200.0f},
        glm::vec2 {0.0f, 200.0f},
    };
    AIMoveComponent aiMove(positions, 200.0f);

    glm::vec2 actual;
    for (int i = 0; i <= 8; ++i)
    {
        actual = aiMove.Lerp(0.5);
    }

    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}
