//#include "GlobScene.hpp"
//#include <atlas/core/GLFW.hpp>
//#include <atlas/core/Log.hpp>
//#include <atlas/core/Macros.hpp>
//#include <atlas/core/Float.hpp>
//#include <iostream>
//float g_distance = 1;
//float g_boundsRadius = 5;
//GlobScene::GlobScene() :
//	mIsPlaying(false),
//	mLastTime(0.0f),
//	mFPS(60.0f),
//	mTick(1.0f / mFPS),
//	mAnimTime(0.0f),
//	mAnimLength(10.0f),
//	mSplineManager(int(mAnimLength * mFPS)),
//	ballPosition{ 4.0f, 0.0f, 4.0f },
//	mCamera(),
//	g_simulator(1, 0.1)
//{
//	glEnable(GL_DEPTH_TEST);
//	auto mat = glm::translate(atlas::math::Matrix4(1.0f), ballPosition);
//	int start = 4;
//	g_blobs.AddBlob(Blob(glm::vec3(-start, 0, 0), 2));
//	//g_blobs.AddBlob(Blob(glm::vec3(start, 0, 0), 2));
//	//g_blobs.AddBlob(Blob(glm::vec3(0, 0, start), 2));
//	g_simulator.SetParticles(&g_blobs);
//	g_polygonizer.SetFunction(&g_blobs);
//	g_polygonizer.Initialize();
//	g_polygonizer.Polygonize();
//	g_distance = 5;
//	g_boundsRadius = 3;
//}
//
//GlobScene::~GlobScene() {
//}
////UNUSED
//void GlobScene::mousePressEvent(int button, int action, int modifiers, double xPos, double yPos) {
//	UNUSED(button);
//	UNUSED(action);
//	UNUSED(modifiers);
//	UNUSED(xPos);
//	UNUSED(yPos);
//}
//void GlobScene::mouseMoveEvent(double xPos, double yPos) {
//	mCamera.mouseUpdate(glm::vec2(xPos, yPos));
//}
//void GlobScene::keyPressEvent(int key, int scancode, int action, int mods) {
//	UNUSED(scancode);
//	UNUSED(mods);
//
//	if (action == GLFW_PRESS)
//	{
//		switch (key)
//		{
//		case GLFW_KEY_T:
//			mCamera.resetCamera();
//			break;
//		case GLFW_KEY_W:
//			mCamera.input(FORWARD);
//			break;
//		case GLFW_KEY_S:
//			mCamera.input(BACK);
//			break;
//		case GLFW_KEY_A:
//			mCamera.input(LEFT);
//			break;
//		case GLFW_KEY_D:
//			mCamera.input(RIGHT);
//			break;
//		case GLFW_KEY_R:
//			mCamera.input(UP);
//			break;
//		case GLFW_KEY_F:
//			mCamera.input(DOWN);
//			break;
//		case GLFW_KEY_Q:
//			mCamera.input(LOOKLEFT);
//			break;
//		case GLFW_KEY_E:
//			mCamera.input(LOOKRIGHT);
//			break;
//		case GLFW_KEY_UP:
//			mCamera.input(TILTDOWN);
//			break;
//		case GLFW_KEY_DOWN:
//			mCamera.input(TILTUP);
//			break;
//		case GLFW_KEY_LEFT:
//			mCamera.input(LOOKLEFT);
//			break;
//		case GLFW_KEY_RIGHT:
//			mCamera.input(LOOKRIGHT);
//			break;
//		case GLFW_KEY_C:
//			mCamera.setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
//			break;
//		case GLFW_KEY_V:
//			mCamera.setLookat(ballPosition);
//			break;
//		case GLFW_KEY_U:
//			mSplineManager.showSpline();
//			break;
//		case GLFW_KEY_I:
//			mSplineManager.showControlPoints();
//			break;
//		case GLFW_KEY_O:
//			mSplineManager.showCage();
//			break;
//		case GLFW_KEY_P:
//			mSplineManager.showSplinePoints();
//			break;
//		case GLFW_KEY_ENTER:
//			mCamera.setTarget(ballPosition);
//			mCamera.startSlerp(3);
//			break;
//		case GLFW_KEY_SPACE:
//			//mIsPlaying = !mIsPlaying;
//			RunSimulationStep();
//		default:
//			break;
//		}
//	}
//}
//void GlobScene::RunSimulationStep()
//{
//	g_simulator.StepSimulation(0.1f);
//	g_blobs.UpdateParticles();
//	g_polygonizer.Polygonize();
//}
//void GlobScene::screenResizeEvent(int width, int height) {
//
//	glViewport(0, 0, width, height);
//	mProjection = glm::perspective(glm::radians(45.0), (double)width / height, 1.0, 1000.0);
//}
//void GlobScene::renderScene() {
//	float grey = 161.0f / 255.0f;
//	glClearColor(grey, grey, grey, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_DEPTH_TEST);
//
//	mView = mCamera.getCameraMatrix();
//	mGrid.renderGeometry(mProjection, mView);
//	//mSplineManager.renderGeometry(mProjection, mView);
//}
//void GlobScene::drawMesh() {
//	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	// set light as headlight
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	//	float fLightPos[4] = {g_center[0] + g_boundsRadius, g_center[1] + g_boundsRadius, g_center[2]+10.0f*g_boundsRadius, 1.0f};
//	float fLightPos[4] = { g_boundsRadius, g_boundsRadius, 10.0f*g_boundsRadius, 1.0f };
//	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
//	float fAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
//	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbient);
//	float fDiffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuse);
//
//	glEnable(GL_DEPTH_TEST);
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glColor3f(0.8f, 0.8f, 1.0f);
//
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0f, 1.0f);
//
//	g_polygonizer.Mesh().DrawFaces_Smooth();
//
//	glDisable(GL_POLYGON_OFFSET_FILL); //Draws Faces
//
//									   // draw mesh edges
//	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT);
//	glDisable(GL_LIGHTING);
//
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glColor3f(0, 0, 0);
//	g_polygonizer.Mesh().DrawFaces_Smooth(); // Draws Wire Frame
//
//	glPopAttrib();
//
//
//	glPushAttrib(GL_ENABLE_BIT);
//	glDisable(GL_LIGHTING);
//	glDisable(GL_DEPTH_TEST);
//	g_blobs.DrawParticles();
//	glPopAttrib();
//}
//void GlobScene::updateScene(double time)
//{
//
//	mTime.currentTime = (float)time;
//	mTime.totalTime += (float)time;
//	if (atlas::core::geq(mTime.currentTime - mLastTime, mTick))
//	{
//		mLastTime += mTick;
//		mTime.deltaTime = mTick;
//
//		if (mIsPlaying)
//		{
//			mAnimTime += mTick;
//			mSplineManager.updateGeometry(mTime);
//
//			if (mSplineManager.doneInterpolation())
//			{
//				mIsPlaying = false;
//				return;
//			}
//
//			auto point = mSplineManager.getSplinePosition();
//			mCamera.setPosition(point);	//Each point of the spline is a new position for the camera
//			auto mat = glm::translate(atlas::math::Matrix4(1.0f), ballPosition);
//		}
//
//	}
//	//Update the SLERP procedure
//	if (mCamera.isSlerping()) {
//		mCamera.updateSlerp(mTime);
//	}
//}
//
