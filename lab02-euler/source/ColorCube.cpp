#include "ColorCube.hpp"
#include "ShaderPaths.hpp"

#include <atlas/gl/Shader.hpp>
#include <atlas/core/Float.hpp>

ColorCube::ColorCube() :
    mVelocity(0.0f),
    mPosition(-15.0f, 15.0f, 0.0f),
    mIsStopped(false)
{
    USING_ATLAS_MATH_NS;
    USING_ATLAS_GL_NS;

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    std::string shaderDir = generated::ShaderPaths::getShaderDirectory();

    std::vector<ShaderInfo> shaders
    {
        { GL_VERTEX_SHADER, shaderDir + "cube.vs.glsl" },
        { GL_FRAGMENT_SHADER, shaderDir + "cube.fs.glsl" }
    };

    // Compile and link the shader.
    mShaders.push_back(ShaderPointer(new Shader));
    mShaders[0]->compileShaders(shaders);
    mShaders[0]->linkShaders();

    // Grab the uniform variables.
    GLuint mvpMat = mShaders[0]->getUniformVariable("mvpMat");
    mUniforms.insert(UniformKey("mvpMat", mvpMat));

    // Set the vertices.
    static const Vector vertexBufferData[] = {
        { -1.0f,-1.0f,-1.0f },
        { -1.0f,-1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f,-1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f },
        { -1.0f,-1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f,-1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f,-1.0f },
        { 1.0f,-1.0f,-1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f,-1.0f },
        { -1.0f, 1.0f,-1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f,-1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f,-1.0f, 1.0 }
    };

    static const GLfloat colorBufferData[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    // Now generate and bind the vertex buffer.
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData) * sizeof(Vector),
        vertexBufferData, GL_STATIC_DRAW);

    glGenBuffers(1, &mColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorBufferData), colorBufferData,
        GL_STATIC_DRAW);

    mModel = glm::translate(Matrix4(1.0f), mPosition);

    mShaders[0]->disableShaders();
}

ColorCube::~ColorCube()
{
    glDeleteVertexArrays(1, &mVao);
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mColorBuffer);
}

void ColorCube::updateGeometry(atlas::utils::Time const& t)
{
    USING_ATLAS_MATH_NS;
    
    if (mIsStopped)
    {
        return;
    }

    // There are only two forces acting on the cube. The initial push,
    // (assume constant) and gravity.

    // Now, suppose our cube has a mass of 10 units, so acceleration is equal
    // to force / 10.

    // Now integrate to get the velocity.
    
    // Finally compute the final position for the cube.

    // Check the ground, if we hit it, then kill everything.

    // Set our model matrix to the new position.
}

void ColorCube::renderGeometry(atlas::math::Matrix4 projection,
    atlas::math::Matrix4 view)
{
    mShaders[0]->enableShaders();

    glBindVertexArray(mVao);

    auto mvpMat = projection * view * mModel;
    glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mColorBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    mShaders[0]->disableShaders();
}

void ColorCube::resetGeometry()
{
    USING_ATLAS_MATH_NS;

    mPosition = Vector(-15, 15, 0);
    mVelocity = Vector(0.0f);
    mModel = glm::translate(Matrix4(1.0f), mPosition);
    mIsStopped = false;
}

