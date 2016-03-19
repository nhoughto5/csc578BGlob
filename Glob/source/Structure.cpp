#include "Structure.h"
#include "Mesh.h"
const GLuint NUM_FLOATS_PER_VERTICE = 9;
const GLuint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
GLuint planeNumIndices, fountainNumberOfIndicies;
Structure::Structure(GLuint dimension_) :
	dimension(dimension_),
	planeColour(0.3f, 0.3f, 0.3f),
	fountainColour((float)249/255, (float)238/255, (float)229/255),
	planeReflection(0.5),
	fountainReflection(1)
	{
	ShapeData plane = ShapeGenerator::makePlane(dimension, planeColour);
	Mesh fountain;
	fountain.setColour(fountainColour);
	fountain.LoadObjModel("C:/Users/Nick/Desktop/NickTop/HomeWork/Elec 578B/A2/csc578BGlob/Glob/lowfountain.obj");

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

	glGenVertexArrays(1, &fountainArrayObject);
	glBindVertexArray(fountainArrayObject);
	glGenBuffers(1, &fountainBufferID);
	glGenBuffers(1, &fountainIndicesID);
	glBindBuffer(GL_ARRAY_BUFFER, fountainBufferID);
	fountainData = fountain.returnMesh();
	fountainIndicies = fountain.getIndicies();
	glBufferData(GL_ARRAY_BUFFER, fountain.vertexBufferSize(), &fountainData[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fountainIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fountain.indexBufferSize(), &fountainIndicies[0], GL_STATIC_DRAW);
	fountainNumberOfIndicies = fountain.indiciesCount();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(3 * sizeof(float))); //Define vertex normal buffer locations
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(6 * sizeof(float))); //Define vertex color buffer locations

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
	mUniforms.insert(UniformKey("specularStrength", mShaders[0]->getUniformVariable("specularStrength")));
	mShaders[0]->disableShaders();
}
Structure::~Structure() {

}

void Structure::renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) {

	mShaders[0]->enableShaders();
	mModel = glm::translate(Matrix4(1.0f), glm::vec3{ 0.0f,0.0f, 0.0f });
	auto mvpMat = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
	glUniform3fv(mUniforms["lightPositionWorld"], 1, &lightPosition[0]);
	glUniform3fv(mUniforms["eyePositionWorld"], 1, &eyePosition[0]);
	glUniform4fv(mUniforms["ambientLight"], 1, &ambientLight[0]);
	glUniformMatrix4fv(mUniforms["modelToWorldMatrix"], 1, GL_FALSE, &mModel[0][0]);
	glUniform1f(mUniforms["specularStrength"], planeReflection);
	glBindVertexArray(mVao);
	glDrawElements(GL_TRIANGLES, planeNumIndices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(fountainArrayObject);
	glUniform1f(mUniforms["specularStrength"], fountainReflection);
	glDrawElements(GL_TRIANGLES, fountainNumberOfIndicies, GL_UNSIGNED_SHORT, nullptr);
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