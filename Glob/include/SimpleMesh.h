#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <glm\glm.hpp>
#define SM_VERTEX_STRIDE   3
#define SM_TRIANGLE_STRIDE 3

#define SM_VERTEX_INDEX(base,offset)   (((base)*SM_VERTEX_STRIDE)+(offset))
#define SM_TRIANGLE_INDEX(base,offset) (((base)*SM_TRIANGLE_STRIDE)+(offset))


class SimpleMesh
{
public:
	SimpleMesh();
	~SimpleMesh(void);

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
	int countThem(int x) { 
		int count = 0;
		for (int i = 0; i < m_vTriangles.size(); ++i) {
			if (x == m_vTriangles[0]) ++count;
		}
		return count; 
	}
	void DrawFaces_Smooth();
	std::vector<glm::vec3> bufferData;
	std::vector<glm::vec3> getBufferData();
	std::vector<float> m_vVertices;
	std::vector<float> m_vNormals;
	std::vector<int> m_vTriangles;
	std::vector<int> tri_Indices;
};
