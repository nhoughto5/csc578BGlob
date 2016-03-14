#pragma once

#include <vector>
#include <atlas/utils/Geometry.hpp>
#include "ShaderPaths.hpp"
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

	int add_vertex();
	int add_vertex(float * position, float * normal);
	int add_triangle();
	int add_triangle(int v1, int v2, int v3);

	void set_normal(int i, float * normal);

	float * vertex(int i) { return &m_vVertices[i*SM_VERTEX_STRIDE]; }
	float * normal(int i) { return &m_vNormals[i*SM_VERTEX_STRIDE]; }
	int * triangle(int i) { return &m_vTriangles[i*SM_TRIANGLE_STRIDE]; }

	void DrawFaces_Smooth();

	GLuint globVertexBufferID, VAO;
	std::vector<float> m_vVertices;
	std::vector<float> m_vNormals;
	std::vector<int> m_vTriangles;

	std::vector<float> myBuffer;

private:
	void organizeData();
	void sendDataToGPU();
};
