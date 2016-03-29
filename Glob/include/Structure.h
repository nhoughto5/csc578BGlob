//The Structure class is used to draw the plane and the fountain
#ifndef STRUCTURE_H
#define STRUCTURE_H
#pragma once
#include <atlas/utils/Geometry.hpp>
#include "ShaderPaths.hpp"
#include "ShapeData.h"
#include "ShapeGenerator.h"
USING_ATLAS_GL_NS;
USING_ATLAS_MATH_NS;
class Structure : public atlas::utils::Geometry {
public:
	Structure(GLuint dimension_);
	~Structure();

	void renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) override;
	void setLightPosition(glm::vec3 LP);
	void setAmbientLight(glm::vec4 aL);
	void setEyePosition(glm::vec3 eye);
private:
	std::vector<glm::vec3> fountainData;
	std::vector<GLushort> fountainIndicies;
	GLuint mBufferID, mVao, dimension, numVertices, numIndicies, mIndiciesID, fountainBufferID, fountainIndicesID, fountainArrayObject;
	glm::vec3 planeColour, fountainColour;
	ShapeData plane;
	GLfloat planeReflection, fountainReflection;
	glm::vec3 lightPosition, eyePosition;
	glm::vec4 ambientLight;
};
#endif // !STRUCTURE_H
