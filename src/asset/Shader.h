#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#define GLM_FORCE_PURE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Shader
{
public:
    Shader();
    Shader(const std::string& vertContents, const std::string& fragContents);

    void SetActive();

    void SetIntUniform(const std::string& name, const int value);

    void SetFloatUniform(const std::string& name, const float value);

    void SetVector2Uniform(const std::string& name, const glm::vec2& value);

    void SetVector2Uniform(const std::string& name, const float v1, const float v2);

    void SetVector2Uniforms(const std::string& name, std::vector<glm::vec2>& values);

    void SetVector3Uniform(const std::string& name, const glm::vec3& value);

    void SetVector3Uniform(const std::string& name, const float v1, const float v2, const float v3);

    void SetVector3Uniforms(const std::string& name, std::vector<glm::vec3>& values);

    void Unload();

private:
    bool CompileShader(const std::string& contents, GLenum shaderType, GLuint& outShader);

    bool IsCompiled(GLuint shader);

    bool IsValidProgram();

    // OpenGL IDs of the shader program
    GLuint shaderProgramId;

    // OpenGL IDs of the vertex shader
    GLuint vertexShaderId;

    // OpenGL IDs of the fragment shader
    GLuint fragShaderId;
};
