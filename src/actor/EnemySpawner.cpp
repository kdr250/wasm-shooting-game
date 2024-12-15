#include "EnemySpawner.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../Game.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"

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

void EnemySpawner::Initialize(const int sceneId)
{
    auto& game          = Game::GetGame();
    auto& assetManager  = game.GetAssetManager();
    auto& entityManager = game.GetEntityManger();

    auto spawner = entityManager.AddEntity(ENEMY_SPAWNER_TAG);
    spawner->AddComponent<EventComponent>();

    std::string filePath = "resources/scene/" + std::to_string(sceneId) + ".txt";
    std::ifstream file;
    file.open(filePath);

    if (!file.is_open())
    {
        SDL_Log("Failed to open scene file: %s", filePath.c_str());
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

    std::map<std::string, std::string> config;

    auto iter = allLine.begin();
    while (iter != allLine.end())
    {
        std::string line = *iter;
        std::stringstream stream(line);
        std::string tag, type;
        stream >> tag;

        if (tag == Enemy::ENEMY_TAG)
        {
            ReadEnemyConfiguration(stream, config);
        }
        else if (tag == "Component")
        {
            ReadComponentConfiguration(stream, config);
        }
        else if (tag == "Event")
        {
            ReadEventConfiguration(stream, config);
        }
        else if (tag == "End")
        {
            std::function<Result(long, int)> spawnEvent =
                [spawner, config](long fromPreviousMilli, int executionCount)
            {
                auto& game          = Game::GetGame();
                auto& assetManager  = game.GetAssetManager();
                auto& entityManager = game.GetEntityManger();

                if (config.contains("SpawnTime"))
                {
                    std::string strSpawnTime = config.at("SpawnTime");
                    long spawnTime           = std::stol(strSpawnTime);
                    long elapsedTime         = game.SceneElapsedTimeSecond();
                    if (elapsedTime < spawnTime)
                    {
                        return Result::NONE;
                    }
                }

                auto enemy  = entityManager.AddEntity(Enemy::ENEMY_TAG);
                float scale = 1.0f;

                if (config.contains("Sprite"))
                {
                    std::string name     = config.at("SpriteName");
                    std::string path     = config.at("SpritePath");
                    std::string strScale = config.at("SpriteScale");

                    if (!assetManager.LoadTexture(name, path))
                    {
                        SDL_Log("Failed to load texture: %s", path.c_str());
                        exit(EXIT_FAILURE);
                    }
                    enemy->AddComponent<SpriteComponent>(Enemy::SPRITE_SHADER_NAME, name);

                    scale         = std::stof(strScale);
                    auto& texture = assetManager.GetTexture(name);

                    enemy->AddComponent<BoxCollisionComponent>(
                        glm::vec2 {texture.GetWidth(), texture.GetHeight()} * scale / 4.0f,
                        std::vector {enemy->GetTag()});
                }

                if (config.contains("Move"))
                {
                    std::string strPoints = config.at("MovePoints");
                    std::string strSpeed  = config.at("MoveSpeed");

                    std::vector<glm::vec2> points;
                    for (auto& strPoint : Split(strPoints, "|"))
                    {
                        auto split = Split(strPoint, ",");
                        float x    = std::stof(split[0]);
                        float y    = std::stof(split[1]);
                        glm::vec2 point {x, y};
                        points.emplace_back(point);
                    }

                    float speed = std::stof(strSpeed);

                    auto& move = enemy->AddComponent<MoveComponent>(points, speed);
                    if (!enemy->HasComponent<TransformComponent>())
                    {
                        enemy->AddComponent<TransformComponent>(move.CurrentPoint(), scale);
                    }
                }

                if (config.contains("SplineMove"))
                {
                    std::string strPoints = config.at("SplineMovePoints");
                    std::string strSpeed  = config.at("SplineMoveSpeed");

                    std::vector<glm::vec2> points;
                    for (auto& strPoint : Split(strPoints, "|"))
                    {
                        auto split = Split(strPoint, ",");
                        float x    = std::stof(split[0]);
                        float y    = std::stof(split[1]);
                        glm::vec2 point {x, y};
                        points.emplace_back(point);
                    }

                    float speed = std::stof(strSpeed);

                    auto& spline = enemy->AddComponent<SplineMoveComponent>(points, speed);
                    if (!enemy->HasComponent<TransformComponent>())
                    {
                        enemy->AddComponent<TransformComponent>(spline.CurrentPoint(), scale);
                    }
                }

                // FIXME
                std::vector<std::function<Result(long, int)>> enemyEvents;
                RegisterSplineMoveEvent(enemy, config, enemyEvents);
                RegisterMoveEvent(enemy, config, enemyEvents);
                RegisterExplosionBulletsEvent(enemy, config, enemyEvents);

                std::vector<std::function<Result(long, int)>> hoge = {
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        long fromPreviousSecond = fromPreviousMilli / 1000;
                        if (executionCount == 1 && fromPreviousSecond >= 4)
                        {
                            return Result::COMPLETED;
                        }
                        if (executionCount < 1)
                        {
                            auto& transform = enemy->GetComponent<TransformComponent>();
                            Bullet::SpawnRollBullets(transform.position,  // position
                                                     Bullet::RED,         // color
                                                     36,                  // num of bullets
                                                     enemy->GetTag(),     // owner tag
                                                     200.0f,              // speed
                                                     200.0f               // size
                            );
                            return Result::CONTINUE;
                        }
                        return Result::NONE;
                    },
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        float deltaTime = Game::GetGame().GetDeltaTime();
                        if (enemy->HasComponent<MoveComponent>())
                        {
                            auto& transform = enemy->GetComponent<TransformComponent>();
                            auto& aiMove    = enemy->GetComponent<MoveComponent>();
                            bool hasReached = aiMove.MoveToNext(deltaTime, transform.position);
                            Result result   = hasReached ? Result::COMPLETED : Result::CONTINUE;
                            return result;
                        }
                        return Result::COMPLETED;
                    },
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        long fromPreviousSecond = fromPreviousMilli / 1000;
                        if (executionCount == 1 && fromPreviousSecond >= 3)
                        {
                            return Result::COMPLETED;
                        }
                        if (executionCount < 1)
                        {
                            auto& transform       = enemy->GetComponent<TransformComponent>();
                            auto& player          = Player::GetPlayer();
                            auto& playerTransform = player->GetComponent<TransformComponent>();

                            Bullet::SpawnSequentialBullets(transform.position,        // position
                                                           playerTransform.position,  // target
                                                           10,               // num of bullets
                                                           Bullet::RED,      // color
                                                           enemy->GetTag(),  // owner tag
                                                           200.0f,           // speed
                                                           200.0f            // size
                            );
                            return Result::CONTINUE;
                        }
                        return Result::NONE;
                    },
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        float deltaTime = Game::GetGame().GetDeltaTime();
                        if (enemy->HasComponent<MoveComponent>())
                        {
                            auto& transform = enemy->GetComponent<TransformComponent>();
                            auto& aiMove    = enemy->GetComponent<MoveComponent>();
                            bool hasReached = aiMove.MoveToNext(deltaTime, transform.position);
                            Result result   = hasReached ? Result::COMPLETED : Result::CONTINUE;
                            return result;
                        }
                        return Result::COMPLETED;
                    },
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        long fromPreviousSecond = fromPreviousMilli / 1000;
                        if (executionCount == 1 && fromPreviousSecond >= 7)
                        {
                            return Result::COMPLETED;
                        }
                        if (executionCount < 1)
                        {
                            auto& transform       = enemy->GetComponent<TransformComponent>();
                            auto& player          = Player::GetPlayer();
                            auto& playerTransform = player->GetComponent<TransformComponent>();

                            Bullet::SpawnWinderBullets(transform.position,        // position
                                                       playerTransform.position,  // target
                                                       100,                       // num of loop
                                                       Bullet::RED,               // color
                                                       enemy->GetTag(),           // owner tag
                                                       200.0f,                    // speed
                                                       200.0f                     // size
                            );
                            return Result::CONTINUE;
                        }
                        return Result::NONE;
                    },
                    [enemy](long fromPreviousMilli, int executionCount)
                    {
                        float deltaTime = Game::GetGame().GetDeltaTime();
                        if (enemy->HasComponent<MoveComponent>())
                        {
                            auto& transform = enemy->GetComponent<TransformComponent>();
                            auto& aiMove    = enemy->GetComponent<MoveComponent>();
                            bool hasReached = aiMove.MoveToNext(deltaTime, transform.position);
                            Result result   = hasReached ? Result::COMPLETED : Result::CONTINUE;
                            return result;
                        }
                        return Result::COMPLETED;
                    },
                };
                enemy->AddComponent<EventComponent>(enemyEvents);

                return Result::COMPLETED;
            };

            auto& eventComponent = spawner->GetComponent<EventComponent>();
            eventComponent.Add(spawnEvent);
        }
        ++iter;
    }
}

