#include "Player.h"
#include <algorithm>
#include "../Game.h"
#include "Bullet.h"
#include "Enemy.h"

std::shared_ptr<Entity> Player::Spawn(const glm::vec2& position)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    if (!assetManager.LoadShader(SPRITE_SHADER_NAME, SPRITE_SHADER_VERT, SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    if (!assetManager.LoadTexture(PLAYER_TEXTURE_NAME, PLAYER_TEXTURE))
    {
        SDL_Log("Failed to load texture");
        exit(EXIT_FAILURE);
    }

    float scale = 10.0f;
    float speed = 200.0f;

    auto player = entityManager.AddEntity(PLAYER_TAG);
    player->AddComponent<StateComponent>();
    player->AddComponent<TransformComponent>(position, scale, speed);
    player->AddComponent<InputComponent>();
    player->AddComponent<SpriteComponent>(SPRITE_SHADER_NAME, PLAYER_TEXTURE_NAME);
    auto& playerTexture = assetManager.GetTexture(PLAYER_TEXTURE_NAME);
    player->AddComponent<BoxCollisionComponent>(
        glm::vec2 {playerTexture.GetWidth(), playerTexture.GetHeight()} * scale / 4.0f,
        std::vector {player->GetTag()});

    return player;
}

void Player::Move(float deltaTime)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    auto& player = GetPlayer();

    auto& input           = player->GetComponent<InputComponent>();
    auto& playerTransform = player->GetComponent<TransformComponent>();

    playerTransform.ResetVelocity();

    if (input.left)
    {
        playerTransform.velocity.x = -playerTransform.speed;
    }
    if (input.right)
    {
        playerTransform.velocity.x = playerTransform.speed;
    }
    if (input.up)
    {
        playerTransform.velocity.y = -playerTransform.speed;
    }
    if (input.down)
    {
        playerTransform.velocity.y = playerTransform.speed;
    }

    auto& playerSprite = player->GetComponent<SpriteComponent>();
    auto& texture      = assetManager.GetTexture(playerSprite.textureName);
    float scale        = playerTransform.scale;

    playerTransform.position += playerTransform.velocity * deltaTime;
    playerTransform.position.x = std::clamp(playerTransform.position.x,
                                            texture.GetWidth() * scale / 4.0f,
                                            Game::WINDOW_WIDTH - texture.GetWidth() * scale / 4.0f);
    playerTransform.position.y =
        std::clamp(playerTransform.position.y,
                   texture.GetHeight() * scale / 4.0f,
                   Game::WINDOW_HEIGHT - texture.GetHeight() * scale / 4.0f);

    input.shootInterval -= deltaTime;
    if (input.shoot && input.shootInterval <= 0.0f)
    {
        Bullet::SpawnDirectionalBullet(playerTransform.position + glm::vec2 {0.0f, -50.0f},
                                       glm::vec2 {0.0f, -600.0f},  // velocity
                                       Bullet::GREEN,              // color
                                       player->GetTag(),           // owner tag
                                       20.0f                       // size
        );
        input.ResetShootInterval();
    }
}

void Player::ProcessAction(const Action& action)
{
    auto& player = GetPlayer();
    auto& input  = player->GetComponent<InputComponent>();

    if (action.type == "START")
    {
        if (action.name == "UP")
        {
            input.up = true;
        }
        else if (action.name == "LEFT")
        {
            input.left = true;
        }
        else if (action.name == "DOWN")
        {
            input.down = true;
        }
        else if (action.name == "RIGHT")
        {
            input.right = true;
        }
        else if (action.name == "SHOOT")
        {
            input.shoot = true;
        }
    }
    else if (action.type == "END")
    {
        if (action.name == "PAUSE")
        {
            input.pause = false;
        }
        else if (action.name == "UP")
        {
            input.up = false;
        }
        else if (action.name == "LEFT")
        {
            input.left = false;
        }
        else if (action.name == "DOWN")
        {
            input.down = false;
        }
        else if (action.name == "RIGHT")
        {
            input.right = false;
        }
        else if (action.name == "SHOOT")
        {
            input.shoot = false;
        }
    }
}

bool Player::NeedsPause()
{
    auto& player = GetPlayer();
    auto& input  = player->GetComponent<InputComponent>();
    return !input.pause;
}

bool Player::IsCollide()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    auto& player        = GetPlayer();

    for (auto& enemy : Enemy::GetEnemies())
    {
        if (Physics::IsOverlap(enemy, player))
        {
            return true;
        }
    }

    for (auto& bullet : Bullet::GetBullets(Enemy::ENEMY_TAG))
    {
        if (Physics::IsOverlap(bullet, player))
        {
            return true;
        }
    }

    return false;
}

void Player::Draw()
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    auto& player = GetPlayer();

    auto& playerTransform = player->GetComponent<TransformComponent>();
    auto& playerSprite    = player->GetComponent<SpriteComponent>();

    auto& spriteShader = assetManager.GetShader(playerSprite.shaderName);
    auto& texture      = assetManager.GetTexture(playerSprite.textureName);
    auto& vertexArray  = assetManager.GetSpriteVertex();

    spriteShader.SetActive();
    vertexArray.SetActive();

    spriteShader.SetVector2Uniform("uWindowSize", Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);
    spriteShader.SetVector2Uniform("uTextureSize", texture.GetWidth(), texture.GetHeight());
    spriteShader.SetVector2Uniform("uTexturePosition", playerTransform.position);
    spriteShader.SetFloatUniform("uTextureScale", playerTransform.scale);

    texture.SetActive();
    glDrawElements(GL_TRIANGLES, vertexArray.GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void Player::Unload()
{
    auto& assetManager = Game::GetGame().GetAssetManager();
    auto& player       = GetPlayer();
    auto& sprite       = player->GetComponent<SpriteComponent>();
    assetManager.RemoveTexture(sprite.textureName);
}

std::shared_ptr<Entity>& Player::GetPlayer()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(PLAYER_TAG)[0];
}
