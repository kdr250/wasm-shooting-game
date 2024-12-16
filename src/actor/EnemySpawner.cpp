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

                auto enemy = entityManager.AddEntity(Enemy::ENEMY_TAG);

                RegisterComponents(enemy, config);
                RegisterEvents(enemy, config);

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
    std::string id, event, key, value;
    stream >> id >> event;

    config.emplace("Event" + id, event);

    while (!stream.eof())
    {
        stream >> key >> value;
        config.emplace(event + key + id, value);
    }
}

void EnemySpawner::RegisterComponents(std::shared_ptr<Entity>& enemy,
                                      std::map<std::string, std::string> config)
{
    RegisterSpriteComponent(enemy, config);
    RegisterMoveComponent(enemy, config);
    RegisterSplineMoveComponent(enemy, config);
}

void EnemySpawner::RegisterSpriteComponent(std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config)
{
    if (!config.contains("Sprite"))
    {
        return;
    }

    auto& assetManager = Game::GetGame().GetAssetManager();

    std::string name     = config.at("SpriteName");
    std::string path     = config.at("SpritePath");
    std::string strScale = config.at("SpriteScale");

    if (!assetManager.LoadTexture(name, path))
    {
        SDL_Log("Failed to load texture: %s", path.c_str());
        exit(EXIT_FAILURE);
    }
    enemy->AddComponent<SpriteComponent>(Enemy::SPRITE_SHADER_NAME, name);

    float scale = std::stof(strScale);

    if (enemy->HasComponent<TransformComponent>())
    {
        enemy->GetComponent<TransformComponent>().scale = scale;
    }
    else
    {
        auto& transform = enemy->AddComponent<TransformComponent>();
        transform.scale = scale;
    }

    auto& texture = assetManager.GetTexture(name);

    enemy->AddComponent<BoxCollisionComponent>(
        glm::vec2 {texture.GetWidth(), texture.GetHeight()} * scale / 4.0f,
        std::vector {enemy->GetTag()});
}

void EnemySpawner::RegisterMoveComponent(std::shared_ptr<Entity>& enemy,
                                         std::map<std::string, std::string> config)
{
    if (!config.contains("Move"))
    {
        return;
    }

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
        enemy->AddComponent<TransformComponent>(move.CurrentPoint(), 1.0);
    }
    else
    {
        auto& transform    = enemy->GetComponent<TransformComponent>();
        transform.position = move.CurrentPoint();
    }
}

void EnemySpawner::RegisterSplineMoveComponent(std::shared_ptr<Entity>& enemy,
                                               std::map<std::string, std::string> config)
{
    if (!config.contains("SplineMove"))
    {
        return;
    }

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
        enemy->AddComponent<TransformComponent>(spline.CurrentPoint(), 1.0);
    }
    else
    {
        auto& transform    = enemy->GetComponent<TransformComponent>();
        transform.position = spline.CurrentPoint();
    }
}

void EnemySpawner::RegisterEvents(std::shared_ptr<Entity>& enemy,
                                  std::map<std::string, std::string> config)
{
    std::vector<std::function<Result(long, int)>> events;
    int id              = 0;
    std::string eventId = "Event" + std::to_string(id);

    while (config.contains(eventId))
    {
        std::string event = config.at(eventId);
        if (event == "MoveEvent")
        {
            RegisterMoveEvent(id, enemy, config, events);
        }
        else if (event == "SplineMoveEvent")
        {
            RegisterSplineMoveEvent(id, enemy, config, events);
        }
        else if (event == "ExplosionBulletsEvent")
        {
            RegisterExplosionBulletsEvent(id, enemy, config, events);
        }
        else if (event == "RollBulletsEvent")
        {
            RegisterRollBulletsEvent(id, enemy, config, events);
        }
        else if (event == "SequentialBulletsEvent")
        {
            RegisterSequentialBulletsEvent(id, enemy, config, events);
        }
        else if (event == "WinderBulletsEvent")
        {
            RegisterWinderBulletsEvent(id, enemy, config, events);
        }
        ++id;
        eventId = "Event" + std::to_string(id);
    }
    enemy->AddComponent<EventComponent>(events);
}

void EnemySpawner::RegisterMoveEvent(const int eventId,
                                     std::shared_ptr<Entity>& enemy,
                                     std::map<std::string, std::string> config,
                                     std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        if (!enemy->HasComponent<MoveComponent>())
        {
            return Result::COMPLETED;
        }

        float deltaTime = Game::GetGame().GetDeltaTime();

        std::string type = config.at("MoveEventType" + strId);
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
        else
        {
            SDL_Log("SplineMoveEventType is not valid: %s", type.c_str());
            return Result::COMPLETED;
        }
    };
    events.emplace_back(event);
}

