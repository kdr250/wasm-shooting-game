#include "Shader.h"
#include <SDL2/SDL.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() {}

Shader::Shader(const std::string& vertContents, const std::string& fragContents)
{
    // Compile vertex and fragment shaders
    if (!CompileShader(vertContents, GL_VERTEX_SHADER, vertexShaderId)
        || !CompileShader(fragContents, GL_FRAGMENT_SHADER, fragShaderId))
    {
        exit(EXIT_FAILURE);
    }

    // Now create a shader program that links together the vertex/frag shaders
    shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgramId, vertexShaderId);
    glAttachShader(shaderProgramId, fragShaderId);
    glLinkProgram(shaderProgramId);

    // Verify that the program linked successfully
    if (!IsValidProgram())
    {
        exit(EXIT_FAILURE);
    }
}

void Shader::SetActive()
{
    // Set this program as the active one
    glUseProgram(shaderProgramId);
}

void Shader::SetFloatUniform(const std::string& name, const float value)
{
    GLuint locationId = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniform1f(locationId, value);
}

void Shader::SetVector2Uniform(const std::string& name, const glm::vec2& value)
{
    GLuint locationId = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniform2fv(locationId, 1, glm::value_ptr(value));
}

void Shader::SetVector2Uniform(const std::string& name, const float v1, const float v2)
{
    GLuint locationId = glGetUniformLocation(shaderProgramId, name.c_str());
    glUniform2f(locationId, v1, v2);
}

void Shader::Unload()
{
    // Delete the program and shaders
    glDeleteProgram(shaderProgramId);
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragShaderId);
}

bool Shader::CompileShader(const std::string& contents, GLenum shaderType, GLuint& outShader)
{
    // Create a shader of the specified type
    outShader = glCreateShader(shaderType);

    // Set the source characters and try to compile
    const char* contentsChar = contents.c_str();
    glShaderSource(outShader, 1, &contentsChar, nullptr);
    glCompileShader(outShader);

    bool result = IsCompiled(outShader);
    return result;
}

bool Shader::IsCompiled(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetShaderInfoLog(shader, 512, nullptr, buffer);
        SDL_Log("GLSL compile status: %s", buffer);
        return false;
    }
    return true;
}

bool Shader::IsValidProgram()
{
    GLint status;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        char buffer[512];
        memset(buffer, 0, 512);
        glGetShaderInfoLog(shaderProgramId, 512, nullptr, buffer);
        SDL_Log("GLSL link status: %s", buffer);
        return false;
    }
    return true;
}