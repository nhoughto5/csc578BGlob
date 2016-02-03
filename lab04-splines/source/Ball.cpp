#include "Ball.hpp"

#include <atlas/core/Float.hpp>
#include <atlas/core/Log.hpp>

Ball::Ball() :
    mBall()
{
    USING_ATLAS_MATH_NS;
    USING_ATLAS_GL_NS;

    std::vector<ShaderInfo> shaders
    {
        { GL_VERTEX_SHADER, "shaders/ball.vs.glsl" },
        { GL_FRAGMENT_SHADER, "shaders/ball.fs.glsl" }
    };

    mShaders.push_back(ShaderPointer(new Shader));
    mShaders[0]->compileShaders(shaders);
    mShaders[0]->linkShaders();

    mUniforms.insert(UniformKey("mvpMat", 
        mShaders[0]->getUniformVariable("mvpMat")));

    mBall.createBuffers();

    mShaders[0]->disableShaders();
}

Ball::~Ball()
{ }

void Ball::renderGeometry(atlas::math::Matrix4 projection,
    atlas::math::Matrix4 view)
{
    mShaders[0]->enableShaders();

    auto mvpMat = projection * view * mModel;
    glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);

    mBall.drawPrimitive();

    mShaders[0]->disableShaders();
}

void Ball::transformGeometry(atlas::math::Matrix4 const& t)
{
    mModel = t;
}