void EnemySpawner::RegisterSplineMoveEvent(const int eventId,
                                           std::shared_ptr<Entity>& enemy,
                                           std::map<std::string, std::string> config,
                                           std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        if (!enemy->HasComponent<SplineMoveComponent>())
        {
            return Result::COMPLETED;
        }

        std::string type = config.at("SplineMoveEventType" + strId);

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
    const int eventId,
    std::shared_ptr<Entity>& enemy,
    std::map<std::string, std::string> config,
    std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        std::string strLoopNum    = config.at("ExplosionBulletsEventLoopNum" + strId);
        std::string strInterval   = config.at("ExplosionBulletsEventInterval" + strId);
        std::string strColor      = config.at("ExplosionBulletsEventColor" + strId);
        std::string strNumBullets = config.at("ExplosionBulletsEventNumBullets" + strId);
        std::string strSpeed      = config.at("ExplosionBulletsEventSpeed" + strId);
        std::string strSize       = config.at("ExplosionBulletsEventSize" + strId);
        std::string strWaitAfter  = config.at("ExplosionBulletsEventWaitAfter" + strId);

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

void EnemySpawner::RegisterRollBulletsEvent(const int eventId,
                                            std::shared_ptr<Entity>& enemy,
                                            std::map<std::string, std::string> config,
                                            std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        std::string strColor      = config.at("RollBulletsEventColor" + strId);
        std::string strNumBullets = config.at("RollBulletsEventNumBullets" + strId);
        std::string strSpeed      = config.at("RollBulletsEventSpeed" + strId);
        std::string strSize       = config.at("RollBulletsEventSize" + strId);
        std::string strWaitAfter  = config.at("RollBulletsEventWaitAfter" + strId);

        float waitAfter = std::stof(strWaitAfter);

        float fromPreviousSecond = fromPreviousMilli / 1000.0f;
        if (executionCount == 1 && fromPreviousSecond >= waitAfter)
        {
            return Result::COMPLETED;
        }
        if (executionCount < 1)
        {
            glm::vec3 color  = Bullet::RED;  // FIXME
            int numOfBullets = std::stoi(strNumBullets);
            float speed      = std::stof(strSpeed);
            float size       = std::stof(strSize);

            auto& transform = enemy->GetComponent<TransformComponent>();
            Bullet::SpawnRollBullets(transform.position,  // position
                                     color,               // color
                                     numOfBullets,        // num of bullets
                                     enemy->GetTag(),     // owner tag
                                     speed,               // speed
                                     size                 // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };
    events.emplace_back(event);
}

void EnemySpawner::RegisterSequentialBulletsEvent(
    const int eventId,
    std::shared_ptr<Entity>& enemy,
    std::map<std::string, std::string> config,
    std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        std::string strColor      = config.at("SequentialBulletsEventColor" + strId);
        std::string strNumBullets = config.at("SequentialBulletsEventNumBullets" + strId);
        std::string strSpeed      = config.at("SequentialBulletsEventSpeed" + strId);
        std::string strSize       = config.at("SequentialBulletsEventSize" + strId);
        std::string strWaitAfter  = config.at("SequentialBulletsEventWaitAfter" + strId);

        float waitAfter = std::stof(strWaitAfter);

        float fromPreviousSecond = fromPreviousMilli / 1000.0f;
        if (executionCount == 1 && fromPreviousSecond >= waitAfter)
        {
            return Result::COMPLETED;
        }
        if (executionCount < 1)
        {
            glm::vec3 color  = Bullet::RED;  // FIXME
            int numOfBullets = std::stoi(strNumBullets);
            float speed      = std::stof(strSpeed);
            float size       = std::stof(strSize);

            auto& transform       = enemy->GetComponent<TransformComponent>();
            auto& player          = Player::GetPlayer();
            auto& playerTransform = player->GetComponent<TransformComponent>();

            Bullet::SpawnSequentialBullets(transform.position,        // position
                                           playerTransform.position,  // target
                                           numOfBullets,              // num of bullets
                                           color,                     // color
                                           enemy->GetTag(),           // owner tag
                                           speed,                     // speed
                                           size                       // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };
    events.emplace_back(event);
}

void EnemySpawner::RegisterWinderBulletsEvent(const int eventId,
                                              std::shared_ptr<Entity>& enemy,
                                              std::map<std::string, std::string> config,
                                              std::vector<std::function<Result(long, int)>>& events)
{
    std::string strId = std::to_string(eventId);

    auto event = [enemy, config, strId](long fromPreviousMilli, int executionCount)
    {
        std::string strColor     = config.at("WinderBulletsEventColor" + strId);
        std::string strLoopNum   = config.at("WinderBulletsEventLoopNum" + strId);
        std::string strSpeed     = config.at("WinderBulletsEventSpeed" + strId);
        std::string strSize      = config.at("WinderBulletsEventSize" + strId);
        std::string strWaitAfter = config.at("WinderBulletsEventWaitAfter" + strId);

        float waitAfter = std::stof(strWaitAfter);

        float fromPreviousSecond = fromPreviousMilli / 1000.0f;
        if (executionCount == 1 && fromPreviousSecond >= waitAfter)
        {
            return Result::COMPLETED;
        }
        if (executionCount < 1)
        {
            glm::vec3 color = Bullet::RED;  // FIXME
            int loopNum     = std::stoi(strLoopNum);
            float speed     = std::stof(strSpeed);
            float size      = std::stof(strSize);

            auto& transform       = enemy->GetComponent<TransformComponent>();
            auto& player          = Player::GetPlayer();
            auto& playerTransform = player->GetComponent<TransformComponent>();

            Bullet::SpawnWinderBullets(transform.position,        // position
                                       playerTransform.position,  // target
                                       loopNum,                   // num of loop
                                       color,                     // color
                                       enemy->GetTag(),           // owner tag
                                       speed,                     // speed
                                       size                       // size
            );
            return Result::CONTINUE;
        }
        return Result::NONE;
    };
    events.emplace_back(event);
}
