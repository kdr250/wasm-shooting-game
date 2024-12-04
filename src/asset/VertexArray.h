#pragma once

class VertexArray
{
public:
    VertexArray();

    ~VertexArray();

    // Activate this vertex array (so we can draw it)
    void SetActive();

    unsigned int GetNumVerts() const
    {
        return numVerts;
    }

    unsigned int GetNumIndices() const
    {
        return numIndices;
    }

private:
    // How many vertices in the vertex buffer?
    unsigned int numVerts;

    // How many indices in the index buffer
    unsigned int numIndices;

    // OpenGL ID of the vertex buffer
    unsigned int vertexBuffer;

    // OpenGL ID of the index buffer
    unsigned int indexBuffer;

    // OpenGL ID of the vertex array object
    unsigned int vertexArray;
};
