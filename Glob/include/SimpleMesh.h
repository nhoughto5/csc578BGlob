#pragma once

#include <vector>
#include <atlas/utils/Geometry.hpp>
#include "ShaderPaths.hpp"
#include <iostream>
#include <fstream>
#define SM_VERTEX_STRIDE   3
#define SM_TRIANGLE_STRIDE 3

#define SM_VERTEX_INDEX(base,offset)   (((base)*SM_VERTEX_STRIDE)+(offset))
#define SM_TRIANGLE_INDEX(base,offset) (((base)*SM_TRIANGLE_STRIDE)+(offset))
USING_ATLAS_GL_NS;
USING_ATLAS_MATH_NS;
class SimpleMesh : public atlas::utils::Geometry
{
public:
	SimpleMesh();
	~SimpleMesh(void);
	void renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) override;
	void reset();

	size_t vertex_count() { return m_vVertices.size() / SM_VERTEX_STRIDE; }
	size_t triangle_count() { return m_vTriangles.size() / SM_TRIANGLE_STRIDE; }

	//int add_vertex();
	int add_vertex(glm::vec3 position, glm::vec3 normal);
	int add_triangle();
	int add_triangle(int v1, int v2, int v3);

	void set_normal(int i, float * normal);

	//glm::vec3 vertex(int i) { return m_vVertices[i*SM_VERTEX_STRIDE]; }
	//glm::vec3 normal(int i) { return m_vNormals[i*SM_VERTEX_STRIDE]; }
	GLuint * triangle(int i) { return &m_vTriangles[i*SM_TRIANGLE_STRIDE]; }
	glm::vec3 vertex(int i) { return m_vVertices[i]; }
	glm::vec3 normal(int i) { return m_vNormals[i]; }
	//GLuint * triangle(int i) { return &m_vTriangles[i]; }
	GLuint globVertexBufferID, VAO, globIndiciesBuffer;
	std::vector<glm::vec3> m_vVertices;
	std::vector<glm::vec3> m_vNormals;
	std::vector<GLuint> m_vTriangles;
	std::vector<GLuint> tri_Indices;

	std::vector<glm::vec3> myBuffer;
	std::vector<int> indicies;
private:
	void organizeData();
	void sendDataToGPU();
	void getIndicies();
};
