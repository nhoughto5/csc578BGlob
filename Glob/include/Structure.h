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
	Structure();
	~Structure();

	void renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) override;
	void setLightPosition(glm::vec3 LP);
	void setAmbientLight(glm::vec4 aL);
	void setEyePosition(glm::vec3 eye);
private:
	GLuint mBufferID, mVao, dimension, numVertices, numIndicies, mIndiciesID;
	glm::vec3 planeColour;
	ShapeData plane;
	glm::vec3 lightPosition, eyePosition;
	glm::vec4 ambientLight;
};
#endif // !STRUCTURE_H
