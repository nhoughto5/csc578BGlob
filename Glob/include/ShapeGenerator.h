#pragma once
#include <ShapeData.h>
typedef unsigned int uint;

class ShapeGenerator
{
	static ShapeData makePlaneVerts(uint dimensions, glm::vec3 colour);
	static ShapeData makePlaneIndices(uint dimensions);

public:
	static ShapeData makeTriangle();
	static ShapeData makeCube();
	static ShapeData makeArrow();
	static ShapeData makePlane(uint dimensions, glm::vec3 colour);
};

