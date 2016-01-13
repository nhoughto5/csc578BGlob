#include "Triangle.hpp"
#include "ShaderPaths.hpp"

Triangle::Triangle()
{
    USING_ATLAS_GL_NS;

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    GLfloat vertices[6][2] = {
        { -0.90f, -0.90f }, // First triangle.
        { 0.85f, -0.90f },
        { -0.90f,  0.85f },
        { 0.90f, -0.85f }, // Second triangle.
        { 0.90f,  0.90f },
        { -0.85f,  0.90f }
    };

    glGenBuffers(1, &mBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Get the path where our shaders are stored.
    std::string shaderDir = generated::ShaderPaths::getShaderDirectory();

    // Now set up the information for our shaders.
    std::vector<ShaderInfo> shaders
    {
        ShaderInfo{ GL_VERTEX_SHADER, shaderDir + "triangle.vs.glsl" },
        ShaderInfo{ GL_FRAGMENT_SHADER, shaderDir + "triangle.fs.glsl" }
    };

    // Create a new shader and add it to our list.

    // Compile the shaders.

    // And link them.

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Disable at the end to avoid mixing shaders.
}

Triangle::~Triangle()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mBuffer);
}

void Triangle::renderGeometry(atlas::math::Matrix4 projection,
    atlas::math::Matrix4 view)
{
    // To avoid warnings from unused variables, you can use the 
    // UNUSED macro.

    // Enable the shaders.

    glBindVertexArray(mVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Disable them.
}