void EnemySpawner::Spawn(float deltaTime)
{
    auto& spawner        = GetSpawner();
    auto& eventComponent = spawner->GetComponent<EventComponent>();
    eventComponent.Execute(deltaTime);
}

std::shared_ptr<Entity>& EnemySpawner::GetSpawner()
{
    auto& entityManager = Game::GetGame().GetEntityManger();
    return entityManager.GetEntities(ENEMY_SPAWNER_TAG)[0];
}

void EnemySpawner::ReadEnemyConfiguration(std::stringstream& stream,
                                          std::map<std::string, std::string>& config)
{
    std::string key, value;
    while (!stream.eof())
    {
        stream >> key >> value;
        config.emplace(key, value);
    }
}

void EnemySpawner::ReadComponentConfiguration(std::stringstream& stream,
                                              std::map<std::string, std::string>& config)
{
    std::string type, key, value;
    stream >> type;

    config.emplace(type, "true");

    while (!stream.eof())
    {
        stream >> key >> value;
        config.emplace(type + key, value);
    }
}

void EnemySpawner::ReadEventConfiguration(std::stringstream& stream,
                                          std::map<std::string, std::string>& config)
{
    std::string event, key, value;
    stream >> event;

    config.emplace(event, "true");

    while (!stream.eof())
    {
        stream >> key >> value;
        config.emplace(event + key, value);
    }
}

