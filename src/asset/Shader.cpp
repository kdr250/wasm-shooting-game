#include "Shader.h"
#include <SDL2/SDL.h>

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
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shader, 512, nullptr, buffer);
    SDL_Log("GLSL compile status: %s", buffer);
    return status == GL_TRUE;
}

bool Shader::IsValidProgram()
{
    GLint status;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &status);
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shaderProgramId, 512, nullptr, buffer);
    SDL_Log("GLSL link status: %s", buffer);
    return status == GL_TRUE;
}
