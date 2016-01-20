#include "SimpleGLScene.hpp"

SimpleGLScene::SimpleGLScene()
{
    // Initialize the matrices to identities.
    mProjection = atlas::math::Matrix4(1.0f);
    mView = atlas::math::Matrix4(1.0f);
}

SimpleGLScene::~SimpleGLScene()
{ }

void SimpleGLScene::renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Tell our triangle to render.
    mTriangle.renderGeometry(mProjection, mView);
}