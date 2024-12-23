#include "Bullet.h"
#include "../Game.h"
#include "Enemy.h"
#include "Player.h"

void Bullet::Move(float deltaTime)
{
    for (auto& ownerTag : {Player::PLAYER_TAG, Enemy::ENEMY_TAG})
    {
        for (auto& bullet : GetBullets(ownerTag))
        {
            auto& transform = bullet->GetComponent<TransformComponent>();
            transform.position += transform.velocity * deltaTime;
        }
    }
}

void Bullet::Draw()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& vertexArray  = assetManager.GetSpriteVertex();

    std::vector<glm::vec2> positions;
    std::vector<glm::vec2> sizes;
    std::vector<glm::vec3> colors;

    for (auto& ownerTag : {Player::PLAYER_TAG, Enemy::ENEMY_TAG})
    {
        auto& bullets = GetBullets(ownerTag);
        if (bullets.empty())
        {
            continue;
        }

        for (auto& bullet : bullets)
        {
            auto& draw      = bullet->GetComponent<DrawComponent>();
            auto& transform = bullet->GetComponent<TransformComponent>();
            float edge      = bullet->GetComponent<RectComponent>().edge;

            positions.emplace_back(transform.position);
            sizes.emplace_back(glm::vec2 {edge, edge});
            colors.emplace_back(draw.color);
        }

        std::string bulletShaderName = bullets[0]->GetComponent<DrawComponent>().shaderName;
        auto& bulletShader           = assetManager.GetShader(bulletShaderName);

        bulletShader.SetActive();
        vertexArray.SetActive();
        bulletShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
        bulletShader.SetIntUniform("uBulletCount", bullets.size());
        bulletShader.SetVector2Uniforms("uBulletPositions", positions);
        bulletShader.SetVector2Uniforms("uBulletSizes", sizes);
        bulletShader.SetVector3Uniforms("uBulletColors", colors);
        glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);

        positions.clear();
        sizes.clear();
        colors.clear();
    }
}

const std::vector<std::shared_ptr<Entity>>& Bullet::GetBullets(const std::string& ownerTag)
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    std::string tag     = GenerateTagName(ownerTag);
    return entityManager.GetEntities(tag);
}

std::string Bullet::GenerateTagName(const std::string& ownerTag)
{
    return BULLET_TAG_PREFIX + "_" + ownerTag;
}
