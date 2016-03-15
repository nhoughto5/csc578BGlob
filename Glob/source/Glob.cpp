#include "Glob.h"
const GLuint NUM_FLOATS_PER_VERTICE = 6;
const GLuint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
Glob::Glob(): g_simulator(1, 0.1) {
	USING_ATLAS_GL_NS;
	USING_ATLAS_MATH_NS;
	int start = 4;
	g_blobs.AddBlob(Blob(glm::vec3(-start, 0, 0), 2));
	g_blobs.AddBlob(Blob(glm::vec3(start, 0, 0), 2));
	g_blobs.AddBlob(Blob(glm::vec3(0, 0, start), 2));
	g_simulator.SetParticles(&g_blobs);
	g_polygonizer.SetFunction(&g_blobs);
	g_polygonizer.Initialize();
	g_polygonizer.Polygonize();

	glGenBuffers(1, &verticesBufferId);
	glGenBuffers(1, &globIndiciesBuffer);
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0); //Define vertex position buffer locations
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(3 * sizeof(float))); //Define vertex normal buffer locations
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globIndiciesBuffer);
	std::string shaderDir = generated::ShaderPaths::getShaderDirectory();
	std::vector<ShaderInfo> shaders
	{
		{ GL_VERTEX_SHADER, shaderDir + "glob.vs.glsl" },
		{ GL_FRAGMENT_SHADER, shaderDir + "glob.fs.glsl" }
	};
	mModel = glm::translate(Matrix4(1.0f), glm::vec3{ 0.0f,0.0f, 0.0f });
	mShaders.push_back(ShaderPointer(new Shader));
	mShaders[0]->compileShaders(shaders);
	mShaders[0]->linkShaders();
	mUniforms.insert(UniformKey("mvpMat", mShaders[0]->getUniformVariable("mvpMat")));
	mShaders[0]->disableShaders();
}

Glob::~Glob() {

}
void Glob::getIndices() {
	for (int i = 0; i < 0.5 * verticesBuffer.size(); ++i) {
		indices.push_back(i);
	}
}
void Glob::renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) {
	verticesBuffer = g_polygonizer.Mesh().getBufferData();
	getIndices();
	glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
	glBufferData(GL_ARRAY_BUFFER, verticesBuffer.size() * sizeof(glm::vec3), &verticesBuffer[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globIndiciesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	mShaders[0]->enableShaders();
	auto mvpMat = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_SHORT, nullptr);
	mShaders[0]->disableShaders();
}
void Glob::updateGeometry(atlas::utils::Time const& t) {

}
void Glob::RunSimulationStep()
{
	g_simulator.StepSimulation(0.1f);
	g_blobs.UpdateParticles();
	g_polygonizer.Polygonize();
}

//void Glob::drawMesh() {
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