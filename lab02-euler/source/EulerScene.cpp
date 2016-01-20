#include "EulerScene.hpp"
#include <atlas/core/GLFW.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Macros.hpp>

EulerScene::EulerScene() :
    mIsPlaying(false),
    mLastTime(0.0)
{
    glEnable(GL_DEPTH_TEST);
}

EulerScene::~EulerScene()
{ }

void EulerScene::mousePressEvent(int button, int action, int modifiers, 
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

void EulerScene::mouseMoveEvent(double xPos, double yPos)
{
    if (mIsDragging)
    {
        mCamera.mouseDrag(atlas::math::Point2(xPos, yPos));
    }
}

void EulerScene::keyPressEvent(int key, int scancode, int action, int mods)
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
            mCube.resetGeometry();
            break;

        case GLFW_KEY_SPACE:
            if (mIsPlaying)
            {
                mLastTime = glfwGetTime();
            }
            else
            {
                glfwSetTime(mLastTime);
                mIsPlaying = !mIsPlaying;
            }

        default:
            break;
        }
    }
}

void EulerScene::screenResizeEvent(int width, int height)
{
    glViewport(0, 0, width, height);
    mProjection = glm::perspective(glm::radians(45.0), 
        (double)width / height, 1.0, 1000.0);
}

void EulerScene::renderScene()
{
    float grey = 161.0f / 255.0f;
    glClearColor(grey, grey, grey, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    mView = mCamera.getCameraMatrix();
    mGrid.renderGeometry(mProjection, mView);
    mCube.renderGeometry(mProjection, mView);
}

void EulerScene::updateScene(double time)
{
    if (mIsPlaying)
    {
        // Handle the timing stuff here.

        // Tell our cube to update itself.
    }
}