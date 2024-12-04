#pragma once

#include <GL/glew.h>
#include <string>

class Shader
{
public:
    Shader();
    Shader(const std::string& vertContents, const std::string& fragContents);

    void SetActive();

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
