#include "CannonBall.hpp"
#include "ShaderPaths.hpp"

#include <atlas/core/Float.hpp>
#include <atlas/core/Log.hpp>

CannonBall::CannonBall() :
    mRefBall(1.0f),
    mModelBall(1.0f),
    mRefPosition(6, 1, -12),
    mModelVelocity(0),
    mModelPosition(-6, 1, -12),
    mForce(0, 0, 10),
    mMass(1.0f),
    mIntegrator(Integrator::EULER)
{
    USING_ATLAS_MATH_NS;
    USING_ATLAS_GL_NS;

    // Set up the initial positions of the balls.
    mRefMatrix = glm::translate(Matrix4(1.0f), mRefPosition);
    mModel = glm::translate(Matrix4(1.0f), mModelPosition);

    std::string shaderDir = generated::ShaderPaths::getShaderDirectory();

    std::vector<ShaderInfo> shaders
    {
        { GL_VERTEX_SHADER, shaderDir + "ball.vs.glsl" },
        { GL_FRAGMENT_SHADER, shaderDir + "ball.fs.glsl" }
    };

    mShaders.push_back(ShaderPointer(new Shader));
    mShaders[0]->compileShaders(shaders);
    mShaders[0]->linkShaders();

    mUniforms.insert(UniformKey("mvpMat", 
        mShaders[0]->getUniformVariable("mvpMat")));

    mRefBall.createBuffers();
    mModelBall.createBuffers();

    mShaders[0]->disableShaders();
}

CannonBall::~CannonBall()
{ }

void CannonBall::updateGeometry(atlas::utils::Time const& t)
{
    USING_ATLAS_MATH_NS;

    switch (mIntegrator)
    {
    case Integrator::EULER:
        eulerIntegrator(t);
        break;

    default:
        break;
    }


    mModel= glm::translate(Matrix4(1.0f), mModelPosition);
    
}

void CannonBall::renderGeometry(atlas::math::Matrix4 projection,
    atlas::math::Matrix4 view)
{
    mShaders[0]->enableShaders();

    auto mvpMat = projection * view * mModel;
    glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);

    mModelBall.drawPrimitive();


    mvpMat = projection * view * mRefMatrix;
    glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
    mRefBall.drawPrimitive();

    mShaders[0]->disableShaders();
}

void CannonBall::resetGeometry()
{

}

void CannonBall::setIntegrator(Integrator integrator)
{
    mIntegrator = integrator;
}

void CannonBall::stopAnimation(atlas::utils::Time const& t)
{
    USING_ATLAS_MATH_NS;

    float time = t.currentTime * t.currentTime;
    mRefPosition += 0.5f * (mForce / mMass) * time;

    mRefMatrix = glm::translate(Matrix4(1.0f), mRefPosition);

    std::string message = "Ref ball at (6, 1, " + 
        std::to_string(mRefPosition.z) + ")";
    std::string message2 = "Ball at (-6, 1, " + 
        std::to_string(mModelPosition.z) + ")";

    INFO_LOG(message);
    INFO_LOG(message2);
}

void CannonBall::eulerIntegrator(atlas::utils::Time const& t)
{
    mModelVelocity = mModelVelocity + t.deltaTime * (mForce / mMass);
    mModelPosition = mModelPosition + t.deltaTime * mModelVelocity;
}