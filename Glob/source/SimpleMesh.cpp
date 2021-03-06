#include "SimpleMesh.h"
#include <algorithm>
#define noop
const GLuint NUM_FLOATS_PER_VERTICE = 6;
const GLuint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

SimpleMesh::SimpleMesh()
{
	glGenBuffers(1, &globVertexBufferID);
	glGenBuffers(1, &globIndiciesBuffer);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, globVertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, 0); //Define vertex position buffer locations
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_BYTE_SIZE, (void*)(3 * sizeof(float))); //Define vertex normal buffer locations
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

SimpleMesh::~SimpleMesh()
{
}
void SimpleMesh::renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) {
	organizeData();
	sendDataToGPU();
	mShaders[0]->enableShaders();
	auto mvpMat = projection * view * mModel;
	glUniformMatrix4fv(mUniforms["mvpMat"], 1, GL_FALSE, &mvpMat[0][0]);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_vTriangles.size(), GL_UNSIGNED_INT, nullptr);
	mShaders[0]->disableShaders();
}
void SimpleMesh::sendDataToGPU() {
	glBindBuffer(GL_ARRAY_BUFFER, globVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, myBuffer.size() * sizeof(float), &myBuffer[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globIndiciesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STREAM_DRAW);
}
void SimpleMesh::getIndicies() {
	size_t nCount = triangle_count();
	//for (unsigned int i = 0; i < nCount; ++i) {
	//	GLuint * pFace = triangle(i);
	//	for (int k = 0; k < 3; ++k) {
	//		indicies.push_back(pFace[k]);
	//	}
	//}
	for (int i = 0; i < nCount; ++i) {
		indicies.push_back(i);
	}
}
void SimpleMesh::organizeData() {
	//getIndicies();
	std::ofstream myfile;
	myfile.open("myVertices.txt");
	//int mycount = std::count(m_vTriangles.begin(), m_vTriangles.end(), 0);
	size_t nCount = triangle_count();
	for (unsigned int i = 0; i < nCount; ++i) {
		GLuint * pFace = triangle(i);
		myfile << "Vertex  " << *triangle(i) << ": ";
		for (int k = 0; k < 3; ++k) {
			myfile << "V" << pFace[k] << ": " << vertex(pFace[k])[0] << " " << vertex(pFace[k])[1] << " " << vertex(pFace[k])[2] << "   ";
			myBuffer.push_back(glm::vec3{ vertex(pFace[k])[0], vertex(pFace[k])[1], vertex(pFace[k])[2] });
			myBuffer.push_back(glm::vec3{ normal(pFace[k])[0], normal(pFace[k])[1], normal(pFace[k])[2] });
			indicies.push_back(pFace[k]);
		}
		//for (int k = 0; k < 3; ++k) {
		//	
		//}
		myfile << "\n";
	}
	myfile.close();
}
void SimpleMesh::reset()
{
	m_vVertices.resize(0);
	m_vNormals.resize(0);
	m_vTriangles.resize(0);
	myBuffer.clear();
}
//int SimpleMesh::add_vertex()
//{
//	int nIndex = (int)m_vVertices.size() / SM_VERTEX_STRIDE;
//	for (int k = 0; k < SM_VERTEX_STRIDE; ++k)
//		m_vVertices.push_back(-1);
//	for (int k = 0; k < SM_VERTEX_STRIDE; ++k)
//		m_vVertices.push_back(-1);
//
//	return nIndex;
//}
int SimpleMesh::add_vertex(glm::vec3 position, glm::vec3 normal)
{
	int nIndex = m_vVertices.size();
	m_vVertices.push_back(position);
	m_vNormals.push_back(normal);

	return nIndex;
}

int SimpleMesh::add_triangle()
{
	int nIndex = (int)m_vTriangles.size() / SM_TRIANGLE_STRIDE;
	for (int k = 0; k < SM_VERTEX_STRIDE; ++k)
		m_vTriangles.push_back(-1);
	return nIndex;
}

int SimpleMesh::add_triangle(int v1, int v2, int v3)
{
	int nIndex = (int)m_vTriangles.size() / SM_TRIANGLE_STRIDE;
	m_vTriangles.push_back(v1);
	m_vTriangles.push_back(v2);
	m_vTriangles.push_back(v3);
	tri_Indices.push_back(nIndex);
	return nIndex;
}

void SimpleMesh::set_normal(int i, float * setn)
{
	glm::vec3 n = normal(i);
	n[0] = setn[0];  n[1] = setn[1];  n[2] = setn[2];
}
