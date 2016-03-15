#include "SimpleMesh.h"
#include <algorithm>
//#include <GL/freeglut.h>

SimpleMesh::SimpleMesh()
{
}

SimpleMesh::~SimpleMesh()
{
}


void SimpleMesh::reset()
{
	m_vVertices.resize(0);
	m_vNormals.resize(0);
	m_vTriangles.resize(0);
}



int SimpleMesh::add_vertex()
{
	int nIndex = (int)m_vVertices.size() / SM_VERTEX_STRIDE;
	for ( int k = 0; k < SM_VERTEX_STRIDE; ++k )
		m_vVertices.push_back(-1);
	for ( int k = 0; k < SM_VERTEX_STRIDE; ++k )
		m_vVertices.push_back(-1);

	return nIndex;
}

int SimpleMesh::add_vertex(float * position, float * normal)
{
	int nIndex = (int)m_vVertices.size() / SM_VERTEX_STRIDE;
	for ( int k = 0; k < SM_VERTEX_STRIDE; ++k )
		m_vVertices.push_back(position[k]);
	for ( int k = 0; k < SM_VERTEX_STRIDE; ++k )
		m_vNormals.push_back(normal[k]);

	return nIndex;
}



int SimpleMesh::add_triangle()
{
	int nIndex = (int)m_vTriangles.size() / SM_TRIANGLE_STRIDE;
	for ( int k = 0; k < SM_VERTEX_STRIDE; ++k )
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
	float * n = normal(i);
	n[0] = setn[0];  n[1] = setn[1];  n[2] = setn[2];
}



void SimpleMesh::DrawFaces_Smooth()
{
	bufferData.clear();
	size_t nCount = triangle_count();
	for ( unsigned int i = 0; i < nCount; ++i ) {
		int * pFace = triangle(i);
		for ( int k = 0; k < 3; ++k ) {
			bufferData.push_back(glm::vec3(vertex(pFace[k])[0], vertex(pFace[k])[1], vertex(pFace[k])[2]));
			bufferData.push_back(glm::vec3(normal(pFace[k])[0], normal(pFace[k])[1], normal(pFace[k])[2]));
		}
	}
}
std::vector<glm::vec3> SimpleMesh::getBufferData() {
	DrawFaces_Smooth();
	return bufferData;
}
