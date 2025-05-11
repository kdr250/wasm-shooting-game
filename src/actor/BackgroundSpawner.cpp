#include "BackgroundSpawner.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../Game.h"
#include "Background.h"
#include "BackgroundShader.h"

void BackgroundSpawner::Spawn(const std::vector<std::string>& allLine)
{
    auto iter = allLine.begin();
    while (iter != allLine.end())
    {
        std::string line = *iter;
        std::stringstream stream(line);
        std::string tag, type;
        stream >> tag >> type;

        if (tag == "Background")
        {
            if (type == "Image")
            {
                std::string imagePath1, imagePath2;
                stream >> imagePath1 >> imagePath2;
                Background::Spawn(imagePath1, imagePath2);
            }
            else if (type == "Shader")
            {
                std::string vertPath, fragPath;
                stream >> vertPath >> fragPath;
                BackgroundShader::Spawn(vertPath, fragPath);
            }
        }

        ++iter;
    }
}
