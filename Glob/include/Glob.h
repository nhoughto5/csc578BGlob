#ifndef GLOB_H
#define GLOB_H
#pragma once
#include <atlas\utils\Geometry.hpp>
#include "BlobSet.h"
#include "ImplicitPolygonizer.h"
#include "ShaderPaths.hpp"
class Glob : public atlas::utils::Geometry{
public:
	Glob();
	~Glob();
	void renderGeometry(atlas::math::Matrix4 projection, atlas::math::Matrix4 view) override;
	void updateGeometry(atlas::utils::Time const& t) override;
	void RunSimulationStep();
	void setLightPosition(glm::vec3 LP);
	void setAmbientLight(glm::vec4 aL);
	void setEyePosition(glm::vec3 eye);
private:
	GLuint verticesBufferId, mVao, globIndiciesBuffer;
	BlobSet g_blobs;
	OriginSpringSimulator g_simulator;
	ImplicitPolygonizer g_polygonizer;
	std::vector<glm::vec3> verticesBuffer;
	std::vector<GLushort> indices;
	void getIndices();
	glm::vec3 lightPosition, eyePosition;
	glm::vec4 ambientLight;
	
};
#endif // !GLOB_H
