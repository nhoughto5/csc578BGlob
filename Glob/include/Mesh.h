#ifndef MESH_H
#define MESH_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <GL/glew.h>
struct face {
	int v[3], n[3];
};
class Mesh
{
public:
	Mesh();//constructor
	~Mesh();
	void LoadObjModel(const char *filename);//function to load obj model
	std::vector<glm::vec3> returnMesh();//return the vertices of mesh data
	std::vector<GLushort> getIndicies();
	GLsizeiptr vertexBufferSize();
	GLsizeiptr indexBufferSize();
	GLuint indiciesCount();
	void setColour(glm::vec3 in);
private:
	face getFace(std::string line);
	glm::vec3 colour;
	std::vector<glm::vec3> vertices;//to store vertex information of 3D model started with v
	std::vector<glm::vec3> meshData;//to store all 3D model face vertices
	std::vector<glm::vec3> faceNormals;//To store all normals
	std::vector<glm::vec3> vertexNormals;//To store all normals
	std::vector<GLushort> faceIndex;//to store the number of face index started with f
	std::vector<face> faces;
};

#endif // MESH_H