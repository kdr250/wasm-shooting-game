#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray()
{
    float vertices[] = {
        -1.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // top left
        1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f   // bottom left
    };
    numVerts               = 4;
    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    numIndices             = 6;

    // Create vertex array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Create vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVerts * 5 * sizeof(float), vertices, GL_STATIC_DRAW);

    // Create index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numIndices * sizeof(unsigned int),
                 indices,
                 GL_STATIC_DRAW);

    // Specify the vertex attributes (For now, assume one vertex format) Position is 3 floats starting at offset 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * 5,
                          reinterpret_cast<void*>(sizeof(float) * 3));
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void VertexArray::SetActive()
{
    glBindVertexArray(vertexArray);
}
