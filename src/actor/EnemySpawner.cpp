#include "EnemySpawner.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "../Game.h"
#include "Bullet.h"
#include "Enemy.h"

// FIXME
std::vector<std::string> Split(std::string& str, const char* delimiter)
{
    int first = 0;
    int last  = str.find_first_of(delimiter);
    std::vector<std::string> result;

    while (first < str.size())
    {
        result.push_back(str.substr(first, last - first));
        first = last + 1;
        last  = str.find_first_of(delimiter, first);
        if (last == std::string::npos)
        {
            last = str.size();
        }
    }

    return result;
}

std::vector<std::shared_ptr<Entity>> EnemySpawner::Spawn(const int sceneId)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    // FIXME
    std::string filePath = "resources/scene/" + std::to_string(sceneId) + ".txt";
    std::ifstream file;
    file.open(filePath);

    if (!file.is_open())
    {
        SDL_Log("Failed to open scene file: %s", filePath);
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::vector<std::string> allLine;

    while (std::getline(file, line))
    {
        allLine.push_back(line);
    }

    file.close();

    if (!assetManager.LoadShader(Enemy::SPRITE_SHADER_NAME,
                                 Enemy::SPRITE_SHADER_VERT,
                                 Enemy::SPRITE_SHADER_FRAG))
    {
        SDL_Log("Failed to load shader");
        exit(EXIT_FAILURE);
    }

    assetManager.CreateSpriteVertex();

    std::vector<std::shared_ptr<Entity>> enemies;

    auto enemy = entityManager.AddEntity(Enemy::ENEMY_TAG);

    for (auto& line : allLine)
    {
        std::stringstream stream(line);
        std::string tag;
        std::string type;
        stream >> tag;

        if (tag == Enemy::ENEMY_TAG)
        {
            enemy = entityManager.AddEntity(Enemy::ENEMY_TAG);
        }
        else if (tag == "End")
        {
            enemies.emplace_back(enemy);
            continue;
        }

        float scale = 1.0f;

        while (!stream.eof())
        {
            std::string type;
            stream >> type;

            if (type == "Sprite")
            {
                std::string texturePath, textureName, strScale;
                stream >> textureName >> texturePath >> strScale;
                if (!assetManager.LoadTexture(textureName, texturePath))
                {
                    SDL_Log("Failed to load texture: %s", texturePath);
                    exit(EXIT_FAILURE);
                }
                enemy->AddComponent<SpriteComponent>(Enemy::SPRITE_SHADER_NAME, textureName);

                scale         = std::stof(strScale);
                auto& texture = assetManager.GetTexture(textureName);

                enemy->AddComponent<BoxCollisionComponent>(
                    glm::vec2 {texture.GetWidth(), texture.GetHeight()} * scale / 4.0f,
                    std::vector {enemy->GetTag()});

                continue;
            }
            else if (type == "Move[")
            {
                std::vector<glm::vec2> points;

                std::string strPoints, strSpeed;
                stream >> strPoints;
                while (strPoints != "]")
                {
                    auto split = Split(strPoints, "-");
                    float x    = std::stof(split[0]);
                    float y    = std::stof(split[1]);
                    glm::vec2 point {x, y};
                    points.emplace_back(point);
                    stream >> strPoints;
                }

                stream >> strSpeed;
                float speed  = std::stof(strSpeed);
                auto& aiMove = enemy->AddComponent<MoveComponent>(points, speed);

                if (!enemy->HasComponent<TransformComponent>())
                {
                    enemy->AddComponent<TransformComponent>(aiMove.CurrentPoint(), scale);
                }
            }
            else if (type == "SplineMove[")
            {
                std::vector<glm::vec2> points;

                std::string strPoints, strSpeed;
                stream >> strPoints;
                while (strPoints != "]")
                {
                    auto split = Split(strPoints, "-");
                    float x    = std::stof(split[0]);
                    float y    = std::stof(split[1]);
                    glm::vec2 point {x, y};
                    points.emplace_back(point);
                    stream >> strPoints;
                }

                stream >> strSpeed;
                float speed  = std::stof(strSpeed);
                auto& spline = enemy->AddComponent<SplineMoveComponent>(points, speed);

                if (!enemy->HasComponent<TransformComponent>())
                {
                    enemy->AddComponent<TransformComponent>(spline.CurrentPoint(), scale);
                }
            }

            // TODO: register events
        }
    }

    return enemies;
}