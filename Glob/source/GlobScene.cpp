#include "GlobScene.hpp"
#include <atlas/core/GLFW.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Macros.hpp>
#include <atlas/core/Float.hpp>
#include <iostream>

float g_distance = 1;
float g_boundsRadius = 5;
GlobScene::GlobScene() :
	mIsPlaying(false),
	mLastTime(0.0f),
	mFPS(60.0f),
	mTick(1.0f / mFPS),
	mAnimTime(0.0f),
	mAnimLength(10.0f),
	mSplineManager(int(mAnimLength * mFPS)),
	ballPosition{ 4.0f, 0.0f, 4.0f },
	mCamera(),
	lightPosition{ -4.0f,5.0f, 0.0f },
	ambientLight{ 0.6f, 0.6f, 0.6f, 1.0f },
	//mStructure(),
	mGlob()
{
	glEnable(GL_DEPTH_TEST);
	auto mat = glm::translate(atlas::math::Matrix4(1.0f), ballPosition);
	mGlob.setLightPosition(lightPosition);
	mGlob.setAmbientLight(ambientLight);
	//mStructure.setLightPosition(lightPosition);
	//mStructure.setAmbientLight(ambientLight);

}

GlobScene::~GlobScene() {
}
//UNUSED
void GlobScene::mousePressEvent(int button, int action, int modifiers, double xPos, double yPos) {
	UNUSED(button);
	UNUSED(action);
	UNUSED(modifiers);
	UNUSED(xPos);
	UNUSED(yPos);
}
void GlobScene::mouseMoveEvent(double xPos, double yPos) {
	mCamera.mouseUpdate(glm::vec2(xPos, yPos));
}
void GlobScene::keyPressEvent(int key, int scancode, int action, int mods) {
	UNUSED(scancode);
	UNUSED(mods);

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_T:
			mCamera.resetCamera();
			break;
		case GLFW_KEY_W:
			mCamera.input(FORWARD);
			break;
		case GLFW_KEY_S:
			mCamera.input(BACK);
			break;
		case GLFW_KEY_A:
			mCamera.input(LEFT);
			break;
		case GLFW_KEY_D:
			mCamera.input(RIGHT);
			break;
		case GLFW_KEY_R:
			mCamera.input(UP);
			break;
		case GLFW_KEY_F:
			mCamera.input(DOWN);
			break;
		case GLFW_KEY_Q:
			mCamera.input(LOOKLEFT);
			break;
		case GLFW_KEY_E:
			mCamera.input(LOOKRIGHT);
			break;
		case GLFW_KEY_UP:
			mCamera.input(TILTDOWN);
			break;
		case GLFW_KEY_DOWN:
			mCamera.input(TILTUP);
			break;
		case GLFW_KEY_LEFT:
			mCamera.input(LOOKLEFT);
			break;
		case GLFW_KEY_RIGHT:
			mCamera.input(LOOKRIGHT);
			break;
		case GLFW_KEY_C:
			mCamera.setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
			break;
		case GLFW_KEY_V:
			mCamera.setLookat(ballPosition);
			break;
		case GLFW_KEY_U:
			mSplineManager.showSpline();
			break;
		case GLFW_KEY_I:
			mSplineManager.showControlPoints();
			break;
		case GLFW_KEY_O:
			mSplineManager.showCage();
			break;
		case GLFW_KEY_P:
			mSplineManager.showSplinePoints();
			break;
		case GLFW_KEY_ENTER:
			mIsPlaying = !mIsPlaying;
			break;
		case GLFW_KEY_SPACE:
			mGlob.RunSimulationStep();
		default:
			break;
		}
	}
}

void GlobScene::screenResizeEvent(int width, int height) {

	glViewport(0, 0, width, height);
	mProjection = glm::perspective(glm::radians(45.0), (double)width / height, 1.0, 1000.0);
}
void GlobScene::renderScene() {
	float grey = 161.0f / 255.0f;
	glClearColor(grey, grey, grey, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	mView = mCamera.getCameraMatrix();
	mGrid.renderGeometry(mProjection, mView);
	//mSplineManager.renderGeometry(mProjection, mView);
	mGlob.setEyePosition(mCamera.getPosition());
	mGlob.renderGeometry(mProjection, mView);/*
	mStructure.setEyePosition(mCamera.getPosition());
	mStructure.renderGeometry(mProjection, mView);*/
}

void GlobScene::updateScene(double time)
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
			mSplineManager.updateGeometry(mTime);

			if (mSplineManager.doneInterpolation())
			{
				mIsPlaying = false;
				return;
			}

			auto point = mSplineManager.getSplinePosition();
			mCamera.setPosition(point);	//Each point of the spline is a new position for the camera
			auto mat = glm::translate(atlas::math::Matrix4(1.0f), ballPosition);
		}

	}
	//Update the SLERP procedure
	if (mCamera.isSlerping()) {
		mCamera.updateSlerp(mTime);
	}
}

