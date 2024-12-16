#include <gtest/gtest.h>
#include "../../../src/ecs/Components.h"

TEST(SplineMoveComponent, MoveTest)
{
    glm::vec2 expected {438.994873f, 94.5088196f};

    std::vector<glm::vec2> points = {
        glm::vec2 {0.0f, 0.0f},
        glm::vec2 {100.0f, 100.0f},
        glm::vec2 {800.0f, 100.0f},
        glm::vec2 {800.0f, 300.0f},
        glm::vec2 {100.0f, 300.0f},
        glm::vec2 {100.0f, 100.0f},
        glm::vec2 {800.0f, 100.0f},
    };

    SplineMoveComponent spline(points, 0.5f);

    glm::vec2 actual;
    for (int i = 0; i < 60; ++i)
    {
        actual = spline.Move(0.016);
    }

    EXPECT_FLOAT_EQ(expected.x, actual.x);
    EXPECT_FLOAT_EQ(expected.y, actual.y);
}
