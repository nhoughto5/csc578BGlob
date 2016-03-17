#include "Structure.h"
const GLuint NUM_FLOATS_PER_VERTICE = 9;
const GLuint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
GLuint planeNumIndices;
Structure::Structure() : 
	dimension(20),
	planeColour(0.3f, 0.3f, 0.3f)
	{
	ShapeData plane = ShapeGenerator::makePlane(20, planeColour);
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glGenBuffers(1, &mBufferID);
	glGenBuffers(1, &mIndiciesID);
	glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	glBufferData(GL_ARRAY_BUFFER, plane.vertexBufferSize(), plane.vertices,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane.indexBufferSize(), plane.indices, GL_STATIC_DRAW);
	planeNumIndices = plane.numIndices;

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(3 * sizeof(float))); //Define vertex normal buffer locations
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(6 * sizeof(float))); //Define vertex color buffer locations
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndiciesID);

	std::string shaderDir = generated::ShaderPaths::getShaderDirectory();

	std::vector<ShaderInfo> shaders
	{
		{ GL_VERTEX_SHADER, shaderDir + "structure.vs.glsl" },
		{ GL_FRAGMENT_SHADER, shaderDir + "structure.fs.glsl" }
	};

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
Structure::~Structure() {

}

void Structure::renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) {
	
	//glBindBuffer(GL_ARRAY_BUFFER, mBufferID);
	//glBufferData(GL_ARRAY_BUFFER, mBuffer.size() * sizeof(glm::vec3), &mBuffer[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, numIndicies);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

	mShaders[0]->enableShaders();
	mModel = glm::translate(Matrix4(1.0f), glm::vec3{ 0.0f,0.0f, 0.0f });
	auto mvpMat = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
	glUniform3fv(mUniforms["lightPositionWorld"], 1, &lightPosition[0]);
	glUniform3fv(mUniforms["eyePositionWorld"], 1, &eyePosition[0]);
	glUniform4fv(mUniforms["ambientLight"], 1, &ambientLight[0]);
	glUniformMatrix4fv(mUniforms["modelToWorldMatrix"], 1, GL_FALSE, &mModel[0][0]);
	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, nullptr);
	mShaders[0]->disableShaders();
}
void Structure::setLightPosition(glm::vec3 LP) {
	lightPosition = LP;
}
void Structure::setAmbientLight(glm::vec4 aL) {
	ambientLight = aL;
}
void Structure::setEyePosition(glm::vec3 eye) {
	eyePosition = eye;
}