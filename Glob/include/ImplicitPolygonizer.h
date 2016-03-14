#pragma once

#include "ImplicitFunction.h"
#include "SimpleMesh.h"


class ImplicitPolygonizer
{
public:
	ImplicitPolygonizer(void);
	~ImplicitPolygonizer(void);

	void SetFunction(ImplicitFunction * pFunction);
	ImplicitFunction * Function() { return m_pFunction; }

	SimpleMesh & Mesh() { return m_mesh; }

	bool Initialize();
	bool Polygonize();

protected:
	SimpleMesh m_mesh;
	ImplicitFunction * m_pFunction;
	void * m_pPolygonizer;
};
