// BlobMesher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstdlib>
#include <cmath>
//#define GLUT_DISABLE_ATEXIT_HACK
#include <GL\freeglut.h>
#include "BlobSet.h"
#include "ImplicitPolygonizer.h"
#include <algorithm>
// get rid of console window
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  


#define M_PI 3.14159


// camera
glm::vec3 g_center;
glm::vec3 g_eye;
float g_viewAngle[2] = { 0,0 };
float g_distance = 1;
float g_boundsRadius = 5;


// scene
BlobSet g_blobs;
OriginSpringSimulator g_simulator = OriginSpringSimulator(1, 0.1);

ImplicitPolygonizer g_polygonizer;

static void InitializeScene()
{
	g_blobs.AddBlob(Blob(glm::vec3(-4, 0, 0), 2));
	g_blobs.AddBlob(Blob(glm::vec3(4, 0, 0), 2));
	g_blobs.AddBlob(Blob(glm::vec3(0, 0, 4), 2));
	g_simulator.SetParticles(&g_blobs);

	g_polygonizer.SetFunction(&g_blobs);
	g_polygonizer.Initialize();
	g_polygonizer.Polygonize();

	g_center = glm::vec3(0, 0, 0);
	g_distance = 3;
	g_boundsRadius = 3;
}


void RunSimulationStep()
{
	g_simulator.StepSimulation(0.1f);
	g_blobs.UpdateParticles();
	g_polygonizer.Polygonize();
	glutPostRedisplay();
}



static void ondraw(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set light as headlight
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//	float fLightPos[4] = {g_center[0] + g_boundsRadius, g_center[1] + g_boundsRadius, g_center[2]+10.0f*g_boundsRadius, 1.0f};
	float fLightPos[4] = { g_boundsRadius, g_boundsRadius, 10.0f*g_boundsRadius, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
	float fAmbient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, fAmbient);
	float fDiffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fDiffuse);

	// construct camera
	float fTheta = g_viewAngle[0];
	float fPhi = std::min((float)M_PI - 0.01f, std::max(g_viewAngle[1] + ((float)M_PI / 2.0f), 0.01f));
	//glm::vec3 eyeVec( cos( fTheta ) * sin( fPhi ),   -cos( fPhi ),  sin( fTheta ) * sin( fPhi ) );
	glm::vec3 eyeVec(sin(fTheta) * sin(fPhi), -cos(fPhi), cos(fTheta) * sin(fPhi));
	/*eyeVec.Normalize();*/
	eyeVec = glm::normalize(eyeVec);
	g_eye = g_center + g_distance*2.0f*g_boundsRadius*eyeVec;
	gluLookAt(g_eye[0], g_eye[1], g_eye[2], g_center[0], g_center[1], g_center[2], 0, 1, 0);


	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.8f, 0.8f, 1.0f);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);

	g_polygonizer.Mesh().DrawFaces_Smooth();

	glDisable(GL_POLYGON_OFFSET_FILL); //Draws Faces

									   // draw mesh edges
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT);
	glDisable(GL_LIGHTING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 0, 0);
	g_polygonizer.Mesh().DrawFaces_Smooth(); // Draws Wire Frame

	glPopAttrib();


	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	g_blobs.DrawParticles();
	glPopAttrib();


	glutSwapBuffers();
};

static void onreshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width / (float)height, 0.01f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}
static void onkey(unsigned char k, int x, int y)
{
	switch (k) {
	case 27:
		exit(0);

	case ' ':
		RunSimulationStep();
		break;

	}
}

static int g_button = -1;
static int last_x;  int last_y;
static void onmouse(int b, int state, int x, int y)
{
	if (b == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		g_button = GLUT_LEFT_BUTTON;
	}
	else if (b == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		g_button = GLUT_RIGHT_BUTTON;
	}
	else
		g_button = -1;

	last_x = x;		last_y = y;
}
static void onmotion(int x, int y)
{
	if (g_button == GLUT_LEFT_BUTTON) {
		int dx = x - last_x;		int dy = y - last_y;
		g_viewAngle[0] -= (float)dx * 0.01f;
		g_viewAngle[1] += (float)dy * 0.01f;
		glutPostRedisplay();
	}
	else if (g_button == GLUT_RIGHT_BUTTON) {
		int dx = x - last_x;		int dy = y - last_y;
		g_distance += (float)dy*0.01f;
		if (g_distance < 0.01f)
			g_distance = 0.01f;
		glutPostRedisplay();
	}
	last_x = x;		last_y = y;
}




int _tmain(int argc, _TCHAR* argv[])
{
	glutInitWindowSize(1100, 800);
	glutInitWindowPosition(300, 50);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("mesher");

	glutReshapeFunc(onreshape);
	glutDisplayFunc(ondraw);
	glutKeyboardFunc(onkey);
	glutMouseFunc(onmouse);
	glutMotionFunc(onmotion);


	InitializeScene();


	glutMainLoop();

	return 0;

}