void EnemySpawner::RegisterMoveEvent(std::shared_ptr<Entity>& enemy,
                                     std::map<std::string, std::string> config,
                                     std::vector<std::function<Result(long, int)>>& events)
{
    if (!config.contains("MoveEvent"))
    {
        return;
    }

    auto event = [enemy, config](long fromPreviousMilli, int executionCount)
    {
        if (!enemy->HasComponent<MoveComponent>())
        {
            return Result::COMPLETED;
        }

        float deltaTime = Game::GetGame().GetDeltaTime();

        std::string type = config.at("MoveEventType");
        if (type == "OneRound")
        {
            auto& transform    = enemy->GetComponent<TransformComponent>();
            auto& move         = enemy->GetComponent<MoveComponent>();
            transform.position = move.Move(deltaTime);
            Result result      = move.IsFinished() ? Result::COMPLETED : Result::CONTINUE;
            return result;
        }
        else if (type == "EveryPoints")
        {
            auto& transform = enemy->GetComponent<TransformComponent>();
            auto& move      = enemy->GetComponent<MoveComponent>();
            bool hasReached = move.MoveToNext(deltaTime, transform.position);
            Result result   = hasReached ? Result::COMPLETED : Result::CONTINUE;
            return result;
        }
    };
    events.emplace_back(event);
}

void EnemySpawner::RegisterSplineMoveEvent(std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config,
                                           std::vector<std::function<Result(long, int)>>& events)
{
    if (!config.contains("SplineMoveEvent"))
    {
        return;
    }

    auto event = [enemy, config](long fromPreviousMilli, int executionCount)
    {
        if (!enemy->HasComponent<SplineMoveComponent>())
        {
            return Result::COMPLETED;
        }

        std::string type = config.at("SplineMoveEventType");

        if (type == "All")
        {
            float deltaTime    = Game::GetGame().GetDeltaTime();
            auto& transform    = enemy->GetComponent<TransformComponent>();
            auto& spline       = enemy->GetComponent<SplineMoveComponent>();
            transform.position = spline.Move(deltaTime);
            Result result      = spline.IsFinished() ? Result::COMPLETED : Result::CONTINUE;
            return result;
        }
        else
        {
            SDL_Log("SplineMoveEventType is not valid: %s", type.c_str());
            return Result::COMPLETED;
        }
    };
    events.emplace_back(event);
}

void EnemySpawner::RegisterExplosionBulletsEvent(
    std::shared_ptr<Entity>& enemy,
    std::map<std::string, std::string> config,
    std::vector<std::function<Result(long, int)>>& events)
{
    if (!config.contains("ExplosionBulletsEvent"))
    {
        return;
    }

    auto event = [enemy, config](long fromPreviousMilli, int executionCount)
    {
        std::string strLoopNum    = config.at("ExplosionBulletsEventLoopNum");
        std::string strInterval   = config.at("ExplosionBulletsEventInterval");
        std::string strColor      = config.at("ExplosionBulletsEventColor");
        std::string strNumBullets = config.at("ExplosionBulletsEventNumBullets");
        std::string strSpeed      = config.at("ExplosionBulletsEventSpeed");
        std::string strSize       = config.at("ExplosionBulletsEventSize");
        std::string strWaitAfter  = config.at("ExplosionBulletsEventWaitAfter");

        int loopNum     = std::stoi(strLoopNum);
        float interval  = std::stof(strInterval);
        float waitAfter = std::stof(strWaitAfter);

        float fromPreviousSecond = fromPreviousMilli / 1000.0;
        if (executionCount == loopNum && fromPreviousSecond >= waitAfter)
        {
            return Result::COMPLETED;
        }
        if (executionCount < loopNum && fromPreviousSecond >= interval)
        {
            glm::vec3 color  = Bullet::RED;  // FIXME
            int numOfBullets = std::stoi(strNumBullets);
            float speed      = std::stof(strSpeed);
            float size       = std::stof(strSize);

            Bullet::SpawnExplosionBullets(enemy->GetComponent<TransformComponent>().position,
                                          color,
                                          numOfBullets,     // number of bullets
                                          enemy->GetTag(),  // owner tag
                                          speed,            // speed
                                          size              // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };
    events.emplace_back(event);
}
