#include "Glob.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
const GLuint NUM_FLOATS_PER_VERTICE = 6;
const GLuint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
USING_ATLAS_GL_NS;
USING_ATLAS_MATH_NS;
Glob::Glob(): 
	g_simulator(1, 0.1)
	{

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
	mUniforms.insert(UniformKey("lightPositionWorld", mShaders[0]->getUniformVariable("lightPositionWorld")));
	mUniforms.insert(UniformKey("ambientLight", mShaders[0]->getUniformVariable("ambientLight")));
	mUniforms.insert(UniformKey("eyePositionWorld", mShaders[0]->getUniformVariable("eyePositionWorld")));
	mUniforms.insert(UniformKey("modelToWorldMatrix", mShaders[0]->getUniformVariable("modelToWorldMatrix")));
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
	mModel = glm::translate(Matrix4(1.0f), glm::vec3{ 0.0f,8.0f, 0.0f });
	glBindBuffer(GL_ARRAY_BUFFER, verticesBufferId);
	glBufferData(GL_ARRAY_BUFFER, verticesBuffer.size() * sizeof(glm::vec3), &verticesBuffer[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globIndiciesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
	mShaders[0]->enableShaders();
	auto mvpMat = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
	glUniform3fv(mUniforms["lightPositionWorld"], 1, &lightPosition[0]);
	glUniform3fv(mUniforms["eyePositionWorld"], 1, &eyePosition[0]);
	glUniform4fv(mUniforms["ambientLight"], 1, &ambientLight[0]);
	glUniformMatrix4fv(mUniforms["modelToWorldMatrix"], 1, GL_FALSE, &mModel[0][0]);
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
void Glob::setLightPosition(glm::vec3 LP) {
	lightPosition = LP;
}
void Glob::setAmbientLight(glm::vec4 aL) {
	ambientLight = aL;
}
void Glob::setEyePosition(glm::vec3 eye) {
	eyePosition = eye;
}