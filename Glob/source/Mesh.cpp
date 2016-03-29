#include "Mesh.h"
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
Mesh::Mesh()
{
}
Mesh::~Mesh()
{
	faceIndex.clear();
}
//Split string seperated by spaces and place in vector
std::vector<std::string> splitSpace(const std::string &s) {
	std::stringstream ss(s);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	return vstrings;
}
//Splits a string by a delimiter into a vector of strings
std::vector<std::string> split(const std::string &txt, std::string delimiter)
{
	std::vector<std::string> strs;
	size_t pos = 0;
	std::string token;
	std::string s = txt;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		strs.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	token = s.substr(0, pos);
	strs.push_back(token);
	return strs;
}
face Mesh::getFace(std::string line) {
	face f;
	std::string::size_type sz;
	std::vector<std::string> tokens = splitSpace(line);
	std::vector<std::string> X;
	for (int i = 1; i < tokens.size(); ++i) {
		 X = split(tokens[i], "//");
		 f.v[i - 1] = std::stoi(X[0]);
		 f.n[i - 1] = std::stoi(X[1]);
	}
	return f;
}

//Read model into simulation from .obj file and store as a Mesh Object
void Mesh::LoadObjModel(const char *filename)
{
	face f;
	std::ifstream in(filename, std::ios::in);
	if (!in)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		exit(1);

	}
	std::string line;
	std::vector<std::vector<int>> verticesNormalIndicies;
	while (std::getline(in, line))
	{
		//cout<<line<<endl;
		if (line.substr(0, 2) == "v ") {
			std::istringstream v(line.substr(2));
			glm::vec3 vert;
			double x, y, z;
			v >> x; v >> y; v >> z;
			vert = glm::vec3(x, y, z);
			vertices.push_back(vert);
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream v(line.substr(2));
			GLuint a, b, c, l, m, n;
			v >> a; v >> b; v >> c;
			f = getFace(line);
			a--; b--; c--;
			faceIndex.push_back(f.v[0] - 1);
			faceIndex.push_back(f.v[1] - 1);
			faceIndex.push_back(f.v[2] - 1);
			faces.push_back(f);
		}
		else if (line.substr(0, 2) == "vn") {
			std::istringstream v(line.substr(2));
			glm::vec3 norm;
			double x, y, z;
			v >> x; v >> y; v >> z;
			norm = glm::vec3(x, y, z);
			faceNormals.push_back(norm);
		}
	}
	vertexNormals.resize(vertices.size());
	std::fill(vertexNormals.begin(), vertexNormals.end(), glm::vec3{ 0.0f, 0.0f, 0.0f });
	for (int i = 0; i < faces.size(); ++i) {
		vertexNormals[faces[i].v[0] - 1].x += faceNormals[faces[i].n[0] - 1].x;
		vertexNormals[faces[i].v[0] - 1].y += faceNormals[faces[i].n[0] - 1].y;
		vertexNormals[faces[i].v[0] - 1].z += faceNormals[faces[i].n[0] - 1].z;

		vertexNormals[faces[i].v[1] - 1].x += faceNormals[faces[i].n[1] - 1].x;
		vertexNormals[faces[i].v[1] - 1].y += faceNormals[faces[i].n[1] - 1].y;
		vertexNormals[faces[i].v[1] - 1].z += faceNormals[faces[i].n[1] - 1].z;

		vertexNormals[faces[i].v[2] - 1].x += faceNormals[faces[i].n[2] - 1].x;
		vertexNormals[faces[i].v[2] - 1].y += faceNormals[faces[i].n[2] - 1].y;
		vertexNormals[faces[i].v[2] - 1].z += faceNormals[faces[i].n[2] - 1].z;
	}
	for (int i = 0; i < vertexNormals.size(); ++i) {
		vertexNormals[i] = glm::normalize(vertexNormals[i]);
	}
	if (vertexNormals.size() != vertices.size()) {
		std::cerr << "Number of vertecies and vertex normals are not equal\n";
		exit(1);
	}
	for (int i = 0; i < vertexNormals.size(); ++i) {
		meshData.push_back(vertices[i]);
		meshData.push_back(colour);
		meshData.push_back(vertexNormals[i]);
	}
	vertices.clear();
	vertexNormals.clear();
	faces.clear();

}
std::vector<GLushort> Mesh::getIndicies() {
	return faceIndex;
}
std::vector<glm::vec3> Mesh::returnMesh()
{
	return meshData;
}
GLsizeiptr Mesh::vertexBufferSize()
{
	return meshData.size() * sizeof(glm::vec3);
}
GLsizeiptr Mesh::indexBufferSize()
{
	return faceIndex.size() * sizeof(GLushort);
}
GLuint Mesh::indiciesCount() {
	return faceIndex.size();
}
void Mesh::setColour(glm::vec3 in) {
	colour = in;
}