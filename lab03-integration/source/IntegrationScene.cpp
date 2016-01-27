#include "IntegrationScene.hpp"
#include <atlas/core/GLFW.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Macros.hpp>
#include <atlas/core/Float.hpp>

IntegrationScene::IntegrationScene() :
    mIsPlaying(false),
    mLastTime(0.0f),
    mFPS(60.0f),
    mTick(1.0f / mFPS),
    mAnimTime(0.0f),
    mAnimLength(2.0f)
{
    glEnable(GL_DEPTH_TEST);
}

IntegrationScene::~IntegrationScene()
{ }

void IntegrationScene::mousePressEvent(int button, int action, int modifiers,
    double xPos, double yPos)
{
    USING_ATLAS_MATH_NS;

    if (button == GLFW_MOUSE_BUTTON_LEFT && modifiers == GLFW_MOD_ALT)
    {
        if (action == GLFW_PRESS)
        {
            mIsDragging = true;
            mCamera.mouseDown(Point2(xPos, yPos),
                MayaCamera::CameraMovements::TUMBLE);
        }
        else
        {
            mIsDragging = false;
            mCamera.mouseUp();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE && modifiers == GLFW_MOD_ALT)
    {
        if (action == GLFW_PRESS)
        {
            mIsDragging = true;
            mCamera.mouseDown(Point2(xPos, yPos),
                MayaCamera::CameraMovements::TRACK);
        }
        else
        {
            mIsDragging = false;
            mCamera.mouseUp();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && modifiers == GLFW_MOD_ALT)
    {
        if (action == GLFW_PRESS)
        {
            // first click.
            mIsDragging = true;
            mCamera.mouseDown(Point2(xPos, yPos),
                MayaCamera::CameraMovements::DOLLY);
        }
        else
        {
            mIsDragging = false;
            mCamera.mouseUp();
        }
    }
    else if (action != GLFW_PRESS)
    {
        mIsDragging = false;
        mCamera.mouseUp();
    }
}

void IntegrationScene::mouseMoveEvent(double xPos, double yPos)
{
    if (mIsDragging)
    {
        mCamera.mouseDrag(atlas::math::Point2(xPos, yPos));
    }
}

void IntegrationScene::keyPressEvent(int key, int scancode, int action, int mods)
{
    UNUSED(scancode);
    UNUSED(mods);

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_R:
            mCamera.resetCamera();
            break;

        case GLFW_KEY_C:

            break;

        case GLFW_KEY_SPACE:
            mIsPlaying = !mIsPlaying;


        default:
            break;
        }
    }
}

void IntegrationScene::screenResizeEvent(int width, int height)
{
    glViewport(0, 0, width, height);
    mProjection = glm::perspective(glm::radians(45.0),
        (double)width / height, 1.0, 1000.0);
}

void IntegrationScene::renderScene()
{
    float grey = 161.0f / 255.0f;
    glClearColor(grey, grey, grey, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mView = mCamera.getCameraMatrix();
    mGrid.renderGeometry(mProjection, mView);
    mBall.renderGeometry(mProjection, mView);
}

void IntegrationScene::updateScene(double time)
{
    mTime.currentTime = (float)time;
    mTime.totalTime += (float)time;

    if (atlas::core::geq(mTime.currentTime - mLastTime, mTick))
    {
        mLastTime += mTick;
        mTime.deltaTime = mTick;

        if (mIsPlaying)
        {
            mAnimTime += mTick;
            mBall.updateGeometry(mTime);

            if (atlas::core::geq(mAnimTime, mAnimLength))
            {
                mBall.stopAnimation(mAnimLength);
                mIsPlaying = false;
            }
        }

    }
}
