#include <gtest/gtest.h>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>

TEST(LearningGlm, Vector2AddTest)
{
    glm::vec2 expected {4.0f, 6.0f};
    glm::vec2 actual = glm::vec2 {1.0f, 2.0f} + glm::vec2 {3.0f, 4.0f};
    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}

TEST(LearningGlm, Vector2MultiplyTest)
{
    glm::vec2 expected {12.0f, 18.0f};
    glm::vec2 actual = glm::vec2 {4.0f, 6.0f} * 3.0f;
    EXPECT_EQ(expected.x, actual.x);
    EXPECT_EQ(expected.y, actual.y);
}
